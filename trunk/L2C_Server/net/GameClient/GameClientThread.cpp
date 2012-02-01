#include "pch.h"
#include "Log.h"
#include "l2c_utils.h"
#include "GameClient.h"
#include "GS.h"
#include "utils/Exception.h"

bool GameClient::startThread()
{
	if( m_isRunning ) return false;
	m_flagStop = false;
	unsigned int tid = 0;
	HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0,
		(unsigned int (__stdcall *)(void *))GameClient::ClientThread, (void *)this,
		0, &tid );
	bool ret = false;
	if( hThread != NULL )
	{
		ret = true;
		CloseHandle( hThread );
	}
	return ret;
}

bool GameClient::stopThread()
{
	if( !m_isRunning ) return false;
	m_flagStop = true;
	while( m_isRunning ) Sleep(50);
	m_flagStop = false;
	return true;
}

void GameClient::disconnectForceAndWaitThreadToStop( bool server_shutdown, bool reasonDualLogin )
{
	m_was_force_disconnected = true;
	// TODO: send SystemMessage about account in use if dual login
	if( reasonDualLogin )
	{
	}
	// TODO: send ServerClose / ExRestartClient? if server shutdown
	if( server_shutdown )
	{
	}
	stopThread(); // stops thread; thread cleanups socket himself
}

void GameClient::signalThreadStop( bool setMarkToDeleteSelfAndCleanupClientPool )
{
	m_flagStop = true;
	m_was_force_disconnected = !setMarkToDeleteSelfAndCleanupClientPool;
}


DWORD WINAPI GameClient::ClientThread( LPVOID lpvParam )
{
	GameClient *pl = (GameClient *)lpvParam;
	pl->m_was_force_disconnected = false;
	GameServer *gs = GameServer::getInstance();
	pl->m_isRunning = true;
	pl->m_state = CLIENT_STATE_CONNECTED;
	pl->m_netStats.clear(); // reset nework statistics

	int r, bReadyRead, bReadyWrite;
	unsigned char *packbuffer = NULL;
	unsigned int rcvdLen = 0;
	L2GamePacket *pack = NULL;

	try
	{
		// allocate packets recv buffer
		packbuffer = (unsigned char *)malloc( 65536 );
		if( !packbuffer ) throw std::bad_alloc( "allocate packbuffer (64 Kb) failed" );

		// packets recv loop
		while( !pl->m_flagStop )
		{
			// wait for socket ready to receive data
			r = L2PNet_select( pl->m_sock, L2PNET_SELECT_READ, 100, &bReadyRead, &bReadyWrite );
			if( r == 0 )
			{
				// select timout
				// update world state? move moving objects?...
				continue;
			}
			if( r == -1 ) // select error
			{
				LogError( L"%s: L2PNet_select() error!", pl->toString() );
				throw Exception( "select error" );
			}

			// recv packet
			r = L2PacketReceive_buffer( pl->m_sock, 1000, &rcvdLen, packbuffer );
			if( r <= 0 )
			{
				if( r == 0 )
				{
					LogWarning( L"%s: closed connection? (received %u, retval %d)", pl->toString(), rcvdLen, r );
					pl->m_flagStop = true;
					break;
				}
				if( r < 0 )
				{
					LogError( L"%s: failed to receive packet (received %u, retval %d)", pl->toString(), rcvdLen, r );
					throw Exception( "network packet receive error" );
				}
			}

			// count stats
			pl->m_netStats.addRcvdPacket( rcvdLen );

			// create packet object
			pack = new L2GamePacket( packbuffer, rcvdLen );

			// decode XOR if enabled
			if( pl->m_xor_enabled ) pack->decodeXOR( pl->m_xor_key_recv );

			// deobfuscate opcode if enabled
			if( pl->m_opcodeObfuscationSeed > 0 )
			{
				// HACK: here buffer that should be constant, is modified (but length is not changed thoug)
				pl->m_obf->decodeIDs( (unsigned char *)pack->getBytesPtr() );
			}

#ifdef _DEBUG
			int opcode = pack->getByteAt(2);
			LogDebug( L"%s: opcode %02X", pl->toString(), opcode );
#endif

			// handle game client packet...
			pl->m_ph->handlePacket( pack );

			// free packet object
			delete pack; pack = NULL;
		}
	}
	catch( L2P_Exception& e )
	{
		LogError( L"ClientThread: %s: L2Packets exception: %S", pl->toString(), e.what() );
	}
	catch( std::bad_alloc& e )
	{
		LogError( L"ClientThread: %s: bad_alloc %S", pl->toString(), e.what() );
	}
	catch( std::exception& e )
	{
		LogError( L"ClientThread: %s: std::excpt %S", pl->toString(), e.what() );
	}
	catch( Exception& e )
	{
		LogError( L"ClientThread: Exception: %S", e.what() );
		e.logStackTrace();
	}

	// player disconnected - remove player account from login server list of authed accounts
	if( pl->m_account[0] ) // has authed??
		gs->logoutAccountFromLoginServer( pl->m_account );

	// cleanup socket
	L2PNet_shutdown( pl->m_sock );
	L2PNet_closesocket( pl->m_sock );
	pl->m_sock = INVALID_SOCKET;
	pl->m_state = CLIENT_STATE_OFFLINE;
	// cleanup opcode obfuscator
	if( pl->m_obf )
	{
		delete pl->m_obf;
		pl->m_obf = NULL;
		pl->m_opcodeObfuscationSeed = 0;
	}

	// free packets buffer
	if( packbuffer ) free( packbuffer ); packbuffer = NULL;

	// mark self as finished
	pl->m_isRunning = false;

	// notify GS ClientPool about client stop, if needed
	if( !pl->m_was_force_disconnected )
	{
#ifdef _DEBUG
		LogDebug( L"ClientThread s: Calling ClientPool to cleanup & deleting self", pl->toString() );
#endif
		gs->getClientPool()->notifyClientThreadEnded( pl );
		delete pl;
	}
	return 0;
}
