#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"
#include "utils.h"

void L2Client::ph_SystemMessage( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	//unsigned int sm_ID = p->readUInt();
	//unsigned int paramCount = p->readUInt();
	wchar_t *sm = (wchar_t *)malloc( 20480 );
	if( !sm ) return;
	memset( sm, 0, 20480 );
	Utils_process_SystemMessage( p, sm );
	pcls->addChatToTab( CHAT_SYS, sm );
	free( sm ); sm = NULL;
}
