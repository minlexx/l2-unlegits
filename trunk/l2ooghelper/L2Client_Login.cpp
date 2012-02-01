#include "stdafx.h"
#include "Resource.h"
#include "Logger.h"
#include "MainWindow.h"
#include "L2Client.h"
#include "ChooseServerDlg.h"

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

bool L2Client::L2ClientThread_Login( class L2Client *pcls )
{
	bool was_error = false;
	int i = 0;
	int rdyRead = 0, rdyWrite = 0;
	unsigned char *packbuffer = NULL;
	int r = -1;
	long recvTimeoutMsec = 5000;
	long sendTimeoutMsec = 5000;
	unsigned int plen = 0;
	unsigned char ptype = 0;

	pcls->setState( L2Client::STATE_OFFLINE );
	pcls->sock = INVALID_SOCKET;
	pcls->sock = L2PNet_TCPsocket_create( true );
	if( pcls->sock == INVALID_SOCKET )
	{
		log_error( LOG_ERROR, "L2ClientThread: socket crt failed!\n" );
		pcls->threadNetCleanup( pcls );
		return false;
	}

	pcls->setState( L2Client::STATE_CONNECTING_LOGIN );
	pcls->addChatToTabFormat( L2Client::CHAT_SYS, L"Connecting to login %S:%d...",
		pcls->account.loginServerIP, pcls->account.loginServerPort );
	L2PNet_connect( pcls->sock, pcls->account.loginServerIP,
		(unsigned short)pcls->account.loginServerPort );

	L2PNet_select( pcls->sock, L2PNET_SELECT_WRITE, 10000, &rdyRead, &rdyWrite );
	if( !rdyWrite )
	{
		log_error( LOG_ERROR, "L2ClientThread: login connect failed!\n" );
		pcls->threadNetCleanup( pcls );
		pcls->addChatToTab( CHAT_SYS, L"Connect to login failed!" );
		return false;
	}

	packbuffer = (unsigned char *)malloc( 10240 );
	if( !packbuffer )
	{
		log_error( LOG_ERROR, "L2ClientThread: malloc() packbuffer failed!\n" );
		pcls->threadNetCleanup( pcls );
		return false;
	}

	pcls->addChatToTab( L2Client::CHAT_SYS, L"Connected to login." );

	// TODO: Login server session vars
	// login server session vars
	unsigned char  ls_sessionID[4] = {0,0,0,0};
	unsigned int   ls_sessionIDUInt = 0;
	unsigned int   ls_protoVer = 0;
	unsigned char  ls_RSA_pubKeyMod[128];
	unsigned char  ls_ggShit[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
	unsigned char  ls_newBFKey[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
	unsigned int   ls_ggAuthResponse = 0;
	unsigned char  ls_sessionKey1[8] = {0,0,0,0, 0,0,0,0};
	unsigned char  ls_sessionKey2[8] = {0,0,0,0, 0,0,0,0};
	unsigned char  ls_nGameServersCount = 0;
	unsigned char  ls_lastServerID = 0;
	int            ls_chosenGameServer = -1;
	// login server session vars memset
	memset( ls_RSA_pubKeyMod, 0, sizeof(ls_RSA_pubKeyMod) );
	memset( pcls->login_sessionKey1, 0, sizeof(pcls->login_sessionKey1) );
	memset( pcls->login_sessionKey2, 0, sizeof(pcls->login_sessionKey1) );
	pcls->login_selectedGsId = 0;
	memset( pcls->login_selectedGsIP, 0, sizeof(pcls->login_selectedGsIP) );
	pcls->login_selectedGsPort = 0;
	const int MAX_GAMESERVERS = 32;
	L2GameServerInfo ls_gameservers[MAX_GAMESERVERS];
	memset( ls_gameservers, 0, sizeof(ls_gameservers) );

	// packet objects
	L2LoginPacket              *pack = NULL;
	L2Login_Init               *p_init = NULL;
	L2Login_RequestGGAuth      *p_rgga = NULL;
	L2Login_GGAuthResponse     *p_ggar = NULL;
	L2Login_RequestAuthLogin   *p_ral = NULL;
	L2Login_LoginOK            *p_lok = NULL;
	L2Login_LoginFail          *p_lfail = NULL;
	L2Login_AccountKicked      *p_acckick = NULL;
	L2Login_RequestServerList  *p_rsl = NULL;
	L2Login_ServerList         *p_sl = NULL;
	L2Login_RequestServerLogin *p_rgsl  = NULL;
	L2Login_PlayOK             *p_pok   = NULL;
	L2Login_PlayFail           *p_pfail = NULL;

	// receive Init
	r = L2PacketReceive_buffer( pcls->sock, recvTimeoutMsec, &plen, packbuffer );
	if( r <= 0 ) goto netErrorRecv;
	if( plen != 186 )
	{
		log_error( LOG_ERROR, "L2ClientThread: recv Init: received %d bytes instead of %d! Maybe wrong loginserver!\n",
			plen, 186 );
		goto netErrorRecv;
	}
	p_init = new L2Login_Init();
	p_init->setBytes( packbuffer, plen );
	if( p_init->decodeBlowfish( true ) )
	{
		if( p_init->decodeXOR() )
		{
			p_init->read_SessionID( ls_sessionID );
			memcpy( &ls_sessionIDUInt, ls_sessionID, sizeof(ls_sessionID) );
			ls_protoVer = p_init->read_ProtoVer();
			p_init->read_RSA_pubKeyMod( ls_RSA_pubKeyMod );
			p_init->read_GGShit( ls_ggShit );
			p_init->read_DynamicBFKey( ls_newBFKey );
		}
		else log_error( LOG_ERROR, "L2ClientThread: XOR decode failed\n" );
	}
	else log_error( LOG_ERROR, "L2ClientThread: BF decode failed\n" );
	delete p_init;
	p_init = NULL;

	// reply RequestGGAuth
	p_rgga = new L2Login_RequestGGAuth();
	memcpy( p_rgga->sessionId, ls_sessionID, 4 );
	p_rgga->create();
	p_rgga->padPacketTo8ByteLen();
	p_rgga->appendChecksum( true );
	p_rgga->appendMore8Bytes();
	p_rgga->setDynamicBFKey( ls_newBFKey, 16 );
	p_rgga->encodeBlowfish( false );
	r = L2PacketSend2( p_rgga->getBytesPtr(), pcls->sock, sendTimeoutMsec, &plen );
	delete p_rgga;
	p_rgga = NULL;
	if( r == -1 ) goto netErrorSend;

	// recv GGAuthResponse
	r = L2PacketReceive_buffer( pcls->sock, recvTimeoutMsec, &plen, packbuffer );
	if( r <= 0 ) goto netErrorRecv;
	p_ggar = new L2Login_GGAuthResponse();
	p_ggar->setBytes( packbuffer, plen );
	p_ggar->setDynamicBFKey( ls_newBFKey, 16 );
	p_ggar->decodeBlowfish( false );
	ptype = p_ggar->getPacketType();
	if( ptype != 0x0B )
		log_error( LOG_WARNING, "L2ClientThread: login: Unknown GGAuthResponse code 0x%02X!!!!\n",
			(unsigned int)ptype );
	else
		log_error( LOG_OK, "L2ClientThread: login: GGAuthResponse: bypass GameGuard authorization...\n" );
	ls_ggAuthResponse = p_ggar->read_Response();
	if( ls_ggAuthResponse != ls_sessionIDUInt )
	{
		log_error( LOG_WARNING, "L2ClientThread: login: sessionID != ggAuthResponse (%04X != %04X)\n",
			ls_sessionIDUInt, ls_ggAuthResponse );
	}
	else
	{
		log_error( LOG_DEBUG, "L2ClientThread: login: sessionID == ggAuthResponse, good\n" );
		pcls->addChatToTab( L2Client::CHAT_SYS, L"lol gameguard :)" );
	}
	delete p_ggar;
	p_ggar = NULL;

	// convert charset
	char al2login[32], al2pass[32];
	memset( al2login, 0, sizeof(al2login) );
	memset( al2pass, 0, sizeof(al2pass) );
	WideCharToMultiByte( CP_ACP, 0, pcls->account.login, -1, al2login, 31, NULL, NULL );
	WideCharToMultiByte( CP_ACP, 0, pcls->account.pass, -1, al2pass, 31, NULL, NULL );
	log_error( LOG_DEBUG, "L2ClientThread: login.. [%s] [%s]\n", al2login, al2pass );
	L2Login_Init::unscramble_RSA_PubKeyMod( ls_RSA_pubKeyMod );
	// response with RequestAuthLogin
	p_ral = new L2Login_RequestAuthLogin();
	p_ral->create( al2login, al2pass, ls_ggAuthResponse, ls_RSA_pubKeyMod );
	p_ral->padPacketTo8ByteLen();
	p_ral->appendChecksum( true );
	p_ral->appendMore8Bytes();
	p_ral->setDynamicBFKey( ls_newBFKey, 16 );
	p_ral->encodeBlowfish( false );
	r = L2PacketSend2( p_ral->getBytesPtr(), pcls->sock, sendTimeoutMsec, &plen );
	delete p_ral;
	p_ral = NULL;
	if( r == -1 ) goto netErrorSend;

	// recv response to RequestAuthLogin
	// it will be LoginOK or LoginFail
	r = L2PacketReceive_buffer( pcls->sock, recvTimeoutMsec, &plen, packbuffer );
	if( r <= 0 ) goto netErrorRecv;
	pack = new L2LoginPacket( packbuffer, plen );
	pack->setDynamicBFKey( ls_newBFKey, 16 );
	pack->decodeBlowfish( false );
	ptype = pack->getPacketType();
	log_error( LOG_DEBUGDUMP, "L2ClientThread: response to RequestAuthLogin is 0x%02X\n", ptype );
	if( ptype == 0x03 ) // login ok
	{
		log_error( LOG_DEBUG, "L2ClientThread: Login OK\n" );
		p_lok = new L2Login_LoginOK( pack->getBytesPtr(), pack->getPacketSize() );
		p_lok->getPacketType();
		p_lok->read_sessionKey1( ls_sessionKey1 );
		delete p_lok;
		p_lok = NULL;
		pcls->addChatToTab( L2Client::CHAT_SYS, L"Login OK" );
		pcls->setState( STATE_AUTHED_LOGIN );
	}
	else if( ptype == 0x01 )
	{
		p_lfail = new L2Login_LoginFail( pack->getBytesPtr(), pack->getPacketSize() );
		p_lfail->getPacketType();
		unsigned int reason = p_lfail->read_reason();
		log_error( LOG_ERROR, "L2ClientThread: Login failed: 0x%02X\n", reason );
		wchar_t strReason[64] = {0};
		switch( reason )
		{
		case L2Login_LoginFail::REASON_SYSTEM_ERROR         : wcscpy( strReason, L"REASON_SYSTEM_ERROR" ); break;
		case L2Login_LoginFail::REASON_PASS_WRONG           : wcscpy( strReason, L"REASON_PASS_WRONG" ); break;
		case L2Login_LoginFail::REASON_USER_OR_PASS_WRONG   : wcscpy( strReason, L"REASON_USER_OR_PASS_WRONG" ); break;
		case L2Login_LoginFail::REASON_ACCESS_FAILED        : wcscpy( strReason, L"REASON_ACCESS_FAILED" ); break;
		case L2Login_LoginFail::REASON_ACCOUNT_IN_USE       : wcscpy( strReason, L"REASON_ACCOUNT_IN_USE" ); break;
		case L2Login_LoginFail::REASON_SERVER_OVERLOADED    : wcscpy( strReason, L"REASON_SERVER_OVERLOADED" ); break;
		case L2Login_LoginFail::REASON_SERVER_MAINTENANCE   : wcscpy( strReason, L"REASON_SERVER_MAINTENANCE" ); break;
		case L2Login_LoginFail::REASON_TEMP_PASS_EXPIRED    : wcscpy( strReason, L"REASON_TEMP_PASS_EXPIRED" ); break;
		case L2Login_LoginFail::REASON_DUAL_BOX             : wcscpy( strReason, L"REASON_DUAL_BOX" ); break;
		default                               : wcscpy( strReason, L"REASON_UNKNOWN" ); break;
		}
		wchar_t mes[128] = {0};
		wsprintfW( mes, L"Login FAILED! Reason: [%s]", strReason );
		pcls->addChatToTab( L2Client::CHAT_SYS, mes );
		delete p_lfail;
		p_lfail = NULL;
		delete pack;
		pack = NULL;
		goto netErrorRecv;
	}
	else if( ptype == 0x02 )
	{
		p_acckick = new L2Login_AccountKicked( pack->getBytesPtr(), pack->getPacketSize() );
		p_acckick->getPacketType();
		unsigned int reason = p_acckick->read_reason();
		log_error( LOG_ERROR, "L2ClientThread: Login failed: AccountKicked: 0x%02X\n", reason );
		wchar_t strReason[64] = {0};
		switch( reason )
		{
		case L2Login_AccountKicked::REASON_DATA_STEALER       : wcscpy( strReason, L"REASON_DATA_STEALER" ); break;
		case L2Login_AccountKicked::REASON_GENERIC_VIOLATION  : wcscpy( strReason, L"REASON_GENERIC_VIOLATION" ); break;
		case L2Login_AccountKicked::REASON_7_DAYS_SUSPENDED   : wcscpy( strReason, L"REASON_7_DAYS_SUSPENDED" ); break;
		case L2Login_AccountKicked::REASON_PERMANENTLY_BANNED : wcscpy( strReason, L"REASON_PERMANENTLY_BANNED" ); break;
		default                             : wcscpy( strReason, L"REASON_UNKNOWN" ); break;
		}
		wchar_t mes[128] = {0};
		wsprintfW( mes, L"Login FAILED! Reason: [%s]", strReason );
		pcls->addChatToTab( L2Client::CHAT_SYS, mes );
		delete p_acckick;
		p_acckick = NULL;
		delete pack;
		pack = NULL;
		goto netErrorRecv;
	}
	else
	{
		wchar_t mes[128] = {0};
		wsprintfW( mes, L"Login failed: Unknown response: 0x%02X\n", ptype );
		pcls->addChatToTab( L2Client::CHAT_SYS, mes );
		log_error( LOG_ERROR, "L2ClientThread: Login failed: Unknown response: 0x%02X\n", ptype );
		delete pack;
		pack = NULL;
		goto netErrorRecv;
	}
	delete pack;
	pack = NULL;

	// request server list
	p_rsl = new L2Login_RequestServerList();
	p_rsl->create( ls_sessionKey1 );
	p_rsl->padPacketTo8ByteLen();
	p_rsl->appendChecksum( true );
	p_rsl->appendMore8Bytes();
	p_rsl->setDynamicBFKey( ls_newBFKey, 16 );
	p_rsl->encodeBlowfish( false );
	r = L2PacketSend2( p_rsl->getBytesPtr(), pcls->sock, sendTimeoutMsec, &plen );
	delete p_rsl;
	p_rsl = NULL;
	if( r == -1 ) goto netErrorSend;

	// receive server list
	r = L2PacketReceive_buffer( pcls->sock, recvTimeoutMsec, &plen, packbuffer );
	if( r <= 0 ) goto netErrorRecv;
	p_sl = new L2Login_ServerList( packbuffer, plen );
	p_sl->setDynamicBFKey( ls_newBFKey, 16 );
	p_sl->decodeBlowfish( false );
	p_sl->read_header( &ls_nGameServersCount, &ls_lastServerID );
	for( i=0; i<ls_nGameServersCount; i++ )
	{
		p_sl->read_next_GS_Info( &ls_gameservers[i] );
		log_error( LOG_DEBUGDUMP, "GS #%d: ", (int)ls_gameservers[i].gsID );
		log_error_np( LOG_DEBUGDUMP, "Addr: %d.%d.%d.%d : %d;   ",
			(int)ls_gameservers[i].gsIP[0], (int)ls_gameservers[i].gsIP[1],
			(int)ls_gameservers[i].gsIP[2], (int)ls_gameservers[i].gsIP[3],
			(int)ls_gameservers[i].gsPort );
		log_error_np( LOG_DEBUGDUMP, "Online: %d / %d\n",
			(int)ls_gameservers[i].gsPlayersOnline, (int)ls_gameservers[i].gsPlayersMax );
	}
	delete p_sl;
	p_sl = NULL;

	pcls->addChatToTab( L2Client::CHAT_SYS, L"Received ServerList" );

	// now we must choose server
	// in manual mode, run dialog
	// in auto mode, use accoutn setting
	if( pcls->account.gameserverSelectManual )
	{
		int sel_idx = ChooseServer( g_hWnd, ls_gameservers, ls_nGameServersCount );
		if( sel_idx == -1 ) goto normalEnd;
		ls_chosenGameServer = ls_gameservers[ sel_idx ].gsID;
		log_error( LOG_DEBUG, "L2ClientThread: login: manual select game server idx[%d]: id = %d\n",
			sel_idx, ls_chosenGameServer );
		sprintf( pcls->login_selectedGsIP, "%d.%d.%d.%d",
			ls_gameservers[ sel_idx ].gsIP[0], ls_gameservers[ sel_idx ].gsIP[1],
			ls_gameservers[ sel_idx ].gsIP[2], ls_gameservers[ sel_idx ].gsIP[3] );
		pcls->login_selectedGsPort = (int)ls_gameservers[ sel_idx ].gsPort;
	}
	else
	{
		ls_chosenGameServer = pcls->account.gameserverID;
		log_error( LOG_DEBUG, "L2ClientThread: login: auto select game server id %d\n",
			ls_chosenGameServer );
		pcls->login_selectedGsId = ls_chosenGameServer;
		int idx = -1;
		for( i=0; i<ls_nGameServersCount; i++ )
		{
			if( ls_gameservers[i].gsID == ls_chosenGameServer ) idx = i;
		}
		if( idx >= 0 )
		{
			sprintf( pcls->login_selectedGsIP, "%d.%d.%d.%d",
				ls_gameservers[ idx ].gsIP[0], ls_gameservers[ idx ].gsIP[1],
				ls_gameservers[ idx ].gsIP[2], ls_gameservers[ idx ].gsIP[3] );
			pcls->login_selectedGsPort = (int)ls_gameservers[ idx ].gsPort;
		}
	}

	// request server login
	p_rgsl = new L2Login_RequestServerLogin();
	p_rgsl->create( ls_sessionKey1, (unsigned char)(ls_chosenGameServer & 0x000000ff) );
	p_rgsl->padPacketTo8ByteLen();
	p_rgsl->appendChecksum( true );
	p_rgsl->appendMore8Bytes();
	p_rgsl->setDynamicBFKey( ls_newBFKey, 16 );
	p_rgsl->encodeBlowfish( false );
	r = L2PacketSend2( p_rgsl->getBytesPtr(), pcls->sock, sendTimeoutMsec, &plen );
	delete p_rgsl;
	p_rgsl = NULL;
	if( r == -1 ) goto netErrorSend;

	// recv PalyOK / PlayFail
	r = L2PacketReceive_buffer( pcls->sock, recvTimeoutMsec, &plen, packbuffer );
	if( r <= 0 ) goto netErrorRecv;
	pack = new L2LoginPacket( packbuffer, plen );
	pack->setDynamicBFKey( ls_newBFKey, 16 );
	pack->decodeBlowfish( false );
	ptype = pack->getPacketType();
	if( ptype == 0x07 )
	{
		log_error( LOG_DEBUG, "L2ClientThread: login: PlayOK\n" );
		pcls->addChatToTab( L2Client::CHAT_SYS, L"Play OK" );
		p_pok = new L2Login_PlayOK( pack->getBytesPtr(), pack->getPacketSize() );
		p_pok->getPacketType();
		p_pok->read_sessionKey2( ls_sessionKey2 );
		delete p_pok;
		p_pok = NULL;
	}
	else if( ptype == 0x06 )
	{
		p_pfail = new L2Login_PlayFail( pack->getBytesPtr(), pack->getPacketSize() );
		p_pfail->getPacketType();
		unsigned int reason = (unsigned int)p_pfail->read_reason();
		log_error( LOG_ERROR, "L2ClientThread: login: PlayFail: 0x%02X\n", reason );
		wchar_t msg[128];
		wchar_t strReason[128] = {0};
		switch( reason )
		{
		case L2Login_PlayFail::REASON_SYSTEM_ERROR       : wcscpy( strReason, L"REASON_SYSTEM_ERROR" ); break;
		case L2Login_PlayFail::REASON_USER_OR_PASS_WRONG : wcscpy( strReason, L"REASON_USER_OR_PASS_WRONG" ); break;
		case L2Login_PlayFail::REASON3                   : wcscpy( strReason, L"REASON3" ); break;
		case L2Login_PlayFail::REASON_ACCESS_FAILED      : wcscpy( strReason, L"REASON_ACCESS_FAILED" ); break;
		case L2Login_PlayFail::REASON_TOO_MANY_PLAYERS   : wcscpy( strReason, L"REASON_TOO_MANY_PLAYERS" ); break;
		default: wcscpy( strReason, L"REASON_UNKNOWN" ); break;
		}
		wsprintfW( msg, L"PlayFail: %s", strReason );
		pcls->addChatToTab( L2Client::CHAT_SYS, msg );
		delete p_pfail;
		p_pfail = NULL;
		delete pack;
		pack = NULL;
		goto netErrorRecv;
	}
	else
	{
		log_error( LOG_DEBUG, "L2ClientThread: login: unknown response 0x%02X to RequestServerLogin\n",
			(int)ptype );
		wchar_t msg[128];
		wsprintfW( msg, L"Unknown response 0x%02X to RequestServerLogin\n", (int)ptype );
		pcls->addChatToTab( L2Client::CHAT_SYS, msg );
		delete pack;
		pack = NULL;
		goto netErrorRecv;
	}
	delete pack;
	pack = NULL;

	L2PNet_shutdown( pcls->sock );
	L2PNet_closesocket( pcls->sock );
	pcls->sock = INVALID_SOCKET;

	// save session vars
	memcpy( pcls->login_sessionKey1, ls_sessionKey1, sizeof(ls_sessionKey1) );
	memcpy( pcls->login_sessionKey2, ls_sessionKey2, sizeof(ls_sessionKey2) );

	was_error = false;
	goto normalEnd;
	// error handlers
netErrorRecv:
	was_error = true;
	log_error( LOG_ERROR, "L2ClientThread: recv failed; some network error?\n" );
	pcls->addChatToTab( CHAT_SYS, L"Network error! (recv)" );
	goto normalEnd;
netErrorSend:
	was_error = true;
	log_error( LOG_ERROR, "L2ClientThread: send failed; some network error?\n" );
	pcls->addChatToTab( CHAT_SYS, L"Network error! (send)" );
normalEnd:
	free( packbuffer );
	packbuffer = NULL;
	pcls->threadNetCleanup( pcls );
	pcls->addChatToTab( CHAT_SYS, L"Login connection closed." );
	if( was_error ) return false;
	if( ls_chosenGameServer == -1 ) return false;
	return true;
}

