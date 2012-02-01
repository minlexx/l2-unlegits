#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/** Server: Len   15 [ChangeMoveType]
0F 00
28
B2 16 00 10  // object id
00 00 00 00  // 0 - walk, 1 - run
00 00 00 00  // ??    **/
void L2Client::ph_ChangeMoveType( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	unsigned int objectID = p->readUInt();
	int run = p->readInt();
	//log_error( LOG_OK, "ChangeMoveType: objectID %u to %d\n", objectID, run );
	if( pcls->usr.objectID == objectID )
	{
		pcls->usr.isRunning = run;
		pcls->postUpdateUI( UPDATE_USER );
		//log_error( LOG_OK, "User ChangeMoveType\n" );
		if( run ) pcls->addChatToTab( CHAT_DMG, L"You run" );
		else pcls->addChatToTab( CHAT_DMG, L"You walk" );
	}
	else
	{
		WorldObjectTreeNode wotNode;
		if( pcls->world_tree.GetInfoByObjectID( objectID, &wotNode ) )
		{
			int idx = wotNode.getArrayIdx();
			switch( wotNode.getObjectType() )
			{
			case L2OT_PC:
				{
					pcls->world_chars.chars_array[idx]->isRunning = run;
				} break;
			case L2OT_NPC:
				{
					pcls->world_npcs.npcs_array[idx]->isRunning = run;
				} break;
			}
		}
		else log_error( LOG_ERROR, "WOT cannot find objectID while ChangeMoveType\n" );
	}
}
