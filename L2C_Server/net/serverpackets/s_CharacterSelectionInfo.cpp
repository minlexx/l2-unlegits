#include "pch.h"
#include "Log.h"
#include "l2c_utils.h"
#include "net/GameClient/GameClient.h"
#include "../ServerPackets.h"
#include "GS.h"
#include "world/templates/item/L2ItemTemplate.h"

L2GamePacket *ServerPackets::CharacterSelectionInfo( GameClient *pl )
{
	GameServer *gs = GameServer::getInstance();
	MysqlConnection *con = gs->getDBConnection();
	L2Game_CharSelectionInfoBlock csb[7];
	memset( &csb, 0, sizeof(csb) );
	int           i = 0;
	int           num_chars = 0;
	unsigned int  max_lastAccess = 0;
	int           activeCharIndex = -1;
	MysqlQuery    q;
	q.create( L"SELECT account_name, charId, char_name, level, maxHp, curHp, maxMp, curMp, face, hairStyle, "
	          L"hairColor, sex, heading, x, y, z, exp, sp, karma, pvpkills, pkkills, clanid, race, classid, "
	          L"deletetime, cancraft, title, rec_have, rec_left, accesslevel, online, char_slot, lastAccess, "
	          L"base_class, transform_id FROM characters WHERE account_name='%s' LIMIT 7", pl->getAccount() );
	if( con->executeQuery( q ) )
	{
		i = 0;
		while( q.getNextRow() )
		{
			unsigned int lastAccess = q.getFieldUInt( "lastAccess" );
			if( lastAccess > max_lastAccess )
			{
				max_lastAccess = lastAccess;
				activeCharIndex = i;
			}
			//
			q.getFieldStrW( 0, csb[i].accountName, 32 );
			csb[i].charID    = q.getFieldUInt( 1 );
			q.getFieldStrW( 2, csb[i].charName, 32 );
			csb[i].level     = q.getFieldUInt( 3 );
			csb[i].HP_max    = q.getFieldDouble( 4 );
			csb[i].HP_cur    = q.getFieldDouble( 5 );
			csb[i].MP_max    = q.getFieldDouble( 6 );
			csb[i].MP_cur    = q.getFieldDouble( 7 );
			csb[i].face      = q.getFieldUInt( 8 );
			csb[i].hairStyle = q.getFieldUInt( 9 );
			csb[i].hairColor = q.getFieldUInt( 10 );
			csb[i].sex       = q.getFieldUInt( 11 );
			//q.getFieldUInt( 12, ???? ); // no heading
			csb[i].x         = q.getFieldInt( 13 );
			csb[i].y         = q.getFieldInt( 14 );
			csb[i].z         = q.getFieldInt( 15 );
			csb[i].Exp       = q.getFieldUInt64( 16 );
			csb[i].SP        = q.getFieldUInt( 17 );
			csb[i].karma     = q.getFieldUInt( 18 );
			csb[i].PVP_kills = q.getFieldUInt( 19 );
			csb[i].PK_kills  = q.getFieldUInt( 20 );
			csb[i].clanID    = q.getFieldUInt( 21 );
			csb[i].race      = q.getFieldUInt( 22 );
			csb[i].classID   = q.getFieldUInt( 23 );
			csb[i].deleteSeconds = q.getFieldUInt( 24 );
			//q.getFieldUInt( 25, can craft??? );
			//q.getFieldStrW( 26, title?? );
			//q.getFieldUInt( 27, rec_have );
			csb[i].baseClassID = q.getFieldUInt( "base_class" );
			csb[i].isActive = 1;
			// fake
			//csb[i].iid_chest = 0x0967; // DC Robe
			//csb[i].iid_R_hand = 0x2ec2; // Common Branch of Mother tree
			//
			i++;
		}
		num_chars = i;
	}
	else
	{
		LogError( L"CharacterSelectionInfo: MySQL error: %s", con->getErrorStr() );
	}

	// TODO: load also equipped inventory items
	for( i=0; i<num_chars; i++ )
	{
		q.clear();
		q.create( L"SELECT object_id,item_id,loc_data,enchant_level FROM items WHERE owner_id=%u AND loc='PAPERDOLL'", csb[i].charID );
		if( con->executeQuery( q ) )
		{
			while( q.getNextRow() )
			{
				int slot = q.getFieldInt( "loc_data" );
				int item_id = q.getFieldInt( "item_id" );
				int enchant_level = q.getFieldInt( "enchant_level" );
				//
				switch( slot )
				{
				case 1: csb[i].iid_R_ear       = item_id; break;
				case 2: csb[i].iid_L_ear       = item_id; break;
				//case 3: LREAR
				case 4: csb[i].iid_neck        = item_id; break;
				case 5: csb[i].iid_L_finger    = item_id; break;
				case 6: csb[i].iid_R_finger    = item_id; break;
				//case 7: LRFINGER
				case 8: csb[i].iid_head        = item_id; break;
				case 9: csb[i].iid_R_hand      = item_id; break;
				case 10: csb[i].iid_L_hand     = item_id; break;
				case 11: csb[i].iid_gloves     = item_id; break;
				case 12: csb[i].iid_chest      = item_id; break;
				case 13: csb[i].iid_legs       = item_id; break;
				case 14: csb[i].iid_feet       = item_id; break;
				case 15: csb[i].iid_back       = item_id; break;
				case 16: csb[i].iid_R_hand = csb[i].iid_L_hand = item_id; break; // LRHAND
				case 17: csb[i].iid_chest      = item_id; break; // fullarmor?
				case 18: csb[i].iid_hair       = item_id; break;
				//case 19: ALLDRESS
				case 20: csb[i].iid_hair_2     = item_id; break;
				case 21: csb[i].iid_hair_all   = item_id; break;
				case 22: csb[i].iid_R_bracelet = item_id; break;
				case 23: csb[i].iid_L_bracelet = item_id; break;
				// 24..29 DECOs
				case 30: csb[i].iid_belt       = item_id; break;
				}
				if( slot == SLOT_R_HAND  &&  enchant_level > 0 ) // weapon
					csb[i].enchantEffect = (unsigned char)(enchant_level & 0x000000FF);
			}
		}
		else
		{
			LogError( L"CharacterSelectionInfo: MySQL error: %s", con->getErrorStr() );
		}
	}
	q.clear();
	gs->releaseDBConnection( con );

	// retreive playOk1
	unsigned char playKey[8];
	memset( playKey, 0, 8 );
	pl->getPlayKey( playKey );
	unsigned int playOk1 = *(unsigned int *)&playKey[0];
	//
	L2GamePacket *p = new L2GamePacket();
	p->setPacketType( 0x09 );
	p->writeD( num_chars ); // number of chars
	p->writeD( 0x07 ); // server max_chars
	p->writeC( 0x00 ); // 0x00
	//
	for( i=0; i<num_chars; i++ )
	{
		p->writeS( csb[i].charName );
		p->writeUInt( csb[i].charID );
		p->writeS( pl->getAccount() );
		p->writeUInt( playOk1 ); // TODO: playOK id 1
		p->writeUInt( csb[i].clanID );
		p->writeD( 0x00 );
		//
		p->writeUInt( csb[i].sex );
		p->writeUInt( csb[i].race );
		p->writeUInt( csb[i].baseClassID );
		p->writeUInt( (i == activeCharIndex) ? 0x01 : 0x00 ); // is active
		//
		p->writeInt( csb[i].x );
		p->writeInt( csb[i].y );
		p->writeInt( csb[i].z );
		p->writeDouble( csb[i].HP_cur );
		p->writeDouble( csb[i].MP_cur );
		//
		p->writeUInt( csb[i].SP );
		p->writeUInt64( csb[i].Exp );
		p->writeUInt( csb[i].level );
		//
		p->writeUInt( csb[i].karma );
		p->writeUInt( csb[i].PK_kills );
		p->writeUInt( csb[i].PVP_kills );
		//
		p->writeD( 0x00 ); p->writeD( 0x00 ); p->writeD( 0x00 ); p->writeD( 0x00 );
		p->writeD( 0x00 ); p->writeD( 0x00 ); p->writeD( 0x00 ); // 7 0x00
		//
		// 26 inventory item ids // TODO: equipment ids
		p->writeD( csb[i].iid_hair_all ); //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_HAIRALL));
		p->writeD( csb[i].iid_R_ear );    //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_REAR));
		p->writeD( csb[i].iid_L_ear );    //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_LEAR));
		p->writeD( csb[i].iid_neck );     //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_NECK));
		p->writeD( csb[i].iid_R_finger ); //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_RFINGER));
		p->writeD( csb[i].iid_L_finger ); //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_LFINGER));
		p->writeD( csb[i].iid_head );     //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_HEAD));
		p->writeD( csb[i].iid_R_hand );   //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_RHAND));
		p->writeD( csb[i].iid_L_hand );   //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_LHAND));
		p->writeD( csb[i].iid_gloves );   //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_GLOVES));
		p->writeD( csb[i].iid_chest );    //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_CHEST));
		p->writeD( csb[i].iid_legs );     //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_LEGS));
		p->writeD( csb[i].iid_feet );     //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_FEET));
		p->writeD( csb[i].iid_back );     //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_BACK));
		p->writeD( csb[i].iid_LR_hand );  //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_LRHAND));
		p->writeD( csb[i].iid_hair );     //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_HAIR));
		p->writeD( csb[i].iid_hair_2 );   //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_HAIR2));
		p->writeD( csb[i].iid_R_bracelet ); //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_RBRACELET));
		p->writeD( csb[i].iid_L_bracelet ); //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_LBRACELET));
		p->writeD( 0x00 ); //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_DECO1));
		p->writeD( 0x00 ); //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_DECO2));
		p->writeD( 0x00 ); //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_DECO3));
		p->writeD( 0x00 ); //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_DECO4));
		p->writeD( 0x00 ); //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_DECO5));
		p->writeD( 0x00 ); //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_DECO6));
		p->writeD( csb[i].iid_belt ); //writeD(charInfoPackage.getPaperdollItemId(Inventory.PAPERDOLL_BELT));
		//
		p->writeUInt( csb[i].hairStyle );
		p->writeUInt( csb[i].hairColor );
		p->writeUInt( csb[i].face );
		//LogDebug( L"Writing hs,hc,face = %u,%u,%u", csb[i].hairStyle, csb[i].hairColor, csb[i].face );
		//
		p->writeDouble( csb[i].HP_max );
		p->writeDouble( csb[i].MP_max );
		//
		p->writeUInt( csb[i].deleteSeconds );
		p->writeUInt( csb[i].classID ); // not base classid
		p->writeUInt( 0x00 ); // TODO: is last used char
		//
		p->writeC( 0x00 ); // TODO: enchant Effect
		p->writeD( 0x00 ); // TODO: aug. ID
		p->writeD( 0x00 ); // transformID is always 0 here
	}
	//
	//FILE *_f = fopen( "CharacterSelectionInfo.log", "wt" );
	//p->dumpToFile( _f );
	//fclose( _f );
	//
	return p;
}