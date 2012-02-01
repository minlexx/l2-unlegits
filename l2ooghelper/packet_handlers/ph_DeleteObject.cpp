#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**
writeC(0x08);
writeD(_objectId);
writeD(0x00); //c2 */
void L2Client::ph_DeleteObject( class L2Client *pcls, L2GamePacket *p )
{
	unsigned int objectID;
	p->getPacketType();
	objectID = p->readUInt();
	if( objectID == 0 ) return; // O_o WTF
	// delete object from world object tree and from corresponding array
	// first try to find object in world tree
	WorldObjectTreeNode wotNode;
	if( pcls->world_tree.GetInfoByObjectID( objectID, &wotNode ) )
	{
		// delete from world tree
		pcls->world_tree.DelObject( objectID );
		// delete from specified array/list
		switch( wotNode.getObjectType() )
		{
		case L2OT_PC:
			{
				pcls->world_chars.DeleteCharByArrayIdx( wotNode.getArrayIdx() );
				pcls->postUpdateUI( UPDATE_MAP_PLAYERS );
			} break;
		case L2OT_NPC:
			{
				int isAttackable = 0;
				L2Npc *deletingNpc = wotNode.cast_L2Npc();
				if( deletingNpc ) isAttackable = deletingNpc->isAttackable;
				pcls->world_npcs.DelNPCByArrayIdx( wotNode.getArrayIdx() );
				if( isAttackable ) pcls->postUpdateUI( UPDATE_MAP_MOBS );
				else pcls->postUpdateUI( UPDATE_MAP_NPCS );
			} break;
		case L2OT_ITEM:
			{
				int del_idx = wotNode.getArrayIdx();
				//log_error( LOG_USERAI, "DeleteObject: delete item [%s] idx %d!\n",
				//	pcls->world_ground_items.gi_array[del_idx]->itemName, del_idx );
				pcls->world_ground_items.DelGIByArrayIdx( del_idx );
				pcls->postUpdateUI( UPDATE_MAP_ITEMS );
			} break;
		default: log_error( LOG_ERROR, "DeleteObject: unknown objectType for oid %u\n", objectID ); break;
		}
	}
	else log_error( LOG_ERROR, "WOT cannot find OID %u while DeleteObject\n", objectID );
}
