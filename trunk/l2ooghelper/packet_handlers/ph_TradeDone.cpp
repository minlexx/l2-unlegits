#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**  Server: Len    7 [TradeDone]
07 00
1C              // pcode
01 00 00 00     // confirmed ? 0x01 : 0x00   **/
void L2Client::ph_TradeDone( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	int num = p->readInt();
	log_error( LOG_DEBUG, "TradeDone: %d\n", num );
	if( num )
	{
		pcls->addChatToTabFormat( CHAT_SYS, L"Player %s confirmed trade.", pcls->tradep2pdlg->getPartnerName() );
	}
	else
	{
		pcls->addChatToTabFormat( CHAT_SYS, L"Player %s cancelled trade.", pcls->tradep2pdlg->getPartnerName() );
	}
	pcls->tradep2pdlg->tradeDone();
}
