#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_NpcInfo( class L2Client *pcls, L2GamePacket *p )
{
	L2Npc npcInfo;
	npcInfo.parse_NpcInfo( p, pcls->account.getL2Version() );

	// set last time when chars coordinates were known exactly
	//npcInfo.lastMoveTickTime = GetTickCount(); // done by parse()

	// first try to find
	WorldObjectTreeNode wotNode;
	if( pcls->world_tree.GetInfoByObjectID( npcInfo.objectID, &wotNode ) )
	{
		// already exists, update info
		pcls->world_npcs.UpdateNpcInfo( wotNode.getArrayIdx(), &npcInfo );
	}
	else
	{
		// add new NPC
		int idx = pcls->world_npcs.AddNpcInfo( &npcInfo );
		wotNode.setNpc( &npcInfo );
		wotNode.setArrayIdx( idx );
		pcls->world_tree.AddObject( npcInfo.objectID, &wotNode );
	}
	if( npcInfo.isAttackable ) pcls->postUpdateUI( UPDATE_MAP_MOBS );
	else pcls->postUpdateUI( UPDATE_MAP_NPCS );
}
