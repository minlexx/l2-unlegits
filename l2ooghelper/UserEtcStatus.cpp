#include "stdafx.h"
#include "UserEtcStatus.h"

/**
Server: F9 EtcStatusUpdate: len 35
23 00
F9
00 00 00 00    // 1-7 increase force, lvl
00 00 00 00    // 1-4 weight penalty, lvl (1=50%, 2=66.6%, 3=80%, 4=100%)
00 00 00 00    // 1 = block all chat 
00 00 00 00    // 1 = danger area
00 00 00 00    // 1 = grade penalty 
00 00 00 00    // 1 = charm of courage (allows resurrection on the same spot upon death on the siege battlefield)
00 00 00 00    // 1-15 death penalty, lvl (combat ability decreased due to death)
00 00 00 00    // _activeChar.getSouls()
*/

void UserEtcStatus::parse_EtcStatusUpdate( void *l2_game_packet )
{
	if( !l2_game_packet ) return;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	p->getPacketType();
	this->forceLevel         = p->readUInt();
	this->weightPenaltyLevel = p->readUInt();
	this->blockAllChat       = p->readUInt();
	this->dangerArea         = p->readUInt();
	this->gradePenalty       = p->readUInt();
	this->charmOfCourage     = p->readUInt();
	this->deathPenaltyLevel  = p->readUInt();
	this->souls              = p->readUInt();
}

void UserEtcStatus::clear()
{
	this->forceLevel         = 0;
	this->weightPenaltyLevel = 0;
	this->blockAllChat       = 0;
	this->dangerArea         = 0;
	this->gradePenalty       = 0;
	this->charmOfCourage     = 0;
	this->deathPenaltyLevel  = 0;
	this->souls              = 0;
}
