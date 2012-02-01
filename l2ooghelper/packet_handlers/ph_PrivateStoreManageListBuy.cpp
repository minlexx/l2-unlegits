#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/** 3 section to this packet
 * 1)playerinfo which is always sent
 * 2)list of items which can be added to buy
 * 3)list of items which have already been setup */
void L2Client::ph_PrivateStoreManageListBuy( class L2Client *pcls, L2GamePacket *p )
{
	//log_error( LOG_OK, "PrivateStoreManageListBuy\n" );
	L2_VERSION ver = pcls->account.getL2Version();

	p->getPacketType();
	int cnt = 0;
	int i = 0;
	//
	TradeItemsList *list1 = new TradeItemsList();
	TradeItemsList *list2 = new TradeItemsList();
	// section 1
	p->readUInt(); // objectID
	list1->packageSale = list2->packageSale = 0; // buylist cannot be package

	// cur. money
	if( ver < L2_VERSION_T23 ) list1->curMoney = p->readInt();
	else list1->curMoney = p->readInt64();

	// section 2
	cnt = p->readInt();
	for( i=0; i<cnt; i++ )
	{
		TradeItem it;
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
		p->readUInt(); // b.part
		it.type2 = (int)p->readUShort();
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
		list1->addItem( it );
	}
	// section 3
	cnt = p->readInt();
	for( i=0; i<cnt; i++ )
	{
		TradeItem it;
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
		p->readUInt(); // b.part
		it.type2 = (int)p->readUShort();
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
		list2->addItem( it );
	}
	pcls->handle_PrivateStoreManageListBuy( list1, list2 );
}
