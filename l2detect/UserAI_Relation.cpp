#include "stdafx.h"
#include "UserAI.h"
#include "Logger.h"
#include "ConfigIni.h"
#include "RadarDllWnd.h"
#include "utils.h"

extern CConfig g_cfg;

/**
Server: Len   23 [RelationChanged]
17 00 / CE /
B4 C6 38 10
40 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00  **/
void UserAI::UAI_Parse_RelationChanged( class UserAI *cls, void *l2_game_packet )
{
	if( !l2_game_packet ) return;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	p->getPacketType();
	int unk00 = -1;
	// ебаная крума со своим кривым 148-м протоколом
	if( (g_cfg.L2_version >= (int)L2_VERSION_T24) &&
		(g_cfg.OverrideGameProtocolVersion != 148) &&
		(g_cfg.GraciaEpilogueProtocol_148_hacks == 0) )
	{
		unk00 = p->readD(); // 0x01// CT24 unknown
	}
	unsigned int objectID = p->readUInt();
	unsigned int relation = p->readUInt();
	int autoAttackable    = p->readInt();
	int karma = -1;
	int pvpflag = -1;
	if( g_cfg.L2_version >= (int)L2_VERSION_T22 )
	{
		karma = p->readD();
		pvpflag = p->readD();
	}
	//
	L2OBJECT_TYPE objType = L2OT_NONE;
	int idx = -1;
	if( WorldObjectTree_GetInfoByObjectID( objectID, &objType, &idx ) )
	{
		if( objType == L2OT_PC )
		{
			if( chars_array[idx]->isUnused() == false )
			{
				// save prev. relation
				unsigned int prev_relation = chars_array[idx]->relation;
				// set new relation
				chars_array[idx]->relation = relation;
				chars_array[idx]->autoAttackable = autoAttackable;
				chars_array[idx]->karma = karma; // можно ли этим данным доверять на этой кривой яве?
				chars_array[idx]->pvpFlag = pvpflag;
				// play sound alert (only if relation really changed)
				if( prev_relation != relation )
				{
					log_error( LOG_DEBUG, "RelationChanged for Player: %S\n", chars_array[idx]->getName() );
					bool bIsEnemy = false, bIs2side = false;
					utils_detectClanWar( relation, &bIsEnemy, &bIs2side );
					if( bIsEnemy || bIs2side )
					{
						// this is enemy; play alert (if soundAlert_alertOnClanWar == 0, no sound will be played)
						//log_error( LOG_USERAI, "UserAI::UAI_Parse_RelationChanged(): found enemy!\n" );
						cls->playSoundAlert( cls->soundAlert_alertOnClanWar );
					}
				}
				// notify radar window
				RadarWnd_UpdChar( chars_array[idx]->objectID );
			}
			else log_error( LOG_USERAI, "RelationChanged for UNUSED Player??? maybe it is error! [idx = %d, oid = %u]\n",
				idx, objectID );
		}
		else if( objType == L2OT_NPC )
		{
			char npc_name[256], npc_title[256];
			npc_name[0] = npc_title[0] = 0;
			L2Data_DB_GetNPCNameTitleByID( npc_array[idx]->templateID, npc_name, npc_title );
			log_error( LOG_DEBUG, "RelationChanged for NPC??? ololo! [oid %u, npcID %u] Npc %s / title %s\n",
				objectID, npc_array[idx]->templateID, npc_name, npc_title );
		}
		else log_error( LOG_USERAI, "RelationChanged NOT FOR Player/NPC??? Impossible! ERROR! (objType = %d, oid = %u)\n",
			(int)objType, objectID );
	}
}

/*
public static final int RELATION_PVP_FLAG     = 0x00002; // pvp ???
public static final int RELATION_HAS_KARMA    = 0x00004; // karma ???
public static final int RELATION_LEADER 	  = 0x00080; // leader
public static final int RELATION_INSIEGE   	  = 0x00200; // true if in siege
public static final int RELATION_ATTACKER     = 0x00400; // true when attacker
public static final int RELATION_ALLY         = 0x00800; // blue siege icon, cannot have if red
public static final int RELATION_ENEMY        = 0x01000; // true when red icon, doesn't matter with blue
public static final int RELATION_MUTUAL_WAR   = 0x08000; // double fist
public static final int RELATION_1SIDED_WAR   = 0x10000; // single fist
*/

/** RelationChanged
protected final void writeImpl()
	{
		writeC(0xce);
		writeD(_objId);
		writeD(_relation);
		writeD(_autoAttackable);
		writeD(_karma);
		writeD(_pvpFlag);
	} */
