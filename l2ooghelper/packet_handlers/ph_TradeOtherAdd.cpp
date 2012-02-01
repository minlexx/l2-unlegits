#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**   Server: Len   65 [TradeOtherAdd]
41 00
1B            // pcode
01 00         // item cnt (always 1 on l2j)
04 00         // type1
C0 09 00 10   // oid
39 00 00 00   // iid
0D 00 00 00   // cnt
04 00         // type2
00 00         // 00
00 00 00 00   // b.part
00 00         // enchatnt
00 00 00 00   // 00 ??
FE FF FF FF  00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00 // attribs    **/
void L2Client::ph_TradeOtherAdd( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	int it_cnt      = p->readH();
	if( it_cnt != 1 )
	{
		log_error( LOG_ERROR, "TradeOtherAdd: items count != 1, some protocol error?\n" );
		pcls->addChatToTab( CHAT_SYS, L"TradeOtherAdd: items count != 1, some protocol error?" );
		return;
	}
	//
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

	// process
	pcls->tradep2pdlg->tradeOtherAdd( it );
}
