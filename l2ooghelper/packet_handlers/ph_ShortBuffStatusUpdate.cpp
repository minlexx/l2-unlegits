#include "stdafx.h"
#include "../Logger.h"
#include "../L2Client.h"

void L2Client::ph_ShortBuffStatusUpdate( class L2Client *pcls, L2GamePacket *p )
{
	//pcls->log_packet( true, packbuffer, plen );
	pcls->buffs.parse_ShortBuffStatusUpdate( p );
	pcls->postUpdateUI( UPDATE_BUFFS );
}
