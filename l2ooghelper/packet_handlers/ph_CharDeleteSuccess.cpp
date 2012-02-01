#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_CharDeleteSuccess( class L2Client *pcls, L2GamePacket *p )
{
	log_error( LOG_DEBUG, "CharDeleteSuccess\n" );
	p->getPacketType(); // just trigger
	// request character selection info
	pcls->send_RequestGotoLobby();
	pcls->addChatToTab( CHAT_SYS, L"Character marked to deletion." );
}
