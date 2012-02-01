#include "pch.h"
#include "Log.h"
#include "LoginController.h"
#include "LoginServer.h"


LoginController::LoginClientOnGS::LoginClientOnGS( const char *account, unsigned char *sessionKey1, unsigned char *sessionKey2 )
{
	strcpy( m_account, account );
	memcpy( m_sessionKey1, sessionKey1, 8 );
	memcpy( m_sessionKey2, sessionKey2, 8 );
}

/*bool LoginController::LoginClientOnGS::areSessionKeysEqual( unsigned char *sessionKey1, unsigned char *sessionKey2 )
{
	bool key1_ok = (memcmp( sessionKey1, m_sessionKey1, 8 ) == 0);
	bool key2_ok = (memcmp( sessionKey2, m_sessionKey2, 8 ) == 0);
	return key1_ok && key2_ok;
}*/

void LoginController::LoginClientOnGS::getSessionKeys( unsigned char *sessionKey1, unsigned char *sessionKey2 )
{
	memcpy( sessionKey1, m_sessionKey1, 8 );
	memcpy( sessionKey2, m_sessionKey2, 8 );
}



LoginController::LoginController( LoginServer *pServer )
{
	InitializeCriticalSectionAndSpinCount( &m_lock, 10 );
	m_loginServer = pServer;
	int i;
	for( i=0; i<MAX_RSA_KEYS; i++ ) m_rsa_keyPairs[i] = NULL;
	//for( i=0; i<MAX_BLOWFISH_KEYS; i++ ) m_bf_keys[i][0] = NULL; // not needed really
	m_clients.clear();
	m_clientsOnGS.clear();
}

LoginController::~LoginController()
{
	cleanup();
	DeleteCriticalSection( &m_lock );
}

void LoginController::cleanup()
{
	closeAllClients( true );
	// remove all clietns on gs
	LoginClientOnGSList::iterator iter;
	int n = 0;
	for( iter = m_clientsOnGS.begin(); iter != m_clientsOnGS.end(); iter++ )
	{
		LoginClientOnGS *ptr = (*iter);
		if( ptr ) { delete ptr; n++; }
	}
	m_clientsOnGS.clear();
	if( n>0 ) LogDebug( L"Deleted %d \"clients on GS\"", n );
	// clear hack protection/IP bans
	m_hackProtection.clear();
	m_bannedIps.clear();
	//
	freeRSAKeyPairs();
	freeBFKeys();
}

bool LoginController::init()
{
	Log( L"Loading LoginController..." );
	generateRSAKeyPairs();
	generateBFKeys();
	loadBannedIps();
	return true;
}

void LoginController::Lock()
{
	EnterCriticalSection( &m_lock );
}

void LoginController::Unlock()
{
	LeaveCriticalSection( &m_lock );
}


void LoginController::generateRSAKeyPairs()
{
	wchar_t mes[256];
	wsprintfW( mes, L"Generating %d RSA keys for communication...  [", MAX_RSA_KEYS );
	LogFull( true, false, RGB(0,0,0), RGB(255,255,255), mes );
	int i;
	for( i=0; i<MAX_RSA_KEYS; i++ )
	{
		m_rsa_keyPairs[i] = RSA_generate_key( RSA_NUM_BITS, RSA_PUBLIC_EXPONENT, NULL, NULL );
		Log_np( L"|" );
		if( m_rsa_keyPairs[i] == NULL )
		{
			Log_np( L"\n" );
			LogError( L"ERROR: RSA_generate_key() returned NULL!\n" );
			//ERR_print_errors_fp( stderr );
		}
	}
	Log_np( L"]   OK\n" );
}

void LoginController::freeRSAKeyPairs()
{
	int i;
	for( i=0; i<MAX_RSA_KEYS; i++ )
	{
		if( m_rsa_keyPairs[i] ) RSA_free( m_rsa_keyPairs[i] );
		m_rsa_keyPairs[i] = NULL;
	}
}

RSA *LoginController::getRandomRSAKeyPair()
{
	srand( (unsigned)time(NULL) );
	int idx = rand() % MAX_RSA_KEYS;
	RSA *ret_rsa_key = RSAPrivateKey_dup( m_rsa_keyPairs[idx] );
	//RSA *ret_rsa_key = RSAPublicKey_dup( m_rsa_keyPairs[idx] ); // cannot be used for decryption :)
	return ret_rsa_key;
}

const unsigned char *LoginController::getRandomBlowfishKey()
{
	srand( (unsigned)time(NULL) );
	int idx = rand() % MAX_BLOWFISH_KEYS;
	return (const unsigned char *)m_bf_keys[ idx ];
}

void LoginController::generateBFKeys()
{
	int i = 0, j = 0;
	for( i=0; i<MAX_BLOWFISH_KEYS; i++ )
	{
		for( j=0; j<16; j++ ) m_bf_keys[i][j] = (unsigned char)( (rand() % 256) & 0xFF );
	}
	Log( L"Generated %d blowfish keys.", MAX_BLOWFISH_KEYS );
}

void LoginController::freeBFKeys()
{
	// nothing
}

void LoginController::loadBannedIps()
{
	FILE *f = fopen( ".\\config_login\\banned_ip.cfg", "rt" );
	if( !f )
	{
		LogWarning( L"Failed to load banned IPs config. (config_login\\banned_ip.cfg)" );
		return;
	}
	int nLoaded = 0;
	char line[1024];
	while( !feof( f ) )
	{
		line[0] = 0;
		fgets( line, sizeof(line)-1, f );
		if( line[0] == 0 ) continue;
		if( line[0] == '#' ) continue;
		//
		char delim[] = " \r\n\t";
		char *token1 = strtok( line, delim );
		char *token2 = strtok( NULL, delim );
		//
		if( !token1 ) continue;
		//
		time_t ban_expire = 0;
		char sban_expire[64];
		strcpy( sban_expire, "NEVER" );
		if( token2 )
		{
			sscanf( token2, "%I64d", &ban_expire );
			struct tm *gt = localtime( &ban_expire );
			sprintf( sban_expire, "%s", asctime( gt ) );
		}
		//
		LogDebug( L"IP banned: %S; expires: %S", token1, sban_expire );
		//
		std::string sip = token1;
		BanInfo b( token1, ban_expire );
		m_bannedIps[sip] = b;
		nLoaded++;
	}
	fclose( f );
	Log( L"Loaded %d IP bans.", nLoaded );
}

void LoginController::closeAllClients( bool force )
{
	Lock();
	LoginClientList::iterator iter = m_clients.begin();
	while( iter != m_clients.end() )
	{
		LoginClient *cl = (*iter);
		cl->disconnect( force );
		iter++;
	}
	m_clients.clear();
	Unlock();
}

bool LoginController::addLoginClient( LoginClient *cl )
{
	Lock();
	m_clients.push_back( cl );
	Unlock();
	cl->beginProcessing();
	return true;
}

bool LoginController::removeClient( LoginClient *cl )
{
	bool ret = false;
	Lock();
	{
		LoginClientList::iterator iter = m_clients.begin();
		while( iter != m_clients.end() )
		{
			LoginClient *vcl = (*iter);
			if( vcl == cl )
			{
				if( getConfig()->Debug ) LogDebug( L"LoginController: removing client %s", cl->toString() );
				m_clients.erase( iter );
				ret = true;
				break;
			}
			iter++;
		}
	}
	Unlock();
	if( !ret ) LogError( L"LoginController: cannot find client %s to remove", cl->toString() );
	return false;
}

const LoginConfig *LoginController::getConfig() const
{
	if( m_loginServer ) return m_loginServer->getConfig();
	throw std::exception( "LoginController::getConfig(): m_loginServer null pointer" );
}


LoginController::AuthLoginResult LoginController::tryAuthLogin(
	const char *account, const char *password, LoginClient *client ) throw(HackingException)
{
	AuthLoginResult ret = LoginController::INVALID_PASSWORD;
	if( isLoginValid( account, password, client ) )
	{
		// login was successful, verify presence on Gameservers
		ret = LoginController::ALREADY_ON_GS;
		if( !m_loginServer->getGameServerTable()->isAccountInAnyGameServer( account ) )
		{
			// account isnt on any GS verify LS itself
			ret = LoginController::ALREADY_ON_LS;
			
			// dont allow 2 simultaneous login
			Lock();
			{
				bool alreadyOnLs = false;
				LoginClientList::const_iterator iter = m_clients.begin();
				while( iter != m_clients.end() )
				{
					LoginClient *cl = (*iter);
					if( strcmp( cl->getAccount(), account ) == 0 )
					{
						alreadyOnLs = true;
						break;
					}
					iter++;
				}

				// change return value and set account to client, while this code section is locked
				if( !alreadyOnLs )
				{
					ret = LoginController::AUTH_SUCCESS;
					client->setAccount( account );
				}
			}
			Unlock();
		}
	}
	else
	{
		if( client->getAccessLevel() < 0 ) ret = LoginController::ACCOUNT_BANNED;
	}
	return ret;
}

bool LoginController::isLoginValid( const char *user, const char *password, LoginClient *client )
{
	bool ok = false;
	// log it anyway
	if( getConfig()->LogLoginController )
		m_loginServer->LogFile( "logins_try", "'%s' %s\n", user, client->getIpAddress() );

	MysqlConnection *con = NULL;
	con = m_loginServer->getDBConnection();

	try
	{
		unsigned char hash[128];
		char hash_base64[128];
		memset( hash, 0, sizeof(hash) );
		memset( hash_base64, 0, sizeof(hash_base64) );
		SHA1( (const unsigned char *)password, strlen(password), hash ); // SHA1 or SHA?
		base64_encode_string( (const char *)hash, 20, hash_base64 );
		
		char expected[256] = {0};
		int access = 0;
		int lastServer = 1;
		
		MysqlQuery q;
		char *e_user = con->escapeStringMalloc( user );
		q.create( L"SELECT password, accessLevel, lastServer FROM accounts WHERE login='%S'", e_user );
		free( e_user );
		if( !con->executeQuery( q ) ) throw std::exception( "execute query (check login)" );
		if( q.getNextRow() )
		{
			// account exists
			memset( expected, 0, sizeof(expected) );
			strcpy( expected, q.getFieldStr( "password" ) );
			access = q.getFieldInt( "accessLevel" );
			lastServer = q.getFieldInt( "lastServer" );
			if( lastServer <= 0 ) lastServer = 1; // minServerId is 1 in Interlude
			if( getConfig()->Debug ) LogDebug( L"LoginController: account %S exists", user );
		}
		else // if account doesnt exist
		{
			LogWarning( L"LoginController: account missing for user %S", user );
			if( getConfig()->AutoCreateAccounts )
			{
				size_t usrlen = strlen( user );
				if( (usrlen >= 2) && (usrlen <= 14) )
				{
					MysqlQuery q2;
					char *e_user = con->escapeStringMalloc( user );
					long long int t = time(NULL) * 1000;
					q2.create( L"INSERT INTO accounts (login, password, lastactive, accessLevel, lastIP) "
						L"VALUES ('%S','%S', '%I64d', '0', '%S')", e_user, hash_base64, t, client->getIpAddress() );
					free( e_user );
					if( con->executeQuery( q2 ) ) Log( L"Created new account for %S", user );
					else throw std::exception( "create account" );
					return true;
				}
				LogWarning( L"Invalid username creation/use attempt: %S (%S)", user, client->getIpAddress() );
				return false;
			}
			return false;
		}
		
		// is this account banned?
		if( access < 0 )
		{
			client->setAccessLevel( access );
			return false;
		}
		
		// check password hash
		ok = true;
		int i;
		for( i = 0; i < (int)strlen(expected); i++ )
		{
			if( hash_base64[i] != expected[i] )
			{
				ok = false;
				break;
			}
		}
		
		// update last access time & ip
		if( ok )
		{
			client->setAccessLevel( access );
			client->setLastServer( lastServer );
			char *e_user = con->escapeStringMalloc( user );
			long long int t = time(NULL) * 1000;
			q.create( L"UPDATE accounts SET lastactive='%I64d', lastIP='%S' WHERE login='%S'",
				t, client->getIpAddress(), user );
			free( e_user );
			if( !con->executeQuery( q ) ) throw std::exception( "update last access time & ip" );
		}
	}
	catch( std::exception e )
	{
		LogWarning( L"LoginController: Client %s: Could not check password: %S", client->toString(), e.what() );
		LogWarning( L"last MySQL error: %s", con->getErrorStr() );
		ok = false;
	}
	
	m_loginServer->releaseDBConnection( con );
	
	if( !ok )
	{
		LogWarning( L"Failed auth: %S (%S)", user, client->getIpAddress() );
		if( getConfig()->LogLoginController )
			m_loginServer->LogFile( "logins_ip_fails", "'%s' %s\n", user, client->getIpAddress() );
		
		std::string clientAddress( client->getIpAddress() );
		FailedLoginAttempt *failedAttempt = getFailedLoginAttemptForAddress( clientAddress );
		int failedCount;
		if( failedAttempt == NULL )
		{
			FailedLoginAttempt new_attempt( password );
			m_hackProtection[clientAddress] = new_attempt;
			failedCount = 1;
		}
		else
		{
			failedAttempt->increaseCounter( password );
			failedCount = failedAttempt->getCount();
		}
		
		if( failedCount >= getConfig()->LoginTryBeforeBan )
		{
			LogWarning( L"Banning '%S' (acc %S) for %d seconds due to %d invalid user/pass attempts",
				client->getIpAddress(), user, getConfig()->LoginBlockAfterBan, failedCount );
			addBanForAddress( client->getIpAddress(), getConfig()->LoginBlockAfterBan );
		}
	}
	else
	{
		std::string clientAddress( client->getIpAddress() );
		removeHackProtectionForAddress( clientAddress );
		if( getConfig()->LogLoginController )
			m_loginServer->LogFile( "logins_ok", "'%s' %s\n", user, client->getIpAddress() );
	}

	return ok;
}


bool LoginController::getSessionKeysForAccount( const char *account, unsigned char *player_loginKey, unsigned char *player_playKey )
{
	bool ret = false;
	Lock();
	{
		LoginClientOnGSList::const_iterator iter = m_clientsOnGS.begin();
		while( iter != m_clientsOnGS.end() )
		{
			LoginClientOnGS *clientInfo = (*iter);
			if( clientInfo )
			{
				if( strcmp( account, clientInfo->getAccount() ) == 0 )
				{
					clientInfo->getSessionKeys( player_loginKey, player_playKey );
					ret = true;
					break;
				}
			}
			iter++;
		}
	}
	Unlock();
	return ret;
}

void LoginController::addClientOnGS( const char *account, unsigned char *player_loginKey, unsigned char *player_playKey )
{
	LoginClientOnGS *c = new LoginClientOnGS( account, player_loginKey, player_playKey );
	Lock();
	m_clientsOnGS.push_back( c );
	Unlock();
#ifdef _DEBUG
	LogDebug( L"LoginController::addClientOnGS(): added account %S", account );
#endif
}

void LoginController::removeAuthedGSClient( const char *account )
{
	if( !account ) return;
	Lock();
	LoginClientOnGSList::iterator iter;
	for( iter = m_clientsOnGS.begin(); iter != m_clientsOnGS.end(); iter++ )
	{
		LoginClientOnGS *cl = (*iter);
		if( cl )
		{
			if( strcmp( cl->getAccount(), account ) == 0 )
			{
				m_clientsOnGS.erase( iter );
#ifdef _DEBUG
				LogDebug( L"LoginController::removeAuthedGSClient(): removed account %S", account );
#endif
				break;
			}
		}
	}
	Unlock();
}

void LoginController::setAccountAccessLevel( const wchar_t *account, int level )
{
	MysqlQuery q;
	q.create( L"UPDATE accounts SET accessLevel=%d WHERE login='%s'", level, account );
	MysqlConnection *con = m_loginServer->getDBConnection();
	if( !con->executeQuery( q ) )
	{
		LogError( L"Cannot update account %s accesslevel to %d: %s",
			account, level, con->getErrorStr() );
	}
	m_loginServer->releaseDBConnection( con );
}

FailedLoginAttempt *LoginController::getFailedLoginAttemptForAddress( std::string& address )
{
	HackProtection::iterator iter = m_hackProtection.find( address );
	if( iter == m_hackProtection.end() ) return NULL;
	return &(iter->second);
}

void LoginController::removeHackProtectionForAddress( std::string& address )
{
	HackProtection::iterator iter = m_hackProtection.find( address );
	if( iter == m_hackProtection.end() ) return;
	m_hackProtection.erase( iter );
}

void LoginController::addBanForAddress( const char *ip, time_t seconds )
{
	if( !ip ) return;
	std::string sip = ip;
	BannedIpList::const_iterator iter = m_bannedIps.find( sip );
	if( iter == m_bannedIps.end() )
	{
		// calc expiration time as now time + ban seconds
		time_t expirationTime = time(NULL) + (time_t)seconds;
		BanInfo b( ip, expirationTime );
		m_bannedIps[sip] = b;
	}
}

void LoginController::removeBanForAddress( const char *ip )
{
	if( !ip ) return;
	std::string sip = ip;
	BannedIpList::const_iterator iter = m_bannedIps.find( sip );
	if( iter != m_bannedIps.end() ) m_bannedIps.erase( iter );
}

bool LoginController::isIpBanned( const char *ip )
{
	if( !ip ) return false;
	in_addr addr;
	addr.s_addr = L2PNet_inet_addr( ip );
	//
	char cc[32] = {0};
	std::string sip = ip;
	BannedIpList::const_iterator iter = m_bannedIps.find( sip );
	if( iter == m_bannedIps.end() )
	{
		// try to find ip "%d.%d.%d.0"
		sprintf( cc, "%d.%d.%d.0", (int)addr.S_un.S_un_b.s_b1, (int)addr.S_un.S_un_b.s_b2, (int)addr.S_un.S_un_b.s_b3 );
		sip.assign( cc );
		iter = m_bannedIps.find( sip );
	}
	if( iter == m_bannedIps.end() )
	{
		// try to find ip "%d.%d.0.0"
		sprintf( cc, "%d.%d.0.0", (int)addr.S_un.S_un_b.s_b1, (int)addr.S_un.S_un_b.s_b2 );
		sip.assign( cc );
		iter = m_bannedIps.find( sip );
	}
	if( iter == m_bannedIps.end() )
	{
		// try to find ip "%d.0.0.0"
		sprintf( cc, "%d.0.0.0", (int)addr.S_un.S_un_b.s_b1 );
		sip.assign( cc );
		iter = m_bannedIps.find( sip );
	}
	// still not found? not banned :)
	if( iter == m_bannedIps.end() ) return false;
	// found, but maybe ban expired?
	if( iter->second.hasExpired() )
	{
		LogWarning( L"LoginController: ban for ip %S expired.", sip.c_str() );
		m_bannedIps.erase( iter );
		return false;
	}
	// not expired, still banned
	return true;
}
