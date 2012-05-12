#include "stdafx.h"
#include "USerAI.h"
#include "Logger.h"
#include "RadarDllWnd.h"

void UserAI::UAI_Parse_Die( class UserAI *cls, void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	// parse
	p->getPacketType();
	unsigned int objectID = p->readUInt();
	// end parse
	if( objectID == cls->usr.objectID )
	{
		cls->usr.isDead = 1;
		log_error( LOG_USERAI, "You died!\n" );
	}
	else
	{
		bool oid_found = false;
		L2OBJECT_TYPE objType = L2OT_NONE;
		int idx = -1;
		if( WorldObjectTree_GetInfoByObjectID( objectID, &objType, &idx ) )
		{
			switch( objType )
			{
			case L2OT_PC:
				{
					chars_array[idx]->isAlikeDead = 1;
					log_error( LOG_USERAI, "Character [%S] died\n", chars_array[idx]->charName );
					RadarWnd_UpdChar( objectID );
				} break;
			case L2OT_NPC:
				{
					npc_array[idx]->isAlikeDead = 1;
					RadarWnd_UpdNpc( objectID );
				} break;
			}
			oid_found = true;
		}
		// update also party if needed
		int party_idx = -1;
		if( cls->party.isInParty( objectID, &party_idx ) )
		{
			L2Player *partyMember = cls->party.getPartyPlayer( party_idx );
			if( partyMember )
			{
				log_error( LOG_USERAI, "Party member [%d] = [%S] died!\n", party_idx, partyMember->charName );
				partyMember->isAlikeDead = 1;
				oid_found = true;
			}
		}
		if( !oid_found ) log_error( LOG_ERROR, "UserAI::UAI_Parse_Die(): cannot find objectID while Die\n" );
	}
}

void UserAI::UAI_Parse_Revive( class UserAI *cls, void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	int party_idx = 0;
	// parse
	p->getPacketType();
	unsigned int objectID = p->readUInt();
	// end parse
	if( objectID == cls->usr.objectID )
	{
		// user is revived!
		log_error( LOG_USERAI, "You revived!\n" );
		cls->usr.isDead = 0;
		//pcls->usr.canResurrectToVillage = pcls->usr.canResurrectToCH = pcls->usr.canResurrectToCastle = 
		//	pcls->usr.canResurrectToFortress = pcls->usr.canResurrectToSiegeHQ = pcls->usr.canResurrectFixed = 0;
	}
	else if( cls->party.isInParty( objectID, &party_idx ) )
	{
		L2Player *partyMember = cls->party.getPartyPlayer( party_idx );
		if( partyMember )
		{
			partyMember->isAlikeDead = 0;
			log_error( LOG_USERAI, "Party player [%S] revived\n", partyMember->charName );
		}
	}
	// someone in the world revived
	int idx = -1;
	L2OBJECT_TYPE objType = L2OT_NONE;
	if( WorldObjectTree_GetInfoByObjectID( objectID, &objType, &idx ) )
	{
		switch( objType )
		{
		case L2OT_PC:
			{
				chars_array[idx]->isAlikeDead = 0;
				log_error( LOG_USERAI, "Character [%S] revived\n", chars_array[idx]->charName );
				RadarWnd_UpdChar( objectID );
			} break;
		}
	}
}
