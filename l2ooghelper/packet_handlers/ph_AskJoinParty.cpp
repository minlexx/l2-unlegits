#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**
writeC(0x39);
writeS(_requestorName);
writeD(_itemDistribution); */
// 0 - Finders Keepers
// 1 - Random
// 2 - Random including spoil
// 3 - by turn
// 4 - by turn including spoil
void L2Client::ph_AskJoinParty( class L2Client *pcls, L2GamePacket *p )
{
	WCHAR requesterName[256] = {0};
	unsigned int itemDistribution;
	p->getPacketType();
	wcscpy( requesterName, p->readUnicodeStringPtr() );
	itemDistribution = p->readUInt();
	pcls->handle_AskJoinParty( requesterName, itemDistribution );
}
