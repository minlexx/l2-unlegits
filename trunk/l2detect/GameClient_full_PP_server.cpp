#include "stdafx.h"
#include "ConfigIni.h"
#include "Logger.h"
#include "GameClient.h"
#include "GameListener.h"
#include "ProtocolConverter.h"
#include "L2PacketTypes.h"
#include "RadarDllWnd.h"
#include "PacketInjector.h"

extern class CConfig g_cfg;

// Parameters:
//
// returns true if packet was modified
// if packet was not modified:
//     sets (*newLen) to len;
//     sets (*newBytes) to bytes
//      - caller should ignore newLen and newBytes content in this case
//      - caller should send original packet
// if packet was modified:
//     sets (*newLen) to new length of packet
//     sets (*newBytes) to point to new bytes buffer
//      - newBytes are properly encoded if needed,
//      - caller just should send newBytes and free them after sending
// if packet should be dropped:
//     sets (*newLen) to -1
//      - caller should ignore newLen and newBytes content
//      - caller should not send any packet :)
//
bool GameClient::PP_full_fromServer( unsigned char *bytes, unsigned int len,
		unsigned char **newBytes, unsigned int *newLen )
{
	// 
	(*newBytes) = bytes;
	(*newLen) = len;
	//
	int i = 0;
	bool bPacketWasModified = false;
	if( !bytes || len<1 ) return bPacketWasModified;
	if( len<3 )
	{
		log_error( LOG_WARNING, "PP_full_fromServer(): packet len < 3!\n" );
		return bPacketWasModified;
	}

	// should we decrypt it?
	if( this->xor_enabled ) L2GamePacket::decodeXOR_buffer( bytes, len, this->key_server_sc );

	// log packet here, when it is decrypted, if log game packets enabled
	if( this->logfile ) logPacket( bytes, len, true ); // true - from server

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

	// opcode obfuscation is NOT used for Server -> Client packets!
	/*if( lpco )
	{
		if( lpco->isEnabled() && (g_cfg.L2_version != (int)L2_VERSION_T23) )
		{
			unsigned char pcode_prev = ptype;
			unsigned short pcode2_prev = ptype2;
			bool decode_res = lpco->decodeOpcode( ptype, ptype2, ptype3 );
			if( decode_res )
			{
				if( pcode2_prev == ptype2 )
					log_error( LOG_DEBUG, "PP_full_fromServer(): de-obfuscated %02X -> %02X\n", pcode_prev, ptype );
				else
					log_error( LOG_DEBUG, "PP_full_fromServer(): de-obfuscated %02X:%02X -> %02X:%02X\n",
						pcode_prev, pcode2_prev, ptype, ptype2 );
			}
			else
				log_error( LOG_ERROR, "PP_full_fromServer(): ERROR de-obfuscating %02X\n", pcode_prev, ptype );
		}
	}*/
	// opcode obfuscation is NOT used for Server -> Client packets!

	L2PacketTypes_LogServer( (L2_VERSION)g_cfg.L2_version, this->state, ptype, ptype2, ptype3 );

	// exception handler block
	try
	{

	switch( this->state )
	{
	case GCST_CONNECTED:
	{
		switch( ptype )
		{
		case 0x2e: // Hellbound: KeyPacket, FirstKey
			{
				// check length...
				if( len < 25 )
				{
					log_error( LOG_WARNING, "Received KeyPacket len = %u, but must be 25! Some bot-protection?\n" );
					log_error( LOG_WARNING, "Or old L2Rebellion-based fucking Java server?\n", len );
				}
				// try parse KeyPacket
				L2Game_KeyPacket *p_key = new L2Game_KeyPacket( bytes, len );
				p_key->parse();
				this->opcodeObfuscator = p_key->p_obfuscatorSeed;
				memcpy( this->key_client_cs, p_key->p_initialKey, 16 );
				// check allowed proto
				if( !p_key->p_protocolIsOK ) log_error( LOG_ERROR, "Server tells this protocolVersion is not supported!\n" );
				delete p_key;
				// create copies of keys
				memcpy( this->key_client_sc, this->key_client_cs, sizeof(this->key_client_cs) );
				memcpy( this->key_server_sc, this->key_client_cs, sizeof(this->key_client_cs) );
				memcpy( this->key_server_cs, this->key_client_cs, sizeof(this->key_client_cs) );
				this->xor_enabled = true;
				log_error( LOG_DEBUGDUMP, "Server: 2e KeyPacket: key: " );
				for( i=0; i<16; i++ ) log_error_np( LOG_DEBUGDUMP, "%02X ", this->key_client_cs[i] );
				log_error_np( LOG_DEBUGDUMP, "\n" );
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
					lpco->setVersionMode( (L2_VERSION)g_cfg.L2_version );
					lpco->init_tables( this->opcodeObfuscator );
					this->clsObfuscator = (void *)lpco;
				}
				else log_error( LOG_PACKETNAME, "Server: 2e KeyPacket: not using obfuscation key! OK\n" );
			} break; // KeyPacket
		case 0x09: // CharacterSelectionInfo // Hellbound
			{
				//int nChars = bytes[3];
				//log_error( LOG_PACKETNAME, "Server: 09 CharacterSelectionInfo: %d chars\n", nChars );
				this->state = GCST_AUTHED;
				this->postNotify( GCN_STATECHANGE, this->state );
				log_error( LOG_DEBUG, "Server: 09 CharacterSelectionInfo: switch state to AUTHED\n" );
			} break; // CharacterSelectionInfo
		//default:
		//	{
		//		LOG_LEVEL log_level = LOG_PACKETNAME;
		//		if( g_cfg.WarnUnknownPacketsToStdout ) log_level = LOG_WARNING;
		//		log_error( log_level, "Server: Unknown packet %02X in state: CONNECTED\n",
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
				this->state = GCST_IN_GAME;
				log_error( LOG_DEBUG, "Server: 0b CharSelected: switch state from AUTHED to IN_GAME\n" );
				// parse
				L2Game_CharSelected *p_charsel = new L2Game_CharSelected( bytes, len );
				p_charsel->parse();
				this->opcodeObfuscator = p_charsel->p_opcodeObfuscatorSeed;
				delete p_charsel;
				// obfuscator manage
				if( this->opcodeObfuscator != 0x00000000 )
				{
					log_error( LOG_WARNING, "Opcode obfuscation is enabled in CharSelected packet, 0x%08X\n",
						this->opcodeObfuscator );
					// delete obfuscator, if exists
					if( lpco )
					{
						lpco->clear();
						delete lpco;
						this->clsObfuscator = NULL;
					}
					lpco = new L2PCodeObfuscator();
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
		//case 0x73: // SSQInfo // Hellbound
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 73 SSQInfo\n" );
		//		// FIXED: SSQInfo should not change client state to IN_GAME, only CharSelected should
		//		//this->state = GCST_IN_GAME;
		//		//this->postNotify( GCN_STATECHANGE, this->state );
		//	} break; // SSQInfo
		//default:
		//	{
		//		LOG_LEVEL log_level = LOG_PACKETNAME;
		//		if( g_cfg.WarnUnknownPacketsToStdout ) log_level = LOG_WARNING;
		//		log_error( log_level, "Server: Unknown packet %02X in state: AUTHED\n",
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
	} break; // GCST_AUTHED


	case GCST_IN_GAME:
	{
		switch( ptype )
		{
		case 0x00: // Die
			{
				//log_error( LOG_PACKETNAME, "Server: 00 Die\n" );
				ai.notifyEventPacket( UAI_PEVENT_DIE, bytes, len );
			} break; // Die
		case 0x01: // Revive
			{
				//log_error( LOG_PACKETNAME, "Server: 01 Revive\n" );
				ai.notifyEventPacket( UAI_PEVENT_REVIVE, bytes, len );
			} break; // Revive
		case 0x05: // SpawnItem
			{
				ai.notifyEventPacket( UAI_PEVENT_SPAWNITEM, bytes, len );
			} break; // SpawnItem
		case 0x06: // SellList
			{
				if( ProtoConv_IsEnabled() )
					if( ProtoConv_ConvertPacket( bytes, len, newBytes, newLen ) )
						bPacketWasModified = true; // it will be encoded again in the end of this function ;)
			} break; // SellList
		case 0x07: // BuyList
			{
				if( ProtoConv_IsEnabled() )
					if( ProtoConv_ConvertPacket( bytes, len, newBytes, newLen ) )
						bPacketWasModified = true; // it will be encoded again in the end of this function ;)
			} break; // BuyList
		case 0x08: // DeleteObject
			{
				//log_error( LOG_PACKETNAME, "Server: 08 DeleteObject\n" );
				ai.notifyEventPacket( UAI_PEVENT_DELETEOBJECT, bytes, len );
			} break; // DeleteObject
		case 0x0b: // CharSelected (must be in AUTHED state, but it fixes problem on some L2J servers that do not send SSQInfo packet)
			{
				this->state = GCST_IN_GAME;
				log_error( LOG_DEBUG, "Server: 0b CharSelected: switch state from IN_GAME to IN_GAME\n" );
				// parse
				L2Game_CharSelected *p_charsel = new L2Game_CharSelected( bytes, len );
				p_charsel->parse();
				this->opcodeObfuscator = p_charsel->p_opcodeObfuscatorSeed;
				delete p_charsel;
				// obfuscator manage
				if( this->opcodeObfuscator != 0x00000000 )
				{
					log_error( LOG_WARNING, "Opcode obfuscation is enabled in CharSelected packet, 0x%08X\n",
						this->opcodeObfuscator );
					// delete obfuscator, if exists
					if( lpco )
					{
						lpco->clear();
						delete lpco;
						this->clsObfuscator = NULL;
					}
					lpco = new L2PCodeObfuscator();
					lpco->setVersionMode( (L2_VERSION)g_cfg.L2_version );
					lpco->init_tables( this->opcodeObfuscator );
					this->clsObfuscator = (void *)lpco;
				}
			} break;
		case 0x0c: // NpcInfo (mob) [CharInfo morphed]?
			{
				//log_error( LOG_PACKETNAME, "Server: 0c NpcInfo\n" );
				ai.notifyEventPacket( UAI_PEVENT_NPCINFO, bytes, len );
			} break;
		case 0x11: // ItemList
			{
				//log_error( LOG_PACKETNAME, "Server: 11 ItemList\n" );
				ai.notifyEventPacket( UAI_PEVENT_ITEMLIST, bytes, len );
				if( ProtoConv_IsEnabled() )
					if( ProtoConv_ConvertPacket( bytes, len, newBytes, newLen ) )
						bPacketWasModified = true; // it will be encoded again in the end of this function ;)
			} break; // ItemList
		//case 0x12: // SunRise
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 12 SunRise\n" );
		//	} break;
		case 0x14: // TradeStart
			{
				if( ProtoConv_IsEnabled() )
					if( ProtoConv_ConvertPacket( bytes, len, newBytes, newLen ) )
						bPacketWasModified = true;
			} break; // TradeStart
		case 0x16: // DropItem
			{
				ai.notifyEventPacket( UAI_PEVENT_DROPITEM, bytes, len );
			} break;
		//case 0x17: // GetItem
		//	{
		//		// TODO: Items handling
		//		log_error( LOG_USERAI, "Server: 17 GetItem\n" );
		//	} break;
		case 0x18: // StatusUpdate
			{
				ai.notifyEventPacket( UAI_PEVENT_STATUSUPDATE, bytes, len );
			} break; // StatusUpdate
		case 0x19: // NpcHtmlMessage
			{
				ai.UAI_process_NpcHtml( bytes, len );
			} break; // NpcHtmlMessage
		case 0x1A: // TradeOwnAdd
			{
				if( ProtoConv_IsEnabled() )
					if( ProtoConv_ConvertPacket( bytes, len, newBytes, newLen ) )
						bPacketWasModified = true;
			} break; // TradeOwnAdd
		case 0x1B: // TradeOtherAdd
			{
				if( ProtoConv_IsEnabled() )
					if( ProtoConv_ConvertPacket( bytes, len, newBytes, newLen ) )
						bPacketWasModified = true;
			} break; // TradeOtherAdd
		//case 0x1F: // ActionFailed
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 1F ActionFailed\n" );
		//	} break; // ActionFailed
		case 0x21: // InventoryUpdate
			{
				ai.notifyEventPacket( UAI_PEVENT_INVENTORYUPDATE, bytes, len );
				if( ProtoConv_IsEnabled() )
					if( ProtoConv_ConvertPacket( bytes, len, newBytes, newLen ) )
						bPacketWasModified = true; // it will be encoded again in the end of this function ;)
			} break; // InventoryUpdate
		case 0x22: // TeleportToLocation
			{
				ai.notifyEventPacket( UAI_PEVENT_TELEPORTTOLOCATION, bytes, len );
			} break; // TeleportToLocation
		case 0x23: // TargetSelected
			{
				ai.notifyEventPacket( UAI_PEVENT_TARGETSELECTED, bytes, len );
			} break; // TargetSelected
		case 0x24: // TargetUnselected
			{
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
		case 0x28: // ChangeMoveType
			{
				ai.UAI_process_ChangeMoveType( bytes, len );
			} break; // ChangeMoveType
		case 0x29: // ChangeWaitType
			{
				ai.UAI_process_ChangeWaitType( bytes, len );
			} break; // ChangeWaitType
		case 0x2f: // MoveToLocation
			{
				// some pc/npc moves, also user (player) moves possibly
				//log_error( LOG_PACKETNAME, "Server: 2f MoveToLocation\n" );
				ai.notifyEventPacket( UAI_PEVENT_MOVETOLOCATION, bytes, len );
			} break; // MoveToLocation
		//case 0x30: // NpcSay
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 30 NpcSay\n" );
		//	} break; // NpcSay
		case 0x31: // CharInfo
			{
				//log_error( LOG_PACKETNAME, "Server: 31 CharInfo\n" );
				ai.notifyEventPacket( UAI_PEVENT_CHARINFO, bytes, len );
			} break; // CharInfo
		case 0x32: // UserInfo
			{
				//log_error( LOG_PACKETNAME, "Server: 32 UserInfo\n" );
				ai.notifyEventPacket( UAI_PEVENT_USERINFO, bytes, len );
			} break; // UserInfo
		//case 0x33: // Attack
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 33 Attack\n" );
		//		//ai.notifyEventPacket( UAI_PEVENT_ATTACK, bytes, len );
		//	} break;
		//case 0x41: // WareHouseDepositList
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 41 WareHouseDepositList\n" );
		//		// TODO: protocol convert WareHouseDepositList
		//	} break;
		//case 0x42: // WareHouseWithdrawalList
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 42 WareHouseWithdrawalList\n" );
		//		// TODO: protocol convert WareHouseWithdrawalList
		//	} break;
		//case 0x44: // ShortCutRegister
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 44 ShortCutRegister\n" );
		//	} break; // ShortCutRegister
		//case 0x45: // ShortCutInit
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 45 ShortCutInit\n" );
		//	} break; // ShortCutInit
		case 0x47: // StopMove
			{
				ai.notifyEventPacket( UAI_PEVENT_STOPMOVE, bytes, len );
			} break;
		case 0x48: // MagicSkillUse
			{
				ai.notifyEventPacket( UAI_PEVENT_MAGICSKILLUSE, bytes, len );
			} break; // MagicSkillUse
		case 0x49: // MagicSkillCanceld
			{
				ai.notifyEventPacket( UAI_PEVENT_MAGICSKILLCANCELLED, bytes, len );
			} break; // MagicSkillCanceld
		//case 0x4A: // CreatureSay
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 4A CreatureSay\n" );
		//		// TODO: CreatureSay parse
		//	} break; // CreatureSay
		case 0x41: // WarehouseDepositList
			{
				if( ProtoConv_IsEnabled() )
					if( ProtoConv_ConvertPacket( bytes, len, newBytes, newLen ) )
						bPacketWasModified = true;
			} break; // WarehouseDepositList
		case 0x42: // WarehouseWithdrawList
			{
				if( ProtoConv_IsEnabled() )
					if( ProtoConv_ConvertPacket( bytes, len, newBytes, newLen ) )
						bPacketWasModified = true;
			} break; // WarehouseWithdrawList
		//case 0x4C: // DoorInfo
		//	{
		//		log_error( LOG_OK, "DoorInfo\n" );
		//	} break; // DoorInfo
		//case 0x4D: // DoorStatusUpdate
		//	{
		//		log_error( LOG_OK, "DoorStatusUpdate\n" );
		//	} break; // DoorStatusUpdate
		case 0x4E: // PartySmallWindowAll
			{
				ai.notifyEventPacket( UAI_PEVENT_PARTYALL, bytes, len );
			} break; // PartySmallWindowAll
		case 0x4F: // PartySmallWindowAdd
			{
				ai.notifyEventPacket( UAI_PEVENT_PARTYADD, bytes, len );
			} break; // PartySmallWindowAdd
		case 0x50: // PartySmallWindowDeleteAll
			{
				ai.notifyEventPacket( UAI_PEVENT_PARTYDELALL, bytes, len );
			} break; // PartySmallWindowDeleteAll
		case 0x51: // PartySmallWindowDelete
			{
				ai.notifyEventPacket( UAI_PEVENT_PARTYDEL, bytes, len );
			} break; // PartySmallWindowDelete
		case 0x52: // PartySmallWindowUpdate
			{
				ai.notifyEventPacket( UAI_PEVENT_PARTYUPDATE, bytes, len );
			} break; // PartySmallWindowUpdate
		//case 0x54: // MagicSkillLaunched
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 54 MagicSkillLaunched\n" );
		//		L2GamePacket *msl = new L2GamePacket();
		//		msl->setBytes( bytes, len );
		//		msl->readReset();
		//		msl->getPacketType();
		//		unsigned int oid = msl->readUInt(); // caster id
		//		int skillId      = msl->readD(); // skill id
		//		int skillLvl     = msl->readD(); // skill lvl
		//		delete msl;
		//		if( oid == this->ai.usr.objectID )
		//		{
		//			log_error_np( LOG_OK, "You launched skill %d lvl %d\n", skillId, skillLvl );
		//		}
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
		case 0x5F: // SkillList
			{
				ai.notifyEventPacket( UAI_PEVENT_SKILLLIST, bytes, len );
			} break; // SkillList
		//case 0x62: // SystemMessage
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 62 SystemMessage\n" );
		//	} break; // SystemMessage
		case 0x6b: // SetupGauge
			{
				ai.notifyEventPacket( UAI_PEVENT_SETUPGAUGE, bytes, len );
			} break; // SetupGauge
		//case 0x6c: // VehicleDeparture
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 6C VehicleDeparture\n" );
		//	} break;
		//case 0x72: // MoveToPawn
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 72 MoveToPawn\n" );
		//	} break; // MoveToPawn
		case 0x71: // RestartResponse (RestartOK)
			{
				L2GamePacket *pack = new L2GamePacket( bytes, len );
				pack->getPacketType(); // 0x71
				int restartOK = pack->readD();
				if( restartOK == 0x01 )
				{
					// notify user AI that user is not in game
					ai.userLogout();
					//log_error( LOG_PACKETNAME, "Server: 71 RestartResponse (RestartOK)\n" );
					this->state = GCST_AUTHED;
					log_error( LOG_DEBUG,  "Server: 71 RestartResponse: switch state to AUTHED\n" );
					this->postNotify( GCN_STATECHANGE, this->state );
				}
				else
					log_error( LOG_USERAI, "RestartResponse: logout failed!\n" );
				delete pack;
			} break; // RestartResponse
		case 0x74: // GameGuardQuery
			{
				//log_error( LOG_PACKETNAME, "Server: 74 GameGuardQuery\n" );
				// if enabled, radar can emulate GameGuard reply to standard L2J Query
				if( g_cfg.ReplyL2JGameGuardQuery )
				{
					L2GamePacket *pack = new L2GamePacket( bytes, len );
					pack->getPacketType();
					unsigned int q1 = pack->readUInt();
					unsigned int q2 = pack->readUInt();
					unsigned int q3 = pack->readUInt();
					unsigned int q4 = pack->readUInt();
					delete pack;
					pack = NULL;
					// parse request
					if( q1 == 0x27533DD9 && q2 == 0x2E72A51D && q3 == 0x2017038B && q4 == 0xC35B1EA3 )
					{
						log_error( LOG_WARNING, "Received standard l2J GameGuardQuery, replying :)\n" );
						// reply with well known answer
						PGen_GameGuardReply_L2J();
					}
					else
					{
						log_error( LOG_WARNING, "Received unknown GameGuardQuery 0x%08X 0x%08X 0x%08X 0x%08X!\n",
							q1, q2, q3, q4 );
						// do nothing...
					}
				} // if( g_cfg.ReplyL2JGameGuardQuery )
			} break; // GameGuardQuery
		//case 0x75: // FriendList
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 75 FriendList\n" );
		//	} break; // FriendList
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
				// simple trigger
				ai.userLogout();
			} break; // LeaveWorld
		case 0x85: // AbnormalStatusUpdate
			{
				ai.notifyEventPacket( UAI_PEVENT_ABNORMALSTATUSUPDATE, bytes, len );
			} break; // AbnormalStatusUpdate
		//case 0x86: // QuestList
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 86 QuestList\n" );
		//	} break; // QuestList
		case 0x89: // PledgeInfo
			{
				// writeC(0x89);
				// writeD(_clan.getClanId());
				// writeS(_clan.getName());
				// writeS(_clan.getAllyName());
				unsigned int clanID = bytes[3] | (bytes[4] << 8) | (bytes[5] << 16) | (bytes[6] << 24);
				//wchar_t clan_name[128] = {0};
				//wchar_t ally_name[128] = {0};
				unsigned int offset = 7; // 2(plen) + 1(pcode) + 4(clanID);
				//wcscpy( clan_name, (const wchar_t *)(bytes + offset) ); // wcscpy is too slow...
				const wchar_t *clan_name = (const wchar_t *)( bytes + offset );
				offset += wcslen( clan_name ) * 2 + 2; // now we can read ally_name from offset
				//wcscpy( ally_name, (const wchar_t *)(bytes + offset) ); // wcscpy is too slow...
				const wchar_t *ally_name = (const wchar_t *)( bytes + offset );
				ClanList_Add( clanID, clan_name, ally_name );
				log_error( LOG_PACKETNAME, "Server: 89 PledgeInfo %u [%S] Ally: [%S]\n",
					clanID, clan_name, ally_name );
				RadarWnd_ForceUpdateCharsList(); // force update chars list
			} break; // PledgeInfo
		//case 0x8C: // Ride
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 8C Ride\n" );
		//	} break; // Ride
		//case 0x9F: // StaticObject
		//	{
		//		log_error( LOG_PACKETNAME, "Server: 9F StaticObject\n" );
		//		L2GamePacket *so = new L2GamePacket();
		//		so->setBytes( bytes, len );
		//		so->getPacketType();  // read 0x9F
		//		unsigned int staticObjectID = so->readUInt();
		//		unsigned int objectID = so->readUInt();
		//		unsigned int soType = so->readUInt();
		//		so->readUInt();
		//		so->readUInt();
		//		unsigned int isClosed = so->readUInt();
		//		so->readUInt();
		//		int curHp = so->readInt();
		//		int maxHp = so->readInt();
		//		if( soType == 1 ) // this is door
		//		{
		//			char dooropen[32];
		//			strcpy( dooropen, "OPEN" );
		//			if( isClosed ) strcpy( dooropen, "CLOSED" );
		//			log_error_np( LOG_OK, "Door id [%u] objectId [%u] is %s (HP %d / %d)\n",
		//				staticObjectID, objectID, dooropen, curHp, maxHp );
		//		}
		//		else log_error_np( LOG_OK, "StaticObject %u is not door\n", staticObjectID );
		//		delete so;
		//	} break; // StaticObject
		case 0xA1: // PrivateStoreListSell
			{
				//log_error( LOG_PACKETNAME, "Server: A1 PrivateStoreListSell\n" );
				if( ProtoConv_IsEnabled() )
					if( ProtoConv_ConvertPacket( bytes, len, newBytes, newLen ) )
						bPacketWasModified = true;
			} break; // PrivateStoreListSell
		//case 0xA2: // PrivateStoreMsgSell
		//	{
		//		//log_error( LOG_PACKETNAME, "Server: A2 PrivateStoreMsgSell\n" );
		//	} break; // PrivateStoreMsgSell
		case 0xB9: // MyTargetSelected
			{
				//log_error( LOG_PACKETNAME, "Server: B9 MyTargetSelected [%u]\n", objectID );
				ai.notifyEventPacket( UAI_PEVENT_MYTARGETSELECTED, bytes, len );
			} break; // MyTargetSelected
		case 0xBE: // PrivateStoreListBuy
			{
				//log_error( LOG_PACKETNAME, "Server: BE PrivateStoreListBuy\n" );
				if( ProtoConv_IsEnabled() )
					if( ProtoConv_ConvertPacket( bytes, len, newBytes, newLen ) )
						bPacketWasModified = true;
			} break; // PrivateStoreListBuy
		//case 0xBF: // PrivateStoreMsgBuy
		//	{
		//		log_error( LOG_PACKETNAME, "Server: BF PrivateStoreMsgBuy\n" );
		//	} break; // PrivateStoreMsgBuy
		case 0xC7: // SkillCoolTime
			{
				ai.notifyEventPacket( UAI_PEVENT_SKILLCOOLTIME, bytes, len );
			} break; // SkillCoolTime
		//case 0xCD: // PledgeStatusChanged
		//	{
		//		log_error( LOG_PACKETNAME, "Server: CD PledgeStatusChanged\n" );
		//	} break; // PledgeStatusChanged
		case 0xCE: // RelationChanged
			{
				//log_error( LOG_PACKETNAME, "Server: CE RelationChanged\n" );
				ai.notifyEventPacket( UAI_PEVENT_RELATIONCHANGED, bytes, len );
				if( ProtoConv_IsEnabled() )
					if( ProtoConv_ConvertPacket( bytes, len, newBytes, newLen ) )
						bPacketWasModified = true; // it will be encoded again in the end of this function ;)
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
		case 0xF4: // PartySpelled
			{
				ai.notifyEventPacket( UAI_PEVENT_PARTYSPELLED, bytes, len );
			} break;
		//case 0xF9: // EtcStatusUpdate
		//	{
		//		log_error( LOG_PACKETNAME, "Server: F9 EtcStatusUpdate\n" );
		//	} break; // EtcStatusUpdate
		case 0xFA: // ShortBuffStatusUpdate
			{
				ai.notifyEventPacket( UAI_PEVENT_SHORTBUFFSTATUSUPDATE, bytes, len );
			} break; // ShortBuffStatusUpdate
		//case 0xFD: // AgitDecoInfo
		//	{
		//		log_error( LOG_PACKETNAME, "Server: FD AgitDecoInfo\n" );
		//	} break; // AgitDecoInfo
		case 0xFE: // double-byte packet
			{
				ptype2 = 0x00;
				if( len >= 5 )
				{
					ptype2 |= (unsigned short int)(bytes[3] & 0xFFFF);
					ptype2 |= (unsigned short int)(bytes[4] << 8 & 0xFFFF);
					switch( ptype2 )
					{
					//case 0x01: // ExRegenMax
					//	{
					//		log_error( LOG_PACKETNAME, "Server: FE:01 ExRegenMax\n" );
					//	} break;
					//case 0x0c: // ExAutoSoulShot FE:0C
					//	{
					//		log_error( LOG_PACKETNAME, "Server: FE:0C ExAutoSoulShot\n" );
					//	} break;
					//case 0x22: // ExSendManorList FE:22
					//	{
					//		log_error( LOG_PACKETNAME, "Server: FE:22 ExSendManorList\n" );
					//	} break;
					//case 0x2f: // ExStorageMaxCount FE:2f
					//	{
					//		log_error( LOG_PACKETNAME, "Server: FE:2F ExStorageMaxCount\n" );
					//	} break;
					//case 0x32: // ExPCCafePointInfo FE:32
					//	{
					//		log_error( LOG_PACKETNAME, "Server: FE:32 ExPCCafePointInfo\n" );
					//	} break;
					//case 0x33: // ExSetCompassZoneCode FE:33
					//	{
					//		log_error( LOG_PACKETNAME, "Server: FE:33 ExSetCompassZoneCode\n" );
					//	} break;
					//case 0x3a: // PledgeSkillList FE:3A
					//	{
					//		log_error( LOG_PACKETNAME, "Server: FE:3A PledgeSkillList\n" );
					//	} break;
					case 0x3f: // PledgeReceiveWarList FE:3F
						{
							log_error( LOG_PACKETNAME, "Server: FE:3F PledgeReceiveWarList\n" );
							ai.notifyEventPacket( UAI_PEVENT_PLEDGERECEIVEWARLIST, bytes, len );
						} break;
					//case 0x40: // PledgeReceiveSubPledgeCreated FE:40
					//	{
					//		log_error( LOG_PACKETNAME, "Server: FE:40 PledgeReceiveSubPledgeCreated\n" );
					//	} break;
					//case 0x5f: // ExBasicActionList FE:5F
					//	{
					//		log_error( LOG_PACKETNAME, "Server: FE:5F ExBasicActionList\n" );
					//	} break;
					case 0x8D: // Gracia Final FE:8D ExNpcQuestHtmlMessage
						{
							if( g_cfg.L2_version >= (int)L2_VERSION_T23 )
								ai.UAI_process_NpcHtml( bytes, len );
						} break;
					//case 0xAC: // possibly old ExBrExtraUserInfo
					//	{
					//		log_error( LOG_WARNING, "[PACK] possibly old ExBrExtraUserInfo FE:AC\n" );
					//	} break; // possibly old ExBrExtraUserInfo
					case 0xB7: // ExBuySellListPacket
						{
							if( ProtoConv_IsEnabled() )
								if( ProtoConv_ConvertPacket( bytes, len, newBytes, newLen ) )
									bPacketWasModified = true; // it will be encoded again in the end of this function ;)
						} break; // ExBuySellListPacket
					//default: log_error( LOG_PACKETNAME, "Server: Unknown opcode2 %04X for IN_GAME packet 0xFE\n",
					//			(unsigned int)ptype2 ); break;
					}
				}
				else log_error( LOG_WARNING, "Server: (IN_GAME) sent 0xFE without second opcode!\n" );
			} break; // double-byte packet
		//default:
		//	{
		//		LOG_LEVEL log_level = LOG_PACKETNAME;
		//		if( g_cfg.WarnUnknownPacketsToStdout ) log_level = LOG_WARNING;
		//		log_error( log_level, "Server: Unknown packet %02X in state: IN_GAME\n",
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
	} break; // GCST_IN_GAME
	} // switch( state )

	} // try
	catch( L2P_Exception& e )
	{
		log_error( LOG_ERROR, "L2P_Exception in PP_full_fromServer\n", e.what() );
	}

	// all encoding takes place ONLY IF PACKET IS NOT SET TO DROP!!!!
	if( (*newLen) != -1 )
	{
		// re-encode packet again, independently if it was modified or not.
		// ... BUT NEVER ENCODE KeyPacket 0x2E! (though XOR is enabled now)
		// this encodes only original unmodified packet!!!
		if( (bPacketWasModified == false) && (ptype != 0x2e) )
		{
			//PP_full_reencode_packet( bytes, len, this->key_client_sc );
			//log_error( LOG_ERROR, "PP_full_reencode_packet() FAILED\n" );
			//log_error( LOG_OK, "Encoding unmodified packet\n" );
			if( this->xor_enabled ) L2GamePacket::encodeXOR_buffer( bytes, len, this->key_client_sc );
		}
		// encode only modified packet, if packet was modified
		if( bPacketWasModified == true )
		{
			// log modified packet here, when it is decrypted, if log game packets enabled
			if( this->logfile )
			{
				fprintf( this->logfile, "Next is modified:\n" );
				logPacket( (*newBytes), (*newLen), true ); // true - from server
			}

			//log_error( LOG_OK, "Encoding modified packet\n" ); // <==DEL
			if( this->xor_enabled ) L2GamePacket::encodeXOR_buffer( (*newBytes), (*newLen), this->key_client_sc );
		}
	}
	//else log_error( LOG_OK, "Server: No encode - DROP packet!\n" );

	return bPacketWasModified;
}
