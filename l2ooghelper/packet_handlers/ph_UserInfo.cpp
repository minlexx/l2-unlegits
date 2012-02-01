#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_UserInfo( class L2Client *pcls, L2GamePacket *p )
{
	pcls->usr.parse_UserInfo( p, pcls->account.getL2Version() );
	pcls->postUpdateUI( UPDATE_USER );
	// bot IPC
	// test if we registered in bot IPC, register or update info
	if( pcls->botipc_index == -1 )
	{
		BotIPC *ipc = BotIPC::getInstance();
		BOT_INFO binfo;
		BotInfo_Initialize( &binfo, pcls->usr.charName, pcls->usr.objectID,
			pcls->usr.classID, pcls->usr.level );
		pcls->botipc_index = ipc->addBotInfo( &binfo );
		if( pcls->botipc_index == -1 )
		{
			log_error( LOG_ERROR, "ph_UserInfo(): failed to register in Bot IPC!\n" );
		}
		else
		{
			log_error( LOG_DEBUG, "Registered in Bot IPC at index %d.\n", pcls->botipc_index );
		}
	}
	else // already registered in BotIPC, should update
	{
		BotIPC *ipc = BotIPC::getInstance();
		BOT_INFO binfo;
		BotInfo_Initialize( &binfo, pcls->usr.charName, pcls->usr.objectID,
			pcls->usr.classID, pcls->usr.level );
		if( !ipc->setBotInfo( pcls->botipc_index, &binfo ) )
			log_error( LOG_ERROR, "ph_UserInfo(): failed to update self in BotIPC!\n" );
	}
}
