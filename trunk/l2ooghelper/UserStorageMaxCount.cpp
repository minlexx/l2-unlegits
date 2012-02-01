#include "stdafx.h"
#include "UserStorageMaxCount.h"

void UserStorageMaxCount::parse_ExStorageMaxCount( void *l2_game_packet )
{
	if( !l2_game_packet ) return;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	p->getPacketType(); // writeC(0xfe);
	p->readUShort();    // writeH(0x2f);
	this->inventory   = p->readUInt(); // writeD(_inventory);
	this->warehouse   = p->readUInt(); // writeD(_warehouse);
	this->freight     = p->readUInt(); // writeD(_freight);
	this->privateSell = p->readUInt(); // writeD(_privateSell);
	this->privateBuy  = p->readUInt(); // writeD(_privateBuy);
	this->receipeD    = p->readUInt(); // writeD(_receipeD);
	this->recipe      = p->readUInt(); // writeD(_recipe);
}
