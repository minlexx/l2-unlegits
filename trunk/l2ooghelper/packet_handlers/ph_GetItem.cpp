#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**
writeC(0x17);
writeD(_playerId);            // who picked item?
writeD(_item.getObjectId());  // item objectID
writeD(_item.getX());
writeD(_item.getY());
writeD(_item.getZ());  */
void L2Client::ph_GetItem( class L2Client *pcls, L2GamePacket *p )
{
	UNREFERENCED_PARAMETER(pcls);
	GroundItem it;
	unsigned int charObjectID = 0;
	p->getPacketType();
	charObjectID = p->readUInt();
	it.objectID = p->readUInt();
	it.x = p->readInt();
	it.y = p->readInt();
	it.z = p->readInt();
	//log_error( LOG_USERAI, "GetItem player [%u] picked up [%u] at (%d,%d,%d)\n",
	//	charObjectID, it.objectID, it.x, it.y, it.z );
	// before GetItem server sends DeleteObject, so no need to delete item once more
}
