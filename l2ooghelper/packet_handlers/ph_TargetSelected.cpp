#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**
writeC(0x23);
writeD(_objectId);
writeD(_targetObjId);
writeD(_x);
writeD(_y);
writeD(_z);
writeD(0x00); **/
void L2Client::ph_TargetSelected( class L2Client *pcls, L2GamePacket *p )
{
	int party_idx = 0;
	p->getPacketType();
	unsigned int oid = p->readUInt();
	unsigned int toid = p->readUInt();
	/*int x = */p->readInt();
	/*int y = */p->readInt();
	/*int z = */p->readInt();
	//
	if( oid == pcls->usr.objectID )
	{
		pcls->usr.targetObjectID = toid;
		//log_error( LOG_OK, "User TargetSelected\n" );
		wchar_t wmes[32] = {0};
		swprintf( wmes, 31, L"You target %u", toid );
		pcls->addChatToTab( CHAT_DMG, wmes );
		pcls->postUpdateUI( UPDATE_USER_TARGET );
	}
	else if( pcls->party.isInParty( oid, &party_idx ) )
	{
		//L2Player *partyMember = pcls->party.getPartyPlayer( party_idx );
		//log_error( LOG_OK, "Party member [%S] target to %u\n", partyMember->charName, toid );
		pcls->party.setPlayerTarget( oid, toid );
		//wchar_t wmes[64] = {0};
		//swprintf( wmes, 63, L"Party member [%s] changed target to %u", partyMember->charName, toid );
		//pcls->addChatToTab( CHAT_DMG, wmes );
	}
	// update target in the world alse
	WorldObjectTreeNode wotNode;
	if( pcls->world_tree.GetInfoByObjectID( oid, &wotNode ) )
	{
		int idx = wotNode.getArrayIdx();
		switch( wotNode.getObjectType() )
		{
		case L2OT_PC:
			{
				pcls->world_chars.chars_array[idx]->targetObjectID = toid;
				//wchar_t wmes[256] = {0};
				//swprintf( wmes, 255, L"Character [%s] selected target %u", pcls->world_chars.chars_array[idx]->charName, toid );
				//pcls->addChatToTab( CHAT_DMG, wmes );
				if( toid == pcls->usr.objectID )
				{
					//log_error( LOG_USERAI, "You are targeted by [%S]\n", pcls->world_chars.chars_array[idx]->charName );
					wchar_t wmessage[256] = {0};
					swprintf( wmessage, 255, L"You are targeted by [%s]", pcls->world_chars.chars_array[idx]->charName );
					pcls->addChatToTab( CHAT_DMG, wmessage, NULL );
				}
				else if( pcls->party.isInParty( toid, &party_idx ) )
				{
					L2Player *partyMember = pcls->party.getPartyPlayer( party_idx );
					wchar_t wmes[64] = {0};
					swprintf( wmes, 63, L"Party member [%s] is on target of [%s]",
						partyMember->charName, pcls->world_chars.chars_array[idx]->charName );
					pcls->addChatToTab( CHAT_DMG, wmes );
				}
			} break;
		}
	}
}
