#include "stdafx.h"
#include "ConfigIni.h"
#include "Logger.h"
#include "LoginListener.h"
#include "LoginClient.h"
#include "GameListener.h"

extern class CConfig g_cfg;         // declared in main.cpp
extern class GameListener *g_pgame; // declared in main.cpp
extern HWND g_radardll_hwnd;        // declared in RadarDllWnd.cpp

struct FLLT_INFO
{
	class LoginListener *cls;
};

void FLS_Thread_freeInfo( struct FLLT_INFO *pinfo )
{
	if( pinfo->cls->m_listen_socket != INVALID_SOCKET )
	{
		L2PNet_shutdown( pinfo->cls->m_listen_socket );
		L2PNet_closesocket( pinfo->cls->m_listen_socket );
		pinfo->cls->m_listen_socket = INVALID_SOCKET;
	}
	if( pinfo->cls->m_hThread ) CloseHandle( pinfo->cls->m_hThread );
	pinfo->cls->m_hThread = NULL;
	pinfo->cls->m_signal = 0;
	free( pinfo );
	log_error( LOG_DEBUG, "FLS_Thread(): thread info struct freed\n" );
}

DWORD WINAPI LoginListener::FLS_Thread( LPVOID lpParam )
{
	struct FLLT_INFO *pinfo = (struct FLLT_INFO *)lpParam;
	lpParam = NULL;
	if( !pinfo )
	{
		log_error( LOG_ERROR, "FLS_Thread(): lpParam is NULL! Exit\n" );
		return 0;
	}
	class LoginListener *cls = pinfo->cls;
	if( !cls )
	{
		log_error( LOG_ERROR, "FLS_Thread(): pinfo->cls is NULL! Exit\n" );
		FLS_Thread_freeInfo( pinfo );
		return 0;
	}
	cls->m_listen_socket = INVALID_SOCKET;
	//DWORD wait_res = 0;
	//SOCKET s = sock_tcp_create();
	SOCKET s = L2PNet_TCPsocket_create( true );
	if( s == INVALID_SOCKET )
	{
		log_error( LOG_ERROR, "FLS_Thread(): socket create failed!\n" );
		FLS_Thread_freeInfo( pinfo );
		return 0;
	}

	int mb_yes = 0;
	int bind_fail = 0;
ll_bind:
	bind_fail = L2PNet_bind( s, g_cfg.FakeListenLoginIP, (unsigned short)g_cfg.FakeListenLoginPort );
	if( bind_fail )
	{
		log_error( LOG_ERROR, "FLS_Thread(): socket bind at %s:%d failed\n",
			g_cfg.FakeListenLoginIP, g_cfg.FakeListenLoginPort );
		int wsaerr = L2PNet_WSAGetLastError();
		if( wsaerr == WSAEADDRINUSE )
		{
			wchar_t errmsg[256];
			wsprintfW( errmsg, L"Порт %S:%d уже используется другим процессом!\n"
				L"Попробовать ещё раз?",
				g_cfg.FakeListenLoginIP, g_cfg.FakeListenLoginPort );
			mb_yes = MessageBoxW( g_radardll_hwnd, errmsg, L"L2Detect: Ошибка!", MB_ICONSTOP | MB_YESNO );
			if( mb_yes == IDYES ) goto ll_bind;
		}
		FLS_Thread_freeInfo( pinfo );
		return 0;
	}

	if( L2PNet_listen( s ) )
	{
		log_error( LOG_ERROR, "FLS_Thread(): listen() failed\n" );
		FLS_Thread_freeInfo( pinfo );
		return 0;
	}

	cls->m_listen_socket = s;

	log_error( LOG_DEBUG, "FLS_Thread(): started, listening at %s:%d\n",
		g_cfg.FakeListenLoginIP, g_cfg.FakeListenLoginPort );

	// raise thread priority
	if( !SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL ) )
		log_error( LOG_WARNING, "FLS_Thread(): raise thread priority failed!\n" );

	int r = 0;
	char clientIP[32] = {0};
	unsigned short clientPort = 0;
	SOCKET scl = INVALID_SOCKET;
	int rdyR = 0, rdyW = 0; // ready to recv/send
	unsigned int scode = 0; // signal to thread
	int should_exit = 0;

	while( 1 )
	{
		should_exit = 0;
		//r = sock_tcp_wait_ready_to_recv( s, 0, 500000 );
		rdyR = 0;
		r = L2PNet_select( s, L2PNET_SELECT_READ, 500, &rdyR, &rdyW );
		scode = cls->getLastSignal();
		if( scode == FLLS_STOP )
		{
			log_error( LOG_DEBUG, "FLS_Thread(): exit signal\n" );
			should_exit = 1;
		}
		if( should_exit ) break; // break while()
		if( r == -1 )
		{
			log_error( LOG_ERROR, "FLS_Thread(): socket select() error\n" );
			break;
		}
		if( r == 1 && rdyR )
		{
			//log_error( LOG_DEBUG, "FLS_Thread(): Somebody connected? :)\n" );
			clientIP[0] = 0;
			clientPort = 0;
			scl = INVALID_SOCKET;
			//scl = sock_tcp_accept( s, clientIP, &clientPort );
			scl = L2PNet_accept( s, clientIP, &clientPort );
			if( scl == INVALID_SOCKET )
			{
				log_error( LOG_ERROR, "FLS_Thread(): accept() failed...\n" );
			}
			else
			{
				//log_error( LOG_DEBUG, "FLS_Thread(): accepted client %s:%d\n", clientIP, clientPort );
				log_error( LOG_OK, "LoginListener: accepted client %s:%d\n", clientIP, clientPort );
				LoginClient lc;
				if( lc.ProcessClient( scl ) )
				{
					unsigned char sKey1[8] = {0,0,0,0, 0,0,0,0};
					unsigned char sKey2[8] = {0,0,0,0, 0,0,0,0};
					struct PlayServerInfo sinfo;
					lc.getSessionKeys( sKey1, sKey2 );
					lc.getPlayServerAddress( sinfo.ip, &(sinfo.port) );

					// ONLY IN OUTGAME MODE stop listener after successful client acception
					if( g_cfg.isInGameMode == false )
					{
						// stop listener after successful client acception
						log_error( LOG_DEBUG, "FLS_Thread(): LoginListener stopping self in outgame mode, "
							"after client accepted, freeing port %s:%d\n",
							g_cfg.FakeListenLoginIP, g_cfg.FakeListenLoginPort );
						//cls->signalStop();
						should_exit = 1;
					}

					if( !g_pgame )
					{
						log_error( LOG_ERROR, "FLS_Thread(): Game listener is NULL????" );
					}
					else
					{
						// set play server info to game listener
						g_pgame->setPlayServerInfo( &sinfo );
					}
				}
			}
		} // somebody connected?
		if( should_exit ) break; // break while()
	} // while(1)

	//sock_tcp_close_shutdown( s, SD_BOTH );
	// network sockets clenup done by FLS_Thread_freeInfo()

	FLS_Thread_freeInfo( pinfo );
	log_error( LOG_DEBUG, "FLS_Thread() ended\n" );
	return 0;
}


/// class


LoginListener::LoginListener()
{
	this->_initNull();
}

LoginListener::~LoginListener()
{
	if( this->m_hThread )      CloseHandle( this->m_hThread );
	this->_initNull();
}


bool LoginListener::start()
{
	// cleanup ?
	if( this->m_hThread )      CloseHandle( this->m_hThread );
	this->_initNull();
	// alloc thread info struct
	struct FLLT_INFO *pinfo = (struct FLLT_INFO *)malloc( sizeof( struct FLLT_INFO ) );
	if( !pinfo ) return false;
	// fill info
	pinfo->cls = this;
	// create thread object
	this->m_hThread = NULL;
	DWORD dwTID = 0;
	this->m_hThread = (HANDLE)_beginthreadex(
		NULL, // security
		0,    // stack size
		(unsigned int (__stdcall*)(void *))FLS_Thread, // thread function
		(void *)pinfo, // thread function argument - lpParam
		0,    // flags
		(unsigned int *)(&dwTID) );
	if( !this->m_hThread )
	{
		free( pinfo );
		return false;
	}
	return true;
}

bool LoginListener::waitStopped( unsigned int timeoutMsec )
{
	if( !this->m_hThread ) return true;
	DWORD wait_res = WaitForSingleObject( this->m_hThread, (DWORD)timeoutMsec );
	if( wait_res != WAIT_OBJECT_0 )
	{
		log_error( LOG_DEBUG, "LoginListener::waitStopped(): WaitForSingleObject() returned 0x%08X\n", wait_res );
		if( wait_res == WAIT_TIMEOUT ) log_error( LOG_DEBUG, "LoginListener::waitStopped(): WAIT_TIMEOUT\n" );
		return false;
	}
	// wait OK, close kernel objects
	if( this->m_hThread )      CloseHandle( this->m_hThread );
	// initialize self
	this->_initNull();
	return true;
}

void LoginListener::terminateThread()
{
	if( this->m_hThread )
	{
		TerminateThread( this->m_hThread, 0xBAADF00D );
		CloseHandle( this->m_hThread );
		this->m_hThread = NULL;
		this->m_signal = 0;
		// thread resources
		if( m_listen_socket != INVALID_SOCKET )
		{
			//sock_tcp_close_shutdown( m_listen_socket, SD_BOTH );
			L2PNet_shutdown( m_listen_socket );
			L2PNet_closesocket( m_listen_socket );
		}
		m_listen_socket = INVALID_SOCKET;
	}
}

void LoginListener::_initNull()
{
	m_hThread = NULL;
	m_signal = 0;
	// thread resources
	m_listen_socket = INVALID_SOCKET;
}

bool LoginListener::isRunning() const
{
	if( this->m_hThread != NULL ) return true;
	return false;
}

