#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_CharDeleteFail( class L2Client *pcls, L2GamePacket *p )
{
	L2Game_CharDeleteFail *cdf = new L2Game_CharDeleteFail(
		p->getBytesPtr(), p->getPacketSize() );
	if( cdf->parse() )
	{
		char reason[256] = {0};
		L2Game_CharDeleteFail::reasonCodeToString( cdf->p_reasonCode, reason );
		log_error( LOG_ERROR, "CharDeleteFail: %s\n", reason );
		pcls->addChatToTabFormat( CHAT_SYS, L"CharDeleteFail: %S\n", reason );
	}
	delete cdf;
	cdf = NULL;

	// request charSelection info
	pcls->send_RequestGotoLobby();
}
