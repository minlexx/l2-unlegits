#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/** writeC(0x49);
writeD(_objectId); **/
void L2Client::ph_MagicSkillCanceld( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	unsigned int objectID = p->readUInt();
	if( objectID == pcls->usr.objectID )
	{
		pcls->skills.stopCasting();
		pcls->postUpdateUI( UPDATE_USER_CASTING );
		pcls->addChatToTab( CHAT_DMG, L"Casting stopped" );
	}
}
