#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_CharCreateFail( class L2Client *pcls, L2GamePacket *p )
{
	L2Game_CharCreateFail *pf = new L2Game_CharCreateFail(
		p->getBytesPtr(), p->getPacketSize() );
	if( pf->parse( pcls->account.getL2Version() ) )
	{
		char reason[256] = {0};
		pf->reasonCodeToString( pf->p_reasonCode, reason );
		log_error( LOG_ERROR, "CharCreateFail: %s\n", reason );
		pcls->addChatToTabFormat( CHAT_SYS, L"Char create error: %S", reason );
	}
	delete pf;
	pf = NULL;
}
