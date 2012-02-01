#include "stdafx.h"
#include "GameListener.h"
#include "ConfigIni.h"
#include "Logger.h"
#include "GameClient.h"
#include "LoginListener.h"

extern class CConfig g_cfg;             // in main.cpp
extern class LoginListener *g_plogin;   // in main.cpp
extern class GameClient *g_game_client; // in main.cpp

struct FLGT_INFO
{
	class GameListener *cls;
};

void FGS_Thread_freeInfo( struct FLGT_INFO *pinfo )
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
	log_error( LOG_DEBUG, "FGS_Thread(): thread info struct freed\n" );
}

DWORD WINAPI FGS_Thread( LPVOID lpParam )
{
	struct FLGT_INFO *pinfo = (struct FLGT_INFO *)lpParam;
	lpParam = NULL;
	if( !pinfo )
	{
		log_error( LOG_ERROR, "FGS_Thread(): lpParam is NULL! Exit\n" );
		return 0;
	}
	class GameListener *cls = pinfo->cls;
	if( !cls )
	{
		log_error( LOG_ERROR, "FGS_Thread(): pinfo->cls is NULL! Exit\n" );
		FGS_Thread_freeInfo( pinfo );
		return 0;
	}
	cls->m_signal = 0;
	cls->m_listen_socket = INVALID_SOCKET;

	SOCKET s = L2PNet_TCPsocket_create( true );
	if( s == INVALID_SOCKET )
	{
		log_error( LOG_ERROR, "FGS_Thread(): socket create failed!\n" );
		FGS_Thread_freeInfo( pinfo );
		return 0;
	}

	int sbind_tries = 32;
	bool bind_OK = false;
	int FakeListenGamePort_prev = g_cfg.FakeListenGamePort;
	while( sbind_tries > 0 )
	{
		if( L2PNet_bind( s, g_cfg.FakeListenGameIP, (unsigned short)g_cfg.FakeListenGamePort ) )
		{
			g_cfg.FakeListenGamePort++;
			sbind_tries--;
			continue;
		}
		else
		{
			bind_OK = true;
			break;
		}
	}
	if( !bind_OK )
	{
		log_error( LOG_ERROR, "FGS_Thread(): ERROR: all 32 attempts of socket binds failed\n" );
		FGS_Thread_freeInfo( pinfo );
		return 0;
	}
	log_error( LOG_DEBUG, "FGS_Thread(): successfully bound GameListener to %s:%d\n",
		g_cfg.FakeListenGameIP, g_cfg.FakeListenGamePort );

	if( L2PNet_listen( s ) )
	{
		log_error( LOG_ERROR, "FGS_Thread(): listen() failed\n" );
		FGS_Thread_freeInfo( pinfo );
		return 0;
	}

	cls->m_listen_socket = s;

	log_error( LOG_DEBUG, "FGS_Thread(): started, listening at %s:%d\n",
		g_cfg.FakeListenGameIP, g_cfg.FakeListenGamePort );

	// raise thread priority if enabled
	if( g_cfg.ThreadProirityRaiseEnable )
	{
		log_error( LOG_DEBUG, "FGS_Thread(): raising thread priority\n" );
		if( !SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_HIGHEST ) )
			log_error( LOG_WARNING, "FGS_Thread(): raise thread priority failed!\n" );
	}

	ErrorLogger_FlushLogFile();

	bool processClient_result = false;
	int r = 0;
	char clientIP[32] = {0};
	unsigned short int clientPort = 0;
	SOCKET scl = INVALID_SOCKET;
	int should_exit = 0;
	unsigned int scode = 0; // signal to thread

	while( 1 )
	{
		r = L2PNet_select( s, L2PNET_SELECT_READ, 50, NULL, NULL );

		should_exit = 0;
		scode = cls->getLastSignal();
		if( scode == FLGS_STOP )
		{
			log_error( LOG_DEBUG, "FGS_Thread(): exit signal\n" );
			should_exit = 1;
			break;
		}
		if( should_exit == 1 ) break;

		if( r == -1 )
		{
			log_error( LOG_ERROR, "FGS_Thread(): socket select() error\n" );
			break;
		}
		if( r == 1 )
		{
			//log_error( LOG_DEBUG, "FGS_Thread: Somebody connected? :)\n" );
			clientIP[0] = 0;
			clientPort = 0;
			scl = INVALID_SOCKET;
			scl = L2PNet_accept( s, clientIP, &clientPort );
			if( scl == INVALID_SOCKET )
			{
				log_error( LOG_ERROR, "FGS_Thread(): ERROR: accept failed...\n" );
			}
			else // Game client connected
			{
				log_error( LOG_DEBUG, "FGS_Thread(): accepted client %s:%d\n",
					clientIP, clientPort );
				PlayServerInfo sinfo;
				memset( &sinfo, 0, sizeof(PlayServerInfo) );
				cls->getPlayServerInfo( &sinfo );
				should_exit = 0;
				while( (sinfo.ip[0] == 0) || (sinfo.port == 0 ) )
				{
					log_error( LOG_DEBUG, "FGS_Thread(): PlayServerInfo is meaningless, "
						"waiting for setting correct values...\n" );
					sinfo.ip[0] = 0; sinfo.port = 0;
					Sleep( 300 ); // wait 300 msec while threads switch :)
					cls->getPlayServerInfo( &sinfo );
					// enable stopping game server listener from this loop
					should_exit = 0;
					scode = cls->getLastSignal();
					if( scode == FLGS_STOP )
					{
						log_error( LOG_DEBUG, "FGS_Thread: exit signal\n" );
						should_exit = 1;
						break; // break from first loop
					}
				}
				if( should_exit ) break; // break from next (upper) loop

				// wait for LoginListener is stopped in outgame mode
				if( (g_cfg.isInGameMode == false) && g_plogin )
				{
#ifdef _DEBUG
					DWORD tc = GetTickCount();
					log_error( LOG_DEBUG, "GameListener: waiting for LoginListener to stop in OOG mode...\n" );
#endif
					g_plogin->waitStopped( 1000 );
#ifdef _DEBUG
					DWORD tc_passed = GetTickCount() - tc;
					log_error( LOG_DEBUG, "GameListener: wait for LoginListener to stop in OOG mode ended in %u msec\n",
						tc_passed );
#endif
				}
				
				// initialize gameClient UserAI
				g_game_client->ai.init();

				/* only sniffer mode? */
				if( g_cfg.EnableModifyGameTraffic == 0 )
				{
					//log_error( LOG_DEBUG, "FGS_Thread(): start ProcessClient, sniffing only\n" );
					log_error( LOG_OK, "GameListener: start processing client, sniffing only\n" );
					processClient_result = g_game_client->PC_sniff( scl, sinfo.ip, sinfo.port );
				}
				else
				{
					log_error( LOG_DEBUG, "FGS_Thread(): start ProcessClient, full processing mode\n" );
					processClient_result = g_game_client->PC_full( scl, sinfo.ip, sinfo.port );
				} // if( g_cfg.EnableModifyGameTraffic == 0 )

				// TODO: better error handling
				if( !processClient_result )
				{
					log_error( LOG_ERROR, "GameListener: FGS_Thread(): ProcessClient_xxxx() returned false, network error?\n" );
					ErrorLogger_FlushLogFile();
				}

				if( cls->getLastSignal() == FLGS_STOP ) break;

				// run login listener again in outgame mode
				//if( processClient_result && (g_cfg.isInGameMode == false) )
				if( g_cfg.isInGameMode == false )
				{
					if( g_plogin )
					{
						if( !g_game_client->wasJustServerPing() )
						{
							log_error( LOG_DEBUG, "GameListener: starting LoginListener again in outgame mode\n" );
							if( !g_plogin->start() )
								log_error( LOG_ERROR, "GameListener: failed starting LoginListener again in outgame mode!\n" );
						}
						else
						{
							if( g_plogin->isRunning() )
							{
								log_error( LOG_WARNING, "GameListener: GameClient ended, "
									"but it was just server ping - no start LoginListener!\n" );
							}
							else
							{
								log_error( LOG_WARNING, "GameListener: GameClient ended after client->server ping, "
									"but LoginListener is not running...? O_o\n" );
							}
						}
					}
				}

			} // accept() OK
		} // somebody connected?
	} // while(1)

	// restore prev. listen game port that was set in config after client processing ended
	g_cfg.FakeListenGamePort = FakeListenGamePort_prev;

	FGS_Thread_freeInfo( pinfo );
	log_error( LOG_DEBUG, "FGS_Thread() ended\n" );
	return 0;
}


/// class


GameListener::GameListener()
{
	this->_initNull();
}

GameListener::~GameListener()
{
	if( this->m_hThread )      CloseHandle( this->m_hThread );
	this->_initNull();
}


bool GameListener::start()
{
	// cleanup ?
	if( this->m_hThread )      CloseHandle( this->m_hThread );
	this->_initNull();
	// alloc thread info struct
	struct FLGT_INFO *pinfo = (struct FLGT_INFO *)malloc( sizeof( struct FLGT_INFO ) );
	if( !pinfo ) return false;
	// fill info
	pinfo->cls = this;

	// create thread object
	this->m_hThread = NULL;
	DWORD dwTID = 0;
	this->m_hThread = (HANDLE)_beginthreadex(
		NULL, // security
		0,    // stack size
		(unsigned int (__stdcall*)(void *))FGS_Thread, // thread function
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

bool GameListener::waitStopped( unsigned int timeoutMsec )
{
	if( !this->m_hThread ) return true;
	DWORD wait_res = WaitForSingleObject( this->m_hThread, (DWORD)timeoutMsec );
	if( wait_res != WAIT_OBJECT_0 )
	{
		log_error( LOG_DEBUG, "GameListener::waitStopped(): WaitForSingleObject() returned 0x%08X\n", wait_res );
		if( wait_res == WAIT_TIMEOUT ) log_error( LOG_DEBUG, "GameListener::waitStopped(): WAIT_TIMEOUT\n" );
		return false;
	}
	// wait OK, close kernel objects
	if( this->m_hThread )      CloseHandle( this->m_hThread );
	// initialize self
	this->_initNull();
	return true;
}

void GameListener::terminateThread()
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

bool GameListener::isRunning() const
{
	if( this->m_hThread != NULL ) return true;
	return false;
}

void GameListener::_initNull()
{
	m_hThread = NULL;
	m_signal = 0;
	m_sinfo.ip[0] = m_sinfo.ip[1] = m_sinfo.ip[2] = m_sinfo.ip[3] = 0;
	m_sinfo.port = 0;
	// thread resources
	m_listen_socket = INVALID_SOCKET;
}

void GameListener::setPlayServerInfo( struct PlayServerInfo *pinfo )
{
	if( !pinfo ) return;
	// copy struct :)
	memcpy( &(this->m_sinfo), pinfo, sizeof(struct PlayServerInfo) );
	// enable forcing game server IP:port Select
	if( g_cfg.ForceGameServerIP[0] && g_cfg.ForceGameServerPort )
	{
		in_addr force_addr;
		force_addr.s_addr = L2PNet_inet_addr( g_cfg.ForceGameServerIP );
		if( force_addr.s_addr == INADDR_NONE )
		{
			// try to resolve
			L2PNet_resolveHostname( g_cfg.ForceGameServerIP, &force_addr );
		}
		m_sinfo.ip[0] = force_addr.S_un.S_un_b.s_b1;
		m_sinfo.ip[1] = force_addr.S_un.S_un_b.s_b2;
		m_sinfo.ip[2] = force_addr.S_un.S_un_b.s_b3;
		m_sinfo.ip[3] = force_addr.S_un.S_un_b.s_b4;
		m_sinfo.port = g_cfg.ForceGameServerPort & 0x0000FFFF;
		log_error( LOG_WARNING, "GameListener: FORCED game server IP:port to %d.%d.%d.%d:%d\n",
			(int)m_sinfo.ip[0], (int)m_sinfo.ip[1], (int)m_sinfo.ip[2], (int)m_sinfo.ip[3],
			(int)m_sinfo.port );
	}
	//
	log_error( LOG_DEBUG,  "GameListener: set play server address: %d.%d.%d.%d:%d\n",
		(int)m_sinfo.ip[0], (int)m_sinfo.ip[1], (int)m_sinfo.ip[2], (int)m_sinfo.ip[3],
		(int)m_sinfo.port );
}

void GameListener::getPlayServerInfo( struct PlayServerInfo *pinfo ) const
{
	if( !pinfo ) return;
	memcpy( pinfo, &(this->m_sinfo), sizeof(struct PlayServerInfo) );
}
