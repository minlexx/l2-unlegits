#include "pch.h"
#include "Log.h"
#include "GS.h"
#include "utils/Debugging.h"
#include "utils/Exception.h"
#include "world/model/base/ClassIdTree.h"
#include "datatables/CharNameTable.h"
#include "datatables/CharTemplateTable.h"
#include "datatables/ItemTable.h"

// init class static vars
GameServer *GameServer::_self           = NULL;
int         GameServer::_referenceCount = 0;

GameServer *GameServer::getInstance()
{
	if( !_self )
	{
		_self = new GameServer();
		_referenceCount++;
	}
	return _self;
}

void GameServer::freeInstance()
{
	if( _self )
	{
		_referenceCount--;
		if( _referenceCount == 0 )
		{
			delete _self;
			_self = NULL;
		}
	}
}

GameServer::GameServer()
{
	m_serverStatus = SERVER_STATUS_DOWN;
	m_cfg = new GameConfig();
	m_mysql = new MysqlConnectionManager();
	m_loginConnection = new LoginConnection();
	m_isRunning = m_flagStop = false;
	m_clientPool = new ClientPool();
	m_idf = new IdFactory();
}

GameServer::~GameServer()
{
	stop();
	if( m_cfg ) { delete m_cfg; m_cfg = NULL; }
	if( m_mysql ) { delete m_mysql; m_mysql = NULL; }
	if( m_loginConnection ) { delete m_loginConnection; m_loginConnection = NULL; }
	if( m_clientPool ) { delete m_clientPool; m_clientPool = NULL; }
	if( m_idf ) { delete m_idf; m_idf = NULL; }
}

bool GameServer::start()
{
	if( m_isRunning ) return false;
	HANDLE hThread = NULL;
	unsigned int threadId = 0;
	m_gsLock.Lock();
	{
		m_flagStop = false;
		m_isRunning = false;
		hThread = (HANDLE)_beginthreadex( NULL, 0,
			(unsigned int (__stdcall *)(void *))(GameServer::GS_Thread), (void *)this,
			0, &threadId );
		if( hThread )
		{
			m_isRunning = true;
			CloseHandle( hThread );
		}
		else
		{
			m_isRunning = false;
		}
	}
	m_gsLock.Unlock();
	return (hThread != NULL);
}

bool GameServer::stop()
{
	if( !m_isRunning ) return false;
	m_gsLock.Lock();
	{
		this->m_flagStop = true;
		while( this->m_isRunning ) Sleep( 20 );
		this->m_flagStop = false;
	}
	m_gsLock.Unlock();
	return true;
}

bool GameServer::isRunning() const
{
	return m_isRunning;
}

GameConfig *GameServer::getConfig() const
{
	return m_cfg;
}

MysqlConnection *GameServer::getDBConnection()
{
	MysqlConnection *ret = m_mysql->getConnection();
	if( ret ) return ret;
	LogError( L"GameServer::getDBConnection(): not enough free connections, %d/%d busy",
		m_mysql->getCountActiveConnections(), m_mysql->getCountMaxConnections() );
	throw Exception( "GameServer::getDBConnection(): no free connections!" );
}

bool GameServer::releaseDBConnection( MysqlConnection *con )
{
	if( !con ) return false;
	return m_mysql->releaseConnection( con );
}

void GameServer::LogFile( const char *fn, const char *_Format, ... )
{
	char ffn[256] = {0};
	sprintf( ffn, ".\\log\\game_%s.log", fn );
	FILE *f = fopen( ffn, "at" );
	if( !f ) return;
	SYSTEMTIME st;
	GetLocalTime( &st );
	fprintf( f, "%02d.%02d.%04d %02d:%02d:%03d: ", (int)st.wDay, (int)st.wMonth, (int)st.wYear,
		(int)st.wHour, (int)st.wMinute, (int)st.wMilliseconds );
	va_list _ArgList;
	va_start( _ArgList, _Format );
	vfprintf( f, _Format, _ArgList );
	fclose( f );
}

const wchar_t *GameServer::getServerName() const
{
	if( !m_loginConnection ) return L"NULL";
	return m_loginConnection->getRegisteredServerName();
}

int GameServer::getServerId() const
{
	if( !m_loginConnection ) return 0;
	return m_loginConnection->getRegisteredServerId();
}

/* Server will be considered as Good when the number of  online players
 * is less than half the maximum. as Normal between half and 4/5
 * and Full when there's more than 4/5 of the maximum number of players */
ServerStatus GameServer::getServerStatus()
{
	// TODO: ServerStatus may also be GM_ONLY
	// SERVER_STATUS_DOWN is set when server is shut down
	if( m_serverStatus != SERVER_STATUS_DOWN )
	{
		int curOnline = getCurrentOnline();
		int maxOnline = getMaxPlayers();
		float ratio = 100.0f * ((float)curOnline) / ((float)maxOnline);
		if( ratio <= 0.5f ) m_serverStatus = SERVER_STATUS_GOOD;
		if( ratio > 0.5f && ratio <= 0.8f ) m_serverStatus = SERVER_STATUS_NORMAL;
		if( ratio > 0.8f ) m_serverStatus = SERVER_STATUS_FULL;
	}
	return m_serverStatus;
}

int GameServer::getMaxPlayers() const
{
	return m_cfg->max_players;
}

int GameServer::getCurrentOnline()
{
	if( !m_clientPool ) return 0;
	return m_clientPool->getCurrentOnline();
}

bool GameServer::checkFreePort( const wchar_t *bind_addr, int bind_port )
{
	char a_addr[128] = {0};
	l2c_unicode_to_ansi( bind_addr, a_addr, 128 );
	unsigned short s_port = (unsigned short)( bind_port & 0xFFFF );
	SOCKET s = L2PNet_TCPsocket_create( true );
	if( s == INVALID_SOCKET ) return false;
	int r = L2PNet_bind( s, a_addr, s_port );
	L2PNet_shutdown( s );
	L2PNet_closesocket( s );
	return (r==0);
}

ClientPool *GameServer::getClientPool() const
{
	return m_clientPool;
}

IdFactory *GameServer::getIdFactory()
{
	return m_idf;
}

bool GameServer::kickPlayerByAccount( const wchar_t *accountName, bool reasonDualLogin )
{
	if( !accountName ) return false;
	if( !m_clientPool ) return false;
	bool ret = m_clientPool->disconnectPlayerByAccount( accountName, reasonDualLogin );
	return ret;
}


bool GameServer::logoutAccountFromLoginServer( const wchar_t *accountName )
{
	if( !m_loginConnection || !accountName ) return false;
	m_loginConnection->send_PlayerLogOut( accountName );
	return true;
}

void GameServer::addWaitingClientAndSendPlayerAuthRequest( const wchar_t *accountName, unsigned char *loginKey, unsigned char *playKey )
{
	if( !m_loginConnection ) return;
	m_loginConnection->send_PlayerAuthRequest( accountName, loginKey, playKey );
}

void GameServer::notifyLoginPlayerAuth( const wchar_t *account, int ok )
{
	if( !m_clientPool ) return;
	m_clientPool->notifyClientAuthResponse( account, ok ? true : false );
}


DWORD WINAPI GameServer::GS_Thread( LPVOID lpvParam )
{
	GameServer *gs = (GameServer *)lpvParam;
	// remember load time start
	DWORD startTick = GetTickCount();
	DWORD endTick = 0;
	int secs_load = 0;
	LogFull( false, true, RGB(0,200,0), RGB(255,255,255), L"===================================" );
	Log( L"Starting game server..." );

	SOCKET gs_acceptor = INVALID_SOCKET;

	try
	{

	///////////////////////////////////
	// begin server initialization
	///////////////////////////////////
	// ensure log dir exists
	if( CreateDirectory( TEXT(".\\log"), NULL ) ) Log( L"Created logs directory." );
	// Load config
	if( !gs->m_cfg->load() ) throw Exception( "GS_Thread: error loading config!" );
	LogDebug( L"Config loaded." );

	// check binding port is free
	if( !checkFreePort( gs->m_cfg->game_server_bind_address, gs->m_cfg->game_server_bind_port ) )
	{
		LogError( L"Error: bind address %s:%d already in use (another GS is running?)",
			gs->m_cfg->game_server_bind_address, gs->m_cfg->game_server_bind_port );
		throw Exception( "Check free bind port failed" );
	}

	// Initialize MySQL
	if( !gs->m_mysql->initialize( gs->m_cfg->mysql_max_connections,	gs->m_cfg->mysql_host,
		gs->m_cfg->mysql_user, gs->m_cfg->mysql_pass, gs->m_cfg->mysql_db ) )
		throw Exception( "MySQL init error!" );
	MysqlConnection *test_con = gs->m_mysql->getConnection();
	if( !test_con ) throw Exception( "MySQL init error (no free connections???)" );
	if( !test_con->isConnected() )
	{
		char comment[256] = {0};
		sprintf( comment, "MySQL test connection error: %S", test_con->getErrorStr() );
		gs->m_mysql->releaseConnection( test_con );
		throw Exception( comment );
	}
	LogDebug( L"MySQL connection test OK" );

	// init IdFactory
	gs->m_idf->init();
	Log( L"IdFactory: used %u object IDs, %I64u bytes", gs->m_idf->getUsedCount(), gs->m_idf->getUsedMemoryBytes() );

	// init & load datatables
	ItemTable::getInstance()->load();
	ClassIdTree::getInstance();
	CharTemplateTable::getInstance()->load();
	CharNameTable::getInstance();

	// TODO: load skills...
	// TODO: load scripts/quests... TODO

	// create listening socket
	gs_acceptor = L2PNet_TCPsocket_create( true );
	char a_bind_ip[128] = {0};
	l2c_unicode_to_ansi( gs->m_cfg->game_server_bind_address, a_bind_ip, 128 );
	if( L2PNet_bind( gs_acceptor, a_bind_ip, (unsigned short)gs->m_cfg->game_server_bind_port ) != 0 )
	{
		LogError( L"GS acceptor bind error at %s:%d!", gs->m_cfg->game_server_bind_address,
			gs->m_cfg->game_server_bind_port );
		throw Exception( "GS acceptor bind error!" );
	}
	L2PNet_listen( gs_acceptor );

	// measure load time
	endTick = GetTickCount();
	secs_load = (endTick - startTick) / 1000;
	Log( L"Game server loaded in %d seconds (%d ms).", secs_load, (int)(endTick - startTick) );
	LogFull( false, true, RGB(0,200,0), RGB(255,255,255), L"===================================" );

	// mark server as running
	gs->m_serverStatus = SERVER_STATUS_AUTO;

	// Initiaite login server connection thread
	gs->m_loginConnection->start( gs->m_cfg->login_server_address,
		gs->m_cfg->login_server_auth_port, gs->m_cfg->login_protocol_version );

	////////////////////////////////////////////
	// initiate main server loop
	////////////////////////////////////////////
	while( !gs->m_flagStop )
	{
		int r = 0, bReadyRead = 0, bReadyWrite = 0;
		r = L2PNet_select( gs_acceptor, L2PNET_SELECT_READ, 200, &bReadyRead, &bReadyWrite );
		if( r == 0 ) continue; // timeout
		if( r == -1 ) // error
			throw Exception( "GS acceptor select() error!" );
		// accepted client?
		if( bReadyRead )
		{
			char clientIp[32] = {0};
			unsigned short clientPort = 0;
			SOCKET s_cl = L2PNet_accept( gs_acceptor, clientIp, &clientPort );
			if( s_cl != INVALID_SOCKET )
			{
				LogDebug( L"Accepted client %S:%d", clientIp, (int)clientPort );
				if( !gs->m_clientPool->addClient( s_cl, clientIp, (int)clientPort ) )
				{
					LogError( L"Failed to add client %S:%d to clients list!", clientIp, (int)clientPort );
				}
			}
			else LogWarning( L"gs acceptor: invalid_socket accepted :(" );
		}
	}
	///////////////////////////////////////////
	// end main server loop
	///////////////////////////////////////////
	}
	catch( std::exception& e )
	{
		LogError( L"GS_Thread: std::exception: %S\n", e.what() );
	}
	catch( Exception& e )
	{
		LogError( L"GS_Thread: Exception: %S", e.what() );
		e.logStackTrace();
	}

	////////////////////////////////////////////
	// initiate shutdown procedures
	////////////////////////////////////////////
	// notify login server that server will now be offline
	gs->m_serverStatus = SERVER_STATUS_DOWN;
	gs->m_loginConnection->send_ServerStatus();

	// disconnect all players
	gs->m_clientPool->disconnectAllPlayers();

	// TODO: save all data
	// close GS acceptor socket
	if( gs_acceptor != INVALID_SOCKET )
	{
		L2PNet_shutdown( gs_acceptor );
		L2PNet_closesocket( gs_acceptor );
		gs_acceptor = INVALID_SOCKET;
	}

	// cleanup datatables
	CharNameTable::freeInstance();
	CharTemplateTable::freeInstance();
	ClassIdTree::freeInstance();
	ItemTable::freeInstance();

	// close id factory
	gs->m_idf->clear();

	// stop login conn thread
	gs->m_loginConnection->stop();

	// mark as not running
	gs->m_isRunning = false;
	return 0;
}
