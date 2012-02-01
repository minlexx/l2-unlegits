#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**   Server: Len    7 [Revive]
07 00
01
3F 8E 05 10   // objectID?    ***/
void L2Client::ph_Revive( class L2Client *pcls, L2GamePacket *p )
{
	int party_idx = 0;
	p->getPacketType();
	unsigned int objectID = p->readUInt();
	if( objectID == pcls->usr.objectID )
	{
		// user is revived!
		log_error( LOG_USERAI, "User is Revived!\n" );
		pcls->addChatToTab( CHAT_DMG, L"You are revived!" );
		pcls->usr.isDead = 0;
		pcls->usr.canResurrectToVillage = pcls->usr.canResurrectToCH = pcls->usr.canResurrectToCastle = 
			pcls->usr.canResurrectToFortress = pcls->usr.canResurrectToSiegeHQ = pcls->usr.canResurrectFixed = 0;
	}
	else if( pcls->party.isInParty( objectID, &party_idx ) )
	{
		L2Player *partyMember = pcls->party.getPartyPlayer( party_idx );
		partyMember->isAlikeDead = 0;
		wchar_t wmes[128] = {0};
		swprintf( wmes, 127, L"Party player [%s] revived", partyMember->charName );
		pcls->addChatToTab( CHAT_DMG, wmes );
	}
	// someone in the world revived
	WorldObjectTreeNode wotNode;
	if( pcls->world_tree.GetInfoByObjectID( objectID, &wotNode ) )
	{
		int idx = wotNode.getArrayIdx();
		switch( wotNode.getObjectType() )
		{
		case L2OT_PC:
			{
				pcls->world_chars.chars_array[idx]->isAlikeDead = 0;
				wchar_t wmes[128] = {0};
				swprintf( wmes, 127, L"Character [%s] revived", pcls->world_chars.chars_array[idx]->charName );
				pcls->addChatToTab( CHAT_DMG, wmes );
			} break;
		}
	}
}

