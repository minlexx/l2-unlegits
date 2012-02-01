#include "stdafx.h"
#include "ConfigIni.h"
#include "Logger.h"
#include "LoginClient.h"

extern class CConfig g_cfg;

/**
 * Function replaces IP and port of server with ID = serverID
 *   in ServerList packet.
 * Usage:
 *   unsigned char IP_new  = {127,0,0,1};
 *   int port_new          = 7777;
 *   unsigned int serverID = 1;
 *   bool ret = FL_ReplaceServerIPPort( rawBytes, rawBytesLen, serverID, IP_new, port_new );
 * 
 * returns true on success, false on error
 */
bool FL_ReplaceServerIPPort( unsigned char *b, unsigned int blen )
{
	if( !b || (blen<26) ) return false;
	unsigned char  fake_gsip[4] = {0,0,0,0};
	unsigned long lInetAddr = L2PNet_inet_addr( g_cfg.FakeListenGameIP );
	memcpy( (void *)&fake_gsip, (const void *)&lInetAddr, sizeof(lInetAddr) );
	unsigned short int sport = (unsigned short int)g_cfg.FakeListenGamePort;
	unsigned char cport[2] = { 
		(unsigned char)( sport & 0x00FF),
		(unsigned char)((sport & 0xFF00) >> 8)
	};
	int gscount = (int)b[3];
	const int ServerList_Header_Size = 5;
	const int ServerList_OneServerInfo_Size = 21;
	int pos = ServerList_Header_Size; // pass packet header
	int i;
	//while( pos < (int)blen )
	log_error( LOG_DEBUG, "FL_ReplaceServerIPPort(): replacing IPs and ports for %d servers in list\n", gscount );
	for( i=0; i<gscount; i++ )
	{
		pos = ServerList_Header_Size + (i * ServerList_OneServerInfo_Size);
		//if( b[pos] == serverID )
		//{
		// found!
		//log_error( LOG_DEBUG, "FL_ReplaceServerIPPort(): found server ID %d at offset %d\n",
		//	(int)serverID, pos );
		// b[pos] is ServerID
		// set new IP
		b[pos+1] = fake_gsip[0];
		b[pos+2] = fake_gsip[1];
		b[pos+3] = fake_gsip[2];
		b[pos+4] = fake_gsip[3];
		// set new port
		b[pos+5] = cport[0];
		b[pos+6] = cport[1];
		b[pos+7] = 0x00;
		b[pos+8] = 0x00;
		//return true;
		//}
		//else
		//pos += ServerList_OneServerInfo_Size; // go to next serverID...
	}
	//log_error( LOG_WARNING, "FL_ReplaceServerIPPort(): not found server ID %d",
	//	(int)serverID );
	//return false;
	return true;
}

bool FL_RecalculateChechksum(
		unsigned char *b,
		unsigned int   blen )
{
	if( !b || (blen<26) ) return false; // some error?
	L2LoginPacket lp;
	unsigned int newlen = 0;
	// set bytes, copy all except last 4 bytes - old checksum
	if( !lp.setBytes( b, blen-4 ) )
	{
		log_error( LOG_ERROR,
			"FL_RecalculateChechksum(): L2LoginPacket::setBytes( 0x%p, %u ) failed!\n",
				b, blen-4 );
		return false;
	}
	// append new checksum
	lp.appendChecksum( false ); // do not append 4 bytes
	// check resulting packet length - must be as original
	newlen = lp.getPacketSize();
	if( newlen == blen )
	{
		log_error( LOG_DEBUG, "FL_RecalculateChechksum(): newlen == blen, all OK\n" );
		memcpy( b, lp.getBytesPtr(), newlen );
		return true;
	}
	//else
	log_error( LOG_ERROR, "FL_RecalculateChechksum(): newlen (%u) != oldlen (%u)\n", newlen, blen );
	return false;
}

/**
 * Was only for debugging
void GenerateFakePacket( L2Login_ServerList *p_sl )
{
	if( !p_sl ) return;
	// ugly! but this packet is known sure to work!
	unsigned char b[] = {
		0x22, 0x00,             // length
		0x04, 0x01, 0x01,       // ptype, lastServer, nServers
		0x01,                   // serverID
		0x7F, 0x00, 0x00, 0x01, // serverIP
		0x61, 0x1E, 0x00, 0x00, // serverPort 
		0x00,                   // age limit
		0x01,                   // is PvP
		0x00, 0x00,             // current online
		0x0A, 0x00,             // max online
		0x01,                   // is up
		0x00, 0x00, 0x00, 0x00, 0x00, // flags...
		0xA5, 0x4A, 0xAA, 0xCE, // <-- WTF is here????
		0xB5, 0x54, 0xAA, 0xCE }; // here must be checksum
	p_sl->setBytes( b, sizeof(b) );
}
*/

LoginClient::LoginClient()
{
	memset( sessionKey1, 0, sizeof(sessionKey1) );
	memset( sessionKey2, 0, sizeof(sessionKey2) );
	gsIP[0] = gsIP[1] = gsIP[2] = gsIP[3] = 0;
	gsPort = 0;
	g_login_client = this;
	state = LCST_OFFLINE;
}

LoginClient::~LoginClient()
{
	g_login_client = NULL;
	memset( sessionKey1, 0, sizeof(sessionKey1) );
	memset( sessionKey2, 0, sizeof(sessionKey2) );
	gsIP[0] = gsIP[1] = gsIP[2] = gsIP[3] = 0;
	gsPort = 0;
	state = LCST_OFFLINE;
}

bool LoginClient::getSessionKeys( unsigned char *sKey1, unsigned char *sKey2 )
{
	if( !sKey1 || !sKey2 ) return false;
	memcpy( sKey1, this->sessionKey1, 8 );
	memcpy( sKey2, this->sessionKey2, 8 );
	return true;
}

bool LoginClient::getPlayServerAddress( unsigned char *ip, unsigned short int *port )
{
	if( !ip || !port ) return false;
	memcpy( ip, this->gsIP, sizeof(this->gsIP) );
	(*port) = this->gsPort;
	return true;
}

bool LoginClient::ProcessClient( SOCKET scl )
{
	bool retVal = false;
	state = LCST_OFFLINE;

	// connect to real login server
	log_error( LOG_DEBUG, "Establishing redirecting connection to %s:%d ",
		g_cfg.RealLoginServerIP, g_cfg.RealLoginServerPort );

	SOCKET sr = L2PNet_TCPsocket_create( true );
	if( sr == INVALID_SOCKET )
	{
		log_error_np( LOG_ERROR, "\n" );
		log_error( LOG_ERROR, "ProcessClient(): cannot create socket!\n" );
		return false;
	}

	L2PNet_connect( sr, g_cfg.RealLoginServerIP, (unsigned short)g_cfg.RealLoginServerPort );
	int nTries = 20;
	int r = 0;
	while( nTries > 0 )
	{
		log_error_np( LOG_DEBUG, "." );
		r = L2PNet_select( sr, L2PNET_SELECT_WRITE, 20000, NULL, NULL );
		if( r == 1 ) break;
		nTries--;
	}
	
	if( r == 1 )
	{
		log_error_np( LOG_DEBUG, " Connected. Tries left: %d\n", nTries );
		state = LCST_CONNECTED;
	}
	else
	{
		state = LCST_OFFLINE;
		// close sockets
		L2PNet_shutdown( scl );
		L2PNet_closesocket( scl );
		L2PNet_shutdown( sr );
		L2PNet_closesocket( sr );
		log_error_np( LOG_DEBUG, " Not connected.\n" );
		return false;
	}

	this->postNotify( LCN_STATECHANGE, 1 ); // connected
	
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
	unsigned char  ls_lastServerID = 0; // last server ID
	unsigned char  myGameServerIP[4] = {0,0,0,0};
	//char           sMyGSIP[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
	unsigned short int myGameServerPort = 0;
	int            ls_chosenGameServer = -1;
	// login server session vars memset
	memset( ls_RSA_pubKeyMod, 0, sizeof(ls_RSA_pubKeyMod) );
	
	// network vars
	int            netTimeout = 30;
	int            i = 0;
	unsigned char  ptype = 0; // packet type byte
	unsigned char *rraw = NULL; // raw packet bytes
	unsigned int   rcvdLen = 0;
	unsigned int   sentLen = 0;
	
	// packet objects
	L2LoginPacket              *pack = NULL;
	L2Login_Init               *p_init = NULL;
	//L2Login_RequestGGAuth      *p_rgga = NULL;
	L2Login_GGAuthResponse     *p_ggar = NULL;
	L2Login_LoginOK            *p_lok = NULL;
	//L2Login_LoginFail          *p_lfail = NULL;
	L2Login_ServerList         *p_sl = NULL;
	L2Login_RequestServerLogin *p_rgsl  = NULL;
	L2Login_PlayOK             *p_pok   = NULL;
	//L2Login_PlayFail           *p_pfail = NULL;
	
	// ServerInfo struct
	struct L2GameServerInfo gsi;
	memset( &gsi, 0, sizeof(gsi) );
	struct L2GameServerInfo gameservers_info[32];
	memset( gameservers_info, 0, sizeof(gameservers_info) );
	
	// debug file
	//FILE *f = NULL;
	
	// Init: receive Init packet from real login server
	rcvdLen = 0;
	rraw = L2PacketReceive_malloc( sr, netTimeout*1000, &rcvdLen );
	if( !rraw ) { log_error( LOG_ERROR, "receive failed!\n" ); goto l_netError; }
	log_error( LOG_DEBUG, "ProcessClient(): rcvd %u bytes of Init packet from server.\n", rcvdLen );
	// resend it to client
	sentLen = 0;
	L2PacketSend2( rraw, scl, netTimeout*1000, &sentLen );
	log_error( LOG_DEBUG, "ProcessClient(): resent %u bytes of Init packet to client.\n", sentLen );
	if( sentLen != rcvdLen ) { log_error( LOG_ERROR, "rcvd != sent! (%u != %u)\n", rcvdLen, sentLen ); goto l_netError; }
	
	// decode and parse server's Init packet
	p_init = new L2Login_Init( rraw, rcvdLen );
	if( p_init->decodeBlowfish( true ) ) // true - Use Static BF Key
	{
		if( p_init->decodeXOR() )
		{
			// read packet data
			p_init->read_SessionID( ls_sessionID );
			memcpy( &ls_sessionIDUInt, ls_sessionID, sizeof(ls_sessionID) );
			ls_protoVer = p_init->read_ProtoVer();
			p_init->read_RSA_pubKeyMod( ls_RSA_pubKeyMod );
			p_init->read_GGShit( ls_ggShit );
			p_init->read_DynamicBFKey( ls_newBFKey );
			// printfs
			log_error( LOG_DEBUGDUMP, "SessionID: %02X %02X %02X %02X\n",
				ls_sessionID[0], ls_sessionID[1], ls_sessionID[2], ls_sessionID[3] );
			log_error( LOG_DEBUGDUMP, "Protocol ver: %08X\n", ls_protoVer );
			p_init->unscramble_RSA_PubKeyMod( ls_RSA_pubKeyMod );
			log_error( LOG_DEBUGDUMP, "Dynamic Blowfish key: " );
			i = 0;
			while( i < 16 )	log_error_np( LOG_DEBUGDUMP, "%02X ", ls_newBFKey[i++] );
			log_error_np( LOG_DEBUGDUMP, "\n" );
		}
		else log_error( LOG_ERROR, "XOR decode failed\n" );
	}
	else log_error( LOG_ERROR, "BF decode failed\n" );
	delete p_init;
	p_init = NULL;
	// free raw pack buffer
	free( rraw );
	rraw = NULL;
	
	// RequestGGAuth
	// receive client's answer to server's Init packet. probably it is RequestGGAuth packet
	rcvdLen = 0;
	rraw = L2PacketReceive_malloc( scl, netTimeout*1000, &rcvdLen );
	if( !rraw ) { log_error( LOG_ERROR, "receive failed!\n" ); goto l_netError; }
	log_error( LOG_DEBUG, "ProcessClient(): rcvd %u bytes of RequestGGAuth packet from client.\n", rcvdLen );
	// resend it to server
	sentLen = 0;
	L2PacketSend2( rraw, sr, netTimeout*1000, &sentLen );
	log_error( LOG_DEBUG, "ProcessClient(): resent %u bytes of RequestGGAuth packet to server.\n", sentLen );
	if( sentLen != rcvdLen ) { log_error( LOG_ERROR, "rcvd != sent! (%u != %u)\n", rcvdLen, sentLen ); goto l_netError; }
	// try to decode and parse client's response
	//p_rgga = new L2Login_RequestGGAuth( rraw, rcvdLen );
	//p_rgga->setDynamicBFKey( ls_newBFKey, 16 );
	//p_rgga->decodeBlowfish( false );
	//p_rgga->dumpToFile( stdout );
	//delete p_rgga;
	//p_rgga = NULL;
	// free raw pack buffer
	free( rraw );
	rraw = NULL;
	
	// GGAuthResponse
	// receive server's answer to client's RequstGGAuth packet
	rcvdLen = 0;
	rraw = L2PacketReceive_malloc( sr, netTimeout*1000, &rcvdLen );
	if( !rraw ) { log_error( LOG_ERROR, "receive failed!\n" ); goto l_netError; }
	log_error( LOG_DEBUG, "ProcessClient(): rcvd %u bytes of GGAuthResponse packet from server.\n", rcvdLen );
	// resend it to client
	sentLen = 0;
	L2PacketSend2( rraw, scl, netTimeout*1000, &sentLen );
	log_error( LOG_DEBUG, "ProcessClient(): resent %u bytes of GGAuthResponse packet to client.\n", sentLen );
	if( sentLen != rcvdLen ) { log_error( LOG_ERROR, "rcvd != sent! (%u != %u)\n", rcvdLen, sentLen ); goto l_netError; }
	p_ggar = new L2Login_GGAuthResponse( rraw, rcvdLen );
	//if( p_ggar ) printf( "GGAuthResponse object created\n" );
	//printf( "Setting bytes 0x%p %u len\n", rraw, rcvdLen );
	//if( p_ggar->setBytes( rraw, rcvdLen ) )
	//	printf( "ProcessClient(): GGAuthResponse bytes set OK\n" );
	p_ggar->setDynamicBFKey( ls_newBFKey, 16 );
	p_ggar->decodeBlowfish( false );
	ptype = p_ggar->getPacketType();
	if( ptype != 0x0B )
		log_error( LOG_WARNING, "Unknown GGAuthResponse code 0x%02X!!!!\n",
			(unsigned int)ptype );
	else
		log_error( LOG_OK, "GGAuthResponse: bypass GameGuard authorization...\n" );
	ls_ggAuthResponse = p_ggar->read_Response();
	if( ls_ggAuthResponse != ls_sessionIDUInt )
	{
		log_error( LOG_WARNING, "sessionID != ggAuthResponse (%04X != %04X)\n",
			ls_sessionIDUInt, ls_ggAuthResponse );
	}
	else log_error( LOG_DEBUG, "++++ SessionID == ggAuthResponse ++++\n" );
	delete p_ggar;
	p_ggar = NULL;
	// free raw pack buffer
	free( rraw );
	rraw = NULL;

	state = LCST_AUTHED_GG;
	
	// RequestAuthLogin
	// client must now attempt to login to server with its login and password
	rcvdLen = 0;
	rraw = L2PacketReceive_malloc( scl, netTimeout*1000, &rcvdLen );
	if( !rraw ) { log_error( LOG_ERROR, "receive failed!\n" ); goto l_netError; }
	log_error( LOG_DEBUG, "ProcessClient(): rcvd %u bytes of RequestAuthLogin packet from client.\n", rcvdLen );
	// resend it to server
	sentLen = 0;
	//L2PacketSend( rraw, sr, netTimeout, 0, &sentLen );
	L2PacketSend2( rraw, sr, netTimeout*1000, &sentLen );
	log_error( LOG_DEBUG, "ProcessClient(): resent %u bytes of RequestAuthLogin packet to server.\n", sentLen );
	if( sentLen != rcvdLen ) { log_error( LOG_ERROR, "rcvd != sent! (%u != %u)\n", rcvdLen, sentLen ); goto l_netError; }
	// display RequestAuthLogin contents
	//pack = new L2LoginPacket();
	//pack->setBytes( rraw, sentLen );
	//pack->setDynamicBFKey( ls_newBFKey, 16 );
	//pack->decodeBlowfish( false );
	//pack->dumpToFile( stdout );
	//delete pack;
	//pack = NULL;
	// free raw pack buffer
	free( rraw );
	rraw = NULL;
	
	// LoginOK / LoginFail
	// Login server must now reply to RequestAuthLogin and tell us about result
	rcvdLen = 0;
	rraw = L2PacketReceive_malloc( sr, netTimeout*1000, &rcvdLen );
	if( !rraw ) { log_error( LOG_ERROR, "receive failed!\n" ); goto l_netError; }
	log_error( LOG_DEBUG, "ProcessClient(): rcvd %u bytes of response to RequestAuthLogin from server.\n", rcvdLen );
	// resend it to client
	sentLen = 0;
	L2PacketSend2( rraw, scl, netTimeout*1000, &sentLen );
	log_error( LOG_DEBUG, "ProcessClient(): resent %u bytes of response to client.\n", sentLen );
	if( sentLen != rcvdLen ) { log_error( LOG_ERROR, "rcvd != sent! (%u != %u)\n", rcvdLen, sentLen ); goto l_netError; }
	log_error( LOG_DEBUG, "Server answer to RequestAuthLogin:\n" );
	pack = new L2LoginPacket( rraw, rcvdLen );
	pack->setDynamicBFKey( ls_newBFKey, 16 );
	pack->decodeBlowfish( false );
	//pack->dumpToFile( stdout );
	ptype = pack->getPacketType();
	if( ptype == 0x03 )
	{
		log_error( LOG_DEBUG, "++++ Login OK! ++++\n" );
		state = LCST_AUTHED_LOGIN;
		p_lok = new L2Login_LoginOK();
		p_lok->setBytes( pack->getBytesPtr(), pack->getPacketSize() );
		p_lok->read_sessionKey1( ls_sessionKey1 );
		delete p_lok;
		p_lok = NULL;
		log_error( LOG_DEBUGDUMP, "SessionKey1: " );
		for( i=0; i<sizeof(ls_sessionKey1); i++ )
			log_error_np( LOG_DEBUGDUMP, "%02X ", ls_sessionKey1[i] );
		log_error_np( LOG_DEBUGDUMP, "\n" );
	}
	else if( ptype == 0x01 )
	{
		state = LCST_AUTH_ERROR;
		log_error( LOG_ERROR, "---- Login failed! (invalid user/pass? server offline?) ----\n" );
		goto l_netError;
	}
	else if( ptype == 0x02 )
	{
		state = LCST_AUTH_ERROR;
		log_error( LOG_ERROR, "---- Login failed! (account banned?)----\n" );
		goto l_netError;
	}
	else
		log_error( LOG_WARNING, "???? unknown reply to RequestAuthLogin: packet type: 0x%02X\n",
			(unsigned int)ptype );
	delete pack;
	pack = NULL;
	// free raw pack buffer
	free( rraw );
	rraw = NULL;
	
	// RequestServerList
	// after receiving packet LoginOK client displays license agreement
	// and after user clicks 'Agree' client sends RequestServerList packet
	rcvdLen = 0;
	rraw = L2PacketReceive_malloc( scl, netTimeout*1000, &rcvdLen );
	if( !rraw ) { log_error( LOG_ERROR, "receive failed!\n" ); goto l_netError; }
	log_error( LOG_DEBUG, "ProcessClient(): rcvd %u bytes of RequestServerList packet from client.\n", rcvdLen );
	// resend it to server
	sentLen = 0;
	L2PacketSend2( rraw, sr, netTimeout*1000, &sentLen );
	log_error( LOG_DEBUG, "ProcessClient(): resent %u bytes of RequestServerList packet to server.\n", sentLen );
	if( sentLen != rcvdLen ) { log_error( LOG_ERROR, "rcvd != sent! (%u != %u)\n", rcvdLen, sentLen ); goto l_netError; }
	// dump info about RequestServerList packet
	// [cut]
	// free raw pack buffer
	free( rraw );
	rraw = NULL;
	
	// ServerList
	// login server now sends to client list of Game servers
	rcvdLen = 0;
	rraw = L2PacketReceive_malloc( sr, netTimeout*1000, &rcvdLen );
	if( !rraw ) { log_error( LOG_ERROR, "receive failed!\n" ); goto l_netError; }
	log_error( LOG_DEBUG, "ProcessClient(): rcvd %u bytes of ServerList packet from server.\n", rcvdLen );
	// TODO: ServerList: NO, do not resend! :) modify it!!!
	// resend it to client
	/*sentLen = 0;
	L2PacketSend( rraw, scl, 5, 0, &sentLen );
	log_error( LOG_DEBUG, "ProcessClient(): resent %u bytes of ServerList packet to client.\n", sentLen );
	if( sentLen != rcvdLen ) { log_error( LOG_ERROR, "rcvd != sent! (%u != %u)\n", rcvdLen, sentLen ); goto l_netError; }*/
	
	log_error( LOG_DEBUG, "ServerList packet:\n" );
	p_sl = new L2Login_ServerList();
	p_sl->setBytes( rraw, rcvdLen );
	// out RAW
	//printf( "RAW ServerList packet:\n" );
	//p_sl->dumpToFile( stdout );
	p_sl->setDynamicBFKey( ls_newBFKey, 16 );
	p_sl->decodeBlowfish( false );
	// out BF decoded
	//printf( "BF Decoded ServerList packet:\n" );
	//p_sl->dumpToFile( stdout );
	// save to file :)
	//f = fopen( "ServerList.txt", "wt" );
	//if( f ) p_sl->dumpToFile( f );
	//fclose( f );
	// parse
	ptype = p_sl->getPacketType();
	if( ptype == 0x04 ) log_error( LOG_DEBUG, "++++ ServerList ++++\n" );
	p_sl->read_header( &ls_nGameServersCount, &ls_lastServerID );
	log_error_np( LOG_DEBUGDUMP, "\n" );
	log_error( LOG_DEBUGDUMP, "ServerList: %d game servers, last server: %d\n",
		(int)ls_nGameServersCount, (int)ls_lastServerID );
	for( i=0; i<ls_nGameServersCount; i++ )
	{
		memset( &gsi, 0, sizeof(gsi) );
		if( p_sl->read_next_GS_Info( &gsi ) )
		{
			log_error( LOG_DEBUGDUMP, "*** Server %d Info ***\n", i );
			log_error( LOG_DEBUGDUMP, "*** Server ID: %d\n", (int)gsi.gsID );
			log_error( LOG_DEBUGDUMP, "*** Server addr: %d.%d.%d.%d:%d\n",
				(int)gsi.gsIP[0],
				(int)gsi.gsIP[1],
				(int)gsi.gsIP[2],
				(int)gsi.gsIP[3],
				(int)gsi.gsPort );
			log_error( LOG_DEBUGDUMP, "*** Online: %d / %d (up: %d)\n", (int)gsi.gsPlayersOnline,
				(int)gsi.gsPlayersMax, (int)gsi.gsIsUp );
			log_error_np( LOG_DEBUGDUMP, "\n" );
			// save this gameserver in array
			memcpy( (&gameservers_info[i]), &gsi, sizeof(L2GameServerInfo) );
			// is this GS chosen to play on? // ignore
			/*if( ((int)gsi.gsID) == g_cfg.PlayGameServerNo )
			{
				log_error( LOG_DEBUGDUMP, "^^^^ we play here! ^^^^\n\n" );
				memcpy( myGameServerIP, gsi.gsIP, 4 );
				myGameServerPort = gsi.gsPort;
			}*/
		}
		else
		{
			log_error( LOG_ERROR, "p_sl->read_next_GS_Info( &gsi ) failed!\n" );
		}
	}
	
	// replace server IP and port to what we need to intercept GS connection
	if( FL_ReplaceServerIPPort(
		(unsigned char *)p_sl->getBytesPtr(),
		p_sl->getPacketSize() ) )
	{
		log_error( LOG_DEBUG, "[++++] OK: ServerList packet modified\n" );
		// packet modified, we must update checksum in it
		if( FL_RecalculateChechksum(
			(unsigned char *)p_sl->getBytesPtr(),
			p_sl->getPacketSize() ) )
		{
			log_error( LOG_DEBUGDUMP, "[++++] OK: ServerList packet checksum recalculated\n" );
		}
		else
		{
			log_error( LOG_ERROR, "ServerList packet checksum recalculate error!\n" );
		}
	}
	else log_error( LOG_ERROR, "ServerList packet modify error\n" );
	//GenerateFakePacket( p_sl ); // was called for debug only
	
	//printf( "================\n" );
	//printf( "[I] ServerList packet after replacing its IP & port:\n" );
	//p_sl->dumpToFile( stdout );
	//printf( "=================\n" );
	
	// encode modified packet again
	p_sl->encodeBlowfish( false );
	// out encoded
	//printf( "BF Encoded ServerList packet:\n" );
	//p_sl->dumpToFile( stdout );
	//rcvdLen = p_sl->getPacketSize();
	// resend it to client
	log_error( LOG_DEBUG, "Now resend %u bytes of ServerList to client...\n", rcvdLen );
	sentLen = 0;
	//L2PacketSend( p_sl->getBytesPtr(), scl, netTimeout, 0, &sentLen );
	L2PacketSend2( p_sl->getBytesPtr(), scl, netTimeout*1000, &sentLen );
	log_error( LOG_DEBUG, "ProcessClient(): resent %u bytes of ServerList packet to client.\n", sentLen );
	if( sentLen != rcvdLen ) { log_error( LOG_ERROR, "rcvd != sent! (%u != %u)\n", rcvdLen, sentLen ); goto l_netError; }
	delete p_sl;
	p_sl = NULL;
	// free raw pack buffer
	free( rraw );
	rraw = NULL;
	
	// RequestServerLogin
	// client now must request server login
	rcvdLen = 0;
	//rraw = L2PacketReceive( scl, netTimeout, 0, &rcvdLen );
	rraw = L2PacketReceive_malloc( scl, netTimeout*1000, &rcvdLen );
	if( !rraw ) { log_error( LOG_ERROR, "receive failed!\n" ); goto l_netError; }
	log_error( LOG_DEBUG, "ProcessClient(): rcvd %u bytes of RequestServerLogin packet from client.\n", rcvdLen );
	// resend it to server
	sentLen = 0;
	//L2PacketSend( rraw, sr, netTimeout, 0, &sentLen );
	L2PacketSend2( rraw, sr, netTimeout*1000, &sentLen );
	log_error( LOG_DEBUG, "ProcessClient(): resent %u bytes of RequestServerLogin packet to server.\n", sentLen );
	if( sentLen != rcvdLen ) { log_error( LOG_ERROR, "rcvd != sent! (%u != %u)\n", rcvdLen, sentLen ); goto l_netError; }
	// dump info about RequestServerLogin packet
	p_rgsl = new L2Login_RequestServerLogin( rraw, rcvdLen );
	p_rgsl->setDynamicBFKey( ls_newBFKey, 16 );
	p_rgsl->decodeBlowfish( false );
	// out
	//pack->dumpToFile( stdout );
	ptype = p_rgsl->getPacketType();
	if( ptype == 0x02 ) log_error( LOG_DEBUG, "++++ RequestServerLogin ++++\n" );
	p_rgsl->readInt(); p_rgsl->readInt(); // pass sessionKey1
	ls_chosenGameServer = (int)p_rgsl->read_GameServerID();
	log_error( LOG_DEBUG, "[INFO]: Client chooses to play on gameserver %d\n", ls_chosenGameServer );
	delete p_rgsl;
	p_rgsl = NULL;
	// free raw pack buffer
	free( rraw );
	rraw = NULL;

	// copy info about chosen game server
	if( (ls_chosenGameServer>=1) && (ls_chosenGameServer<=255) )
	{
		for( i=0; i<(int)ls_nGameServersCount; i++ )
		{
			if( gameservers_info[i].gsID == ls_chosenGameServer )
			{
				memcpy( &(myGameServerIP), &(gameservers_info[i].gsIP), 4 );
				myGameServerPort = gameservers_info[i].gsPort;
				break;
			}
		}
		//g_cfg.PlayGameServerNo = ls_chosenGameServer; // PlayGameServerNo removed from config
		log_error( LOG_DEBUG, "we play on GS #%d: %d.%d.%d.%d:%d\n",
			ls_chosenGameServer,
			(int)myGameServerIP[0], (int)myGameServerIP[1], (int)myGameServerIP[2], (int)myGameServerIP[3],
			myGameServerPort );
	}
	else
	{
		log_error( LOG_ERROR, "Client choosed GS ID %d, which is INVALID! must be [1..255]\n", ls_chosenGameServer );
	}
	
	// PlayOK / PlayFail
	// login server now sends to client if client can play o selected server
	rcvdLen = 0;
	//rraw = L2PacketReceive( sr, netTimeout, 0, &rcvdLen );
	rraw = L2PacketReceive_malloc( sr, netTimeout*1000, &rcvdLen );
	if( !rraw ) { log_error( LOG_ERROR, "receive failed!\n" ); goto l_netError; }
	log_error( LOG_DEBUG, "ProcessClient(): rcvd %u bytes of response to RequestServerLogin packet from server.\n", rcvdLen );
	// resend it to client
	sentLen = 0;
	L2PacketSend2( rraw, scl, netTimeout*1000, &sentLen );
	log_error( LOG_DEBUG, "ProcessClient(): resent %u bytes of response to RequestServerLogin packet to client.\n", sentLen );
	if( sentLen != rcvdLen ) { log_error( LOG_ERROR, "rcvd != sent! (%u != %u)\n", rcvdLen, sentLen ); goto l_netError; }
	pack = new L2LoginPacket();
	pack->setBytes( rraw, sentLen );
	pack->setDynamicBFKey( ls_newBFKey, 16 );
	pack->decodeBlowfish( false );
	// out...
	//printf( "\nResponse to RequestServerLogin:\n" );
	//pack->dumpToFile( stdout );
	// parse result
	ptype = pack->getPacketType();
	if( ptype == 0x06 ) // PlayFail
	{
		log_error( LOG_ERROR, "---- PlayFail! ----\n" );
		goto l_netError;
	}
	else if( ptype == 0x07 ) // PlayOK!
	{
		log_error( LOG_DEBUG, "++++ PlayOK! ++++\n" );
		p_pok = new L2Login_PlayOK();
		p_pok->setBytes( pack->getBytesPtr(), pack->getPacketSize() );
		p_pok->read_sessionKey2( ls_sessionKey2 );
		delete p_pok;
		p_pok = NULL;
		log_error( LOG_DEBUGDUMP, "SessionKey#2: " );
		for( i=0; i<8; i++ ) log_error_np( LOG_DEBUGDUMP, "%02X ", ls_sessionKey2[i] );
		log_error_np( LOG_DEBUGDUMP, "\n" );
	}
	else
	{
		log_error( LOG_ERROR, "=====================================\n" );
		log_error( LOG_ERROR, "???? Unknown packet type 0x%02X! ????\n",
			(unsigned int)ptype );
		pack->dumpToFile( stdout );
		log_error( LOG_ERROR, "=====================================\n" );
		goto l_netError;
	}
	// delete...
	delete pack;
	pack = NULL;
	// free raw packet data
	free( rraw );
	rraw = NULL;
	
	// mark as connected OK
	retVal = true;
	
	goto l_closeSockets; // jump over netError handler
l_netError:
	log_error( LOG_ERROR, "ProcessClient(): some network error :\\\n" );
	if( rraw ) free( rraw );
	rraw = NULL;
	retVal = false; // mark as connection failed
l_closeSockets:
	// close sockets
	L2PNet_shutdown( scl );
	L2PNet_closesocket( scl );
	L2PNet_shutdown( sr );
	L2PNet_closesocket( sr );
	state = LCST_OFFLINE;
	
	// save data
	memcpy( this->sessionKey1, ls_sessionKey1, sizeof(ls_sessionKey1) );
	memcpy( this->sessionKey2, ls_sessionKey2, sizeof(ls_sessionKey2) );
	this->gsPort = myGameServerPort;
	memcpy( this->gsIP, myGameServerIP, sizeof(myGameServerIP) );

	// notify radar window about disconnection
	this->postNotify( LCN_STATECHANGE, 0 ); // disconnected
	
	log_error( LOG_DEBUG, "ProcessClient(): ended client processing. Returning %d\n",
		(int)retVal );
	
	// return indicator
	return retVal;
}


void LoginClient::setNotifyWindow( HWND notifyHwnd, UINT notifyMsg )
{
	this->hWndNotify = notifyHwnd;
	this->uMsgNotify = notifyMsg;
}

extern HWND g_radardll_hwnd;

void LoginClient::postNotify( WPARAM wParam, LPARAM lParam )
{
	//if( hWndNotify == NULL || (uMsgNotify < WM_USER) ) return;
	//PostMessage( hWndNotify, uMsgNotify, wParam, lParam );
	PostMessage( g_radardll_hwnd, (WM_USER+106), wParam, lParam );
}

LoginClient *g_login_client = NULL;
