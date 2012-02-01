#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_RecipeShopManageList( class L2Client *pcls, L2GamePacket *p )
{
	L2_VERSION ver = pcls->account.getL2Version();

	p->getPacketType();
	int cnt;
	int i;
	//
	TradeItemsList *list1 = new TradeItemsList();
	TradeItemsList *list2 = new TradeItemsList();
	// section 1
	p->readUInt(); // objectID
	list1->curMoney = p->readInt();
	list1->packageSale = list2->packageSale = 0; // craft cannot be package
	list1->isCommonCraft = list2->isCommonCraft = p->readUInt();
	// section 2
	cnt = p->readInt();
	for( i=0; i<cnt; i++ )
	{
		TradeItem it;
		it.itemID = p->readUInt(); // recipe ID here
		p->readInt(); // pass seq.number
		list1->addItem( it );
	}
	// section 3
	cnt = p->readInt();
	for( i=0; i<cnt; i++ )
	{
		TradeItem it;
		it.itemID = p->readInt();
		p->readInt(); // pass 0x00
		if( ver < L2_VERSION_T23 ) it.price = p->readInt();
		else it.price = p->readInt64();
		list2->addItem( it );
	}
	pcls->handle_RecipeShopManageList( list1, list2 );
}
