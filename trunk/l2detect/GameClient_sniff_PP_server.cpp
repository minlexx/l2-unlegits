#include "stdafx.h"
#include "ConfigIni.h"
#include "Logger.h"
#include "GameClient.h"
#include "GameListener.h"
#include "CharArray.h"
#include "ClanList.h"
#include "L2PacketTypes.h"

extern class CConfig g_cfg;

void GameClient::PP_sniff_fromServer( unsigned char *bytes, unsigned int len )
{
	int i = 0;
	if( !bytes || len<1 ) return;
	if( len<3 )
	{
		log_error( LOG_WARNING, "PP_sniff_fromServer(): packet len < 3!\n" );
		log_error( LOG_WARNING, "PP_sniff_fromServer(): bytes=0x%p, len=%d\n", bytes, len );
		return;
	}

	// should we decrypt it?
	if( this->xor_enabled ) // yeah :)
	{
		if( !L2GamePacket::decodeXOR_buffer( bytes, len, this->key_client_sc ) )
			log_error( LOG_ERROR, "PP_sniff_fromServer(): decodeXOR_buffer() failed!\n" );
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
		if( lpco->isEnabled() && (g_cfg.L2_version != (int)L2_VERSION_T23) )
		{
			unsigned char pcode_prev = ptype;
			unsigned short pcode2_prev = ptype2;
			bool decode_res = lpco->decodeOpcode( ptype, ptype2, ptype3 );
			if( decode_res )
			{
				if( pcode2_prev == ptype2 )
					log_error( LOG_PACKETNAME, "PP_sniff_fromServer(): de-obfuscated %02X -> %02X\n", pcode_prev, ptype );
				else
					log_error( LOG_PACKETNAME, "PP_sniff_fromServer(): de-obfuscated %02X:%02X -> %02X:%02X\n",
						pcode_prev, pcode2_prev, ptype, ptype2 );
			}
			else
				log_error( LOG_ERROR, "PP_snigg_fromServer(): ERROR de-obfuscating %02X\n", pcode_prev, ptype );
		}
	}

	L2PacketTypes_LogServer( (L2_VERSION)g_cfg.L2_version, this->state, ptype, ptype2, ptype3 );

	switch( this->state )
	{
	case GCST_CONNECTED:
	{
		switch( ptype )
		{
		//case 0x00: // Interlude: KeyPacket, FirstKey
		case 0x2e: // Hellbound: KeyPacket, FirstKey
			{
				if( g_cfg.TeonPvP_hacks )
				{
					L2GamePacket *p = new L2GamePacket( bytes, len );
					unsigned char opcode = p->getPacketType();
					unsigned char protoOk = p->readC();
					p->readBytes( this->key_client_cs, 16 ); // 16 bytes instead of 8?
					int d1 = p->readD();
					int d2 = p->readD();
					int c1 = p->readC();
					int obf_key = p->readD();
					delete p;
					//
					log_error( LOG_PACKETNAME, "TeonPvP: enbaled hacks. KeyPacket [%02X] Read key [", (unsigned)opcode );
					for( i=0; i<16; i++ ) log_error_np( LOG_PACKETNAME, "%02X", (unsigned)this->key_client_cs[i] );
					log_error_np( LOG_PACKETNAME, "]\n" );
					log_error( LOG_PACKETNAME, "  protocolOk  :  %d\n", protoOk );
					log_error( LOG_PACKETNAME, "  d1          :  %d\n", d1 );
					log_error( LOG_PACKETNAME, "  d2          :  %d\n", d2 );
					log_error( LOG_PACKETNAME, "  c1          :  %d\n", c1 );
					log_error( LOG_PACKETNAME, "  obf_key     :  %d\n", obf_key );
				}
				else
				{
					L2Game_KeyPacket *p = new L2Game_KeyPacket( bytes, len );
					p->read_key( this->key_client_cs );
					p->read_GameServerID();
					this->opcodeObfuscator = p->read_OpcodeObfuscator();
					L2Game_KeyPacket::createInitialHellboundKey( this->key_client_cs,
						this->key_client_cs );
					delete p;
				}
				memcpy( this->key_client_sc, this->key_client_cs,
					sizeof(this->key_client_cs) );
				this->xor_enabled = true;
				log_error( LOG_PACKETNAME, "Server: 2e KeyPacket\n" );
				log_error( LOG_PACKETNAME, "Server: 2e KeyPacket: key: " );
				for( i=0; i<16; i++ ) log_error_np( LOG_PACKETNAME, "%02X ", this->key_client_cs[i] );
				log_error_np( LOG_PACKETNAME, "\n" );
				// log obfuscator, if it is != 0x00000000
				LOG_LEVEL log_level = LOG_DEBUGDUMP;
				if( this->opcodeObfuscator != 0x00000000 )
				{
					log_level = LOG_WARNING;
					log_error( log_level, "Server: 2e KeyPacket: Obfuscator: 0x%04X\n", this->opcodeObfuscator );
					// delete obfuscator, if exists
					if( lpco )
					{
						lpco->clear();
						delete lpco;
						this->clsObfuscator = NULL;
					}
					lpco = new L2PCodeObfuscator();
					//lpco->setVersionMode_T22();
					// TODO: Obfuscator set L2 Version
					lpco->setVersionMode( (L2_VERSION)g_cfg.L2_version );
					lpco->init_tables( this->opcodeObfuscator );
					this->clsObfuscator = (void *)lpco;
				}
				else log_error( LOG_PACKETNAME, "Server: 2e KeyPacket: not using obfuscator\n" );
			} break; // KeyPacket
		case 0x09: // CharacterSelectionInfo // Hellbound
			{
				int nChars = bytes[3];
				log_error( LOG_PACKETNAME, "Server: 09 CharacterSelectionInfo: %d chars\n", nChars );
				this->state = GCST_AUTHED;
				log_error( LOG_DEBUG, "Server: 09 CharacterSelectionInfo: switch state to AUTHED\n" );
			} break; // CharacterSelectionInfo
		default:
			{
				log_error( LOG_PACKETNAME, "Server: Unknown packet %02X in state: CONNECTED\n",
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
	} break; // GCST_CONNECTED


	case GCST_AUTHED:
	{
		switch( ptype )
		{
		case 0x09: // CharacterSelectionInfo // Hellbound
			{
				int nChars = bytes[3];
				log_error( LOG_PACKETNAME, "Server: 09 CharacterSelectionInfo: %d chars\n", nChars );
			} break; // CharacterSelectionInfo
		case 0x0b: // CharSelected
			{
				// TODO: CharSelected parse, get clan ID
				wchar_t char_name[128] = {0};
				wcscpy( char_name, (const wchar_t *)(bytes+3) );
				log_error( LOG_PACKETNAME, "Server: 0b CharSelected: \"%S\"\n", char_name );
				this->state = GCST_IN_GAME;
				log_error( LOG_DEBUG, "Server: 0b CharSelected: switch state from AUTHED to IN_GAME\n" );
				this->opcodeObfuscator = 0;
				this->opcodeObfuscator = bytes[len-4] | (bytes[len-3] << 8) | (bytes[len-2] << 16) | (bytes[len-1] << 24);
				if( this->opcodeObfuscator != 0x00000000 )
				{
					log_error( LOG_DEBUG, "Opcode obfuscation is enabled in CharSelected packet, 0x%08X\n",
						this->opcodeObfuscator );
					// delete obfuscator, if exists
					if( lpco )
					{
						lpco->clear();
						delete lpco;
						this->clsObfuscator = NULL;
					}
					lpco = new L2PCodeObfuscator();
					//lpco->setVersionMode_T22();
					// TODO: Obfuscator set L2 Version
					lpco->setVersionMode( (L2_VERSION)g_cfg.L2_version );
					lpco->init_tables( this->opcodeObfuscator );
					this->clsObfuscator = (void *)lpco;
				}
			} break;
		//case 0x0d: // NewCharacterSuccess
		//	{
		//		int nTemplates = bytes[3];
		//		log_error( LOG_PACKETNAME, "Server: 0d NewCharacterSuccess; char_templates: %d\n",
		//			nTemplates );
		//	} break; // NewCharacterSuccess
		//case 0x0f: // CharCreateOK
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 0f CharCreateOK\n" );
		//	} break; // CharCreateOK
		case 0x73: // SSQInfo // Hellbound
			{
				//log_error( LOG_PACKETNAME, "Server: 73 SSQInfo\n" );
				this->state = GCST_IN_GAME;
				log_error( LOG_PACKETNAME, "Server: 73 SSQInfo, switch state to IN_GAME\n" );
			} break; // SSQInfo
		default:
			{
				log_error( LOG_DEBUGDUMP, "Server: Unknown packet %02X in state: AUTHED\n",
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
	} break; // GCST_AUTHED


	case GCST_IN_GAME:
	{
		switch( ptype )
		{
		//case 0x00: // Die
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 00 Die\n" );
		//	} break; // Die
		//case 0x05: // SpawnItem
		//	{
				// TODO: SpawnItem parse
				/*	protected final void writeImpl()
				{
					writeC(0x05);
					writeD(_objectId);
					writeD(_itemId);

					writeD(_x);
					writeD(_y);
					writeD(_z);
					// only show item count if it is a stackable item
					writeD(_stackable);
					writeD(_count);
					writeD(0x00); //c2
				}*/
		//		log_error( LOG_PACKETNAME, "Server: 05 SpawnItem\n" );
		//	} break; // SpawnItem
		case 0x08: // DeleteObject
			{
				// TODO: parse DeleteObject
				// [3,4,5,6] - objectId
				//unsigned int oid = bytes[3] | (bytes[4] << 8) | (bytes[5] << 16) | (bytes[6] << 24);
				//log_error( LOG_PACKETNAME, "Server: 08 DeleteObject %u\n", oid );
				//CharArray_DeleteCharByObjectID( oid );
				ai.notifyEventPacket( UAI_PEVENT_DELETEOBJECT, bytes, len );
			} break; // DeleteObject
		case 0x0b: // CharSelected
			{
				// TODO: CharSelected parse, get clan ID
				wchar_t char_name[128] = {0};
				wcscpy( char_name, (const wchar_t *)(bytes+3) );
				log_error( LOG_PACKETNAME, "Server: 0b CharSelected: \"%S\"\n", char_name );
				this->state = GCST_IN_GAME;
				log_error( LOG_DEBUG, "Server: 0b CharSelected: switch state from IN_GAME to IN_GAME\n" );
				this->opcodeObfuscator = bytes[len-4] | (bytes[len-3] << 8) | (bytes[len-2] << 16) | (bytes[len-1] << 24);
				if( this->opcodeObfuscator != 0x00000000 )
				{
					log_error( LOG_DEBUG, "Opcode obfuscation is enabled in CharSelected packet, 0x%08X\n",
						this->opcodeObfuscator );
					// delete obfuscator, if exists
					if( lpco )
					{
						lpco->clear();
						delete lpco;
						this->clsObfuscator = NULL;
					}
					lpco = new L2PCodeObfuscator();
					//lpco->setVersionMode_T22();
					// TODO: Obfuscator set L2 Version
					lpco->setVersionMode( (L2_VERSION)g_cfg.L2_version );
					lpco->init_tables( this->opcodeObfuscator );
					this->clsObfuscator = (void *)lpco;
				}
			} break;
		case 0x0c: // NpcInfo (mob)?
			{
				/*unsigned int idTemplate = 0;
				unsigned char *p = (unsigned char *)&idTemplate;
				// bytes[2] is ptype(0x0c); objectID[3,4,5,6]; idTemplate[7,8,9,10]....
				p[0] = bytes[7];
				p[1] = bytes[8];
				p[2] = bytes[9];
				p[3] = bytes[10];
				idTemplate -= 1000000; // ? :) L2J adds 1000000 to this field
				// c,18d,4f,3d,5c,name,title!
				int name_offset = 3 + 18*sizeof(int) + 4*sizeof(double) + 3*sizeof(int) + 5;
				wchar_t name[128];
				memset( name, 0, sizeof(name) );
				wcsncpy( name, (const wchar_t *)(bytes+name_offset), 127 );
				name[127] = 0;
				log_error( LOG_PACKETNAME, "Server: 0c NpcInfo (mob/morphed char?): id_%u, [%S]\n",
					idTemplate, name );*/
				ai.notifyEventPacket( UAI_PEVENT_NPCINFO, bytes, len );
			} break;
		case 0x11: // ItemList
			{
				//log_error( LOG_PACKETNAME, "Server: 11 ItemList\n" );
				ai.notifyEventPacket( UAI_PEVENT_ITEMLIST, bytes, len );
			} break; // ItemList
		case 0x18: // StatusUpdate
			{
				//log_error( LOG_PACKETNAME, "Server: 18 StatusUpdate\n" );
				ai.notifyEventPacket( UAI_PEVENT_STATUSUPDATE, bytes, len );
			} break; // StatusUpdate
		//case 0x19: // NpcHtmlMessage
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 19 NpcHtmlMessage\n" );
		//	} break; // NpcHtmlMessage
		//case 0x1F: // ActionFailed
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 1F ActionFailed\n" );
		//	} break; // ActionFailed
		case 0x21: // InventoryUpdate
			{
				//log_error( LOG_PACKETNAME, "Server: 21 InventoryUpdate\n" );
				ai.notifyEventPacket( UAI_PEVENT_INVENTORYUPDATE, bytes, len );
			} break; // InventoryUpdate
		case 0x23: // TargetSelected
			{
				// TODO: TargetSelected
				/*  writeC(0x23);
				    writeD(_objectId);
				    writeD(_targetObjId);
				    writeD(_x);
				    writeD(_y);
				    writeD(_z);
				    writeD(0x00); */
				// bytes[0], bytes[1] - packet size;   bytes[3] - pcode
				unsigned int objectID = bytes[3] | (bytes[4] << 8) | (bytes[5] << 16) | (bytes[6] <<24);
				unsigned int targetObjID = bytes[7] | (bytes[8] << 8) | (bytes[9] << 16) | (bytes[10] <<24);
				log_error( LOG_PACKETNAME, "Server: 23 TargetSelected [%u] -> [%u]\n",
					objectID, targetObjID );
				ai.notifyEventPacket( UAI_PEVENT_TARGETSELECTED, bytes, len );
			} break; // TargetSelected
		case 0x24: // TargetUnselected
			{
				//log_error( LOG_PACKETNAME, "Server: 24 TargetUnselected\n" );
				ai.notifyEventPacket( UAI_PEVENT_TARGETUNSELECTED, bytes, len );
			} break; // TargetUnselected
		//case 0x25: // AutoAttackStart
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 25 AutoAttackStart\n" );
		//	} break; // AutoAttackStart
		//case 0x26: // AutoAttackStop
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 26 AutoAttackStop\n" );
		//	} break; // AutoAttackStop
		//case 0x27: // SocialAction
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 27 SocialAction\n" );
		//	} break; // SocialAction
		//case 0x28: // ChangeMoveType
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 28 ChangeMoveType\n" );
		//	} break; // ChangeMoveType
		case 0x2f: // MoveToLocation
			{
				//
				L2GamePacket *p = new L2GamePacket( bytes, len );
				unsigned int oid = p->readUInt();
				delete p;
				int itsme = (int)( oid == ai.usr.objectID );
				//
				log_error( LOG_PACKETNAME, "Server: 2f MoveToLocation [%u] (my oid %u; its me: %d)\n",
					oid, ai.usr.objectID, itsme );
				ai.notifyEventPacket( UAI_PEVENT_MOVETOLOCATION, bytes, len );
			} break; // MoveToLocation
		case 0x31: // CharInfo
			{
				//log_error( LOG_PACKETNAME, "Server: 31 CharInfo\n" );
				//CharList_Add( name, title, oid, x,y,z, heading, race, sex, baseClass, clanID );
				ai.notifyEventPacket( UAI_PEVENT_CHARINFO, bytes, len );
			} break; // NpcInfo
		case 0x32: // UserInfo
			{
				//
				ai.notifyEventPacket( UAI_PEVENT_USERINFO, bytes, len );
			} break; // UserInfo
		//case 0x44: // ShortCutRegister
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 44 ShortCutRegister\n" );
		//	} break; // ShortCutRegister
		//case 0x45: // ShortCutInit
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 45 ShortCutInit\n" );
		//	} break; // ShortCutInit
		//case 0x47: // StopMove
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 47 StopMove\n" );
		//	} break; // StopMove
		//case 0x48: // MagicSkillUse
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 48 MagicSkillUse\n" );
		//	} break; // MagicSkillUse
		//case 0x4A: // CreatureSay
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 4A CreatureSay\n" );
		//		// TODO: CreatureSay parse
		//	} break; // CreatureSay
		//case 0x54: // MagicSkillLaunched
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 54 MagicSkillLaunched\n" );
		//	} break; // MagicSkillLaunched
		//case 0x5A: // PledgeShowMemberListAll
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 5A PledgeShowMemberListAll\n" );
		//	} break; // PledgeShowMemberListAll
		//case 0x5B: // PledgeShowMemberListUpdate
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 5B PledgeShowMemberListUpdate\n" );
		//	} break; // PledgeShowMemberListUpdate
		//case 0x5C: // PledgeShowMemberListAdd
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 5C PledgeShowMemberListAdd\n" );
		//	} break; // PledgeShowMemberListAdd
		//case 0x5F: // SkillList
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 5F SkillList\n" );
		//	} break; // SkillList
		//case 0x62: // SystemMessage
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 62 SystemMessage\n" );
		//	} break; // SystemMessage
		//case 0x6b: // SetupGauge
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 6B SetupGauge\n" );
		//	} break; // SetupGauge
		//case 0x72: // MoveToPawn
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 72 MoveToPawn\n" );
		//	} break; // MoveToPawn
		case 0x71: // RestartResponse (RestartOK)
			{
				// notify user AI that user is not in game
				ai.userLogout();
				log_error( LOG_PACKETNAME, "Server: 71 RestartResponse (RestartOK)\n" );
				this->state = GCST_AUTHED;
				log_error( LOG_DEBUG,  "Server: 71 RestartResponse: switch state to AUTHED\n" );
				this->postNotify( GCN_STATECHANGE, this->state );
			} break; // RestartResponse
		//case 0x74: // GameGuardQuery
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 74 GameGuardQuery\n" );
		//	} break; // GameGuardQuery
		//case 0x79: // ValidateLocation
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 79 ValidateLocation\n" );
		//	} break; // ValidateLocation
		//case 0x7B: // ShowBoard
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 7B ShowBoard\n" );
		//	} break; // ShowBoard
		case 0x84: // LeaveWorld
			{
				//log_error( LOG_PACKETNAME, "Server: 84 LeaveWorld\n" );
				ai.userLogout();
			} break; // LeaveWorld
		//case 0x85: // AbnormalStatusUpdate
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 85 AbnormalStatusUpdate\n" );
		//	} break; // AbnormalStatusUpdate
		//case 0x86: // QuestList
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 86 QuestList\n" );
		//	} break; // QuestList
		case 0x89: // PledgeInfo
			{
				// TODO: parse PledgeInfo
				// writeC(0x89);
				// writeD(_clan.getClanId());
				// writeS(_clan.getName());
				// writeS(_clan.getAllyName());
				unsigned int clanID = bytes[3] | (bytes[4] << 8) | (bytes[5] << 16) | (bytes[6] << 24);
				wchar_t clan_name[128] = {0};
				wchar_t ally_name[128] = {0};
				unsigned int offset = 7; // 2(plen) + 1(pcode) + 4(clanID);
				wcscpy( clan_name, (const wchar_t *)(bytes + offset) );
				offset += wcslen( clan_name ) * 2 + 2; // now we can read ally_name from offset
				wcscpy( ally_name, (const wchar_t *)(bytes + offset) );
				ClanList_Add( clanID, clan_name, ally_name );
				log_error( LOG_PACKETNAME, "Server: 89 PledgeInfo %u [%S] Ally: [%S]\n",
					clanID, clan_name, ally_name );
			} break; // PledgeInfo
		//case 0x9F: // StaticObject
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 9F StaticObject\n" );
		//	} break; // StaticObject
		//case 0xB9: // MyTargetSelected
		//	{
		//		unsigned int objectID = bytes[3] | (bytes[4] << 8) | (bytes[5] << 16) | (bytes[6] <<24);
		//		log_error( LOG_PACKETNAME, "Server: B9 MyTargetSelected [%u]\n", objectID );
		//	} break; // MyTargetSelected
		//case 0xC7: // SkillCoolTime
		//	{
		//		log_error( LOG_PACKETNAME, "Server: C7 SkillCoolTime\n" );
		//	} break; // SkillCoolTime
		//case 0xCD: // PledgeStatusChanged
		//	{
		//		log_error( LOG_PACKETNAME, "Server: CD PledgeStatusChanged\n" );
		//	} break; // PledgeStatusChanged
		case 0xCE: // RelationChanged
			{
				//log_error( LOG_PACKETNAME, "Server: CE RelationChanged\n" );
				ai.notifyEventPacket( UAI_PEVENT_RELATIONCHANGED, bytes, len );
			} break; // RelationChanged
		//case 0xE5: // HennaInfo
		//	{
		//		log_error( LOG_PACKETNAME, "Server: E5 HennaInfo\n" );
		//	} break; // HennaInfo
		//case 0xE8: // SendMacroList
		//	{
		//		log_error( LOG_PACKETNAME, "Server: E8 SendMacroList\n" );
		//	} break; // SendMacroList
		//case 0xF2: // ClientSetTime
		//	{
		//		log_error( LOG_PACKETNAME, "Server: F2 ClientSetTime\n" );
		//	} break; // ClientSetTime
		//case 0xF9: // EtcStatusUpdate
		//	{
		//		log_error( LOG_PACKETNAME, "Server: F9 EtcStatusUpdate\n" );
		//	} break; // EtcStatusUpdate
		//case 0xFD: // AgitDecoInfo
		//	{
		//		log_error( LOG_PACKETNAME, "Server: FD AgitDecoInfo\n" );
		//	} break; // AgitDecoInfo
		//case 0xFE: // double-byte packet
		//	{
		//		ptype2 = 0x00;
		//		if( len >= 5 )
		//		{
		//			ptype2 |= (unsigned short int)(bytes[3] & 0xFFFF);
		//			ptype2 |= (unsigned short int)(bytes[4] << 8 & 0xFFFF);
		//			switch( ptype2 )
		//			{
		//			case 0x0c: // ExAutoSoulShot FE:0C
		//				{
		//					log_error( LOG_PACKETNAME, "Server: FE:0C ExAutoSoulShot\n" );
		//				} break;
		//			case 0x22: // ExSendManorList FE:22
		//				{
		//					log_error( LOG_PACKETNAME, "Server: FE:22 ExSendManorList\n" );
		//				} break;
		//			case 0x2f: // ExStorageMaxCount FE:2f
		//				{
		//					log_error( LOG_PACKETNAME, "Server: FE:2F ExStorageMaxCount\n" );
		//				} break;
		//			case 0x33: // ExSetCompassZoneCode FE:33
		//				{
		//					log_error( LOG_PACKETNAME, "Server: FE:33 ExSetCompassZoneCode\n" );
		//				} break;
		//			case 0x3a: // PledgeSkillList FE:3A
		//				{
		//					log_error( LOG_PACKETNAME, "Server: FE:3A PledgeSkillList\n" );
		//				} break;
		//			case 0x3f: // PledgeReceiveWarList FE:3F
		//				{
		//					// TODO: PledgeReceiveWarList parse
		//					log_error( LOG_PACKETNAME, "Server: FE:3F PledgeReceiveWarList\n" );
		//				} break;
		//			case 0x40: // PledgeReceiveSubPledgeCreated FE:40
		//				{
		//					log_error( LOG_PACKETNAME, "Server: FE:40 PledgeReceiveSubPledgeCreated\n" );
		//				} break;
		//			case 0x5f: // ExBasicActionList FE:5F
		//				{
		//					log_error( LOG_PACKETNAME, "Server: FE:5F ExBasicActionList\n" );
		//				} break;
		//			default: log_error( LOG_WARNING, "Server: Unknown opcode2 %04X for IN_GAME packet 0xFE\n",
		//						(unsigned int)ptype2 ); break;
		//			}
		//		}
		//		else log_error( LOG_WARNING, "Server: (IN_GAME) sent 0xFE without second opcode!\n" );
		//	} break; // double-byte packet
		default:
			{
				log_error( LOG_PACKETNAME, "Server: Unknown packet %02X in state: IN_GAME\n",
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
	} break; // GCST_IN_GAME
	} // switch( state )
}
