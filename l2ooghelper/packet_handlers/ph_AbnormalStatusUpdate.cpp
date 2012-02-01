#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_AbnormalStatusUpdate( class L2Client *pcls, L2GamePacket *p )
{
	//pcls->log_packet( true, packbuffer, plen );
	pcls->buffs.parse_AbnormalStatusUpdate( p );
	pcls->postUpdateUI( UPDATE_BUFFS );
}
