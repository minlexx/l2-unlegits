#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**
writeC(0x24);
writeD(_targetObjId);
writeD(_x);
writeD(_y);
writeD(_z);
writeD(0x00); **/
void L2Client::ph_TargetUnselected( class L2Client *pcls, L2GamePacket *p )
{
	int party_idx = 0;
	p->getPacketType();
	unsigned int oid = p->readUInt();
	/*int x = */p->readInt();
	/*int y = */p->readInt();
	/*int z = */p->readInt();
	//
	if( oid == pcls->usr.objectID ) // update self target
	{
		pcls->usr.targetObjectID = 0;
		pcls->usr.targetCurHp = 0.0;
		pcls->usr.targetMaxHp = 0.0;
		//log_error( LOG_OK, "User TargetUnselected\n" );
		pcls->addChatToTab( CHAT_DMG, L"You unselect target" );
		pcls->postUpdateUI( UPDATE_USER_TARGET );
	}
	else if( pcls->party.isInParty( oid, &party_idx ) ) // update target in party
	{
		//L2Player *partyMember = pcls->party.getPartyPlayer( party_idx );
		pcls->party.setPlayerTarget( oid, 0 );
		//log_error( LOG_OK, "Party member [%S] cancelled target\n", partyMember->charName );
		//wchar_t wmes[256] = {0};
		//swprintf( wmes, 255, L"Party member [%s] unselected target", partyMember->charName );
		//pcls->addChatToTab( CHAT_DMG, wmes );
	}
	// someone in the world cancelled target, update target of object in the world
	WorldObjectTreeNode wotNode;
	if( pcls->world_tree.GetInfoByObjectID( oid, &wotNode ) )
	{
		int idx = wotNode.getArrayIdx();
		switch( wotNode.getObjectType() )
		{
		case L2OT_PC:
			{
				pcls->world_chars.chars_array[idx]->targetObjectID = 0;
				//wchar_t wmes[256] = {0};
				//swprintf( wmes, 255, L"Character [%s] unselected target", pcls->world_chars.chars_array[idx]->charName );
				//pcls->addChatToTab( CHAT_DMG, wmes );
			} break;
		}
	}
}
