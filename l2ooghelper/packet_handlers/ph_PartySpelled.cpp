#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**
writeC(0xf4);
writeD(_activeChar instanceof L2SummonInstance ? 2 : _activeChar instanceof L2PetInstance ? 1 : 0);
writeD(_activeChar.getObjectId());
writeD(_effects.size());
for (Effect temp : _effects)
{
    writeD(temp._skillId);
    writeH(temp._dat);
    writeD(temp._duration / 1000);
} **/
void L2Client::ph_PartySpelled( class L2Client *pcls, L2GamePacket *p )
{
	pcls->party.parse_PartySpelled( p );
	pcls->postUpdateUI( UPDATE_PARTY_BUFFSDURATION );
}
