#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/** writeC(0x3a);
    writeD(_response); **/
void L2Client::ph_JoinParty( class L2Client *pcls, L2GamePacket *p )
{
	UNREFERENCED_PARAMETER(pcls);
	unsigned int response = 0;
	p->getPacketType();
	response = p->readUInt();
	//log_error( LOG_OK, "ph_JoinParty(): response 0x%08X\n", response );
}
