#include "stdafx.h"
#include "UserAI.h"
#include "ConfigIni.h"
extern class CConfig g_cfg;

/*
writeC(0x16);
writeD(_charObjId);
writeD(_item.getObjectId());
writeD(_item.getItemId());

writeD(_item.getX());
writeD(_item.getY());
writeD(_item.getZ());
// only show item count if it is a stackable item
writeD( _item.isStackable() ? 0x01 : 0x00 );
writeD(_item.getCount());    ////////   writeQ(_count)  since L2 >= T2.3
writeD(1); // unknown  */
void UserAI::UAI_Parse_DropItem( class UserAI *cls, void *l2_game_packet )
{
	UNREFERENCED_PARAMETER(cls);
	if( !l2_game_packet ) return;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	p->getPacketType();
	/*unsigned int charOid = */ p->readUInt();
	unsigned int itemOid = p->readUInt();
	unsigned int itemId = p->readUInt();
	int x = p->readD();
	int y = p->readD();
	int z = p->readD();
	int isStackable = p->readD();
	unsigned long long int count = 0;
	if( g_cfg.L2_version <= (int)L2_VERSION_T22 )
		count = p->readUInt();
	else
		count = p->readUInt64();
	GroundItem gi;
	gi.count = count;
	gi.itemID = itemId;
	gi.objectID = itemOid;
	gi.stackable = isStackable;
	gi.setXYZ( x, y, z );
	//
	GIArray *gia = GIArray::getInstance();
	gia->Lock();
	// try to find existing
	int idx = 0;
	if( (idx = gia->FindGIByObjectID( gi.objectID )) != -1 )
		gia->UpdateGIInfo( idx, &gi );
	else
	{
		idx = gia->AddGIInfo( &gi );
		if( (idx >= 0) && (idx<gia->GA_MAX_ITEMS) )
			WorldObjectTree_AddObject( itemOid, L2OT_ITEM, idx );
	}
	gia->Unlock();
}

/*
writeC(0x05);
writeD(_objectId);
writeD(_itemId);

writeD(_x);
writeD(_y);
writeD(_z);
// only show item count if it is a stackable item
writeD(_stackable);
writeD(_count);     ////////   writeQ(_count)  since L2 >= T2.3
writeD(0x00); //c2 */
void UserAI::UAI_Parse_SpawnItem( class UserAI *cls, void *l2_game_packet )
{
	UNREFERENCED_PARAMETER(cls);
	if( !l2_game_packet ) return;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	p->getPacketType();
	unsigned int itemOid = p->readUInt();
	unsigned int itemId = p->readUInt();
	int x = p->readD();
	int y = p->readD();
	int z = p->readD();
	int isStackable = p->readD();
	unsigned long long int count = 0;
	if( g_cfg.L2_version <= (int)L2_VERSION_T22 )
		count = p->readUInt();
	else
		count = p->readUInt64();
	GroundItem gi;
	gi.count = count;
	gi.itemID = itemId;
	gi.objectID = itemOid;
	gi.stackable = isStackable;
	gi.setXYZ( x, y, z );
	//
	GIArray *gia = GIArray::getInstance();
	gia->Lock();
	// try to find existing
	int idx = 0;
	if( (idx = gia->FindGIByObjectID( gi.objectID )) != -1 )
		gia->UpdateGIInfo( idx, &gi );
	else
	{
		idx = gia->AddGIInfo( &gi );
		if( (idx >= 0) && (idx<gia->GA_MAX_ITEMS) )
			WorldObjectTree_AddObject( itemOid, L2OT_ITEM, idx );
	}
	gia->Unlock();
}
