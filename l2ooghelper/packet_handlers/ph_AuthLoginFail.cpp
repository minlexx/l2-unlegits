#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_AuthLoginFail( class L2Client *pcls, L2GamePacket *p )
{
	L2Game_AuthLoginFail *paf = new L2Game_AuthLoginFail(
		(unsigned char *)p->getBytesPtr(), p->getPacketSize() );
	if( paf->parse() )
	{
		char reason[256] = {0};
		L2Game_AuthLoginFail::reasonCodeToString( paf->p_reasonCode, reason );
		log_error( LOG_ERROR, "AuthLognFail: %s\n", reason );
		pcls->addChatToTabFormat( CHAT_SYS, L"Game AuthLoginFail: %S", reason );
	}
	delete paf;
	paf = NULL;
	pcls->disconnectClient(true);
}
