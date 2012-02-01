#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**
writeC(0x00);
writeD(_charObjId);
writeD(_canTeleport ? 0x01 : 0); 
writeD(0x00);                                               // 6d 01 00 00 00 - to hide away
writeD(0x00);                                               // 6d 02 00 00 00 - to castle
writeD(0x00);                                               // 6d 03 00 00 00 - to siege HQ
writeD(_sweepable ? 0x01 : 0x00);                               // sweepable  (blue glow)
writeD(_access.allowFixedRes() ? 0x01: 0x00);                  // 6d 04 00 00 00 - to FIXED
writeD(0x00);    // 6d 05 00 00 00 - to fortress   **/
void L2Client::ph_Die( class L2Client *pcls, L2GamePacket *p )
{
	int party_idx = 0;
	p->getPacketType();
	unsigned int objectID = p->readUInt();
	//
	if( objectID == pcls->usr.objectID ) // user died :(
	{
		// user died :(((
		log_error( LOG_USERAI, "User died ((((((!\n" );
		pcls->addChatToTab( CHAT_DMG, L"You are dead! :(((" );
		pcls->usr.isDead = 1;
		pcls->usr.canResurrectToVillage  = p->readInt();
		pcls->usr.canResurrectToCH       = p->readInt();
		pcls->usr.canResurrectToCastle   = p->readInt();
		pcls->usr.canResurrectToSiegeHQ  = p->readInt();
		p->readUInt(); // sweepable (user cannot be sweepable) )))
		pcls->usr.canResurrectFixed      = p->readInt();
		pcls->usr.canResurrectToFortress = p->readInt();
		return;
	}
	// died other world object, not user :)
	// TODO: store isSweepable when mob dies
	bool oid_found = false;
	WorldObjectTreeNode wotNode;
	if( pcls->world_tree.GetInfoByObjectID( objectID, &wotNode ) )
	{
		int idx = wotNode.getArrayIdx();
		switch( wotNode.getObjectType() )
		{
		case L2OT_PC:
			{
				pcls->world_chars.chars_array[idx]->isAlikeDead = 1;
				wchar_t wmes[128] = {0};
				swprintf( wmes, 127, L"Character [%s] died", pcls->world_chars.chars_array[idx]->charName );
				pcls->addChatToTab( CHAT_DMG, wmes );
			} break;
		case L2OT_NPC:
			{
				pcls->world_npcs.npcs_array[idx]->isAlikeDead = 1;
			} break;
		}
		oid_found = true;
	}
	// update also party if needed
	if( pcls->party.isInParty( objectID, &party_idx ) )
	{
		L2Player *partyMember = pcls->party.getPartyPlayer( party_idx );
		if( partyMember )
		{
			log_error( LOG_USERAI, "Party member [%d] = [%S] died!\n", party_idx, partyMember->charName );
			partyMember->isAlikeDead = 1;
			wchar_t wmes[128] = {0};
			swprintf( wmes, 127, L"Party member [%s] died!", partyMember->charName );
			pcls->addChatToTab( CHAT_DMG, wmes );
			oid_found = true;
		}
	}
	if( !oid_found ) log_error( LOG_ERROR, "WOT cannot find objectID while Die\n" );
}
