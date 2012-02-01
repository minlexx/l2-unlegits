#include "pch.h"
#include "Log.h"
#include "l2c_utils.h"
#include "net/GameClient/GameClient.h"
#include "ClientPacketHandler.h"
#include "ServerPackets.h"
#include "GS.h"

ClientPacketHandler::ClientPacketHandler( GameClient *cl )
{
	m_cl = cl;
}

ClientPacketHandler::~ClientPacketHandler()
{
	m_cl = NULL;
}

void ClientPacketHandler::handlePacket( L2GamePacket *pack )
{
	L2GamePacket *reply = NULL;
	unsigned char opcode = pack->getByteAt(2);

	switch( m_cl->getState() )
	{
	case CLIENT_STATE_CONNECTED:
		{
			switch( opcode )
			{
			case 0x0E: reply = ProtocolVersion( pack ); break;
			case 0x2B: reply = AuthLogin( pack ); break;
			default: invalidPacket( pack, opcode, 0 ); break;
			}
		} break;
	case CLIENT_STATE_AUTHED:
		{
			switch( opcode )
			{
			case 0x00: reply = Logout( pack ); break;
			case 0x0C: reply = CharacterCreate( pack ); break;
			case 0x12: reply = CharacterSelect( pack ); break;
			case 0x13: reply = NewCharacter( pack ); break;
			case 0xD0:
				{
					unsigned int opcode2 = pack->getByteAt(3) | (pack->getByteAt(4) << 8);
					switch( opcode2 )
					{
					case 0x0036: reply = RequestGotoLobby( pack ); break;
					default: invalidPacket( pack, opcode, opcode2 ); break;
					}
				} break;
			default: invalidPacket( pack, opcode, 0 ); break;
			}
		} break;
	case CLIENT_STATE_IN_GAME:
		{
			switch( opcode )
			{
			case 0x00: reply = Logout( pack ); break;
			case 0xD0:
				{
					unsigned int opcode2 = pack->getByteAt(3) | (pack->getByteAt(4) << 8);
					switch( opcode2 )
					{
					case 0x0001: break; // RequestManorList
					default: invalidPacket( pack, opcode, opcode2 ); break;
					}
				} break;
			default: invalidPacket( pack, opcode, 0 ); break;
			}
		} break;
	}

	if( reply )
	{
		m_cl->sendPacket( reply, true ); // true - delete reply immediately
	}
}

void ClientPacketHandler::invalidPacket( L2GamePacket *p, unsigned int opcode, unsigned int opcode2 )
{
	unsigned int plen = p->getPacketSize();
	unsigned char *buf = (unsigned char *)p->getBytesPtr();
	ClientState st = m_cl->getState();
	wchar_t sn[32] = {0};
	wcscpy( sn, L"Unknown" );
	switch( st )
	{
	case CLIENT_STATE_OFFLINE: wcscpy( sn, L"OFFLINE" ); break;
	case CLIENT_STATE_CONNECTED: wcscpy( sn, L"CONNECTED" ); break;
	case CLIENT_STATE_AUTHED: wcscpy( sn, L"AUTHED" ); break;
	case CLIENT_STATE_IN_GAME: wcscpy( sn, L"IN_GAME" ); break;
	}
	if( opcode2 == 0 )
		LogWarning( L"%s: Invalid packet 0x%02X (len %u) in state: [%s]", m_cl->toString(), opcode, plen, sn );
	else
		LogWarning( L"%s: Invalid packet 0x%02X:%04X (len %u) in state: [%s]", m_cl->toString(), opcode, opcode2, plen, sn );
	unsigned int i = 0;
	while( i < plen )
	{
		wchar_t str[32] = {0};
		swprintf( str, 32, L"%02X ", buf[i] );
		LogFull( false, false, RGB(200,200,200), RGB(0,0,0), str );
		if( i>0 && (i%16 == 0) ) LogFull( false, true, RGB(200,200,200), RGB(0,0,0), L"" );
		i++;
	}
	LogFull( false, true, RGB(128,0,0), RGB(255,255,255), L"" );
	LogFull( false, true, RGB(128,0,0), RGB(255,255,255), L"dump end" );
}

