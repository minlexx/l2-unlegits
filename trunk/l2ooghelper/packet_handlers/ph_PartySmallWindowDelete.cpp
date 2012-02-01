#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/** writeC(0x51);
    writeD(_member.getObjectId());
    writeS(_member.getName());  **/
void L2Client::ph_PartySmallWindowDelete( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	unsigned int oid = p->readUInt();
	//const wchar_t *playerName = p->readUnicodeStringPtr();
	pcls->party.delPlayer( oid );
	//pcls->party.delPlayer( playerName );
	pcls->postUpdateUI( UPDATE_PARTY );
}
