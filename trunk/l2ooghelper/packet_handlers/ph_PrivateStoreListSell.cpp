#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**    Server: Len   85 [PrivateStoreListSell]
55 00
A1
CE 09 00 10              // seller oid
00 00 00 00              // is package sale
EB B0 0B 00              // cur.user adena
01 00 00 00              // sell items count
 /// for each item
05 00 00 00         // type2
D0 09 00 10         // item oid
F4 25 00 00         // item itemID
01 00 00 00         // item count
00 00               // 00
00 00               // enchant
00 00               // 00
00 00 00 00         // body part
01 00 00 00         // sell price
00 00 00 00         // reference price (original price)
FE FF FF FF 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 // attribs  **/
void L2Client::ph_PrivateStoreListSell( class L2Client *pcls, L2GamePacket *p )
{
	L2_VERSION ver = pcls->account.getL2Version();

	int i, cnt;
	TradeItemsList *list = new TradeItemsList();
	p->getPacketType();
	list->listID = p->readUInt(); // save oid as listID
	list->packageSale = p->readInt();

	if( ver < L2_VERSION_T23 ) list->curMoney = p->readInt();
	else list->curMoney = p->readInt64();

	cnt = p->readInt();
	for( i=0; i<cnt; i++ )
	{
		TradeItem it;
		it.type2 = p->readInt();
		it.objectID = p->readUInt();
		it.itemID = p->readUInt();

		if( ver < L2_VERSION_T23 ) it.count = p->readInt();
		else it.count = p->readInt64();

		p->readUShort();
		it.enchantLevel = (int)p->readUShort();
		p->readUShort();
		p->readUInt(); // b. part

		if( ver < L2_VERSION_T23 )
		{
			it.price = p->readInt();
			it.storePrice = p->readInt();
		}
		else
		{
			it.price = p->readInt64();
			it.storePrice = p->readInt64();
		}

		// attribs
		if( ver < L2_VERSION_T23 )
		{
			it.attributeAtkType   = p->readD();
			it.attributeAtkValue  = p->readD();
			it.attributeDefFire   = p->readD();
			it.attributeDefWater  = p->readD();
			it.attributeDefWind   = p->readD();
			it.attributeDefEarth  = p->readD();
			it.attributeDefHoly   = p->readD();
			it.attributeDefUnholy = p->readD();
		}
		else
		{
			it.attributeAtkType   = p->readH();
			it.attributeAtkValue  = p->readH();
			it.attributeDefFire   = p->readH();
			it.attributeDefWater  = p->readH();
			it.attributeDefWind   = p->readH();
			it.attributeDefEarth  = p->readH();
			it.attributeDefHoly   = p->readH();
			it.attributeDefUnholy = p->readH();
		}
		// add
		list->addItem( it );
	}
	// find name
	WorldObjectTreeNode wotNode;
	if( pcls->world_tree.GetInfoByObjectID( list->listID, &wotNode ) )
	{
		if( wotNode.getObjectType() == L2OT_PC )
			wcscpy( list->message, pcls->world_chars.chars_array[wotNode.getArrayIdx()]->charName );
		else log_error( LOG_ERROR, "PrivateStoreListSell: seller must be Player!\n" );
	}
	else log_error( LOG_ERROR, "WOT: cannot find objectID while PrivateStoreListSell!\n" );
	pcls->handle_PrivateStoreSellList( list );
}
