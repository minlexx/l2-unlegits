#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_SSQInfo( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	unsigned short sky = p->readUShort();
	switch( sky )
	{
	case L2Game_SSQInfo::SSQ_DAWN_SKY:
		pcls->addChatToTab( CHAT_SYS, L"SevenSigns: Dawn moon in the sky." ); break;
	case L2Game_SSQInfo::SSQ_DUSK_SKY:
		pcls->addChatToTab( CHAT_SYS, L"SevenSigns: Dusk moon in the sky." ); break;
	case L2Game_SSQInfo::SSQ_NORMAL_SKY:
		pcls->addChatToTab( CHAT_SYS, L"SevenSigns: Normal moon in the sky." ); break;
	}
}
