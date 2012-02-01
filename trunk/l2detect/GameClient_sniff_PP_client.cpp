#include "stdafx.h"
#include "ConfigIni.h"
#include "Logger.h"
#include "GameClient.h"
#include "GameListener.h"
#include "L2PacketTypes.h"

extern class CConfig g_cfg;

void GameClient::PP_sniff_fromClient( unsigned char *bytes, unsigned int len )
{
	//int i = 0;
	if( !bytes || len<1 ) return;
	if( len<3 )
	{
		log_error( LOG_WARNING, "PP_sniff_fromClient(): packet len < 3!\n" );
		log_error( LOG_WARNING, "PP_sniff_fromClient(): bytes=0x%p, len=%d\n", bytes, len );
		return;
	}

	// should we decrypt it?
	if( this->xor_enabled ) // yeah :)
	{
		if( !L2GamePacket::decodeXOR_buffer( bytes, len, this->key_client_cs ) )
			log_error( LOG_ERROR, "PP_sniff_fromClient(): decodeXOR_buffer() failed!\n" );
	}

	// some vars
	unsigned char      ptype  = bytes[2];
	unsigned short int ptype2 = 0x0000;
	unsigned short int ptype3 = 0x0000;
	if( len >= 5 )
	{
		ptype2 |= (unsigned short int)(bytes[3] & 0xFFFF);
		ptype2 |= (unsigned short int)(bytes[4] << 8 & 0xFFFF);
	}
	if( len >= 7 )
	{
		ptype3 |= (unsigned short int)(bytes[5] & 0xFFFF);
		ptype3 |= (unsigned short int)(bytes[6] << 8 & 0xFFFF);
	}

	// obfuscator
	class L2PCodeObfuscator *lpco;
	lpco = (class L2PCodeObfuscator *)this->clsObfuscator;

	if( lpco )
	{
		if( lpco->isEnabled() )
		{
			unsigned char pcode_prev = ptype;
			unsigned short pcode2_prev = ptype2;
			bool decode_res = lpco->decodeOpcode( ptype, ptype2, ptype3 );
			if( decode_res )
			{
				if( pcode2_prev == ptype2 )
					log_error( LOG_PACKETNAME, " **** de-obfuscated %02X -> %02X\n", pcode_prev, ptype );
				else
					log_error( LOG_PACKETNAME, " **** de-obfuscated %02X:%02X -> %02X:%02X\n",
						pcode_prev, pcode2_prev, ptype, ptype2 );
			}
			else
				log_error( LOG_ERROR, "PP_sniff_fromClient(): ERROR de-obfuscating %02X\n", pcode_prev );
		}
	}

	L2PacketTypes_LogClient( (L2_VERSION)g_cfg.L2_client_version, this->state, ptype, ptype2, ptype3 );

	// from client
	switch( this->state )
	{
	case GCST_CONNECTED:
	{
		switch( ptype )
		{
		//case 0x00: // ProtocolVersion // Interlude
		case 0x0e: // ProtocolVersion // Hellbound
			{
				L2Game_ProtocolVersion *p = new L2Game_ProtocolVersion( bytes, len );
				p->read_protoVer( &(this->gameProtoVer) );
				delete p;
				log_error( LOG_PACKETNAME, "Client: 0e ProtocolVersion %u\n", this->gameProtoVer );
				// client sends protocolVersion = 0xFFFFFFFF when it pings server (not tries to connect really)
				if( this->gameProtoVer >= 0xFFFFFFF0 )
				{
					log_error( LOG_PACKETNAME, "Received client ping server... no process\n" );
					this->thisWasJustServerPing = true; // received client ping (sniff)
				}
			} break; // ProtocolVersion
		// case 0x08: // AuthLogin // Interlude
		case 0x2b: // AuthLogin // Hellbound
			{
				L2Game_AuthLogin *p = new L2Game_AuthLogin( bytes, len );
				char login[32] = {0};
				p->read_login( login );
				delete p;
				log_error( LOG_PACKETNAME, "Client: 2b AuthLogin: Login: \"%s\"\n", login );
			} break; // AuthLogin
		default:
			{
				log_error( LOG_DEBUGDUMP, "Client: Unknown packet %02X in state: CONNECTED\n",
					(unsigned int)ptype );
				if( g_cfg.DumpUnknownToStdout )
				{
					printf( "============================================\n" );
					L2GamePacket *p = new L2GamePacket( bytes, len );
					p->dumpToFile( stdout );
					delete p;
					printf( "============================================\n" );
				}
			} break;
		} // switch( ptype )
	} break; // CONNECTED


	case GCST_AUTHED:
	{
		switch( ptype )
		{
		//case 0x00: // LogoutRequest
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 00 LogoutRequest\n" );
		//	} break;
		case 0x12: // CharacterSelect // Hellbound
			{
				int charSlot = bytes[3];
				//L2Game_CharacterSelect *p = new L2Game_CharacterSelect( bytes, len );
				log_error( LOG_PACKETNAME, "Client: 12 CharacterSelect: #%d\n", charSlot );
				//this->state = GCST_IN_GAME;
				//log_error( LOG_DEBUG, "Client: 12 CharacterSelect: switch state to IN_GAME\n" );
			} break; // CharacterSelect
		//case 0x13: // NewCharacter
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 13 NewCharacter\n" );
		//	} break; // NewCharacter
		//case 0x0c: // CharacterCreate
		//	{
		//		wchar_t charNameU[64] = {0};
		//		char charName[64] = {0};
		//		wcscpy( charNameU, (const wchar_t *)(bytes+3) );
		//		sprintf( charName, "%S", charNameU );
		//		log_error( LOG_PACKETNAME, "Client: 0c CharacterCreate [%s]\n", charName );
		//	} break; // CharacterCreate
		case 0xd0:
			{
				ptype2 = 0x0000;
				if( len >= 5 )
				{
					ptype2 |= (unsigned short int)(bytes[3] & 0xFFFF);
					ptype2 |= (unsigned short int)(bytes[4] << 8 & 0xFFFF);
					if( ptype2 == 0x0039 )
					{
						log_error( LOG_PACKETNAME, "Client: d0:0039 RequestGotoLobby\n" );
					}
					else
					{
						log_error( LOG_WARNING, "Client: Unknown opcode2 %04X for state AUTHED packet 0xD0\n",
							(unsigned int)ptype2 );
					}
				}
				else
					log_error( LOG_WARNING, "Client: (AUTHED) sent 0xd0 without second opcode!\n" );
			} break; // double opcode packet
		default:
			{
				log_error( LOG_PACKETNAME, "Client: Unknown packet %02X in state: AUTHED\n",
					(unsigned int)ptype );
				if( g_cfg.DumpUnknownToStdout )
				{
					printf( "============================================\n" );
					L2GamePacket *p = new L2GamePacket( bytes, len );
					p->dumpToFile( stdout );
					delete p;
					printf( "============================================\n" );
				}
			} break;
		} // switch( ptype )
	} break; // AUTHED


	case GCST_IN_GAME:
	{
		switch( ptype )
		{
		//case 0x00: // LogoutRequest
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 00 LogoutRequest\n" );
		//	} break;
		//case 0x0f: // MoveBackwardToLocation
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 0f MoveBackwardToLocation\n" );
		//	} break;
		//case 0x11: // EnterWorld
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 11 EnterWorld\n" );
		//	} break;
		//case 0x14: // RequestItemList
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 14 RequestItemList\n" );
		//	} break;
		//case 0x19: // UseItem
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 19 UseItem\n" );
		//	} break;
		case 0x1F: // Action
			{
				//log_error( LOG_PACKETNAME, "Client: 1F Action\n" );
				L2Game_Action *p = new L2Game_Action( bytes, len );
				unsigned int oid = p->read_objectID();
				p->read_originX();
				p->read_originY();
				p->read_originZ();
				char useShift = p->read_useShift();
				delete p;
				if( useShift /* && g_cfg.altGameViewNpc */ )
				{
					L2OBJECT_TYPE objType = L2OT_NONE;
					int index = -1;
					if( WorldObjectTree_GetInfoByObjectID( oid, &objType, &index ) )
					{
						if( objType == L2OT_NPC )
						{
							L2Npc *npc = npc_array[index];
							char filename[256];
							sprintf( filename, "npc_%d_stats.txt", npc->templateID );
							FILE *f = fopen( filename, "wt" );
							fprintf( f, "x,y,z,heading: (%d,%d,%d) %u\n", npc->x, npc->y, npc->z, npc->heading );
							fprintf( f, "mAtkSpd, pAtkSpd: %d, %d\n", npc->mAtkSpd, npc->pAtkSpd );
							fprintf( f, "runSpeed, walkSpeed: %d, %d\n", npc->runSpeed, npc->walkSpeed );
							fprintf( f, "colR, colH: %0.2f, %0.2f\n", npc->collisionRadius, npc->collisionHeight );
							fprintf( f, "rhand, chest, lhand: %u, %u, %u\n", npc->iid_right_hand, npc->iid_chest, npc->iid_left_hand );
							fclose( f );
						}
					}
				}
			} break;
		//case 0x23: // RequestBypassToServer
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 23 RequestBypassToServer\n" );
		//	} break;
		//case 0x39: // RequestMagicSkillUse
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 39 RequestMagicSkillUse\n" );
		//	} break;
		//case 0x3d: // RequestMagicSkillUse
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 3D RequestShortCutReg\n" );
		//	} break;
		//case 0x48: // RequestTargetCanceld
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 48 RequestTargetCanceld\n" );
		//	} break;
		//case 0x57: // RequestRestart
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 57 RequestRestart\n" );
		//		//this->state = GCST_AUTHED;
		//	} break;
		//case 0x59: // ValidatePosition
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 59 ValidatePosition\n" );
		//	} break;
		//case 0x65: // RequestPledgeInfo
		//	{
		//		// TODO: parse RequestPledgeInfo?
		//		log_error( LOG_PACKETNAME, "Client: 65 RequestPledgeInfo\n" );
		//	} break;
		//case 0x8B: // RequestGmList
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 8B RequestGmList\n" );
		//	} break;
		//case 0xA6: // RequestSkillCoolTime
		//	{
		//		log_error( LOG_PACKETNAME, "Client: A6 RequestSkillCoolTime\n" );
		//	} break;
		//case 0xCB: // GameGuardReply
		//	{
		//		log_error( LOG_PACKETNAME, "Client: CB GameGuardReply\n" );
		//	} break;
		//case 0xd0:
		//	{
		//		ptype2 = 0x00;
		//		if( len >= 5 )
		//		{
		//			ptype2 |= (unsigned short int)(bytes[3] & 0xFFFF);
		//			ptype2 |= (unsigned short int)(bytes[4] << 8 & 0xFFFF);
		//			if( ptype2 == 0x01 )
		//			{
		//				log_error( LOG_PACKETNAME, "Client: D0:01 RequestManorList\n" );
		//			}
		//			else if( ptype2 == 0x0d )
		//			{
		//				log_error( LOG_PACKETNAME, "Client: D0:0D RequestAutoSoulShot\n" );
		//			}
		//			else if( ptype2 == 0x17 )
		//			{
		//				// TODO: RequestPledgeWarList
		//				log_error( LOG_PACKETNAME, "Client: D0:17 RequestPledgeWarList\n" );
		//			}
		//			else if( ptype2 == 0x21 )
		//			{
		//				log_error( LOG_PACKETNAME, "Client: D0:21 RequestKeyMapping\n" );
		//			}
		//			else if( ptype2 == 0x24 )
		//			{
		//				log_error( LOG_PACKETNAME, "Client: D0:24 RequestSaveInventoryOrder\n" );
		//			}
		//			else
		//			{
		//				log_error( LOG_WARNING, "Client: Unknown opcode2 %04X for IN_GAME packet 0xD0\n",
		//					(unsigned int)ptype2 );
		//			}
		//		}
		//		else
		//			log_error( LOG_WARNING, "Client: (IN_GAME) sent 0xD0 without second opcode!\n" );
		//	} break; // double opcode packet
		default:
			{
				log_error( LOG_PACKETNAME, "Client: Unknown packet %02X in state: IN_GAME\n",
					(unsigned int)ptype );
				if( g_cfg.DumpUnknownToStdout )
				{
					printf( "============================================\n" );
					L2GamePacket *p = new L2GamePacket( bytes, len );
					p->dumpToFile( stdout );
					delete p;
					printf( "============================================\n" );
				}
			} break;
		} // switch( ptype )
	} break; // IN_GAME

	} // switch( state )
}