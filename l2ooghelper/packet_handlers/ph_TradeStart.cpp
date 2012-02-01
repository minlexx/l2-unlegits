#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/*
// then both (requester and receiver) receive
Server: Len  189 [TradeStart]
BD 00
14               // pcode
C5 09 00 10      // partner objectID
03 00            // items count (your inv)
   ///   for each item
04 00         // item type1
CF 09 00 10   // item oid
D4 15 00 00   // item iid
01 00 00 00   // item count
05 00         // item type2
00 00         // 00
00 00 00 00   // body part
00 00         // enchant
00 00 00 00   // 0000
FE FF FF FF  00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00 // attribs
*/

void L2Client::ph_TradeStart( class L2Client *pcls, L2GamePacket *p )
{
	unsigned char opcode = p->getPacketType();
	if( opcode != 0x14 ) return;
	int i = 0;

	unsigned int partnerOID = p->readUInt();
	int          itemsCount = p->readH();

	// check trade partner name
	wchar_t partnerName[256] = {0};
	WorldObjectTreeNode wotNode;
	if( !pcls->world_tree.GetInfoByObjectID( partnerOID, &wotNode ) )
	{
		log_error( LOG_ERROR, "TradeStart: unknown name for player [%u]\n", partnerOID );
		return;
	}
	if( !pcls->world_chars.chars_array[wotNode.getArrayIdx()] )
	{
		log_error( LOG_ERROR, "TradeStart: chars_array[%d] is NULL!\n", wotNode.getArrayIdx() );
		return;
	}
	wcscpy( partnerName, pcls->world_chars.chars_array[wotNode.getArrayIdx()]->charName );

	// create trade list
	TradeItemsList *list = new TradeItemsList();
	list->listID = partnerOID; // save partner OID as listID

	// parse each item for our tradeable inventory
	for( i=0; i<itemsCount; i++ )
	{
		TradeItem it;
		//
		it.type1        = p->readH();
		it.objectID     = p->readUInt();
		it.itemID       = p->readUInt();

		if( pcls->getL2Version() < L2_VERSION_T23 )
			it.count    = p->readD(); // <= T2.2
		else it.count   = p->readQ(); // >= T2.3

		it.type2        = p->readH();
		/* 0x00 */        p->readH();
		it.bodyPart     = p->readD();
		it.enchantLevel = p->readH();
		/* 0x00 */        p->readD();

		// attributes
		if( pcls->getL2Version() < L2_VERSION_T23 )
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

		// add item to list
		list->addItem( it );
	}

	pcls->tradep2pdlg->tradeStart( list, partnerName );

	// we can safely delete list now - list is copied to internal by tradep2pdlg
	delete list;
	list = NULL;
}
