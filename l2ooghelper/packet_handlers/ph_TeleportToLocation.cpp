#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**  Server: Len   19 [TeleportToLocation]
13 00
22
6D 08 00 10   // oid 268437613
3F 1D 01 00   //  x 73023
DC CE 01 00   //  y 118492
9D F1 FF FF   //  z -3683    **/
void L2Client::ph_TeleportToLocation( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	unsigned int objectID = p->readUInt();
	int x = p->readInt();
	int y = p->readInt();
	int z = p->readInt();
	if( objectID == pcls->usr.objectID )
	{
		pcls->usr.stopMove();
		pcls->skills.stopCasting();
		//
		// TODO: force clear all known PC/NPC/Items?
		//
		pcls->usr.x = x;
		pcls->usr.y = y;
		pcls->usr.z = z;
		// update UI
		pcls->addChatToTabFormat( CHAT_DMG, L"You teleport to (%d,%d,%d)", x, y, z );
		// reply with ValidatePosition
		pcls->send_ValidatePosition();
		// send Appearing
		pcls->send_Appearing();
		return;
	}
	// some other object in the world teleported nearby? O_o
	WorldObjectTreeNode wotNode;
	if( pcls->world_tree.GetInfoByObjectID( objectID, &wotNode ) )
	{
		int idx = wotNode.getArrayIdx();
		switch( wotNode.getObjectType() )
		{
		case L2OT_PC:
			{
				pcls->world_chars.chars_array[idx]->x = x;
				pcls->world_chars.chars_array[idx]->y = y;
				pcls->world_chars.chars_array[idx]->z = z;
				pcls->world_chars.chars_array[idx]->lastMoveTickTime = GetTickCount();
				// party member?
				int party_idx = 0;
				if( pcls->party.isInParty( objectID, &party_idx ) )
				{
					L2Player *member = pcls->party.getPartyPlayer( party_idx );
					member->setXYZ( x, y, z );
				}
			} break;
		case L2OT_NPC:
			{
				pcls->world_npcs.npcs_array[idx]->x = x;
				pcls->world_npcs.npcs_array[idx]->y = y;
				pcls->world_npcs.npcs_array[idx]->z = z;
				pcls->world_npcs.npcs_array[idx]->lastMoveTickTime = GetTickCount();
			} break;
		}
	}
	else
	{
		log_error( LOG_WARNING, "TeleportToLocation cannot find objID [%u]\n", objectID );
	}
}
