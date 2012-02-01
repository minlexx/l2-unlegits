#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**  Server: Len   23 [ValidateLocation]
17 00
79
B2 16 00 10     // mob/npc objectID
91 AC 00 00     // x
C5 A4 00 00     // y
5D F2 FF FF     // z
FA C5 00 00     // heading   **/
void L2Client::ph_ValidateLocation( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	unsigned int objectID = p->readUInt();
	int x = p->readInt();
	int y = p->readInt();
	int z = p->readInt();
	//
	if( pcls->usr.objectID == objectID )
	{
		pcls->usr.x = x;
		pcls->usr.y = y;
		pcls->usr.z = z;
		pcls->usr.lastMoveTickTime = GetTickCount();
	}
	else
	{
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
				} break;
			case L2OT_NPC:
				{
					//
					pcls->world_npcs.npcs_array[idx]->x = x;
					pcls->world_npcs.npcs_array[idx]->y = y;
					pcls->world_npcs.npcs_array[idx]->z = z;
					pcls->world_npcs.npcs_array[idx]->lastMoveTickTime = GetTickCount();
				} break;
			}
		}
		else log_error( LOG_ERROR, "WOT cannot find objectID while ValidateLocation\n" );
	}
}
