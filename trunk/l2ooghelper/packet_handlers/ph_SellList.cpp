#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/***    Server: Len 1357 [SellList]
4D 05
06
6F A5 34 00  // cur money
00 00 00 00  // list id?
15 00        // items count
 [ for each item ]
04 00        // type1
C4 40 07 10  // objectID
E0 02 00 00  // itemID
0B 00 00 00  // count
05 00        // type2
00 00        // ??
00 00 00 00  // body part
00 00        // enchant level
00 00 00 00  // ??
C8 00 00 00  // sell price
FE FF FF FF // atk attrib type
00 00 00 00 // atk attrib val
00 00 00 00 // def attr fire
00 00 00 00 // -- water
00 00 00 00 // -- wind
00 00 00 00 // -- earth
00 00 00 00 // -- holy
00 00 00 00 // -- unholy     **/
void L2Client::ph_SellList( L2Client *pcls, L2GamePacket *p )
{
	L2_VERSION ver = pcls->account.getL2Version();

	p->getPacketType();
	long long int curMoney = 0;

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
		log_error( LOG_ERROR, "SellList handler error!\n" );
		delete list;
		list = NULL;
		return;
	}
	pcls->handle_SellList( list );
}
