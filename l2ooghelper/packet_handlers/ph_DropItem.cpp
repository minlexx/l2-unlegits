#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**
writeC(0x16);
writeD(_charObjId);           // who dropped item
writeD(_item.getObjectId());  // item object id
writeD(_item.getItemId());    // itemID
writeD(_item.getX());
writeD(_item.getY());
writeD(_item.getZ());
writeD(_item.isStackable());  // only show item count if it is a stackable item
writeD(_item.getCount()); */
void L2Client::ph_DropItem( class L2Client *pcls, L2GamePacket *p )
{
	L2_VERSION ver = pcls->account.getL2Version();

	GroundItem it;
	unsigned int charObjectID = 0;
	p->getPacketType();
	charObjectID = p->readUInt();
	it.objectID = p->readUInt();
	it.itemID = p->readUInt();
	it.x = p->readInt();
	it.y = p->readInt();
	it.z = p->readInt();
	it.stackable = p->readUInt();

	if( ver < L2_VERSION_T23 ) it.count = p->readUInt();
	else it.count = p->readUInt64();
	// readD(); // unknown

	//log_error( LOG_USERAI, "DropItem player [%u] dropped %s [%u] at (%d,%d,%d)\n",
	//	charObjectID, it.itemName, it.itemID, it.x, it.y, it.z );
	// add item to ground items
	int idx = pcls->world_ground_items.AddGIInfo( &it );
	// add item to world object tree
	WorldObjectTreeNode wotNode;
	wotNode.setArrayIdx( idx );
	wotNode.setGroundItem( &it );
	pcls->world_tree.AddObject( it.objectID, &wotNode );
	pcls->postUpdateUI( UPDATE_MAP_ITEMS );
}
