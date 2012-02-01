#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_EtcStatusUpdate( class L2Client *pcls, L2GamePacket *p )
{
	pcls->etcStatus.parse_EtcStatusUpdate( p );
	pcls->postUpdateUI( UPDATE_BUFFS );
}
