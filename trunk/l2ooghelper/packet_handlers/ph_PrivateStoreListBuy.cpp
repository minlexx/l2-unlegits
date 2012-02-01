#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**	  Server: Len   81 [PrivateStoreListBuy]
51 00
BE
CE 09 00 10      // buyer oid
EA B0 0B 00      // cur.user adena
01 00 00 00      // sell items count
  // for each item
C7 09 00 10     // item oid
F4 25 00 00     // item itemID
00 00           // enchant
02 00 00 00     // count
00 00 00 00     // reference price
00 00           // 00
00 00 00 00     // body part
05 00           // type2
01 00 00 00     // buyers price
02 00 00 00     // max buy amount
FE FF FF FF 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 // attributes **/
void L2Client::ph_PrivateStoreListBuy( class L2Client *pcls, L2GamePacket *p )
{
	L2_VERSION ver = pcls->account.getL2Version();

	int i, cnt;
	TradeItemsList *list = new TradeItemsList();
	//
	p->getPacketType();
	list->listID = p->readUInt(); // save oid as listID

	// cur user adena
	if( ver < L2_VERSION_T23 ) list->curMoney = p->readInt();
	else list->curMoney = p->readInt64();

	cnt = p->readInt(); // count
	for( i=0; i<cnt; i++ )
	{
		TradeItem it;
		it.objectID = p->readUInt();
		it.itemID = p->readUInt();
		it.enchantLevel = (int)p->readUShort();

		if( ver < L2_VERSION_T23 )
		{
			it.count = p->readInt();
			it.storePrice = p->readInt();
		}
		else
		{
			it.count = p->readInt64();
			it.storePrice = p->readInt64();
		}

		p->readUShort();
		p->readUInt(); // body part
		it.type2 = (int)p->readUShort();

		if( ver < L2_VERSION_T23 )
		{
			it.price = p->readInt();
			it.count = p->readInt();
		}
		else
		{
			it.price = p->readInt64();
			it.count = p->readInt64();
		}

		// attributes
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
		// add to list
		list->addItem( it );
		//log_error_np( LOG_OK, "PrivateStoreListBuy: iid,oid(%u,%u) cnt,price(%d,%d)\n",
		//	it.itemID, it.objectID, it.count, it.price );
	}
	// find name
	WorldObjectTreeNode wotNode;
	if( pcls->world_tree.GetInfoByObjectID( list->listID, &wotNode ) )
	{
		if( wotNode.getObjectType() == L2OT_PC )
			wcscpy( list->message, pcls->world_chars.chars_array[wotNode.getArrayIdx()]->charName );
		else log_error( LOG_ERROR, "PrivateStoreListBuy: buyer must be Player!\n" );
	}
	else log_error( LOG_ERROR, "WOT: cannot find objectID while PrivateStoreListBuy!\n" );
	pcls->handle_PrivateStoreBuyList( list );
}
