#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

// writeC(0x50);
void L2Client::ph_PartySmallWindowDeleteAll( class L2Client *pcls, L2GamePacket *p )
{
	UNREFERENCED_PARAMETER(p);
	// only trigger, no parse :)
	pcls->party.deleteAll();
	pcls->party.clear();
	pcls->postUpdateUI( UPDATE_PARTY );
}
