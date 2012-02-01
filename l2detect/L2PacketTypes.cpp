#include "stdafx.h"
#include "L2PacketTypes.h"
#include "Logger.h"
#include "ConfigIni.h"

extern class CConfig g_cfg;

//  constants are the same as in GameClient.h
#define GCST_CONNECTED  1
#define GCST_AUTHED     2
#define GCST_IN_GAME    3

void L2PacketTypes_LogClient
(
	L2_VERSION      l2_version,
	int             state,
	unsigned char   ptype,
	unsigned short  ptype2,
	unsigned short  ptype3
)
{
	char pname[256];
	switch( state )
	{
	case GCST_CONNECTED:
	{
		switch( ptype )
		{
		case 0x0e: log_error( LOG_PACKETNAME, "Client: 0e ProtocolVersion\n" ); break;
		case 0x2b: log_error( LOG_PACKETNAME, "Client: 2b AuthLogin\n" ); break;
		default:
			{
				LOG_LEVEL log_level = LOG_PACKETNAME;
				if( g_cfg.WarnUnknownPacketsToStdout ) log_level = LOG_WARNING;
				log_error( log_level, "Client: Unknown packet %02X in state: CONNECTED\n", (unsigned int)ptype );
			} break;
		}
	} break; // CONNECTED

	case GCST_AUTHED:
	{
		switch( ptype )
		{
		case 0x00: log_error( LOG_PACKETNAME, "Client: 00 LogoutRequest\n" ); break;
		case 0x12: log_error( LOG_PACKETNAME, "Client: 12 CharacterSelect\n" ); break;
		case 0x13: log_error( LOG_PACKETNAME, "Client: 13 NewCharacter\n" ); break;
		case 0x0c: log_error( LOG_PACKETNAME, "Client: 0c CharacterCreate\n" ); break;
		case 0xd0:
			{
				switch( ptype2 )
				{
				case 0x0036: log_error( LOG_PACKETNAME, "Client: D0:36 RequestGotoLobby (Final)\n" ); break;
				case 0x0039: log_error( LOG_PACKETNAME, "Client: D0:39 RequestGotoLobby\n" ); break;
				default:
					{
						LOG_LEVEL log_level = LOG_PACKETNAME;
						if( g_cfg.WarnUnknownPacketsToStdout ) log_level = LOG_WARNING;
						log_error( log_level, "Client: Unknown opcode2 %04X in state AUTHED for packet 0xD0\n", (unsigned int)ptype2 );
					} break;
				}
			} break; // double opcode packet
		default:
			{
				LOG_LEVEL log_level = LOG_PACKETNAME;
				if( g_cfg.WarnUnknownPacketsToStdout ) log_level = LOG_WARNING;
				log_error( log_level, "Client: Unknown packet %02X in state: AUTHED\n", (unsigned int)ptype );
			} break;
		}
	} break; // AUTHED

	case GCST_IN_GAME:
	{
		L2Packets_GetL2PacketName( l2_version, false, ptype, ptype2, ptype3, pname, 255 );
		if( ptype == 0xd0 )
		{
			if( ptype2 == 0x51 )
				log_error( LOG_PACKETNAME, "Client: D0:51:%02X %s\n", ptype3, pname );
			else
				log_error( LOG_PACKETNAME, "Client: D0:%02X %s\n", ptype2, pname );
		}
		else
		{
			log_error( LOG_PACKETNAME, "Client: %02X %s\n", ptype, pname );
		}
	} break; // IN_GAME

	} // switch( state )
}

void L2PacketTypes_LogServer
(
	L2_VERSION      l2_version,
	int             state,
	unsigned char   ptype,
	unsigned short  ptype2,
	unsigned short  ptype3
)
{
	UNREFERENCED_PARAMETER(ptype3);
	char pname[256];
	switch( state )
	{
	case GCST_CONNECTED:
		{
		} break;
	case GCST_AUTHED:
		{
		} break;
	case GCST_IN_GAME:
		{
			L2Packets_GetL2PacketName( l2_version, true, ptype, ptype2, ptype2, pname, 255 );
			if( ptype != 0xfe )
			{
				log_error( LOG_PACKETNAME, "Server: %02X %s\n", ptype, pname );
			}
			else
			{
				log_error( LOG_PACKETNAME, "Server: FE:%02X %s\n", ptype2, pname );
			}
		} break;
	}
}
