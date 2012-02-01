#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**
writeC(0x05);
writeD(_objectId);
writeD(_itemId);

writeD(_x);
writeD(_y);
writeD(_z);
// only show item count if it is a stackable item
writeD(_stackable);
writeD(_count);
writeD(0x00); //c2 */
void L2Client::ph_SpawnItem( class L2Client *pcls, L2GamePacket *p )
{
	L2_VERSION ver = pcls->account.getL2Version();

	GroundItem it;
	p->getPacketType();
	it.objectID = p->readUInt();
	it.itemID = p->readUInt();
	it.x = p->readInt();
	it.y = p->readInt();
	it.z = p->readInt();
	it.stackable = p->readUInt();

	if( ver < L2_VERSION_T23 ) it.count = p->readD();
	else it.count = p->readQ();

	//log_error( LOG_USERAI, "SpawnItem %s [%u] at (%d,%d,%d)\n", it.itemName, it.itemID, it.x, it.y, it.z );
	// add item to ground items
	int idx = pcls->world_ground_items.AddGIInfo( &it );
	// add item to world object tree
	WorldObjectTreeNode wotNode;
	wotNode.setArrayIdx( idx );
	wotNode.setGroundItem( &it );
	pcls->world_tree.AddObject( it.objectID, &wotNode );
	pcls->postUpdateUI( UPDATE_MAP_ITEMS );
}
