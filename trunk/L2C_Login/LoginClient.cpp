#include "pch.h"
#include "LoginClient.h"
#include "Log.h"
#include "LoginServer.h"

const wchar_t *LoginClient::toString() const
{
	return m_clientDesc;
}

const char *LoginClient::getIpAddress() const
{
	return m_client_ip;
}

const char *LoginClient::getAccount() const
{
	return m_account;
}

int LoginClient::getAccessLevel() const
{
	return m_accessLevel;
}

void LoginClient::setAccessLevel( int a )
{
	m_accessLevel = a;
}

int  LoginClient::getLastServer() const
{
	return m_lastServer;
}

void LoginClient::setLastServer( int s )
{
	m_lastServer = s;
}

void LoginClient::setAccount( const char *account )
{
	if( account ) strcpy( m_account, account );
	else m_account[0] = 0;
}

bool LoginClient::isUsingInternalIp() const
{
	return m_isUsingInternalIp;
}

bool LoginClient::hasJoinedGS( int *gs_id )
{
	if( m_joinedGS )
	{
		if( gs_id ) (*gs_id) = m_joinedGSID;
		return true;
	}
	return false;
}

void LoginClient::getSessionKeys( unsigned char *sessionKey1, unsigned char *sessionKey2 )
{
	memcpy( sessionKey1, p_sessionKey1, 8 );
	memcpy( sessionKey2, p_sessionKey2, 8 );
}

LoginClient::LoginClient()
{
	m_loginServer = NULL;
	m_sock = 0xFFFFFFFF;
	m_client_ip[0] = 0;
	m_client_port = 0;
	m_running = false;
	m_clientDesc[0] = 0;
	m_state = NONE;
	m_flagBreak = false;
	p_rsaKey = NULL;
	m_accessLevel = 0;
	m_lastServer = 0;
	m_account[0] = 0;
	m_isUsingInternalIp = false;
	m_joinedGS = false;
	m_joinedGSID = 0;
}

LoginClient::LoginClient( const LoginClient& other )
{
	UNREFERENCED_PARAMETER(other);
}

LoginClient::~LoginClient()
{
	disconnect( true );
	m_running = false;
	m_clientDesc[0] = 0;
	m_flagBreak = false;
}

LoginClient::LoginClient( LoginServer *pLoginServer, unsigned int sock, const char *clientIp, int clientPort )
{
	m_loginServer = pLoginServer;
	m_sock = sock;
	strcpy( m_client_ip, clientIp );
	m_client_port = clientPort;
	m_running = false;
	m_clientDesc[0] = 0;
	swprintf( m_clientDesc, 127, L"[IP: %S:%d]", m_client_ip, m_client_port );
	m_state = CONNECTED;
	m_flagBreak = false;
	m_accessLevel = 0;
	m_lastServer = 0;
	m_account[0] = 0;
	// tests for internal IP
	m_isUsingInternalIp = false;
	if( (strstr( clientIp, "127." ) == clientIp) ||
		(strstr( clientIp, "10.70." ) == clientIp) ||
		(strstr( clientIp, "192.168." ) == clientIp) ||
		(strstr( clientIp, "172.27." ) == clientIp) ) m_isUsingInternalIp = true;
}

void LoginClient::disconnect( bool force )
{
	m_flagBreak = true;
	if( !force )
	{
		while( m_running ) Sleep( 100 );
	}
	if( m_sock != INVALID_SOCKET )
	{
		L2PNet_shutdown( m_sock );
		L2PNet_closesocket( m_sock );
		m_sock = INVALID_SOCKET;
	}
	m_client_ip[0] = 0;
	m_client_port = 0;
	m_state = NONE;
	m_account[0] = 0;
}

void LoginClient::beginProcessing()
{
	unsigned int threadId = 0;
	HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0,
		(unsigned int (__stdcall *)(void *))LoginClientThread, (void *)this,
		0, &threadId );
	if( hThread ) CloseHandle( hThread );
}

unsigned int __stdcall LoginClient::LoginClientThread( LoginClient *pcls )
{
	pcls->m_running = true;
	pcls->m_flagBreak = false;

	unsigned int sock = pcls->m_sock;
	unsigned int sentLen = 0, rcvdLen = 0;
	int r;

	// create RSA & BF keys
	pcls->p_rsaKey = pcls->m_loginServer->getLoginController()->getRandomRSAKeyPair();
	BN_bn2bin( pcls->p_rsaKey->n, pcls->p_rsa_pubkey_mod );
	memcpy( pcls->p_bf_dyn_key, pcls->m_loginServer->getLoginController()->getRandomBlowfishKey(), 16 );
	// generate sessionId :)
	pcls->p_sessionId[0] = (unsigned char)( rand() & 0xFF );
	pcls->p_sessionId[1] = (unsigned char)( rand() & 0xFF );
	pcls->p_sessionId[2] = (unsigned char)( rand() & 0xFF );
	pcls->p_sessionId[3] = (unsigned char)( rand() & 0xFF );
	// cleanup sessionKeys
	memset( pcls->p_sessionKey1, 0, sizeof(pcls->p_sessionKey1) );
	memset( pcls->p_sessionKey2, 0, sizeof(pcls->p_sessionKey2) );

	// send Init
	L2Login_Init *pInit = new L2Login_Init();
	memcpy( pInit->p_BF_dyn_key,    pcls->p_bf_dyn_key, 16 );
	memcpy( pInit->p_RSA_pubKeyMod, pcls->p_rsa_pubkey_mod, 128 );
	pInit->p_protoVer = 0x0c621;
	memcpy( pInit->p_sessionId, pcls->p_sessionId, 4 );
	pInit->create( L2_VERSION_T23 );
	pInit->encodeBlowfish( true );
	r = L2PacketSend( sock, pInit, 5000, &sentLen );
	//LogDebug( L"Client %s: sent %u bytes Init", pcls->toString(), sentLen );

	unsigned char *packbuffer = (unsigned char *)malloc( 16384 ); // more than enough...

	pcls->m_flagBreak = false;
	while( !pcls->m_flagBreak )
	{
		// receive timeout - 15 seconds (should be enough)
		r = L2PacketReceive_buffer( sock, LoginController::LOGIN_TIMEOUT, &rcvdLen, packbuffer );
		if( r < 0 )
		{
			LogError( L"LoginClient %s recv error (%d)...", pcls->toString(), r );
			pcls->m_flagBreak = true;
			break;
		}
		if( r == 0 )
		{
			if( pcls->m_loginServer->getConfig()->Debug )
				LogDebug( L"LoginClient %s: client closed connection.", pcls->toString() );
			pcls->m_flagBreak = true;
			break;
		}
		L2LoginPacket *pack = new L2LoginPacket( packbuffer, rcvdLen );
		pack->decodeBlowfish( pcls->p_bf_dyn_key );
		// packet received and decoded, now verify checksum
		if( !pack->verifyChecksum() )
		{
			LogError( L"Incorrect checksum from client %s", pcls->toString() );
			pcls->m_flagBreak = true;
			continue;
		}
		//
		unsigned int opcode = (unsigned int)pack->getByteAt( 2 );
#ifdef _DEBUG
		LogDebug( L"Client: 0x%02X", opcode );
#endif

		// parse opcode
		switch( pcls->m_state )
		{
		case LoginClient::CONNECTED:
			{
				if( opcode == 0x07 ) // RequestGGAuth
					pcls->ph_RequestGGAuth( pack );
				else pcls->invalidPacket( pack );
			} break;
		case LoginClient::AUTHED_GG:
			{
				if( opcode == 0x00 ) // RequestAuthLogin
					pcls->ph_RequestAuthLogin( pack );
				else pcls->invalidPacket( pack );
			} break;
		case LoginClient::AUTHED_LOGIN:
			{
				if( opcode == 0x05 ) // RequestServerList
					pcls->ph_RequestServerList( pack );
				else if( opcode == 0x02 ) // RequestServerLogin
					pcls->ph_RequestServerLogin( pack );
				else pcls->invalidPacket( pack );
			} break;
		}

		delete pack;
		pack = NULL;
	}

	free( packbuffer );
	packbuffer = NULL;

	// remove self from login controller clients list
	pcls->m_loginServer->getLoginController()->removeClient( pcls );
	// cleanup
	if( pcls->p_rsaKey ) RSA_free( pcls->p_rsaKey );
	pcls->p_rsaKey = NULL;
	L2PNet_shutdown( sock );
	L2PNet_closesocket( sock );
	pcls->m_sock = INVALID_SOCKET;
	pcls->m_running = false;
	pcls->m_flagBreak = false;
	return 0;
}

void LoginClient::invalidPacket( L2LoginPacket *pack )
{
	const unsigned char *bytes = pack->getBytesPtr();
	unsigned int opcode = (unsigned int)bytes[2];
	std::wstring stateName = L"NONE";
	switch( m_state )
	{
	case CONNECTED: stateName = L"CONNECTED"; break;
	case AUTHED_GG: stateName = L"AUTHED_GG"; break;
	case AUTHED_LOGIN: stateName = L"AUTHED_LOGIN"; break;
	}
	LogError( L"Client %s: invalid packet %02X (len %u) in state %s!", toString(),
		opcode, pack->getPacketSize(), stateName.c_str() );
	m_flagBreak = true; // end client processing
}

void LoginClient::ph_RequestGGAuth( L2LoginPacket *pack )
{
	pack->getPacketType();
	if( pack->canReadBytes( 20 ) )
	{
		unsigned char sid[4];
		sid[0] = pack->readUChar();
		sid[1] = pack->readUChar();
		sid[2] = pack->readUChar();
		sid[3] = pack->readUChar();
		// more 16 bytes follow, ignore them
		// compare sid
		if( sid[0] == p_sessionId[0] && sid[1] == p_sessionId[1] &&
			sid[2] == p_sessionId[2] && sid[3] == p_sessionId[3] )
		{
			m_state = AUTHED_GG;
			// reply GGAuthResponse
			L2Login_GGAuthResponse p_ggar;
			p_ggar.p_ggAuthResponse = sid[0] | sid[1]<<8 | sid[2]<<16 | sid[3]<<24;
			p_ggar.create( L2_VERSION_T23 );
			encodeAndSendPacket( &p_ggar );
		}
		else
		{
			LogError( L"Client %s: RequestGGAuth invalid sessionId", toString() );
			// send login fail
			L2Login_LoginFail p_lfail;
			p_lfail.p_reasonCode = L2Login_LoginFail::REASON_ACCESS_FAILED;
			p_lfail.create( L2_VERSION_T23 );
			m_flagBreak = true;
			encodeAndSendPacket( &p_lfail );
		}
	}
	else
	{
		LogError( L"Client %s: RequestGGAuth too short packet, protocol error", toString() );
		m_flagBreak = true;
	}
}

void LoginClient::ph_RequestAuthLogin( L2LoginPacket *pack )
{
	pack->getPacketType();
	if( !pack->canReadBytes( 128 ) ) // we need at least 128, ignore rest
	{
		LogError( L"Client %s: RequestAuthLogin too short packet, protocol error", toString() );
		m_flagBreak = true;
		return;
	}
	unsigned char block128[128];
	unsigned char loginBlock[128];
	memset( block128, 0, sizeof(block128) );
	memset( loginBlock, 0, sizeof(loginBlock) );
	// read block with encrypted login/pass
	pack->readBytes( block128, 128 );
	// decode this block with client's RSA key
	int r = RSA_private_decrypt( 128, block128, loginBlock, p_rsaKey, RSA_NO_PADDING );
	if( r == -1 )
	{
		LogError( L"Client %s: RSA_private_decrypt failed!", toString() );
		m_flagBreak = true;
		return;
	}
	// request auth login constants
	char l2login[L2_LOGIN_MAXLEN+1];
	char l2pass[L2_PASSWORD_MAXLEN+1];
	const int l2login_offset = 94;
	const int l2pass_offset = 108;
	// memsets
	memset( l2login, 0, sizeof(l2login) );
	memset( l2pass, 0, sizeof(l2pass) );
	memcpy( l2login, loginBlock + l2login_offset, L2_LOGIN_MAXLEN );
	memcpy( l2pass, loginBlock + l2pass_offset, L2_PASSWORD_MAXLEN );
	// ncotp wtf?
	//unsigned int _ncotp = loginBlock[0x7c];
	//_ncotp |= loginBlock[0x7d] << 8;
	//_ncotp |= loginBlock[0x7e] << 16;
	//_ncotp |= loginBlock[0x7f] << 24;
	try
	{
		// validate login
		LoginController::AuthLoginResult result =
			m_loginServer->getLoginController()->tryAuthLogin( l2login, l2pass, this );
		switch( result )
		{
		case LoginController::AUTH_SUCCESS:
			{
				m_state = LoginClient::AUTHED_LOGIN;
				// if login OK, don't forget to set account
				// and update desc to allow toString() return also account
				// account is set by LoginController in synchronized code block
				swprintf( m_clientDesc, 127, L"Acc: %S [IP: %S:%d]", m_account, m_client_ip, m_client_port );
				// generate session keys
				int i = 0;
				for( i=0; i<8; i++ )
				{
					p_sessionKey1[i] = (unsigned char)(rand() % 256) & 0xFF;
					p_sessionKey2[i] = (unsigned char)(rand() % 256) & 0xFF;
				}
				// send LoginOK
				// L2J can directly send ServerList not sending LoginOK, if Config.SHOW_LICENSE == false
				// but this is not true L2 protcol and half of sessionKey is unknown to client
				/*if (Config.SHOW_LICENCE)
				{
					client.sendPacket(new LoginOk(getClient().getSessionKey()));
				}
				else
				{
					getClient().sendPacket(new ServerList(getClient()));
				}*/
				// we always send LoginOK
				L2Login_LoginOK p_lok;
				memcpy( p_lok.p_sessionKey1, p_sessionKey1, 8 );
				p_lok.create( L2_VERSION_T23 );
				encodeAndSendPacket( &p_lok );
			} break;
		case LoginController::INVALID_PASSWORD:
			{
				// send LoginFail
				L2Login_LoginFail p_lfail;
				p_lfail.p_reasonCode = L2Login_LoginFail::REASON_USER_OR_PASS_WRONG;
				p_lfail.create( L2_VERSION_T23 );
				encodeAndSendPacket( &p_lfail );
				m_flagBreak = true;
			} break;
		case LoginController::ACCOUNT_BANNED:
			{
				// send AccountKicked
				L2Login_AccountKicked p_acc;
				p_acc.p_reasonCode = L2Login_AccountKicked::REASON_PERMANENTLY_BANNED;
				p_acc.create( L2_VERSION_T23 );
				encodeAndSendPacket( &p_acc );
				m_flagBreak = true;
			} break;
		case LoginController::ALREADY_ON_LS:
			{
				LogWarning( L"Client %s: Account %S is already on login server", toString(), l2login );
				// L2J kicks other client, we will not do so
				//L2LoginClient oldClient;
				//if ((oldClient = lc.getAuthedClient(_user)) != null)
				//{
				//	// kick the other client
				//	oldClient.close(LoginFailReason.REASON_ACCOUNT_IN_USE);
				//	lc.removeAuthedLoginClient(_user);
				//}
				// kick current client
				//client.close(LoginFailReason.REASON_ACCOUNT_IN_USE);
				// send LoginFail
				L2Login_LoginFail p_lfail;
				p_lfail.p_reasonCode = L2Login_LoginFail::REASON_ACCOUNT_IN_USE;
				p_lfail.create( L2_VERSION_T23 );
				encodeAndSendPacket( &p_lfail );
				m_flagBreak = true;
			} break;
		case LoginController::ALREADY_ON_GS:
			{
				GameServerInfo *gsi = m_loginServer->getGameServerTable()->getGameServerThreadForAccount( l2login );
				if( gsi != NULL )
				{
					// send LoginFail
					L2Login_LoginFail p_lfail;
					p_lfail.p_reasonCode = L2Login_LoginFail::REASON_ACCOUNT_IN_USE;
					p_lfail.create( L2_VERSION_T23 );
					encodeAndSendPacket( &p_lfail );
					// kick from here
					m_flagBreak = true;
					// kick from there
					if( gsi->isAuthed() ) gsi->kickPlayer( l2login );
				}
			} break;
		} // switch( result)
	}
	catch( LoginController::HackingException& e )
	{
		int sec_to_ban = m_loginServer->getConfig()->LoginBlockAfterBan*1000;
		m_loginServer->getLoginController()->addBanForAddress( m_client_ip, sec_to_ban );
		LogWarning( L"Banned %S (%S) for %d seconds, due to %d incorrect login attempts.",
			l2login, m_client_ip, sec_to_ban, e.getConnects() );
	}
}

void LoginClient::ph_RequestServerList( L2LoginPacket *pack )
{
	pack->getPacketType();
	if( !pack->canReadBytes( 8 ) )
	{
		LogError( L"Client %s: RequestServerList packet too short, protocol error!", toString() );
		m_flagBreak = true;
		return;
	}
	unsigned char read_sessionKey1[8];
	pack->readBytes( read_sessionKey1, 8 );
	bool equal = true;
	/*int i;
	for( i=0; i<8; i++ )
	{
		if( read_sessionKey1[i] != p_sessionKey1[i] )
		{
			equal = false;
			break;
		}
	}*/
	equal = (memcmp( this->p_sessionKey1, read_sessionKey1, 8 ) == 0);
	if( !equal )
	{
		LogError( L"Client %s: RequestServerList: invalid sessionKey1!", toString() );
		// send LoginFail
		L2Login_LoginFail p_lfail;
		p_lfail.p_reasonCode = L2Login_LoginFail::REASON_ACCESS_FAILED;
		p_lfail.create( L2_VERSION_T23 );
		encodeAndSendPacket( &p_lfail );
		// kick
		m_flagBreak = true;
		return;
	}
	// send ServerList...
	GameServerTable *gst = m_loginServer->getGameServerTable();
	const std::map<int, GameServerInfo *> gameServers = gst->getRegisteredGameServers();
	L2Login_ServerList p_sl;
	p_sl.setPacketType( 0x04 );
	p_sl.writeUChar( (unsigned char)gameServers.size() );
	p_sl.writeUChar( (unsigned char)m_lastServer );
	// for each server write its info in packet
	std::map<int, GameServerInfo *>::const_iterator iter;
	for( iter = gameServers.begin();   iter != gameServers.end();   iter++ )
	{
		const GameServerInfo *gsi = iter->second;
		if( !gsi ) continue;
		// server id
		p_sl.writeUChar( (unsigned char)gsi->getId() );
		// server ip
		const char *sip = NULL;
		if( isUsingInternalIp() ) sip = gsi->getInternalIp(); else sip = gsi->getExternalIp();
		in_addr inaddr;
		inaddr.s_addr = L2PNet_inet_addr( sip );
		if( inaddr.s_addr == INADDR_NONE ) inaddr.s_addr = INADDR_ANY; // 255.255.255.255 => 0.0.0.0
		p_sl.writeUChar( inaddr.S_un.S_un_b.s_b1 );
		p_sl.writeUChar( inaddr.S_un.S_un_b.s_b2 );
		p_sl.writeUChar( inaddr.S_un.S_un_b.s_b3 );
		p_sl.writeUChar( inaddr.S_un.S_un_b.s_b4 );
		// server port
		p_sl.writeD( gsi->getPort() );
		p_sl.writeC( 0x00 ); // age limit?
		p_sl.writeC( gsi->isPvP() ? 0x01 : 0x00 );
		p_sl.writeUShort( (unsigned short)gsi->getPlayerCount() );
		p_sl.writeUShort( (unsigned short)gsi->getMaxPlayers() );
		p_sl.writeUChar( gsi->getStatus() == GameServerInfo::STATUS_DOWN ? 0x00 : 0x01 );
		unsigned int bits = 0x00;
		if( gsi->isTestServer() ) bits |= 0x04;
		if( gsi->isShowingClock() ) bits |= 0x02;
		p_sl.writeUInt( bits );
		p_sl.writeUChar( gsi->isShowingBrackets() ? 0x01 : 0x00 );
	}
	encodeAndSendPacket( &p_sl );
}

void LoginClient::ph_RequestServerLogin( L2LoginPacket *pack )
{
	pack->getPacketType();
	// create parser :)
	L2Login_RequestServerLogin *p_rsl = new L2Login_RequestServerLogin(
		pack->getBytesPtr(), pack->getPacketSize() );
	if( !p_rsl->parse( L2_VERSION_T23 ) )
	{
		LogError( L"Client %s: error parsing RequestServerLogin!", toString() );
		m_flagBreak = true;
		delete p_rsl;
		return;
	}
	int serverId = (int)p_rsl->p_gameServerId;
	// compare sessionKey
	bool equal = (memcmp( this->p_sessionKey1, p_rsl->p_sessionKey1, 8 ) == 0);
	if( !equal )
	{
		// send LoginFail:ACCESS_FAILED
		L2Login_LoginFail p_lfail;
		p_lfail.p_reasonCode = L2Login_LoginFail::REASON_ACCESS_FAILED;
		p_lfail.create( L2_VERSION_T23 );
		encodeAndSendPacket( &p_lfail );
		LogError( L"Client %s: RequestServerLogin: invalid session key!", toString() );
		m_flagBreak = true;
		delete p_rsl;
		return;
	}
	//
	delete p_rsl;
	p_rsl = NULL;
	// check maxPlayers
	if( !m_loginServer->getGameServerTable()->isLoginPossible( serverId, m_accessLevel, m_account, m_lastServer ) )
	{
		L2Login_PlayFail p_pfail;
		p_pfail.p_reasonCode = L2Login_PlayFail::REASON_ACCESS_FAILED;
		p_pfail.create( L2_VERSION_T23 );
		encodeAndSendPacket( &p_pfail );
		LogError( L"Client %s: RequestServerLogin: impossible to play on server %d", toString(), serverId );
		m_flagBreak = true;
		return;
	}
	// mark as joined GS
	m_joinedGS = true;
	m_joinedGSID = serverId;
	m_loginServer->getLoginController()->addClientOnGS( m_account, p_sessionKey1, p_sessionKey2 );
	// send PlayOK
	L2Login_PlayOK p_ok;
	memcpy( p_ok.p_sessionKey2, this->p_sessionKey2, 8 );
	p_ok.create( L2_VERSION_T23 );
	encodeAndSendPacket( &p_ok );
}


void LoginClient::encodeAndSendPacket( L2LoginPacket *pack )
{
	// pad packet to 8 bytes len, append checksum, encode packet with client's dynamic BF key
	pack->encodeAndPrepareToSend( p_bf_dyn_key );
	unsigned int sentLen = 0;
	int r = L2PacketSend( m_sock, pack, 10000, &sentLen );
	if( r<=0 || (sentLen != pack->getPacketSize()) )
	{
		LogError( L"Client %s: error sending packet, %u instead of %u bytes sent (result %d)",
			toString(), sentLen, pack->getPacketSize(), r );
		m_flagBreak = true;
	}
}
