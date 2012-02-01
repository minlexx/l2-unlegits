#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**  Server: Len   23 [StopMove]
17 00
47              // StopMove
6C 08 00 10     // objectID of char who stopped
9A AC 00 00     // x
F9 A4 00 00     // y
5D F2 FF FF     // z
45 B9 00 00     // heading   **/
void L2Client::ph_StopMove( class L2Client *pcls, L2GamePacket *p )
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
		pcls->usr.stopMove();
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
					pcls->world_chars.chars_array[idx]->stopMove();
					pcls->world_chars.chars_array[idx]->lastMoveTickTime = GetTickCount();
				} break;
			case L2OT_NPC:
				{
					//
					pcls->world_npcs.npcs_array[idx]->x = x;
					pcls->world_npcs.npcs_array[idx]->y = y;
					pcls->world_npcs.npcs_array[idx]->z = z;
					pcls->world_npcs.npcs_array[idx]->stopMove();
					pcls->world_npcs.npcs_array[idx]->lastMoveTickTime = GetTickCount();
				} break;
			}
		}
		else
		{
			log_error( LOG_ERROR, "WOT cannot find objectID while StopMove\n" );
			pcls->addChatToTab( CHAT_DMG, L"WARN: possible invisible GM nearby!" );
		}
	}
}
