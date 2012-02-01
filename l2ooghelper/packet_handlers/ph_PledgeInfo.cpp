#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**
writeC(0x89);
writeD(_clan.getClanId());
writeS(_clan.getName());
writeS(_clan.getAllyName()); */
void L2Client::ph_PledgeInfo( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	unsigned int clanID = p->readUInt();
	const wchar_t *clanName = p->readUnicodeStringPtr();
	const wchar_t *allyName = p->readUnicodeStringPtr();
	pcls->world_clans.Add( clanID, clanName, allyName );
	log_error( LOG_USERAI, "Clan info: [%S] ally [%S]\n", clanName, allyName );
	pcls->postUpdateUI( UPDATE_MAP_PLAYERS );
}
