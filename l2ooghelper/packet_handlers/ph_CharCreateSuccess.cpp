#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_CharCreateSuccess( class L2Client *pcls, L2GamePacket *p )
{
	L2Game_CharCreateSuccess *pc = new L2Game_CharCreateSuccess(
		p->getBytesPtr(), p->getPacketSize() );
	if( pc->parse( pcls->account.getL2Version() ) )
	{
		log_error( LOG_OK, "Char create OK\n" );
		pcls->addChatToTab( CHAT_SYS, L"Char create OK" );
	}
	delete pc;
}
