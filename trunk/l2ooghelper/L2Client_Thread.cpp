#include "stdafx.h"
#include "Resource.h"
#include "Logger.h"
#include "MainWindow.h"
#include "L2Client.h"
#include "SEH_global.h"

void L2Client::startClient()
{
	if( isThreadRunning() )
	{
		MessageBox( hWnd, TEXT("Уже запущено"), TEXT("O_o"), MB_ICONWARNING );
		return;
	}
	hThread = (HANDLE)_beginthreadex( NULL, 0,
		(unsigned int (__stdcall *)(void *))L2ClientThread, (void *)this,
		0, (unsigned int *)&dwThreadId );
	if( !hThread )
	{
		dwThreadId = 0;
		MessageBox( hWnd, TEXT("_beginthreadex() failed!"),
			TEXT("Cannot start thread!"), MB_ICONSTOP );
	}
}

void L2Client::logoutClient()
{
	if( !isThreadRunning() )
	{
		MessageBox( hWnd, TEXT("Не запущено"), TEXT("O_o"), MB_ICONWARNING );
		return;
	}
	this->game_logoutRequestSent = false;
	this->threadSignal = THREAD_SIGNAL_LOGOUT;
}

void L2Client::disconnectClient( bool force )
{
	if( !isThreadRunning() )
	{
		//MessageBox( hWnd, TEXT("Не запущено"), TEXT("O_o"), MB_ICONWARNING );
		return;
	}
	this->game_logoutRequestSent = false;
	if( !force ) this->threadSignal = THREAD_SIGNAL_DISCONNECT;
	if( force ) this->threadSignal = THREAD_SIGNAL_FORCE_DISCONNECT;
}


void L2Client::threadNetCleanup( class L2Client *pcls )
{
	if( pcls->sock != INVALID_SOCKET )
	{
		L2PNet_shutdown( pcls->sock );
		L2PNet_closesocket( pcls->sock );
	}
	//CloseHandle( pcls->hThread );
	//pcls->hThread = NULL;
	//pcls->dwThreadId = 0;
	pcls->sock = INVALID_SOCKET;
	pcls->setState( STATE_OFFLINE );
	pcls->threadSignal = 0;
	// check Bot IPC
	// remove bot from bot IPC
	pcls->ensureRemoveSelfFromBotIPC();
}

DWORD WINAPI L2Client::L2ClientThread( LPVOID lpParam )
{
#ifdef USE_SEH
#ifdef _MSC_VER
	__try { // start exception handler
#endif
#endif

	class L2Client *pcls = (class L2Client *)lpParam;
	pcls->threadSignal = 0;

	bool ret = L2Client::L2ClientThread_Login( pcls );
	if( ret )
	{
		L2Client::L2ClientThread_Game( pcls );
	}

	pcls->clearUserAndWorldState();

	// mark thread as not running
	CloseHandle( pcls->hThread );
	pcls->hThread = NULL;
	pcls->dwThreadId = 0;

#ifdef USE_SEH
#ifdef _MSC_VER // exception handler
	}
	__except( L2ClientThreadExceptonHandler( GetExceptionInformation() ) )
	{
		log_error( LOG_ERROR, "__except: Exception in L2ClientThread thread...\n" );
		ErrorLogger_FlushLogFile();
	}
#endif
#endif

	return 0;
}

void L2Client::log_packet( L2Client *pcls, bool fromServer,
	const unsigned char *p, unsigned int plen, const char *hintStr, LOG_LEVEL logLevel )
{
	if( !p || (plen<3) ) return;
	unsigned char opcode1 = p[2];
	unsigned short opcode2 = p[3] | ( ((unsigned short)p[4])<<8 );
	unsigned short opcode3 = p[5] | ( ((unsigned short)p[6])<<8 );
	char pname[256] = {0};
	LOG_LEVEL ll = logLevel;
	if( fromServer ) log_error( ll, "Server: " );
	else log_error( ll, "Client: " );
	L2_VERSION l2_version = pcls->account.getL2Version();
	L2Packets_GetL2PacketName( l2_version, fromServer, opcode1, opcode2, opcode3, pname, 127 );
	log_error_np( ll, "%02X %s: len %u\n", opcode1, pname, plen );
	log_error_np( ll, "%02X %02X\n", p[0], p[1] );
	log_error_np( ll, "%02X\n", p[2] );
	if( !hintStr )
	{
		unsigned int i;
		unsigned int il = 0; // number of char in line
		for( i=3; i<plen; i++ )
		{
			log_error_np( ll, "%02X ", p[i] );
			il++;
			// space every 4 chars
			if( il % 4 == 0 ) log_error_np( ll, " " );
			if( il == 16 )
			{
				for( il=(i-16); il<=i; il++ )
				{
					char ch = p[il];
					bool bd = false;
					if( (ch>='0') && (ch<='9') ) bd = true;
					if( (ch>='A') && (ch<='Z') ) bd = true;
					if( (ch>='a') && (ch<='z') ) bd = true;
					//if( (ch>='а') && (ch<='я') ) bd = true;
					//if( (ch>='А') && (ch<='я') ) bd = true;
					if( (ch=='!') || (ch==',') || (ch=='.') || (ch=='\'') || (ch=='\"')
						|| (ch==':') || (ch=='(') || (ch==')') ) bd = true;
					if( bd ) log_error_np( ll, "%c", ch );
					else log_error_np( ll, " " );
				}
				log_error_np( ll, "\n" );
				il = 0;
			}
		}
		/*if( il < 16 )
		{
			for( i=il; i<=16; i++ ) log_error_np( ll, " " );
		}*/
	}
	else
	{
		char *phint = (char *)hintStr;
		unsigned int i = 3;
		while( (*phint) )
		{
			switch( (*phint) )
			{
			case 'c':
				{
					log_error( ll, "c: %c\n", p[i] );
					i++;
				} break;
			case 'h':
				{
					log_error( ll, "h: %hd\n", *((unsigned short *)(p+i)) );
					i += 2;
				} break;
			case 'd':
				{
					log_error( ll, "d: %d\n", *((int *)(p+i)) );
					i += 4;
				} break;
			case 'u':
				{
					log_error( ll, "u: %u\n", *((unsigned int *)(p+i)) );
					i += 4;
				} break;
			case 'x':
				{
					log_error( ll, "x: %x\n", *((unsigned int *)(p+i)) );
					i += 4;
				} break;
			case 'f':
				{
					log_error( ll, "f: %0.6f\n", *((double *)(p+i)) );
					i += 8;
				} break;
			case 'S':
				{
					log_error( ll, "S: %S\n", ((wchar_t *)(p+i)) );
					i += wcslen( (wchar_t *)(p+i) )*2 + 2;
				} break;
			}
			phint++;
		}
	}
	log_error_np( ll, "\n" );
}
