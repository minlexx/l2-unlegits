#include "stdafx.h"
#include "../Logger.h"
#include "../L2Client.h"

/**  Server: Len   15 [SetupGauge]
0F 00
6B
00 00 00 00    // color  0-blue   1-red  2-cyan  3-
57 07 00 00    // skill cast time 1879, ms
57 07 00 00   **/
void L2Client::ph_SetupGauge( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	unsigned int color = p->readInt(); // color
	unsigned int time = p->readUInt();
	//log_error( LOG_OK, "startCasting( %u )\n", time );
	switch( color )
	{
	case 0: pcls->skills.startCasting( time ); break; // skill cast
	// case 1: // arrow shoot?
	case 2: // under water breathe
		{
			if( time == 0 )
				log_error( LOG_USERAI, "Not sinking!" );
			else
				log_error( LOG_USERAI, "Sinking under water!" );
		} break;
	}
}
