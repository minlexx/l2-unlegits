#include "stdafx.h"
#include "Logger.h"
#include "ConfigIni.h"
#include "UserAI_InvisGM.h"
#include "CharArray.h"
#include "GameClient.h"
#include "PacketInjector.h"

extern CConfig        g_cfg;
extern GameClient    *g_game_client;

UserAI_InivisGMManager::UserAI_InivisGMManager()
{
	clear();
}

UserAI_InivisGMManager::~UserAI_InivisGMManager()
{
	clear();
}

void UserAI_InivisGMManager::clear()
{
	int i;
	for( i=0; i<MAX_INVIS_GMS; i++ ) gm_oids[i] = 0;
}

void UserAI_InivisGMManager::handlePossibleInvisGM( unsigned int objectID, int actionId,
		int x, int y, int z,
		int dx, int dy, int dz,
		int skillId, int skillLevel )
{
	UNREFERENCED_PARAMETER(skillId);
	UNREFERENCED_PARAMETER(skillLevel);
	if( g_cfg.InvisGMTrackEnable == 0 ) return; // skip

	bool existing = false;
	int index = -1;
	int i;

	// try to find existing index
	for( i=0; i<MAX_INVIS_GMS; i++ )
	{
		if( gm_oids[i] == objectID )
		{
			existing = true;
			index = i;
		}
	}

	// else find index to insert
	if( !existing )
	{
		log_error_np( LOG_OK, "[INVISGM] Adding new invis GM [%u]\n", objectID );
		for( i=0; i<MAX_INVIS_GMS; i++ )
		{
			if( gm_oids[i] == 0 )
			{
				index = i;
				break;
			}
		}
	}
	else log_error_np( LOG_OK, "[INVISGM] invis GM near [%u]\n", objectID );

	// no place to insert and not existing? O_o
	if( index == -1 )
	{
		log_error( LOG_WARNING, "InivisGMManager: cannot handle invis GM [%u]!\n", objectID );
		log_error( LOG_WARNING, "InivisGMManager: Too many invis GMS (> %d)!\n", MAX_INVIS_GMS );
		return;
	}

	// send fake info to client
	generate_CharInfo( objectID, x, y, z );

	// send other info to client again
	switch( actionId )
	{
	case ACT_MOVE:         PGenC_MoveToLocation( objectID, x, y, z, dx, dy, dz ); break;
	case ACT_STOP_MOVE:    PGenC_StopMove( objectID, x, y, z, 0 ); break;
	//case ACT_SKILL_USE:    generate_MagicSkillUse( objectID, skillId, skillLevel ); break;
	//case ACT_SKILL_LAUNCH: generate_MagicSkillLaunched( objectID, skillId, skillLevel ); break;
	}
}

void UserAI_InivisGMManager::deletePossibleInvisGM( unsigned int objectID )
{
	if( g_cfg.InvisGMTrackEnable == 0 ) return; // skip

	int i;
	for( i=0; i<MAX_INVIS_GMS; i++ )
	{
		if( gm_oids[i] == objectID )
		{
			PGenC_DeleteObject( objectID );
			log_error_np( LOG_OK, "[INVISGM] Deleted objectId %u (idx %d)\n", objectID, i );
			gm_oids[i] = 0;
			return;
		}
	}
}

bool UserAI_InivisGMManager::isInvisGM( unsigned int objectID )
{
	if( g_cfg.InvisGMTrackEnable == 0 ) return false; // skip
	int i;
	for( i=0; i<MAX_INVIS_GMS; i++ )
		if( gm_oids[i] == objectID ) return true;
	return false;
}



void UserAI_InivisGMManager::generate_CharInfo( unsigned int objectID, int x, int y, int z )
{
	if( !g_game_client ) return;
	wchar_t charName[128];
	swprintf( charName, 127, L"GM_%u", objectID );

	L2GamePacket p;
	p.setPacketType( 0x31 ); // CharInfo

	p.writeD( x );
	p.writeD( y );
	p.writeD( z );
	p.writeD( 0 ); //? heading
	p.writeUInt( objectID );
	p.writeS( charName );
	p.writeD( 0 ); // race
	p.writeD( 0 ); // sex
	p.writeD( 0 ); // base class

	// paperdoll item IDs
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_UNDER));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_HEAD));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_RHAND));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_LHAND));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_GLOVES));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_CHEST));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_LEGS));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_FEET));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_BACK));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_LRHAND));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_HAIR));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_HAIR2));
	p.writeD( 0x00 ); // writeD(0x00); // PAPERDOLL_RBRACELET
	p.writeD( 0x00 ); // writeD(0x00); // PAPERDOLL_LBRACELET
	p.writeD( 0x00 ); // writeD(0x00); // PAPERDOLL_DECO1
	p.writeD( 0x00 ); // writeD(0x00); //               2
	p.writeD( 0x00 ); // writeD(0x00); //               3
	p.writeD( 0x00 ); // writeD(0x00); //               4
	p.writeD( 0x00 ); // writeD(0x00); //               5
	p.writeD( 0x00 ); // writeD(0x00); //               6

	if( g_cfg.L2_version >= (int)L2_VERSION_T23 )
	{
		p.writeD( 0x00 ); // writeH(0x00); PAPERDOLL_BELT
	}

	// paperdoll aug IDs
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_UNDER));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_HEAD));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_RHAND));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_LHAND));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_GLOVES));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_CHEST));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_LEGS));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_FEET));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_BACK));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_LRHAND));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_HAIR));
	p.writeD( 0x00 ); // writeD(_inv.getPaperdollItemId(Inventory.PAPERDOLL_HAIR2));
	p.writeD( 0x00 ); // writeD(0x00); // PAPERDOLL_RBRACELET
	p.writeD( 0x00 ); // writeD(0x00); // PAPERDOLL_LBRACELET
	p.writeD( 0x00 ); // writeD(0x00); // PAPERDOLL_DECO1
	p.writeD( 0x00 ); // writeD(0x00); //               2
	p.writeD( 0x00 ); // writeD(0x00); //               3
	p.writeD( 0x00 ); // writeD(0x00); //               4
	p.writeD( 0x00 ); // writeD(0x00); //               5
	p.writeD( 0x00 ); // writeD(0x00); //               6

	if( g_cfg.L2_version >= (int)L2_VERSION_T23 )
	{
		p.writeD( 0x00 ); // writeH(0x00); PAPERDOLL_BELT
		p.writeD( 0x00 );
		p.writeD( 0x00 );
	}

	p.writeD( 0x00 ); // writeD(pvp_flag);
	p.writeD( 200 );  // writeD(karma);

	p.writeD( 333 ); // writeD(_mAtkSpd);
	p.writeD( 333 ); // writeD(_pAtkSpd);

	p.writeD( 0x00 ); // writeD(pvp_flag);
	p.writeD( 200 );  // writeD(karma);

	p.writeD( g_cfg.InvisGMSpeed ); // writeD(_runSpd);
	p.writeD( 80 );  // writeD(_walkSpd);
	p.writeD( 50 );  // writeD(_swimSpd/* 0x32 */); // swimspeed
	p.writeD( 50 );  // writeD(_swimSpd/* 0x32 */); // swimspeed
	p.writeD( 127 ); // writeD(_flRunSpd);
	p.writeD( 80 );  // writeD(_flWalkSpd);
	p.writeD( 127 ); // writeD(_flyRunSpd);
	p.writeD( 80 );  // writeD(_flyWalkSpd);

	p.writeF( 1.0 );  // writeF(speed_move); // _cha.getProperMultiplier()
	p.writeF( 1.0 );  // writeF(speed_atack); // _cha.getAttackSpeedMultiplier()
	p.writeF( 16.0 ); // writeF(col_radius);
	p.writeF( 32.0 ); // writeF(col_height);

	p.writeD( 0x00 ); // writeD(hair_style);
	p.writeD( 0x00 ); // writeD(hair_color);
	p.writeD( 0x00 ); // writeD(face);
	p.writeS( L"Invis GM" ); // writeS(_title);

	p.writeD( 0x00 ); // writeD(clan_id);
	p.writeD( 0x00 ); // writeD(clan_crest_id);
	p.writeD( 0x00 ); // writeD(ally_id);
	p.writeD( 0x00 ); // writeD(ally_crest_id);

	p.writeD( 0x00 ); // writeD(0); // relation, always 0x00 in CharInfo

	p.writeC( 0x00 ); // writeC(_sit);
	p.writeC( 0x01 ); // writeC(_run);
	p.writeC( 0x00 ); // writeC(_combat);
	p.writeC( 0x00 ); // writeC(_dead);
	p.writeC( 0x00 ); // writeC(_invis);
	p.writeC( 0x00 ); // writeC(mount_type); // 1-on Strider, 2-on Wyvern, 3-on Great Wolf, 0-no mount
	p.writeC( 0x00 ); // writeC(private_store);

	//if(cubics == null)
	//	writeH(0);
	//else
	//{
	//	writeH(cubics.size());
	//	for(int id : cubics.keySet())
	//		writeH(id);
	//}
	p.writeH( 0x00 ); // 0 cubics

	p.writeC( 0x00 ); // writeC(partyRoom ? 1 : 0);
	p.writeD( 0x00 ); // writeD(abnormal_effect);

	p.writeC( 0x00 ); // writeC(rec_left); // means isFlying in >= T23
	p.writeH( 0x00 ); // writeH(rec_have);
	p.writeD( 0x00 ); // writeD(mount_id);
	p.writeD( 0x00 ); // writeD(class_id);
	p.writeD( 0x00 ); // writeD(0); //?
	p.writeC( 0x00 ); // writeC(_enchant);

	p.writeC( 0x00 ); // writeC(_team);
	p.writeD( 0x00 ); // writeD(large_clan_crest_id);
	p.writeC( 0x00 ); // writeC(_noble);
	p.writeC( 0x01 ); // writeC(_hero);

	p.writeC( 0x00 ); // writeC(_fishing);
	p.writeD( 0x00 ); // writeD(_fishLoc.x);
	p.writeD( 0x00 ); // writeD(_fishLoc.y);
	p.writeD( 0x00 ); // writeD(_fishLoc.z);

	p.writeD( 0x00 ); // writeD(_nameColor);
	p.writeD( 0x00 ); // writeD(_loc.h); // heading
	p.writeD( 0x00 ); // writeD(plg_class);
	p.writeD( 0x00 ); // writeD(pledge_type);
	p.writeD( 0x00 ); // writeD(title_color);
	p.writeD( 0x00 ); // writeD(cw_level); // cursed weapon id
	p.writeD( 0x00 ); // writeD(clan_rep_score);
	p.writeD( 0x00 ); // writeD(_transform);
	p.writeD( 0x00 ); // writeD(_agathion);
	
	if( g_cfg.L2_client_version >= (int)L2_VERSION_T2 )
	{
		// T2
		p.writeD( 0x00 ); // writeD(0x00);

		if( g_cfg.L2_client_version >= (int)L2_VERSION_T23 )
		{
			// T2.3
			p.writeD( 0x00 ); // _activeChar.getSpecialEffect());
			p.writeD( 0x00 );
			p.writeD( 0x00 );
			p.writeD( 0x00 );
		}
	}

	g_game_client->InjectPacketToClient( (unsigned char *)p.getBytesPtr(), p.getPacketSize() );
}

