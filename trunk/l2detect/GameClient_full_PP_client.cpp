#include "stdafx.h"
#include "ConfigIni.h"
#include "Logger.h"
#include "GameClient.h"
#include "GameListener.h"
#include "L2PacketTypes.h"
#include "accountBind.h"

extern class CConfig g_cfg;

// Parameters:
//
// returns true if packet was modified
// if packet was not modified:
//     sets (*newLen) to len;
//     sets (*newBytes) to bytes
//      - caller can ignore newLen and newBytes content in this case
//      - caller can use newLen and newBytes instead of original values (they are the same)
//      - caller should send original packet
//      - caller should free original packet buffer
// if packet was modified:
//     sets (*newLen) to new length of packet
//     sets (*newBytes) to point to new bytes buffer
//      - newBytes are properly encoded if needed,
//      - caller just should send newBytes and free them after sending
//      - caller should also free original packet buffer
// if packet should be dropped:
//     sets (*newLen) to -1
//      - caller should ignore newLen and newBytes content
//      - caller should not send any packet :)
//      - caller should free original packet buffer
//
bool GameClient::PP_full_fromClient( unsigned char *bytes, unsigned int len,
		unsigned char **newBytes, unsigned int *newLen )
{
	// assume packet was not modified
	(*newLen) = len;
	(*newBytes) = bytes;
	//
	//int i = 0;
	bool bPacketWasModified = false; // initially packet is not modified
	if( !bytes || len<1 ) return bPacketWasModified;
	if( len<3 )
	{
		log_error( LOG_WARNING, "PP_full_fromClient(): packet len < 3!\n" );
		return bPacketWasModified;
	}

	// should we decrypt it?
	if( this->xor_enabled ) L2GamePacket::decodeXOR_buffer( bytes, len, this->key_client_cs );

	// parse it
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
			bool decode_res = false;
			try
			{
				decode_res = lpco->decodeOpcode( ptype, ptype2 );
			}
			catch( L2P_DeObfuscateException& e )
			{
				log_error( LOG_ERROR, "PP_full_fromClient(): L2P_DeObfuscateException: %s\n", e.what() );
			}
			if( decode_res )
			{
				if( pcode2_prev == ptype2 ) // 2nd opcode not changed, not double-opcode packet
					log_error( LOG_DEBUG, " **** de-obfuscated %02X -> %02X ****\n", pcode_prev, ptype );
				else
					log_error( LOG_DEBUG, " **** de-obfuscated %02X:%02X -> %02X:%02X ****\n",
						pcode_prev, pcode2_prev, ptype, ptype2 );
			}
			else
				log_error( LOG_ERROR, "PP_full_fromClient(): ERROR de-obfuscating %02X\n", pcode_prev );
		}
	}

	// log packet here, when it is decrypted, and opcode is deobfuscated, if log game packets is enabled
	if( this->logfile )
	{
		unsigned char prev_1 = bytes[2];
		unsigned short prev_2 = bytes[3] | (bytes[4] << 8);
		unsigned short prev_3 = bytes[5] | (bytes[6] << 8);
		if( this->clsObfuscator )
		{
			// replace opcode byte in bytes array to de-obfuscated opcode
			// to make log readable
			bytes[2] = ptype;
			if( len >= 5 )
			{
				bytes[3] = ptype2 & 0xFF;
				bytes[4] = (ptype2 >> 8) & 0xFF;
				if( len >= 7 )
				{
					bytes[5] = ptype3 & 0xFF;
					bytes[6] = (ptype3 >> 8) & 0xFF;
				}
			}
		}
		logPacket( bytes, len, false ); // false - from client
		if( this->clsObfuscator )
		{
			// restore opcode byte in bytes array to original
			// to keep packet 'as is'
			bytes[2] = prev_1;
			if( len >= 5 )
			{
				bytes[3] = prev_2 & 0xFF;
				bytes[4] = (prev_2 >> 8) & 0xFF;
				if( len >= 7 )
				{
					bytes[5] = prev_3 & 0xFF;
					bytes[6] = (prev_3 >> 8) & 0xFF;
				}
			}
		}
	}

	L2PacketTypes_LogClient( (L2_VERSION)g_cfg.L2_client_version, this->state, ptype, ptype2, ptype3 );

	// special handling packet sequential number
	if( this->counters.ullPacketsSent == 0 )
	{
		// client sends first packet - ProtocolVersion
		if( g_cfg.OverrideGameProtocolVersion > 0 )
		{
			// we can replace ProtocolVersion now...
			int cur_pv = bytes[3] | (bytes[4]<<8) | (bytes[5]<<16) | (bytes[6]<<24);
			// check for client pings server (usually: pv == -2 or 0xFFFFFFFE)
			if( (cur_pv >= 0xFFFFFFF0) || (cur_pv < 0) )
			{
				log_error( LOG_PACKETNAME, "Received client ping server... no process\n" );
				this->thisWasJustServerPing = true; // received client ping (full)
				return false; // flag: packet was not modified, and no need to re-encode it
			}
			//
			log_error( LOG_PACKETNAME, "Client: 0e ProtocolVersion: Overriding ProtocolVersion from %d to %d\n",
				cur_pv, g_cfg.OverrideGameProtocolVersion );
			bytes[3] = (unsigned char)( (g_cfg.OverrideGameProtocolVersion & 0x000000FF) );
			bytes[4] = (unsigned char)( (g_cfg.OverrideGameProtocolVersion & 0x0000FF00) >> 8 );
			if( g_cfg.OverrideGameProtocolVersion == 828 )
			{
				/* Overriding to Kamael T1. we must replace also last 4 bytes */
				bytes[263] = 0x8D;
				bytes[264] = 0x71;
				bytes[265] = 0x5F;
				bytes[266] = 0x08;
				log_error( LOG_PACKETNAME, "Client: 0e ProtocolVersion: Overriding ProtocolVersion to T1: Kamael\n" );
			}
			if( g_cfg.OverrideGameProtocolVersion == 831 )
			{
				/* Overriding to Hellbound T1.5. we must replace also last 4 bytes */
				bytes[263] = 0xFB;
				bytes[264] = 0x87;
				bytes[265] = 0xB9;
				bytes[266] = 0x4A;
				log_error( LOG_PACKETNAME, "Client: 0e ProtocolVersion: Overriding ProtocolVersion to T1.5: Hellbound\n" );
			}
			if( g_cfg.OverrideGameProtocolVersion == 851 )
			{
				/* Overriding to Gracia T2. we must replace also last 4 bytes */
				bytes[263] = 0xDF;
				bytes[264] = 0xB8;
				bytes[265] = 0x3B;
				bytes[266] = 0x54;
				log_error( LOG_PACKETNAME, "Client: 0e ProtocolVersion: Overriding ProtocolVersion to T2: Gracia Part 1\n" );
			}
			if( g_cfg.OverrideGameProtocolVersion >= 12 && g_cfg.OverrideGameProtocolVersion <= 17 )
			{
				/* Overriding to Gracia T2.2. we must replace also last 4 bytes */
				bytes[263] = 0xEB;
				bytes[264] = 0xEF;
				bytes[265] = 0x3D;
				bytes[266] = 0xE6;
				log_error( LOG_PACKETNAME, "Client: 0e ProtocolVersion: Overriding ProtocolVersion to T2.2: Gracia Part 2\n" );
			}
			if( g_cfg.OverrideGameProtocolVersion >= 83 && g_cfg.OverrideGameProtocolVersion <= 87 )
			{
				/* Overriding to Gracia T2.3. we must replace also last 4 bytes */
				bytes[263] = 0x11;
				bytes[264] = 0x5D;
				bytes[265] = 0x1F;
				bytes[266] = 0x60;
				log_error( LOG_PACKETNAME, "Client: 0e ProtocolVersion: Overriding ProtocolVersion to T2.3: Gracia Final\n" );
			}
			if( g_cfg.OverrideGameProtocolVersion >= 146 && g_cfg.OverrideGameProtocolVersion <= 148 )
			{
				/* Overriding to Gracia T2.4. we must replace also last 4 bytes */
				bytes[263] = 0xA6;
				bytes[264] = 0x23;
				bytes[265] = 0xF4;
				bytes[266] = 0xFE;
				log_error( LOG_PACKETNAME, "Client: 0e ProtocolVersion: Overriding ProtocolVersion to T2.4: Gracia Epilogue\n" );
			}
			return true; // flag that packet was modified :)
			// we can use return here, because encoding is not required for this packet
			// packet will be sent to server as is, without encoding
		}
	}
	// special handling packet sequential number end

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
				if( this->gameProtoVer >= 0xFFFFFFF0 ) // < 0
				{
					this->thisWasJustServerPing = true; // received client ping (full)
					log_error( LOG_PACKETNAME, "Received client ping server... no process\n" );
				}
			} break; // ProtocolVersion
		// case 0x08: // AuthLogin // Interlude
		case 0x2b: // AuthLogin // Hellbound
			{
				L2Game_AuthLogin *p = new L2Game_AuthLogin( bytes, len );
				char login[32] = {0};
				p->read_login( login );
				delete p;
				log_error( LOG_DEBUG, "Client: 2b AuthLogin: Login: \"%s\"\n", login );
#ifdef _ACCOUNT_BIND
				if( !isValidAccount( login ) )
				{
					log_error( LOG_ERROR, "Client: invalid account name (not bound to this). Exiting.\n" );
					ErrorLogger_FlushLogFile();
					L2PNet_shutdown( this->sock_client );
					L2PNet_shutdown( this->sock_server );
					Sleep( 2000 );
					L2PNet_closesocket( this->sock_client );
					L2PNet_closesocket( this->sock_server );
					ExitProcess( 123 );
				}
#endif
			} break; // AuthLogin
		//default:
		//	{
		//		log_error( LOG_DEBUGDUMP, "Client: Unknown packet %02X in state: CONNECTED\n",
		//			(unsigned int)ptype );
		//		if( g_cfg.DumpUnknownToStdout )
		//		{
		//			printf( "============================================\n" );
		//			L2GamePacket *p = new L2GamePacket( bytes, len );
		//			p->dumpToFile( stdout );
		//			delete p;
		//			printf( "============================================\n" );
		//		}
		//	} break;
		} // switch( ptype )
	} break; // CONNECTED


	//case GCST_AUTHED:
	//{
	//	switch( ptype )
	//	{
		//case 0x00: // LogoutRequest
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 00 LogoutRequest\n" );
		//	} break;
		//case 0x12: // CharacterSelect // Hellbound
		//	{
		//		int charSlot = bytes[3];
		//		//L2Game_CharacterSelect *p = new L2Game_CharacterSelect( bytes, len );
		//		log_error( LOG_PACKETNAME, "Client: 12 CharacterSelect: #%d\n", charSlot );
		//		//this->state = GCST_IN_GAME;
		//		//log_error( LOG_DEBUG, "Client: 12 CharacterSelect: switch state to IN_GAME\n" );
		//	} break; // CharacterSelect
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
		//case 0xd0:
		//	{
		//		ptype2 = 0x0000;
		//		if( len >= 5 )
		//		{
		//			ptype2 |= (unsigned short int)(bytes[3] & 0xFFFF);
		//			ptype2 |= (unsigned short int)(bytes[4] << 8 & 0xFFFF);
		//			if( ptype2 == 0x0039 )
		//			{
		//				log_error( LOG_PACKETNAME, "Client: D0:39 RequestGotoLobby\n" );
		//			}
		//			else
		//			{
		//				log_error( LOG_WARNING, "Client: Unknown opcode2 %04X for state AUTHED packet 0xD0\n",
		//					(unsigned int)ptype2 );
		//			}
		//		}
		//		else
		//			log_error( LOG_WARNING, "Client: (AUTHED) sent 0xd0 without second opcode!\n" );
		//	} break; // double opcode packet
		//default:
		//	{
		//		log_error( LOG_PACKETNAME, "Client: Unknown packet %02X in state: AUTHED\n",
		//			(unsigned int)ptype );
		//		if( g_cfg.DumpUnknownToStdout )
		//		{
		//			printf( "============================================\n" );
		//			L2GamePacket *p = new L2GamePacket( bytes, len );
		//			p->dumpToFile( stdout );
		//			delete p;
		//			printf( "============================================\n" );
		//		}
		//	} break;
		//} // switch( ptype )
	//} break; // AUTHED


	case GCST_IN_GAME:
	{
		switch( ptype )
		{
		//case 0x00: // LogoutRequest
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 00 LogoutRequest\n" );
		//	} break;
		case 0x0f: // MoveBackwardToLocation
			{
				//log_error( LOG_PACKETNAME, "Client: 0f MoveBackwardToLocation\n" );
				if( g_cfg.L2Walker_FixMoveBackwardToLocation )
				{
					//log_error( LOG_OK, "Fixing L2Walker's MoveBackwardToLocation\n" );
					if( len == 27 )
					{
						//log_error( LOG_OK, "It is L2Walker's packet\n" );
						unsigned char *np = (unsigned char *)malloc( len + 4 );
						if( np )
						{
							(*newBytes) = np;
							(*newLen) = len + 4;
							memcpy( np, bytes, len );
							np[0] = ((unsigned char)31); // length of correct packet must be 31
							np[1] = 0;
							np[len + 0] = 0x01;
							np[len + 1] = 0x00;
							np[len + 2] = 0x00;
							np[len + 3] = 0x00;
							//
							bPacketWasModified = true; // so it will be encoded in the end of this function
						}
						//else log_error( LOG_ERROR, "malloc() error!\n" );
					}
					//else if( len == 31 )
					//{
					//	log_error( LOG_OK, "It is normal client's packet. No need to change\n" );
					//}
					//else log_error( LOG_ERROR, "Unknown MoveBackwardToLocation packet!\n" );
				}
			} break;
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
				unsigned int actionObjectId = p->read_objectID();
				p->read_originX();
				p->read_originY();
				p->read_originZ();
				//char useShift = p->read_useShift();
				delete p;
				// notify LockTarget manager
				ai.lockTargetMgr.onUserRequestAction( actionObjectId );
				//
				//if( useShift /* && g_cfg.altGameViewNpc */ )
				//{
				//	L2OBJECT_TYPE objType = L2OT_NONE;
				//	int index = -1;
				//	if( WorldObjectTree_GetInfoByObjectID( oid, &objType, &index ) )
				//	{
				//		// disable Shift + click on NPC
				//		/*if( objType == L2OT_NPC )
				//		{
				//			L2Npc *npc = npc_array[index];
				//			char filename[256];
				//			sprintf( filename, "npc_%d_stats.txt", npc->templateID );
				//			FILE *f = fopen( filename, "wt" );
				//			fprintf( f, "x,y,z,heading: (%d,%d,%d) %u\n", npc->x, npc->y, npc->z, npc->heading );
				//			fprintf( f, "mAtkSpd, pAtkSpd: %d, %d\n", npc->mAtkSpd, npc->pAtkSpd );
				//			fprintf( f, "runSpeed, walkSpeed: %d, %d\n", npc->runSpeed, npc->walkSpeed );
				//			fprintf( f, "colR, colH: %0.2f, %0.2f\n", npc->collisionRadius, npc->collisionHeight );
				//			fprintf( f, "rhand, chest, lhand: %u, %u, %u\n", npc->iid_right_hand, npc->iid_chest, npc->iid_left_hand );
				//			fclose( f );
				//		}*/
				//		// maybe add Shift + click on player action? :)
				//	}
				//}
			} break;
		case 0x22: // RequestLinkHtml
			{
				// this packet requires handling... it may be dropped if AI decides to
				this->ai.UAI_process_RequestBypassToServer( bytes, len, newBytes, newLen );
			} break;
		case 0x23: // RequestBypassToServer
			{
				// this packet requires handling... it may be dropped if AI decides to
				this->ai.UAI_process_RequestBypassToServer( bytes, len, newBytes, newLen );
			} break;
		case 0x36: // ChangeWaitType2
			{
				if( g_cfg.L2Walker_FixChangeWaitType2 )
				{
					log_error( LOG_OK, "Client: Fixing 36 ChangeWaitType2\n" );
					// ChangeWaitType2 format:
					// * Client: Len    7 | 07 00 / 36 / 00 00 00 00 - sit
					// * Client: Len    7 | 07 00 / 36 / 01 00 00 00 - stand
					// Instead of ChangeWaitType2 send RequestActionUse, so does client!
					// only L2Walker sends ChangeWaitType2 :\
					// RequestActionUse sit/stand format:
					// * Client: Len 12 | 0C 00 / 56 / 00 00 00 00  00 00 00 00  00
					// TODO: opcode obfuscate
					unsigned char opcode_RequestActionUse = 0x56; // RequestActionUse
					unsigned short doubleOpcode = 0x00;
					if( this->clsObfuscator )
					{
						L2PCodeObfuscator *lpco = (L2PCodeObfuscator *)(this->clsObfuscator);
						if( lpco->isEnabled() )
						{
							lpco->encodeOpcode( opcode_RequestActionUse, doubleOpcode );
							log_error( LOG_DEBUG, "Fix ChangeWaitType2: obfuscated 56 -> %02X\n", opcode_RequestActionUse );
						}
					}
					// create packet
					L2GamePacket *gp = new L2GamePacket();
					gp->setPacketType( opcode_RequestActionUse ); //gp->setPacketType( 0x56 );  // opcode :)
					gp->writeUInt( 0 );         // actionID
					gp->writeUInt( 0 );         // ctrlPressed
					gp->writeUChar( 0 );        // shiftPressed
					(*newLen) = gp->getPacketSize();
					if( (*newLen) == 0x0C ) // new length must be = 12 bytes
					{
						log_error( LOG_OK, "Client: Fixed 36 ChangeWaitType2 OK\n" );
						bPacketWasModified = true;
						(*newBytes) = (unsigned char *)malloc( (*newLen) );
						memcpy( (*newBytes), gp->getBytesPtr(), (*newLen) );
					}
					delete gp;
				}
			} break;
		//case 0x39: // RequestMagicSkillUse
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 39 RequestMagicSkillUse\n" );
		//	} break;
		//case 0x3d: // RequestMagicSkillUse
		//	{
		//		log_error( LOG_PACKETNAME, "Client: 3D RequestShortCutReg\n" );
		//	} break;
		case 0x48: // RequestTargetCanceld
			{
				//log_error( LOG_PACKETNAME, "Client: 48 RequestTargetCanceld\n" );
				ai.lockTargetMgr.onUserRequestUnselectTarget();
			} break;
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
		//		log_error( LOG_OK, "Client: 65 RequestPledgeInfo\n" );
		//	} break;
		case 0x8B: // RequestGmList
			{
				// RequestGmList kill packet?
				//log_error( LOG_PACKETNAME, "Client: 8B RequestGmList\n" );
				if( g_cfg.L2Walker_DropRequestGMList )
				{
					log_error( LOG_WARNING, "Client: Droppped RequestGMList!\n" );
					(*newLen) = (unsigned int)-1; // mark to drop
				}
			} break;
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
		//			switch( ptype2 )
		//			{
		//			case 0x0065: // drop :)
		//				log_error( LOG_WARNING, "Client: Droppped D0:65 (RequestPostItemList in Epilogue)!\n" );
		//				(*newLen) = (unsigned int)-1; // mark to drop
		//				break;
		//			}
		//		}
		//		else
		//			log_error( LOG_WARNING, "Client: (IN_GAME) sent 0xD0 without second opcode!\n" );
		//	} break; // double opcode packet
		//default:
		//	{
		//		log_error( LOG_PACKETNAME, "Client: Unknown packet %02X in state: IN_GAME\n",
		//			(unsigned int)ptype );
		//		if( g_cfg.DumpUnknownToStdout )
		//		{
		//			printf( "============================================\n" );
		//			L2GamePacket *p = new L2GamePacket( bytes, len );
		//			p->dumpToFile( stdout );
		//			delete p;
		//			printf( "============================================\n" );
		//		}
		//	} break;
		} // switch( ptype )
	} break; // IN_GAME

	} // switch( state )

	// all encoding takes place ONLY IF PACKET IS NOT SET TO DROP!!!!
	if( (*newLen) != -1 )
	{
		// re-encode packet again =] olny encode original packetif it was not modified
		//PP_full_reencode_packet( bytes, len, this->key_server_cs );
		//log_error( LOG_ERROR, "PP_full_reencode_packet() C->S FAILED\n" );
		if( !bPacketWasModified && this->xor_enabled ) L2GamePacket::encodeXOR_buffer( bytes, len, this->key_server_cs );
		// if packet was modified, encode only modified packet (encode only that will be sent)
		if( bPacketWasModified ) L2GamePacket::encodeXOR_buffer( (*newBytes), (*newLen), this->key_server_cs );
	}
	else log_error( LOG_PACKETNAME, "Client: No encode - DROP packet!\n" );

	return bPacketWasModified;
}
