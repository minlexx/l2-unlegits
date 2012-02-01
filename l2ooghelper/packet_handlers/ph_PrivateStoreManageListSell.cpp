#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/** 3 section to this packet
 * 1)playerinfo which is always sent
 * 2)list of items which can be added to sell
 * 3)list of items which have already been setup */
void L2Client::ph_PrivateStoreManageListSell( class L2Client *pcls, L2GamePacket *p )
{
	//log_error( LOG_DEBUG, "PrivateStoreManageListSell\n" );
	L2_VERSION ver = pcls->account.getL2Version();

	p->getPacketType();
	int cnt;
	int i;
	//
	TradeItemsList *list1 = new TradeItemsList();
	TradeItemsList *list2 = new TradeItemsList();
	// section 1
	p->readUInt(); // objectID
	list1->packageSale = list2->packageSale = p->readUInt(); // is package sale

	// cur money
	if( ver < L2_VERSION_T23 ) list1->curMoney = p->readInt();
	else list1->curMoney = p->readInt64();

	// section 2
	cnt = p->readInt();
	for( i=0; i<cnt; i++ )
	{
		TradeItem it;
		it.type2 = p->readUInt();
		it.objectID = p->readUInt();
		it.itemID = p->readUInt();
		if( ver < L2_VERSION_T23 ) it.count = p->readInt();
		else it.count = p->readInt64();
		p->readUShort();
		it.enchantLevel = (int)p->readUShort();
		p->readUShort();
		p->readUInt(); // body part
		if( ver < L2_VERSION_T23 ) it.storePrice = p->readInt();
		else it.storePrice = p->readInt();
		it.price = 0;
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
		//
		list1->addItem( it );
	}
	// section 3
	cnt = p->readInt();
	for( i=0; i<cnt; i++ )
	{
		TradeItem it;
		it.type2 = p->readUInt();
		it.objectID = p->readUInt();
		it.itemID = p->readUInt();
		if( ver < L2_VERSION_T23 ) it.count = p->readInt();
		else it.count = p->readInt64();
		p->readUShort();
		it.enchantLevel = (int)p->readUShort();
		p->readUShort();
		p->readUInt(); // body part
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
		//
		list2->addItem( it );
	}
	//
	pcls->handle_PrivateStoreManageListSell( list1, list2 );
}

