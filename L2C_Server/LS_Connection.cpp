#include "pch.h"
#include "Log.h"
#include "l2c_utils.h"
#include "LS_Connection.h"
#include "GS.h"
#include "utils/Exception.h"

LoginConnection::LoginConnection()
{
	m_ls_addr[0] = 0;
	m_ls_port = m_ls_protover = 0;
	m_isRunning = m_flagStop = m_isConnected = false;
	m_sock = INVALID_SOCKET;
	m_bfKey[0] = 0;
	m_bfKeyLen = 0;
	m_rsaKey = NULL;
	m_registeredServerName[0] = 0;
	m_registeredServerId = 0;
}

LoginConnection::~LoginConnection()
{
	stop();
	m_ls_addr[0] = 0;
	m_ls_port = m_ls_protover = 0;
	m_isRunning = m_flagStop = m_isConnected = false;
	if( m_sock != INVALID_SOCKET )
	{
		L2PNet_shutdown( m_sock );
		L2PNet_closesocket( m_sock );
	}
	m_sock = INVALID_SOCKET;
	m_bfKey[0] = 0;
	m_bfKeyLen = 0;
	if( m_rsaKey )
	{
		RSA_free( m_rsaKey );
		m_rsaKey = NULL;
	}
	m_registeredServerName[0] = 0;
	m_registeredServerId = 0;
}

int LoginConnection::getRegisteredServerId() const
{
	return m_registeredServerId;
}

const wchar_t *LoginConnection::getRegisteredServerName() const
{
	return (const wchar_t *)m_registeredServerName;
}

void LoginConnection::start( const wchar_t *ls_addr, int ls_port, int ls_protover )
{
	if( m_isRunning ) return;
	HANDLE hThread = NULL;
	unsigned int tid = 0;
	m_flagStop = false;
	m_lock.Lock();
	{
		hThread = (HANDLE)_beginthreadex( NULL, 0,
			(unsigned int (__stdcall *)(void *))LoginConnection::LS_ConnThread, (void *)this,
			0, &tid );
		if( hThread )
		{
			wcsncpy( m_ls_addr, ls_addr, 127 ); m_ls_addr[127] = 0;
			m_ls_port = ls_port;
			m_ls_protover = ls_protover;
			m_isRunning = true;
			CloseHandle( hThread );
		}
	}
	m_lock.Unlock();
}

void LoginConnection::stop()
{
	if( !m_isRunning ) return;
	m_lock.Lock();
	m_flagStop = true;
	while( m_isRunning ) Sleep(50);
	m_flagStop = false;
	m_registeredServerName[0] = 0;
	m_registeredServerId = 0;
	m_lock.Unlock();
	//LogDebug( L"LS Connection stopped." ); // deadlock in SendMessage() :(
}

DWORD WINAPI LoginConnection::LS_ConnThread( LPVOID lpvParam )
{
	LoginConnection *lc = (LoginConnection *)lpvParam;
	//lc->m_isRunning = true; // not here, see line 107
	unsigned char *packbuffer = NULL;
	bool should_retry_connect = true;
	int connect_tries = 0;
	char a_ls_addr[128] = {0};
	l2c_unicode_to_ansi( lc->m_ls_addr, a_ls_addr, 128 );
	int bReadyRead = 0, bReadyWrite = 0, r = 0;
	unsigned int rcvdLen = 0;
	// initialize initial BF key
	char initial_bfKey[] = "_;v.]05-31!|+-%xT!^[$\00";
	unsigned int opcode = 0;
	L2LoginPacket *pack = NULL;

retry_connect:
	lc->m_isRunning = true; // only now we started work!
	lc->m_isConnected = false; // not connected
	connect_tries++;
	if( connect_tries == 1 )
		Log( L"Connecting to login at %s:%d...", lc->m_ls_addr, lc->m_ls_port );
	else
		Log( L"Connecting to login at %s:%d... (try #%d)", lc->m_ls_addr, lc->m_ls_port, connect_tries );

	try
	{
		packbuffer = (unsigned char *)malloc( 64*1024 ); // 64 Kb enough
		if( !packbuffer ) throw std::bad_alloc( "packbuffer (64 Kb)" );

		lc->m_sock = L2PNet_TCPsocket_create( true );
		L2PNet_connect( lc->m_sock, a_ls_addr, (unsigned short)lc->m_ls_port );

		bReadyRead = 0, bReadyWrite = 0, r = 0;
		r = L2PNet_select( lc->m_sock, L2PNET_SELECT_WRITE, 5000, &bReadyRead, &bReadyWrite );
		if( !bReadyWrite ) throw Exception( "Connect to LS failed!" );
		if( GameServer::getInstance()->getConfig()->Debug ) LogDebug( L"Connected to login." );
		lc->m_isConnected = true; // we're connected!

		// suddenly we're signaled to stop?
		if( lc->m_flagStop ) goto lsconn_cleanup;

		rcvdLen = 0;
		memcpy( lc->m_bfKey, initial_bfKey, 23 );
		lc->m_bfKey[22] = '0';
		lc->m_bfKeyLen = 22;

		while( !lc->m_flagStop )
		{
			r = L2PNet_select( lc->m_sock, L2PNET_SELECT_READ, 200, &bReadyRead, &bReadyWrite );
			if( r == 0 ) continue; // timeout
			if( r == -1 ) throw Exception( "Network select() error" );
			r = L2PacketReceive_buffer( lc->m_sock, 2000, &rcvdLen, packbuffer );
			if( r <= 0 ) throw Exception( "recv packet failed" );
			// packet received OK
			// decode blowfish
			pack = new L2LoginPacket( packbuffer, rcvdLen );
			pack->setDynamicBFKey( lc->m_bfKey, lc->m_bfKeyLen );
			pack->decodeBlowfish( false );
			// validate checksum
			if( !pack->verifyChecksum() ) throw Exception( "Invalid packet checksum!" );
#ifdef _DEBUG
			LogDebug( L"DBG: LoginConnection rcv: %u bytes (opcode %02X)", rcvdLen, (unsigned int)pack->getByteAt(2) );
#endif
			// handle packet
			opcode = (unsigned int)pack->getByteAt(2);
			switch( opcode )
			{
			case 0x00: lc->ph_InitLS( pack ); break;
			case 0x01: lc->ph_LoginServerFail( pack ); break;
			case 0x02: lc->ph_LoginServerOK( pack ); break;
			case 0x03: lc->ph_PlayerAuthResponse( pack ); break;
			case 0x04: lc->ph_KickPlayer( pack ); break;
			}
			// free pack mem
			if( pack ) delete pack;
			pack = NULL;
		}
	}
	catch( Exception& e )
	{
		if( !lc->m_flagStop ) // do not write log if signaled to stop (deadlock in SendMessage)
		{
			LogError( L"LoginConnection thread: Exception: %S", e.what() );
			e.logStackTrace();
		}
	}
	catch( std::bad_alloc& ba )
	{
		LogError( L"LoginConnection thread: bad memory allocation: %S", ba.what() );
	}
	catch( std::exception& e )
	{
		LogError( L"LoginConnection thread: error: %S", e.what() );
	}

lsconn_cleanup:
	// cleanup
	if( packbuffer ) free( packbuffer );
	packbuffer = NULL;
	if( pack ) delete pack;
	pack = NULL;
	L2PNet_shutdown( lc->m_sock );
	L2PNet_closesocket( lc->m_sock );
	lc->m_sock = INVALID_SOCKET;
	lc->m_bfKey[0] = 0;
	lc->m_bfKeyLen = 0;
	if( lc->m_rsaKey )
	{
		RSA_free( lc->m_rsaKey );
		lc->m_rsaKey = NULL;
	}
	lc->m_registeredServerName[0] = 0;
	lc->m_registeredServerId = 0;
	lc->m_isRunning = false; // mark as stopped
	lc->m_isConnected = false; // mark as not connected

	// should we retry and connect again?
	if( lc->m_flagStop ) should_retry_connect = false;
	if( should_retry_connect )
	{
		if( GameServer::getInstance()->isRunning() )
			should_retry_connect = true;
		else
			should_retry_connect = false;
	}
	if( should_retry_connect ) goto retry_connect;

	return 0;
}



void LoginConnection::ph_InitLS( L2LoginPacket *pack )
{
	// ddc[b64]
	unsigned char rsa_modulus[128];
	memset( rsa_modulus, 0, sizeof(rsa_modulus) );
	//
	pack->getPacketType(); // 0x00 - InitLS
	int login_proto_rev = pack->readD(); // login protocol rev
	unsigned int rsa_keylen = pack->readD() - 1; // 
	pack->readC(); // ?? wtf
	pack->readBytes( rsa_modulus, rsa_keylen );
	//
	GameServer *gs = GameServer::getInstance();
	if( login_proto_rev != gs->getConfig()->login_protocol_version )
		LogWarning( L"Warning! login server sent protocol version %d (must be %d)",
			login_proto_rev, gs->getConfig()->login_protocol_version );
	if( gs->getConfig()->Debug )
		LogDebug( L"LoginConnection: got InitLS, login protover = %u", login_proto_rev );
	//
	RSA *rsaKey = RSA_new();
	BN_dec2bn( &(rsaKey->e), "65537" );
	rsaKey->n = BN_bin2bn( rsa_modulus, rsa_keylen, NULL );
	m_rsaKey = rsaKey;

	send_BlowfishKey();
	send_GameServerAuthRequest();
}

void LoginConnection::ph_LoginServerFail( L2LoginPacket *pack )
{
	// c
	pack->getPacketType();
	unsigned int reason = (unsigned int)pack->readC();
	bool mustThrow = true;
	wchar_t strReason[32] = {0};
	wcscpy( strReason, L"<unknown>" );
	switch( reason )
	{
	case NOT_AUTHED:               wcscpy( strReason, L"NOT_AUTHED" ); break;
	case REASON_ALREADY_LOGGED_IN: wcscpy( strReason, L"REASON_ALREADY_LOGGED_IN" ); mustThrow = false; break;
	case REASON_ID_RESERVED:       wcscpy( strReason, L"REASON_ID_RESERVED" ); break;
	case REASON_IP_BANNED:         wcscpy( strReason, L"REASON_IP_BANNED" ); break;
	case REASON_NO_FREE_ID:        wcscpy( strReason, L"REASON_NO_FREE_ID" ); break;
	case REASON_WRONG_HEXID:       wcscpy( strReason, L"REASON_WRONG_HEXID" ); break;
	}
	if( mustThrow )
	{
		LogError( L"Failed to auth on login server! Error code %u (%s)", reason, strReason );
		throw Exception( "LS auth failed!" );
	}
	LogWarning( L"Failed to auth on login server! Error code %u (%s)", reason, strReason );
}

void LoginConnection::ph_LoginServerOK( L2LoginPacket *pack )
{
	// cS
	pack->getPacketType();
	m_registeredServerId = (int)pack->readUChar();
	wcsncpy( m_registeredServerName, pack->readUnicodeStringPtr(), 255 );
	m_registeredServerName[255] = 0;
	Log( L"Registered on login as server #%d (%s)", m_registeredServerId, m_registeredServerName );
	//
	send_ServerStatus();
}

void LoginConnection::ph_PlayerAuthResponse( L2LoginPacket *pack )
{
	// Sc
	GameServer *gs = GameServer::getInstance();
	pack->getPacketType();
	wchar_t account[256] = {0};
	wcsncpy( account, pack->readUnicodeStringPtr(), 255 ); account[255] = 0;
	int ok = (int)pack->readUChar();
	//
	if( GameServer::getInstance()->getConfig()->Debug )
		LogDebug( L"LS PlayerAuthResponse: account [%s] response: %d", account, ok );
	//
	gs->notifyLoginPlayerAuth( account, ok );
}

void LoginConnection::ph_KickPlayer( L2LoginPacket *pack )
{
	// S
	pack->getPacketType();
	wchar_t account[256] = {0};
	wcsncpy( account, pack->readUnicodeStringPtr(), 255 ); account[255] = 0;
	//
	GameServer *gs = GameServer::getInstance();
	if( gs->getConfig()->Debug ) Log( L"LS KickPlayer acc [%s]", account );
	gs->kickPlayerByAccount( account, true );
}



void LoginConnection::send_BlowfishKey()
{
	// now reply with BlowfishKey
	// construct array of 64 bytes, first 24 of them are NULLs, next 40 are new BF key.
	unsigned char block[64];
	unsigned char block_e[64];
	memset( block, 0, sizeof(block) );
	memset( block_e, 0, sizeof(block_e) );
	srand( (unsigned int)time(NULL) );
	int i;
	for( i=0; i<40; i++ ) block[24+i] = (unsigned char)( (rand() % 255) & 0xFF );
	
	// encode block with RSA key
	int r = RSA_public_encrypt( 64, block, block_e, m_rsaKey, RSA_NO_PADDING );
	if( r == -1 )
	{
		LogError( L"send BlowfishKey: RSA_public_encrypt() error!" );
		m_flagStop = true;
		return;
	}
	// construct packet BlowfishKey
	L2LoginPacket p_bf;
	p_bf.setPacketType( 0x00 );
	p_bf.writeUInt( 64 ); // length of block
	p_bf.writeBytes( block_e, 64 ); // rsa encrypted block with new BF key

	// send it!
	sendLSPacket( &p_bf );

	// after send! save generated bf key as our new key
	memcpy( m_bfKey, block+24, 40 );
	m_bfKeyLen = 40;

	// now we can free RSA key - we dont need it any more?
	RSA_free( m_rsaKey );
	m_rsaKey = NULL;

	//
	if( GameServer::getInstance()->getConfig()->Debug ) LogDebug( L"LoginConnection: sent BlowfishKey" );
}

void LoginConnection::send_GameServerAuthRequest()
{
	GameServer *gs = GameServer::getInstance();
	L2LoginPacket p;
	p.setPacketType( 0x01 );
	p.writeUChar( (unsigned char)(gs->getConfig()->server_id & 0xFF) );
	p.writeUChar( gs->getConfig()->accept_alternate_server_id ? 0x01 : 0x00 );
	p.writeUChar( 0x00 ); // host reserved = false
	p.writeS( gs->getConfig()->report_external_hostname );
	p.writeS( gs->getConfig()->report_internal_hostname );
	p.writeH( (short)gs->getConfig()->game_server_bind_port );
	p.writeD( gs->getConfig()->max_players );
	// write hexid as byte array
	wchar_t *server_hexid = gs->getConfig()->server_hexid;
	int hexid_len = wcslen( server_hexid );
	int i;
	p.writeD( hexid_len/2 );
	unsigned int val = 0;
	wchar_t temp_ws[3] = {0,0,0};
	for( i=0; i<hexid_len; i+=2 )
	{
		temp_ws[0] = server_hexid[i];
		temp_ws[1] = server_hexid[i+1];
		swscanf( temp_ws, L"%x", &val );
		p.writeUChar( (unsigned char)(val & 0xFF) );
	}
	// send!
	sendLSPacket( &p );
	//
	if( gs->getConfig()->Debug ) LogDebug( L"LoginConnection: sent GameServerAuthRequest" );
}

void LoginConnection::send_PlayerInGame( const wchar_t *accountName )
{
	L2LoginPacket pack;
	pack.setPacketType( 0x02 );
	pack.writeH( 1 ); // 1 account name
	pack.writeS( accountName );
	sendLSPacket( &pack );
}

void LoginConnection::send_PlayerLogOut( const wchar_t *accountName )
{
	L2LoginPacket pack;
	pack.setPacketType( 0x03 );
	pack.writeS( accountName );
	sendLSPacket( &pack );
}

void LoginConnection::send_ChangeAccessLevel( const wchar_t *accountName, int new_access_level )
{
	L2LoginPacket pack;
	pack.setPacketType( 0x04 );
	pack.writeD( new_access_level );
	pack.writeS( accountName );
	sendLSPacket( &pack );
}

void LoginConnection::send_PlayerAuthRequest( const wchar_t *accountName, unsigned char *loginKey, unsigned char *playKey )
{
	L2LoginPacket pack;
	pack.setPacketType( 0x05 );
	pack.writeS( accountName );
	pack.writeBytes( playKey, 8 );
	pack.writeBytes( loginKey, 8 );
	sendLSPacket( &pack );
}

void LoginConnection::send_ServerStatus()
{
	GameServer *gs = GameServer::getInstance();
	L2LoginPacket p;
	p.setPacketType( 0x06 ); // opcode
	//
	p.writeD( 5 ); // parameters count
	// first - server status
	p.writeD( SERVER_LIST_STATUS );
	p.writeD( gs->getServerStatus() );
	// 2 - SERVER_LIST_CLOCK
	p.writeD( SERVER_LIST_CLOCK );
	p.writeD( gs->getConfig()->server_list_clock ? 0x01 : 0x00 );
	// 3 - SERVER_LIST_SQUARE_BRACKET
	p.writeD( SERVER_LIST_SQUARE_BRACKET );
	p.writeD( gs->getConfig()->server_list_brackets ? 0x01 : 0x00 );
	// 4 - max players
	p.writeD( SERVER_LIST_MAX_PLAYERS );
	p.writeD( gs->getMaxPlayers() );
	// 5 - list as test server
	p.writeD( SERVER_LIST_TEST_SERVER );
	p.writeD( gs->getConfig()->list_as_test_server ? 0x01 : 0x00 );
	//
	sendLSPacket( &p );
}

void LoginConnection::sendLSPacket( L2LoginPacket *pack )
{
	// asserts
	if( !pack ) return;
	if( m_sock == INVALID_SOCKET ) return; // socket down
	if( !m_isRunning ) return; // login connection is stopped
	if( !m_isConnected ) return; // maybe login connection thread is running, trying to connect, but not connected
	//
	unsigned int sentLen = 0;
	L2LoginPacket *packet_copy = new L2LoginPacket( pack->getBytesPtr(), pack->getPacketSize() );
	// pad packet to pad_step-byte border
	const int pad_step = 8;
	int dsize = packet_copy->getDataSize();
	int rest = pad_step - (dsize % pad_step);
	if( (rest > 0) && (rest < pad_step) ) // need alignment?
	{
		//LogDebug( L"Need padding %d bytes to %d byte-border", rest, pad_step );
		int i;
		for( i=0; i<rest; i++ ) packet_copy->writeC( 0x00 );
	}
	// now padded to 8-byte border; since we don't need to append more
	// 4 bytes after checksum (appendChecksum( false )), write 4 bytes before chksum
	// to keep padding to 8-byte border after checksum is appended
	packet_copy->writeD( 0x00000000 );
	// finally, checksum (4 bytes)
	packet_copy->appendChecksum( false );
	// now we can encode blowfish
	packet_copy->setDynamicBFKey( m_bfKey, m_bfKeyLen );
	packet_copy->encodeBlowfish( false );
	//
	m_lock.Lock();
	int r = L2PacketSend( m_sock, packet_copy, 2000, &sentLen );
	m_lock.Unlock();
	if( r < 0 || sentLen != packet_copy->getPacketSize() )
	{
		//LogError( L"LoginConnection::sendLSPacket(): sent %u instead of %u bytes! (retval %d)",
		//	sentLen, packet_copy->getPacketSize(), r );
		delete packet_copy;
		throw Exception( "LoginConnection::sendLSPacket(): sent %u instead of %u bytes! (retval %d)",
			sentLen, packet_copy->getPacketSize(), r );
	}
	delete packet_copy;
}
