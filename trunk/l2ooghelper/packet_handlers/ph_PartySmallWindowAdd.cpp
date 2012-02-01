#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**
writeC(0x4f);
writeD(player.getObjectId()); // c3
writeD(0);//writeD(0x04); ?? //c3
writeD(_member.getObjectId());
writeS(_member.getName());
writeD((int) _member.getCurrentCp()); //c4
writeD(_member.getMaxCp()); //c4
writeD((int) _member.getCurrentHp());
writeD(_member.getMaxHp());
writeD((int) _member.getCurrentMp());
writeD(_member.getMaxMp());
writeD(_member.getLevel());
writeD(_member.getClassId().getId());
writeD(0);//writeD(0x01); ??
writeD(0); **/
void L2Client::ph_PartySmallWindowAdd( class L2Client *pcls, L2GamePacket *p )
{
	L2Player pl;
	p->getPacketType();
	//
	p->readUInt();
	p->readUInt();
	pl.objectID = p->readUInt();
	pl.setName( p->readUnicodeStringPtr() );
	pl.curCp = (double)p->readInt();
	pl.maxCp = (double)p->readInt();
	pl.curHp = (double)p->readInt();
	pl.maxHp = (double)p->readInt();
	pl.curMp = (double)p->readInt();
	pl.maxMp = (double)p->readInt();
	pl.level = p->readInt();
	pl.classID = p->readUInt();
	//
	pcls->party.addPlayer( &pl );
	pcls->postUpdateUI( UPDATE_PARTY );
}
