#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**  Server: Len   27 [MoveToPawn]
1B 00
72                 // MoveToPawn
6C 08 00 10        // _charObjId   // 6C080010 objectID who is moving (can be user at least)
B2 16 00 10        // _targetId    // B2160010 is target object id, to which he moves to
39 00 00 00        // _distance  wtf is 57?..
C7 AC 00 00        // x
0A A6 00 00        // y  dunno if it is user coordinates or mob coordinates
5D F2 FF FF        // z
possible these are target's coordinates */
void L2Client::ph_MoveToPawn( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	unsigned int movingObjectId = p->readUInt();
	unsigned int targetObjectId = p->readUInt();
	/*int dist = */p->readInt();
	int targetX = p->readInt();
	int targetY = p->readInt();
	int targetZ = p->readInt();
	//
	if( movingObjectId == pcls->usr.objectID )
	{
		//pcls->usr.targetObjectID = targetObjectId;
		//log_error( LOG_OK, "User (%d,%d,%d) is moving to pawn %u (%d,%d,%d)\n",
		//	pcls->usr.x, pcls->usr.y, pcls->usr.z, targetObjectId, targetX, targetY, targetZ );
		// get target coordinates
		WorldObjectTreeNode wotNode;
		if( pcls->world_tree.GetInfoByObjectID( targetObjectId, &wotNode ) )
		{
			int idx = wotNode.getArrayIdx();
			switch( wotNode.getObjectType() )
			{
			case L2OT_PC:
				{
					targetX = pcls->world_chars.chars_array[idx]->x;
					targetY = pcls->world_chars.chars_array[idx]->y;
					targetZ = pcls->world_chars.chars_array[idx]->z;
				} break;
			case L2OT_NPC:
				{
					targetX = pcls->world_npcs.npcs_array[idx]->x;
					targetY = pcls->world_npcs.npcs_array[idx]->y;
					targetZ = pcls->world_npcs.npcs_array[idx]->z;
				} break;
			}
			pcls->usr.startMoveTo( targetX, targetY, targetZ, pcls->usr.x, pcls->usr.y, pcls->usr.z );
			//
			//log_error( LOG_OK, "Calculated pawn coords: (%d,%d,%d)\n", targetX, targetY, targetZ );
		}
		//log_error( LOG_OK, "MoveToPawn: user move\n" );
	}
	else
	{
		WorldObjectTreeNode wotNode;
		// get target coordinates
		if( pcls->world_tree.GetInfoByObjectID( targetObjectId, &wotNode ) )
		{
			int idx = wotNode.getArrayIdx();
			switch( wotNode.getObjectType() )
			{
			case L2OT_PC:
				{
					targetX = pcls->world_chars.chars_array[idx]->x;
					targetY = pcls->world_chars.chars_array[idx]->y;
					targetZ = pcls->world_chars.chars_array[idx]->z;
				} break;
			case L2OT_NPC:
				{
					targetX = pcls->world_npcs.npcs_array[idx]->x;
					targetY = pcls->world_npcs.npcs_array[idx]->y;
					targetZ = pcls->world_npcs.npcs_array[idx]->z;
				} break;
			}
		}
		else
		{
			// maybe someone is movin to me?
			if( targetObjectId == pcls->usr.objectID )
			{
				targetX = pcls->usr.x;
				targetY = pcls->usr.y;
				targetZ = pcls->usr.z;
			}
		}
		// get moving object's coordinates
		wotNode.~WorldObjectTreeNode();
		if( pcls->world_tree.GetInfoByObjectID( movingObjectId, &wotNode ) )
		{
			int idx = wotNode.getArrayIdx();
			switch( wotNode.getObjectType() )
			{
			case L2OT_PC:
				{
					//log_error( LOG_OK, "MoveToPawn: PC move [%S]\n", pcls->world_chars.chars_array[idx]->charName );
					int x = pcls->world_chars.chars_array[idx]->x;
					int y = pcls->world_chars.chars_array[idx]->y;
					int z = pcls->world_chars.chars_array[idx]->z;
					pcls->world_chars.chars_array[idx]->startMoveTo( targetX, targetY, targetZ, x, y, z );
				} break;
			case L2OT_NPC:
				{
					int x = pcls->world_npcs.npcs_array[idx]->x;
					int y = pcls->world_npcs.npcs_array[idx]->y;
					int z = pcls->world_npcs.npcs_array[idx]->z;
					//log_error( LOG_OK, "MoveToPawn NPC %S\n", pcls->world_npcs.npcs_array[idx]->charName );
					pcls->world_npcs.npcs_array[idx]->startMoveTo( targetX, targetY, targetZ, x, y, z );
				} break;
			}
		}
		else log_error( LOG_ERROR, "WOT cannot find objectID while MoveToPawn\n" );
	}
}
