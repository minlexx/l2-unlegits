#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/***    Server: Len 2765 [BuyList] |
CD 0A
07
C7 55 35 00           // money    3 495 367
90 00 00 00           // list ID     144
2B 00                 // items count  43
  [ for each item ]
04 00            // item type 1   0-weapon/ring/earring/necklace  1-armor/shield  4-item/questitem/adena
00 00 00 00      // objectID
2B 07 00 00      // itemID     1835 Soulshot no grade
00 00 00 00      // current count
05 00            // item type2    0-weapon  1-shield/armor  2-ring/earring/necklace  3-questitem  4-adena  5-item
00 00            // ??
00 00 00 00      // bodypart (for armor)
00 00            // enchant level
00 00 00 00      // ??
07 00 00 00      // price (=7)
FE FF FF FF      // atk attribute type (-2) \\
00 00 00 00      // atk attribute value      \\
00 00 00 00      // fire def                  \\
00 00 00 00      // water def                 ||    8 element attributes
00 00 00 00      // wind def                  ||
00 00 00 00      // earth def                 //
00 00 00 00      // holy def                 //
00 00 00 00      // unholy def              //         ***/
void L2Client::ph_BuyList( L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	long long int curMoney = 0;

	L2_VERSION ver = pcls->account.getL2Version();
	if( ver < L2_VERSION_T23 ) curMoney = p->readInt();
	else curMoney = p->readInt64();

	unsigned int listID = p->readUInt();
	int cnt = (int)p->readUShort();
	if( cnt <= 0 ) return;

	TradeItem it;
	TradeItemsList *list = new TradeItemsList();
	list->curMoney = curMoney;
	list->itemCount = 0;
	list->listID = listID;
	int i = 0;
	for( i=0; i<cnt; i++ )
	{
		it.setUnused();
		it.type1 = (int)p->readUShort(); // type1
		it.objectID = p->readUInt();
		it.itemID = p->readUInt();

		if( ver < L2_VERSION_T23 ) it.count = p->readD();
		else it.count = p->readQ();

		it.type2 = (int)p->readUShort(); // type2
		p->readUShort(); // ??
		p->readUInt(); // bodypart
		it.enchantLevel = (int)p->readUShort();
		p->readUInt(); // ??

		if( ver < L2_VERSION_T23 ) it.price = p->readD();
		else it.price = p->readQ();

		// read 8 attributes
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
		list->addItem( it );
	}
	if( list->itemCount != cnt )
	{
		log_error( LOG_ERROR, "BuyList handler error!\n" );
		delete list;
		list = NULL;
		return;
	}
	pcls->handle_BuyList( list );
}
