#include "pch.h"
#include "Log.h"
#include "GameServerTable.h"
#include "LoginServer.h"

GameServerTable::GameServerTable( LoginServer *pLogin )
{
	m_loginServer = pLogin;
	m_serverNames.clear();
	m_gameServerTable.clear();
	int i;
	for( i=0; i<MAX_RSA_KEYS; i++ ) m_rsa_keys[i] = NULL;
	InitializeCriticalSectionAndSpinCount( &m_lock, 10 );
}

GameServerTable::~GameServerTable()
{
	cleanup();
	DeleteCriticalSection( &m_lock );
}

void GameServerTable::cleanup()
{
	// delete() every gameserver info
	std::map<int, GameServerInfo *>::iterator iter = m_gameServerTable.begin();
	while( iter != m_gameServerTable.end() )
	{
		GameServerInfo *gsi = iter->second;
		if( gsi )
		{
			gsi->stopThread();
			delete gsi;
			iter->second = NULL;
		}
		iter++;
	}
	//
	m_serverNames.clear();
	m_gameServerTable.clear();
	int i;
	for( i=0; i<MAX_RSA_KEYS; i++ )
	{
		if( m_rsa_keys[i] ) RSA_free( m_rsa_keys[i] );
		m_rsa_keys[i] = NULL;
	}
}

const LoginConfig *GameServerTable::getConfig() const
{
	return m_loginServer->getConfig();
}

bool GameServerTable::init()
{
	Log( L"Loading GameServerTable..." );

	loadServerNames();
	Log( L"Loaded %d server names.", m_serverNames.size() );

	loadRegisteredGameServers();
	Log( L"Loaded %d registered game servers.", m_gameServerTable.size() );

	loadRSAKeys();
	return true;
}

RSA *GameServerTable::getRandomRSAKey()
{
	srand( (unsigned int)time(NULL) );
	int idx = rand() % MAX_RSA_KEYS;
	RSA *key_ret = RSAPrivateKey_dup( m_rsa_keys[idx] );
	return key_ret;
}

void GameServerTable::loadServerNames()
{
	FILE *f = _wfopen( L".\\config_login\\servername.xml", L"rt" );
	if( !f )
	{
		LogError( L"GameServerTable: cannot open file 'config_login\\servername.xml'!" );
		return;
	}
	//
	wchar_t line[1024];
	while( !feof( f ) )
	{
		line[0] = 0;
		fgetws( line, 1023, f );
		if( line[0] == 0 ) continue;
		if( wcsstr( line, L"<server id=\"" ) && wcsstr( line, L"name=\"" ) )
		{
			wchar_t *pServerId = wcsstr( line, L" id=\"" );
			pServerId += 5; // wcslen( L" id=\"" );
			int iServerId = 0;
			swscanf( pServerId, L"%d", &iServerId );
			wchar_t *pServerName = wcsstr( line, L"name=\"" );
			wchar_t wServerName[256];
			wchar_t *pName = wServerName;
			pServerName += 6; // wcslen( L"name=\"" );
			while( (*pServerName) != L'\"' ) *pName++ = *pServerName++;
			*pName = 0;
			// add
			m_serverNames[iServerId] = wServerName;
		}
	}
	//
	fclose( f );
}

void GameServerTable::loadRSAKeys()
{
	wchar_t mes[256];
	wsprintfW( mes, L"Generating %d RSA keys for GS communication...  [", MAX_RSA_KEYS );
	LogFull( true, false, RGB(0,0,0), RGB(255,255,255), mes );
	int i;
	for( i=0; i<MAX_RSA_KEYS; i++ )
	{
		m_rsa_keys[i] = RSA_generate_key( RSA_NUM_BITS, RSA_PUBLIC_EXPONENT, NULL, NULL );
		Log_np( L"|" );
		if( m_rsa_keys[i] == NULL )
		{
			Log_np( L"\n" );
			LogError( L"ERROR: RSA_generate_key() returned NULL!\n" );
			//ERR_print_errors_fp( stderr );
		}
	}
	Log_np( L"]   OK\n" );
}

bool GameServerTable::getServerNameById( int id, std::wstring& name )
{
	std::map<int, std::wstring>::const_iterator iter = m_serverNames.find( id );
	if( iter == m_serverNames.end() )
	{
		name = L"<unknown>";
		return false;
	}
	name = iter->second;
	return true;
}

void GameServerTable::loadRegisteredGameServers()
{
	MysqlConnection *con = m_loginServer->getDBConnection();
	if( !con )
	{
		LogError( L"GameServerTable: cannot load reg. game servers (no db connection)." );
		return;
	}

	MysqlQuery q;
	q.create( L"SELECT server_id, hexid FROM gameservers" );

	if( con->executeQuery( q ) )
	{
		while( q.getNextRow() )
		{
			int sid = q.getFieldInt( "server_id" );
			char *shexid = q.getFieldStr( "hexid" );
			GameServerInfo *srv_info = new GameServerInfo( sid, shexid );
			// save
			m_gameServerTable[sid] = srv_info;
		}
	}
	else
	{
		LogError( L"GameServerTable::loadRegisteredGameServers(): Mysql error: %s", con->getErrorStr() );
	}

	m_loginServer->releaseDBConnection( con );
}

GameServerInfo *GameServerTable::getRegisteredGameServerById( int id )
{
	std::map<int, GameServerInfo *>::const_iterator iter = m_gameServerTable.find( id );
	if( iter == m_gameServerTable.end() ) return NULL;
	return iter->second;
}

bool GameServerTable::hasRegisteredGameServerOnId( int id )
{
	std::map<int, GameServerInfo *>::const_iterator iter = m_gameServerTable.find( id );
	if( iter == m_gameServerTable.end() ) return false;
	return true;
}

bool GameServerTable::registerServer( int id, GameServerInfo *gsinfo )
{
	if( !hasRegisteredGameServerOnId( id ) )
	{
		m_gameServerTable[id] = gsinfo;
		return true;
	}
	return false;
}

bool GameServerTable::registerServerInDb( GameServerInfo *gsinfo )
{
	return this->registerServerInDb( gsinfo->getId(), gsinfo->getHexId(), gsinfo->getExternalHost() );
}

bool GameServerTable::registerServerInDb( int id, const char *hexid, const char *externalHost )
{
	MysqlConnection *con = m_loginServer->getDBConnection();
	if( !con ) return false;
	char *escape_hexId = con->escapeStringMalloc( hexid );
	char *escape_hn = con->escapeStringMalloc( externalHost );
	MysqlQuery q;
	q.create( L"INSERT INTO gameservers (hexid, server_id, host) VALUES ('%s','%s','%s')",
		escape_hexId, id, escape_hn );
	free( escape_hexId );
	free( escape_hn );
	if( !con->executeQuery( q ) )
		LogError( L"GameServerTable: failed to register game server in DB: %s", con->getErrorStr() );
	else
		Log( L"GameServerTable: registered new GameServer id %d", id );
	m_loginServer->releaseDBConnection( con );
	return true;
}

bool GameServerTable::registerWithFirstAvaliableId( GameServerInfo *gsinfo )
{
	if( !gsinfo ) return false;
	bool ret = false;
	Lock();
	{
		int i;
		for( i=0; i<(int)m_serverNames.size(); i++ )
		{
			if( !hasRegisteredGameServerOnId( i ) )
			{
				ret = registerServer( i, gsinfo );
				break;
			}
		}
	}
	Unlock();
	return ret;
}


const std::map<int, GameServerInfo *>& GameServerTable::getRegisteredGameServers() const
{
	return this->m_gameServerTable;
}

bool GameServerTable::isAccountInAnyGameServer( const char *account )
{
	if( !account ) return false;
	std::map<int, GameServerInfo *>::const_iterator iter = m_gameServerTable.begin();
	while( iter != m_gameServerTable.end() )
	{
		if( iter->second->hasAccountOnGS( account ) )
		{
			if( getConfig()->Debug )
				LogDebug( L"Account %S is already on GS #%d", account, iter->first );
			return true;
		}
		iter++;
	}
	return false;
}

GameServerInfo *GameServerTable::getGameServerThreadForAccount( const char *account )
{
	if( !account ) return false;
	std::map<int, GameServerInfo *>::const_iterator iter = m_gameServerTable.begin();
	while( iter != m_gameServerTable.end() )
	{
		if( iter->second->hasAccountOnGS( account ) )
		{
			return iter->second;
		}
		iter++;
	}
	//throw std::exception( "GameServerTable::getGameServerThreadForAccount(): returning NULL, while shouldn't" );
	return NULL;
}

bool GameServerTable::isLoginPossible( int serverId, int accesslevel, const char *account, int lastServer )
{
	GameServerInfo *gsi = getRegisteredGameServerById( serverId );
	if( !gsi ) return false;
	if( !gsi->isAuthed() ) return false;
	// check GM only
	if( gsi->getStatus() == GameServerInfo::STATUS_GM_ONLY )
	{
		if( accesslevel <= 0 ) return false;
	}
	// check max players
	if( gsi->getPlayerCount() >= gsi->getMaxPlayers() ) return false;
	// login is possible... update last server in DB if needed
	if( serverId != lastServer )
	{
		MysqlConnection *con = m_loginServer->getDBConnection();
		MysqlQuery q;
		char *e_acc = con->escapeStringMalloc( account );
		q.create( L"UPDATE accounts SET lastServer=%d WHERE login='%S'", serverId, e_acc );
		free( e_acc );
		if( !con->executeQuery( q ) )
			LogError( L"Error updating accounts (lastServer): %s", con->getErrorStr() );
		m_loginServer->releaseDBConnection( con );
	}
	return true;
}

void GameServerTable::Lock()
{
	EnterCriticalSection( &m_lock );
}

void GameServerTable::Unlock()
{
	LeaveCriticalSection( &m_lock );
}



#include "GS_ConnectionInfo.h"

void GameServerTable::beginProcessGameServerClient( const char *ip, unsigned int sock )
{
	GS_ConnectionInfo *pinfo = new GS_ConnectionInfo();
	pinfo->sock = sock;
	strcpy( pinfo->ip, ip );
	pinfo->gst = this;
	pinfo->flagBreak = false;
	pinfo->bfKeyLen = 0;
	pinfo->bfKey[0] = 0;
	pinfo->rsaKey = NULL;
	pinfo->gsi = NULL;
	unsigned int tid = 0;
	HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0,
		(unsigned int (__stdcall *)(void *))GSConnThread, (void *)pinfo, 0, &tid );
	if( hThread ) CloseHandle( hThread );
}

DWORD WINAPI GameServerTable::GSConnThread( LPVOID lpParam )
{
	GS_ConnectionInfo *pinfo = (GS_ConnectionInfo *)lpParam;

	pinfo->rsaKey = pinfo->gst->getRandomRSAKey();
	unsigned char rsa_modulus[64];
	BN_bn2bin( pinfo->rsaKey->n, rsa_modulus );

	char initial_bfKey[] = "_;v.]05-31!|+-%xT!^[$\00";
	memcpy( pinfo->bfKey, initial_bfKey, 23 );
	pinfo->bfKey[22] = '0';
	pinfo->bfKeyLen = 22;

	// send InitLS to server
	L2LoginPacket *pack = new L2LoginPacket();
	pack->setPacketType( 0x00 ); // InitLS
	pack->writeUInt( LoginServer::LOGIN_PROTOCOL_REV ); // login protocol rev
	pack->writeUInt( 64+1 ); // why +1?
	pack->writeC( 0x00 ); // ?? wtf
	pack->writeBytes( rsa_modulus, 64 );
	// pad
	pack->writeUShort( 0x00 );
	sendGSPacket( pinfo, pack );
	delete pack;
	pack = NULL;
#ifdef _DEBUG
	LogDebug( L"GSConnThread: sent InitLS" );
#endif

	unsigned int rcvdLen = 0;
	int r;
	unsigned char *packbuffer = (unsigned char *)malloc( 16384 ); // 16 kb
	while( !pinfo->flagBreak )
	{
		// first run select()
		int bReadyRead = 0;
		r = L2PNet_select( pinfo->sock, L2PNET_SELECT_READ, 100, &bReadyRead, NULL );
		if( r == 0 || !bReadyRead ) continue; // select() timeout, nothing was received
		if( r < 0 ) // select() error
		{
			LogError( L"L2PNet_select() returned %d!", r );
			pinfo->flagBreak = true;
			break;
		}
		if( pinfo->flagBreak ) break;
		// select OK, now recv
		r = L2PacketReceive_buffer( pinfo->sock, 5000, &rcvdLen, packbuffer );
		if( r <= 0 )
		{
			LogError( L"L2PacketReceive_buffer() returned %d!", r );
			pinfo->flagBreak = true;
			break;
		}
		if( pinfo->flagBreak ) break;
		// create packet
		pack = new L2LoginPacket( packbuffer, rcvdLen );
		// decrypt blowfish
		pack->setDynamicBFKey( pinfo->bfKey, pinfo->bfKeyLen );
		pack->decodeBlowfish( false );
		// verify received packet checksum?
		if( !pack->verifyChecksum() )
		{
			LogError( L"Incorrect checksum from GameServer %S. Close connection", pinfo->ip );
			forceClose( pinfo, NOT_AUTHED );
			continue;
		}
		//
		unsigned char opcode = pack->getByteAt( 2 );
		//
#ifdef _DEBUG
		LogDebug( L"GSConnThread: opcode %02X", (unsigned int)opcode );
#endif
		//
		switch( opcode )
		{
			case 00:
				ph_ReceiveBlowfishKey( pinfo, pack );
				break;
			case 01:
				ph_ReceiveGameServerAuth( pinfo, pack );
				break;
			case 02:
				ph_ReceivePlayerInGame( pinfo, pack );
				break;
			case 03:
				ph_ReceivePlayerLogOut( pinfo, pack );
				break;
			case 04:
				ph_ReceiveChangeAccessLevel( pinfo, pack );
				break;
			case 05:
				ph_ReceivePlayerAuthRequest( pinfo, pack );
				break;
			case 06:
				ph_ReceiveServerStatus( pinfo, pack );
				break;
			default:
				LogWarning( L"Unknown Opcode (0x%02X) from GameServer, closing connection.", (unsigned int)opcode );
				forceClose( pinfo, NOT_AUTHED );
				pinfo->flagBreak = true; // close con
				break;
		}
	}
	free( packbuffer );

	if( pinfo->gsi )
	{
		// log
		std::wstring wsname;
		pinfo->gst->getServerNameById( pinfo->gsi->getId(), wsname );
		Log( L"Connection with gameserver %d (%s) lost: set as disconnected",
			pinfo->gsi->getId(), wsname.c_str() );
		// set as disconnected
		pinfo->gsi->setDown();
	}

	L2PNet_shutdown( pinfo->sock );
	L2PNet_closesocket( pinfo->sock );
	delete pinfo;
	return 0;
}





void GameServerTable::ph_ReceiveBlowfishKey( GS_ConnectionInfo *pinfo, L2LoginPacket *pack )
{
	pack->getPacketType();
	unsigned char encoded_block[128];
	unsigned char decoded_block[128];
	memset( encoded_block, 0, sizeof(encoded_block) );
	memset( decoded_block, 0, sizeof(decoded_block) );
	// read size of RSA-encrypted block with BF key
	unsigned int block_size = pack->readUInt();
	if( block_size > 128 )
	{
		LogError( L"GS: protocol error (block_size)" );
		forceClose( pinfo, NOT_AUTHED );
		return;
	}
	// read block
	pack->readBytes( encoded_block, block_size );
	// decrypt RSA
	int r = RSA_private_decrypt( block_size, encoded_block, decoded_block, pinfo->rsaKey, RSA_NO_PADDING );
	if( r == -1 )
	{
		LogError( L"GameServerTable::ph_ReceiveBlowfishKey(): RSA_private_decrypt() error!" );
		forceClose( pinfo, NOT_AUTHED );
		return;
	}
	// there are nulls before the key we must remove them
	unsigned char new_bf_key[128];
	memset( new_bf_key, 0, sizeof(new_bf_key) );
	unsigned int new_bf_key_len = 0;
	unsigned int zero_count = 0;
	while( (decoded_block[zero_count] == 0) && (zero_count < block_size) ) zero_count++;
	new_bf_key_len = block_size - zero_count;
	if( new_bf_key_len == 0 )
	{
		LogError( L"GS: protocol error (new_bf_key_len)" );
		forceClose( pinfo, NOT_AUTHED );
		return;
	}
	// set new key
	memcpy( pinfo->bfKey, decoded_block + zero_count, new_bf_key_len );
	pinfo->bfKeyLen = new_bf_key_len;
	//
	if( pinfo->gst->getConfig()->Debug ) LogDebug( L"GS: new BF key rcvd (len %u)", new_bf_key_len );
}

void GameServerTable::ph_ReceiveGameServerAuth( GS_ConnectionInfo *pinfo, L2LoginPacket *pack )
{
	unsigned char hexid[32];
	char s_hexid[64];
	memset( hexid, 0, sizeof(hexid) );
	memset( s_hexid, 0, sizeof(s_hexid) );
	//
	pack->getPacketType();
	unsigned char desiredId = pack->readUChar();
	bool acceptAlternativeId = (pack->readC() == 0 ? false : true);
	bool hostReserved = (pack->readC() == 0 ? false : true);
	wchar_t *externalHost = pack->readS();
	wchar_t *internalHost = pack->readS();
	int port = (int)pack->readH();
	int maxPlayers = pack->readD();
	int hexid_size = pack->readD();
	if( hexid_size > 32 ) hexid_size = 32;
	pack->readBytes( hexid, hexid_size );
	int i;
	for( i=0; i<hexid_size; i++ )
	{
		sprintf( s_hexid + i*2, "%02x", hexid[i] );
	}
	if( hostReserved ) LogWarning( L"GS %d auth: host is reserved. WTF is this?", (int)desiredId );

	if( pinfo->gst->getConfig()->Debug )
		LogDebug( L"Auth request for GS #%d (%S)", (int)desiredId, s_hexid );
	// -3c9043fed79c281b9c9daaeb027bd8f0
	//Returns a byte array containing the two's-complement representation of this BigInteger.
	//The byte array will be in big-endian byte-order: the most significant byte is in the zeroth
	//element. The array will contain the minimum number of bytes required to represent this BigInteger,
	//including at least one sign bit, which is (ceil((this.bitLength() + 1)/8)).
	//(This representation is compatible with the (byte[]) constructor.) 
	// ... big-endian byte-order: the most significant byte is in the zeroth element. 

	// handle auth process
	bool auth_success = false;
	GameServerTable *gst = pinfo->gst;
	GameServerInfo *gsi = gst->getRegisteredGameServerById( desiredId );
	// this id is registered?..
	if( gsi == NULL ) // ...no
	{
		// can we register new GS on this id?
		if( gst->getConfig()->AcceptNewGameServer )
		{
			gsi = new GameServerInfo( desiredId, s_hexid );
			if( gst->registerServer( desiredId, gsi ) )
			{
				// successfully registered new game server
				attachGameServerInfo( pinfo, gsi, internalHost, externalHost, port, maxPlayers );
				gst->registerServerInDb( gsi );
				auth_success = true;
			}
			else // some one took this ID meanwhile
			{
				delete gsi; gsi = NULL;
				forceClose( pinfo, REASON_ID_RESERVED );
			}
		}
		else // cannot register new GS
		{
			forceClose( pinfo, REASON_WRONG_HEXID );
		}
	}
	else
	{
		if( strcmp( gsi->getHexId(), s_hexid ) == 0 )
		{
			gst->Lock();
			{
				if( gsi->isAuthed() )
				{
					forceClose( pinfo, REASON_ALREADY_LOGGED_IN );
				}
				else
				{
					attachGameServerInfo( pinfo, gsi, internalHost, externalHost, port, maxPlayers );
					auth_success = true;
				}
			}
			gst->Unlock();
		}
		else
		{
			// there is already a server registered with the desired id and different hex id
			// try to register this one with an alternative id
			if( gst->getConfig()->AcceptNewGameServer && acceptAlternativeId )
			{
				gsi = new GameServerInfo( desiredId, s_hexid );
				if( gst->registerWithFirstAvaliableId( gsi ) )
				{
					attachGameServerInfo( pinfo, gsi, internalHost, externalHost, port, maxPlayers );
					gst->registerServerInDb( gsi );
					auth_success = true;
				}
				else // failed to register?
				{
					delete gsi; gsi = NULL;
					forceClose( pinfo, REASON_NO_FREE_ID );
				}
			}
			else // server id is already taken, and we cant get a new one for you
			{
				forceClose( pinfo, REASON_WRONG_HEXID );
			}
		}
	}

	free( internalHost );
	free( externalHost );

	// authorization failed for some reason?
	if( !auth_success ) return;

	std::wstring wserverName;
	gst->getServerNameById( (int)desiredId, wserverName );

	Log( L"Updated gameserver %d %s IPs:", (int)desiredId, wserverName.c_str() );
	Log( L"Internal IP: %S;   External IP: %S", gsi->getInternalIp(), gsi->getExternalIp() );

	// send AuthResponse
	L2LoginPacket p_ar;
	p_ar.setPacketType( 0x02 );
	p_ar.writeUChar( desiredId );
	p_ar.writeS( wserverName.c_str() );
	sendGSPacket( pinfo, &p_ar );
}

void GameServerTable::ph_ReceivePlayerInGame( GS_ConnectionInfo *pinfo, L2LoginPacket *pack )
{
	pack->getPacketType();
	int acc_count = pack->readH();
	int i;
	for( i=0; i<acc_count; i++ )
	{
		const wchar_t *acc = pack->readUnicodeStringPtr();
		if( !acc ) continue;
		char s_acc[256];
		memset( s_acc, 0, sizeof(s_acc) );
		l2c_unicode_to_ansi( acc, s_acc, 256 );
		// add account to game server list of accounts
		pinfo->gsi->addAccountOnGS( s_acc );
		if( pinfo->gst->getConfig()->Debug )
			LogDebug( L"Account %s logged into gameserver %d", acc, pinfo->gsi->getId() );
	}
}

void GameServerTable::ph_ReceivePlayerLogOut( GS_ConnectionInfo *pinfo, L2LoginPacket *pack )
{
	pack->getPacketType();
	const wchar_t *acc = pack->readUnicodeStringPtr();
	if( !acc ) return;
	char s_acc[256];
	memset( s_acc, 0, sizeof(s_acc) );
	l2c_unicode_to_ansi( acc, s_acc, 256 );
	// remove account from gs list of accounts
	pinfo->gsi->removeAccountFromGS( s_acc );
	if( pinfo->gst->getConfig()->Debug )
		LogDebug( L"Account %s logged out from gameserver %d", acc, pinfo->gsi->getId() );
}

void GameServerTable::ph_ReceiveChangeAccessLevel( GS_ConnectionInfo *pinfo, L2LoginPacket *pack )
{
	pack->getPacketType();
	int acc_level = pack->readD();
	const wchar_t *account = pack->readUnicodeStringPtr();
	//
	pinfo->gst->m_loginServer->getLoginController()->setAccountAccessLevel( account, acc_level );
	if( pinfo->gst->getConfig()->Debug )
		LogDebug( L"Changed %s access level to %d", account, acc_level );
}

void GameServerTable::ph_ReceivePlayerAuthRequest( GS_ConnectionInfo *pinfo, L2LoginPacket *pack )
{
	pack->getPacketType();
	const wchar_t *waccount = pack->readUnicodeStringPtr();
	if( !waccount )
	{
		LogError( L"GS: ReceivePlayerAuthRequest: account NULL?" );
		return;
	}
	// convert to ansi
	char account[256] = {0};
	memset( account, 0, sizeof(account) );
	l2c_unicode_to_ansi( waccount, account, 256 );
	// read keys
	unsigned char playKey[8];
	unsigned char loginKey[8];
	pack->readBytes( playKey, 8 );
	pack->readBytes( loginKey, 8 );
	//
	// get keys for account
	unsigned char player_playKey[8];
	unsigned char player_loginKey[8];
	bool ok = false;
	if( pinfo->gst->m_loginServer->getLoginController()->getSessionKeysForAccount( account, player_loginKey, player_playKey ) )
	{
		bool key1_ok = (memcmp( loginKey, player_loginKey, 8 ) == 0);
		bool key2_ok = (memcmp( playKey,  player_playKey, 8 ) == 0);
		ok = key1_ok && key2_ok;
	}
	else LogError( L"getSessionKeysForAccount( %s ) failed!", account );
	if( ok )
	{
		pinfo->gst->m_loginServer->getLoginController()->removeAuthedGSClient( account );
		if( pinfo->gst->getConfig()->Debug )
			LogDebug( L"PlayerAuthRequest for account %s OK", waccount );
		GameServerTable::send_PlayerAuthResponse( pinfo, waccount, true );
	}
	else
	{
		if( pinfo->gst->getConfig()->Debug )
			LogWarning( L"PlayerAuthRequest for account %s failed!", waccount );
		GameServerTable::send_PlayerAuthResponse( pinfo, waccount, false );
	}
}

void GameServerTable::ph_ReceiveServerStatus( GS_ConnectionInfo *pinfo, L2LoginPacket *pack )
{
	if( pinfo->gsi && pinfo->gsi->isAuthed() )
	{
		if( pinfo->gst->getConfig()->Debug )
			LogDebug( L"ServerStatus received (id %d)", pinfo->gsi->getId() );
		// parse ServerStatus
		pack->getPacketType();
		int paramCount = pack->readD();
		int i;
		for( i=0; i<paramCount; i++ )
		{
			int valueType  = pack->readD();
			int valueValue = pack->readD();
			switch( valueType )
			{
			case 0x01: // SERVER_LIST_STATUS
				pinfo->gsi->setStatus( (GameServerInfo::ServerStatus)valueValue );
				if( pinfo->gst->getConfig()->Debug )
				{
					std::wstring ws;
					GameServerInfo::getStatusStr( pinfo->gsi->getStatus(), ws );
					LogDebug( L"SERVER_LIST_STATUS: %s", ws.c_str() );
				}
				break;
			case 0x02: // SERVER_LIST_CLOCK
				pinfo->gsi->setShowingClock( valueValue != 0x00 );
				if( pinfo->gst->getConfig()->Debug )
					LogDebug( L"SERVER_LIST_CLOCK: %S", (valueValue == 0x00 ? "false" : "true") );
				break;
			case 0x03: // SERVER_LIST_SQUARE_BRACKET
				pinfo->gsi->setShowingBrackets( valueValue != 0x00 );
				if( pinfo->gst->getConfig()->Debug )
					LogDebug( L"SERVER_LIST_SQUARE_BRACKET: %S", (valueValue == 0x00 ? "false" : "true") );
				break;
			case 0x04: // SERVER_LIST_MAX_PLAYERS
				pinfo->gsi->setMaxPlayers( valueValue );
				if( pinfo->gst->getConfig()->Debug )
					LogDebug( L"SERVER_LIST_MAX_PLAYERS: %d", valueValue );
				break;
			case 0x05: // SERVER_LIST_TEST_SERVER
				pinfo->gsi->setTestServer( valueValue != 0x00 );
				if( pinfo->gst->getConfig()->Debug )
					LogDebug( L"SERVER_LIST_TEST_SERVER: %S", (valueValue == 0x00 ? "false" : "true") );
				break;
			}
		}
	}
	else
	{
		forceClose( pinfo, NOT_AUTHED );
	}
}







bool GameServerTable::sendGSPacket( GS_ConnectionInfo *pinfo, L2LoginPacket *pack )
{
	// pad packet to pad_step-byte border
	const int pad_step = 8;
	int dsize = pack->getDataSize();
	int rest = pad_step - (dsize % pad_step);
	if( rest > 0 && rest < pad_step )
	{
		//LogDebug( L"Need padding %d bytes to %d byte-border", rest, pad_step );
		int i;
		for( i=0; i<rest; i++ ) pack->writeC( 0x00 );
	}
	// now padded to 8-byte border; since we don't need to append more
	// 4 bytes after checksum (appendChecksum( false )), write 4 bytes before chksum
	// to keep padding to 8-byte border after checksum is appended
	pack->writeD( 0x00000000 );
	// finally, checksum (4 bytes)
	pack->appendChecksum( false );
	//
	//FILE *f = fopen( "out_packets.log", "at" );
	//pack->dumpToFile( f );
	//fclose( f );
	//
	pack->setDynamicBFKey( pinfo->bfKey, pinfo->bfKeyLen );
	pack->encodeBlowfish( false );
	unsigned int sentLen = 0;
	int r = L2PacketSend( pinfo->sock, pack, 5000, &sentLen );
	if( r <= 0 || (sentLen != pack->getPacketSize()) ) return false;
	return true;
}

void GameServerTable::forceClose( GS_ConnectionInfo *pinfo, GameServerTable::LoginFailReason reason )
{
	send_LoginServerFail( pinfo, reason );
	pinfo->flagBreak = true;
}

void GameServerTable::attachGameServerInfo( GS_ConnectionInfo *pinfo, GameServerInfo *gsi, const wchar_t *internalHost, const wchar_t *externalHost, int port, int maxPlayers )
{
	pinfo->gsi = gsi;
	gsi->setGameServerInfo( pinfo->ip, internalHost, externalHost, port, maxPlayers );
	gsi->setAuthed( true );
	// GameServerInfo should know about game server thread
	gsi->setGameServerThread( pinfo );
}

void GameServerTable::send_LoginServerFail( GS_ConnectionInfo *pinfo, GameServerTable::LoginFailReason reason )
{
	L2LoginPacket p;
	p.setPacketType( 0x01 );
	p.writeUChar( (unsigned char)reason & 0xFF );
	sendGSPacket( pinfo, &p );
}

void GameServerTable::send_KickPlayer( GS_ConnectionInfo *pinfo, const wchar_t *account )
{
	if( !account ) return;
	L2LoginPacket p;
	p.setPacketType( 0x04 );
	p.writeS( account );
	sendGSPacket( pinfo, &p );
}

void GameServerTable::send_PlayerAuthResponse( GS_ConnectionInfo *pinfo, const wchar_t *account, bool ok )
{
	if( !account ) return;
	L2LoginPacket p;
	p.setPacketType( 0x03 );
	p.writeS( account );
	p.writeC( ok ? 0x01 : 0x00 );
	sendGSPacket( pinfo, &p );
}

/**
Received gameserver connection from: 127.0.0.1
Sending InitLS...
blowfishKey == null
Length of bf key could be 22
publickey.length: 65
[S] InitLS: 
0000: 00 02 01 00 00 41 00 00 00 00 bc 6a b5 47 3b 14    .....A.....j.G;.
0010: 9b 03 3f 68 3a 0d d1 80 af ac 19 0e d7 a7 03 d8    ..?h:...........
0020: c4 c9 2a a0 c9 71 3b fb 3d 96 0b 21 05 6a 20 86    ..*..q;.=..!.j .
0030: d6 59 54 65 52 74 42 d8 49 86 69 54 29 16 0d a3    .YTeRtB.I.iT)...
0040: 40 8e 93 90 65 0e 38 43 62 35 00 00 fa 4d 91 25    @...e.8Cb5...M.%

Sending packet len 82...
[C]
0000: 00 40 00 00 00 30 84 19 d8 e9 c5 96 31 ff ce 9f    .@...0......1...
0010: a2 90 97 a2 bc c8 2b 49 9e e0 84 08 bd f1 bd d3    ......+I........
0020: 6b 55 bd 14 8e df 91 24 23 34 3a 2b d1 26 d0 2b    kU.....$#4:+.&.+
0030: 17 2c d8 70 03 fd b2 eb a1 68 64 1b 30 90 aa c7    .,.p.....hd.0...
0040: 02 54 94 7c de 00 00 00 00 00 00 00 9a ca fc 2b    .T.|...........+

New BlowFish key received, Blowfih Engine initialized:
onReceiveBlowfishKey() new bf key len 40
[C]
0000: 01 01 01 00 2a 00 00 00 2a 00 00 00 61 1e 0a 00    ....*...*...a...
0010: 00 00 10 00 00 00 c3 6f bc 01 28 63 d7 e4 63 62    .......o..(c..cb
0020: 55 14 fd 84 27 10 00 00 00 00 00 00 79 fe 6e ea    U...'.......y.n.

Auth request received
Updated Gameserver [1] Bartz IP's:
InternalIP: *
ExternalIP: *
[S] AuthResponse:
0000: 02 01 42 00 61 00 72 00 74 00 7a 00 00 00 00 00    ..B.a.r.t.z.....
0010: 00 00 00 00 17 01 4a 00                            ......J.

Sending packet len 26...
Authed: id: 1
[C]
0000: 06 04 00 00 00 03 00 00 00 00 00 00 00 02 00 00    ................
0010: 00 00 00 00 00 05 00 00 00 00 00 00 00 01 00 00    ................
0020: 00 00 00 00 00 00 00 00 00 00 00 00 06 01 00 00    ................

ServerStatus received
**/

