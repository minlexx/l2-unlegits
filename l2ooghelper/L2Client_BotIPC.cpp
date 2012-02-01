#include "stdafx.h"
#include "Resource.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::L2ClientThread_Game_BotIPC( class L2Client *pcls )
{
	BOT_INFO bi;
	BotIPC *ipc = BotIPC::getInstance();
	ipc->getBotInfo( pcls->botipc_index, &bi );
	if( bi.isSlave )
	{
		if( bi.followEnable )
		{
			pcls->m_ai->setFollow( true, bi.oidFollow );
		}
		else
		{
			pcls->m_ai->setFollow( false, 0 );
		}
	}
}
