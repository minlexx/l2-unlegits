#include "pch.h"
#include "Log.h"
#include "LoginServer.h"

LoginServer::LoginServer()
{
	m_config = NULL;
	m_mysql = NULL;
	m_mysql_server[0] = m_mysql_db[0] = 0;
	m_loginController = NULL;
	m_gameServerTable = NULL;
	//
	m_isRunning = false;
	m_hMainThread = NULL;
	m_dwMainThreadId = 0;
	m_mainThreadShouldStop = false;
}

LoginServer::~LoginServer()
{
	stop();
}

bool LoginServer::start()
{
	// already running?
	if( m_hMainThread ) return false;
	// createthread
	m_isRunning = false; // explicitly
	m_mainThreadShouldStop = false; // explicitly
	m_dwMainThreadId = 0;
	m_hMainThread = (HANDLE)_beginthreadex( NULL, 0,
		(unsigned int (__stdcall *)(void *))MainThread, (void *)this,
		0, (unsigned int *)&m_dwMainThreadId );
	if( !m_hMainThread )
	{
		LogError( L"LoginServer::start(): _beginthreadex() failed!" );
		return false;
	}
#ifdef _DEBUG
	LogDebug( L"LoginServer::start(): created thread %u", (unsigned int)m_dwMainThreadId );
#endif
	return true;
}

bool LoginServer::stop()
{
	// not running?
	if( !m_hMainThread ) return false;
	// signal thread to stop
	LogDebug( L"Stopping login server..." );
	Log_FlushFile();
	m_mainThreadShouldStop = true;
	// wait until thread is stopped
	do { Sleep( 200 ); } while( m_isRunning );
#ifdef _DEBUG
	LogDebug( L"LoginServer MainThread stopped." );
#endif
	// call cleanup
	resourceCleanup();
	return true;
}

void LoginServer::resourceCleanup()
{
	// cleanup
	if( m_hMainThread )
	{
		CloseHandle( m_hMainThread );
		m_hMainThread = NULL;
	}
	m_dwMainThreadId = 0;
	if( m_gameServerTable )
	{
		m_gameServerTable->cleanup();
		delete m_gameServerTable;
		m_gameServerTable = NULL;
	}
	if( m_loginController )
	{
		m_loginController->cleanup();
		delete m_loginController;
		m_loginController = NULL;
	}
	m_mysql_server[0] = 0;
	m_mysql_db[0] = 0;
	if( m_mysql )
	{
		delete m_mysql;
		m_mysql = NULL;
	}
	if( m_config )
	{
		delete m_config;
		m_config = NULL;
	}
#ifdef _DEBUG
	LogDebug( L"LoginServer cleanup complete." );
#endif
}

MysqlConnection *LoginServer::getDBConnection()
{
	MysqlConnection *ret = m_mysql->getConnection();
	if( ret ) return ret;
	LogError( L"LoginServer::getDBConnection(): not enough free connections, %d/%d busy",
		m_mysql->getCountActiveConnections(), m_mysql->getCountMaxConnections() );
	throw std::exception( "LoginServer::getDBConnection(): no free connections!" );
	//return NULL;
}

bool LoginServer::releaseDBConnection( MysqlConnection *con )
{
	if( !con ) return false;
	return m_mysql->releaseConnection( con );
}

LoginController *LoginServer::getLoginController()
{
	return m_loginController;
}

const LoginConfig *LoginServer::getConfig() const
{
	return m_config;
}

GameServerTable *LoginServer::getGameServerTable()
{
	return m_gameServerTable;
}

void LoginServer::LogFile( const char *fn, const char *_Format, ... )
{
	char ffn[256] = {0};
	sprintf( ffn, ".\\log\\login_%s.log", fn );
	FILE *f = fopen( ffn, "at" );
	if( !f ) return;
	SYSTEMTIME st;
	GetLocalTime( &st );
	fprintf( f, "%02d.%02d.%04d %02d:%02d:%03d: ",
		(int)st.wDay, (int)st.wMonth, (int)st.wYear, (int)st.wHour, (int)st.wMinute, (int)st.wMilliseconds );
	va_list _ArgList;
	va_start( _ArgList, _Format );
	vfprintf( f, _Format, _ArgList );
	fclose( f );
}

// returns true if ip is banned and closes sockets
bool LoginServer::checkIsBannedIp( unsigned int sock, const char *ip, int port )
{
	if( !ip ) return false;
	if( m_loginController->isIpBanned( ip ) )
	{
		LogWarning( L"Connected IP %S:%d is banned!", ip, port );
		L2PNet_shutdown( sock );
		L2PNet_closesocket( sock );
		return true;
	}
	return false;
}


unsigned int __stdcall LoginServer::MainThread( LoginServer *pcls )
{
	pcls->m_isRunning = true; // mark self as running
	DWORD startTick = GetTickCount();
	LogFull( false, true, RGB(0,200,0), RGB(255,255,255), L"===================================" );
	Log( L"Starting login server..." );
	//
	// ensure log dir exists
	if( CreateDirectory( TEXT(".\\log"), NULL ) ) Log( L"Created logs directory." );
	//
	// load config
	Log( L"Loading config..." );
	pcls->m_config = new LoginConfig();
	if( !pcls->m_config->load() ) LogError( L"Failed to load config!" );
	//else Log( L"Config loaded." );
	//
	//
	// initialize mysql conn manager
	pcls->m_mysql = new MysqlConnectionManager();
	// parse jdbc url... [jdbc:mysql://localhost/l2jdb]
	wchar_t delim[] = L":/";
	wchar_t *_url_copy = _wcsdup( pcls->m_config->URL );
	wchar_t *token = wcstok( _url_copy, delim );
	int i = 0;
	while( token )
	{
		if( i == 2 ) wcscpy( pcls->m_mysql_server, token );
		if( i == 3 ) wcscpy( pcls->m_mysql_db, token );
		i++;
		token = wcstok( NULL, delim );
	}
	free( _url_copy );
	_url_copy = NULL;
	// init
	Log( L"Initializing MySQL... server %s; db %s; maximum connections %d",
		pcls->m_mysql_server, pcls->m_mysql_db, pcls->m_config->MaximumDbConnections );
	if( !pcls->m_mysql->initialize( pcls->m_config->MaximumDbConnections,
		pcls->m_mysql_server, pcls->m_config->Login, pcls->m_config->Password, pcls->m_mysql_db ) )
		LogError( L"Cannot init mysql db!" );
	//else Log( L"MySQL init OK" );
	//
	//
	// init LoginController
	pcls->m_loginController = new LoginController( pcls );
	pcls->m_loginController->init();
	//
	// init game server table
	pcls->m_gameServerTable = new GameServerTable( pcls );
	pcls->m_gameServerTable->init();

	// create listening sockets
	SOCKET s_clients = L2PNet_TCPsocket_create( true );
	SOCKET s_servers = L2PNet_TCPsocket_create( true );
	// calc bind addresses
	char cl_bind_ip[16];
	char srv_bind_ip[16];
	strcpy( cl_bind_ip, "0.0.0.0" );
	strcpy( srv_bind_ip, "0.0.0.0" );
	if( wcscmp( pcls->m_config->LoginserverHostname, L"*" ) )
	{
		char aip[64] = {0};
		l2c_unicode_to_ansi( pcls->m_config->LoginserverHostname, aip, sizeof(aip) );
		in_addr inaddr;
		if( L2PNet_resolveHostname( aip, &inaddr ) )
		{
			strcpy( cl_bind_ip, L2PNet_inet_ntoa( inaddr ) );
			LogDebug( L"Login server will listen to clients on IP %S", cl_bind_ip );
		}
		else
		{
			LogError( L"Invalid hostname: %s, login server will bind client listener to all available IPs",
				pcls->m_config->LoginserverHostname );
		}
	}
	if( wcscmp( pcls->m_config->LoginHostname, L"*" ) )
	{
		char aip[64] = {0};
		l2c_unicode_to_ansi( pcls->m_config->LoginHostname, aip, sizeof(aip) );
		in_addr inaddr;
		if( L2PNet_resolveHostname( aip, &inaddr ) )
		{
			strcpy( srv_bind_ip, L2PNet_inet_ntoa( inaddr ) );
			LogDebug( L"Login server will listen to gameservers on IP %S", srv_bind_ip );
		}
		else
		{
			LogError( L"Invalid hostname: %s, login server will bind GS listener to all available IPs",
				pcls->m_config->LoginHostname );
		}
	}

	int r = 0;
	DWORD endTick = 0;
	int secs_load = 0;
	unsigned int socks_array[2] = { s_clients, s_servers };
	int bReadyRead[2] = {0,0}, bReadyWrite[2] = {0,0};

	try
	{
		// bind listening sockets
		// bind clients listener
		r = L2PNet_bind( s_clients, cl_bind_ip, (unsigned short)pcls->m_config->LoginserverPort );
		if( r != 0 ) throw std::exception( "Bind client listener error: L2 clients port busy?" );
		// bind servers listener
		r = L2PNet_bind( s_servers, srv_bind_ip, (unsigned short)pcls->m_config->LoginPort );
		if( r != 0 ) throw std::exception( "Bind GS servers listener error: GS listen port busy?" );
		// start listening
		// start client listener
		r = L2PNet_listen( s_clients );
		if( r != 0 ) throw std::exception( "Listen client listener error" );
		// start server listener
		r = L2PNet_listen( s_servers );
		if( r != 0 ) throw std::exception( "Listen GS servers listener error" );
		Log( L"Listening to gameservers on %s:%d", pcls->m_config->LoginHostname, pcls->m_config->LoginPort );
		Log( L"Login server ready on %s:%d", pcls->m_config->LoginserverHostname, pcls->m_config->LoginserverPort );

		// measure load time
		endTick = GetTickCount();
		secs_load = (endTick - startTick) / 1000;
		Log( L"Login server loaded in %d seconds (%d ms).", secs_load, (int)(endTick - startTick) );
		LogFull( false, true, RGB(0,200,0), RGB(255,255,255), L"===================================" );

		// run main loop
		while( pcls->m_mainThreadShouldStop == false )
		{
			bReadyRead[0] = bReadyRead[1] = 0;
			bReadyWrite[0] = bReadyWrite[1] = 0;
			socks_array[0] = s_clients;
			socks_array[1] = s_servers;
			r = L2PNet_select_multi( socks_array, 2, L2PNET_SELECT_READ, 200, bReadyRead, bReadyWrite );
			if( pcls->m_mainThreadShouldStop ) break;
			if( r == 0 ) continue;
			if( r == -1 ) throw std::exception( "L2PNet_select_multi(): return value -1!" );
			// accept login client?
			if( bReadyRead[0] )
			{
				char acceptedIP[16] = {0};
				unsigned short acceptedPort = 0;
				unsigned int accepted_client_socket = L2PNet_accept( s_clients, acceptedIP, &acceptedPort );
				if( accepted_client_socket != INVALID_SOCKET )
				{
					if( pcls->getConfig()->Debug )
						LogDebug( L"Accepted L2 client %S:%d... ", acceptedIP, (int)acceptedPort );
					if( !pcls->checkIsBannedIp( accepted_client_socket, acceptedIP, acceptedPort ) )
					{
						LoginClient *cl = new LoginClient( pcls, accepted_client_socket, acceptedIP, (int)acceptedPort );
						pcls->m_loginController->addLoginClient( cl );
					}
				}
			}
			// accept game server client?
			if( bReadyRead[1] )
			{
				char acceptedIP[16] = {0};
				unsigned short acceptedPort = 0;
				unsigned int accepted_socket = L2PNet_accept( s_servers, acceptedIP, &acceptedPort );
				if( pcls->getConfig()->Debug )
					LogDebug( L"Accepted gameserver %S:%d... ", acceptedIP, (int)acceptedPort );
				if( !pcls->checkIsBannedIp( accepted_socket, acceptedIP, acceptedPort ) )
				{
					pcls->m_gameServerTable->beginProcessGameServerClient( acceptedIP, accepted_socket );
				}
			}
		}
	}
	catch( std::exception& e )
	{
		LogError( L"LoginServer MainThread C++ std::exception: %S", e.what() );
		LogError( L"LoginServer will stop." );
	}

	// close sockets
	L2PNet_shutdown( s_clients );
	L2PNet_closesocket( s_clients );
	L2PNet_shutdown( s_servers );
	L2PNet_closesocket( s_servers );
	// cleanup self (mark as thread end)
	pcls->m_isRunning = false;
	LogDebug( L"Login server stopping." );
	return 0;
}
