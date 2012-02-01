#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**
writeC(0x29);
writeD(_charObjId);
writeD(_moveType);
writeD(_x);
writeD(_y);
writeD(_z); **/
void L2Client::ph_ChangeWaitType( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	unsigned int objectID = p->readUInt();
	unsigned int moveType = p->readUInt();
	/*int x = */p->readInt();
	/*int y = */p->readInt();
	/*int z = */p->readInt();
	//
	const unsigned int WT_SITTING = 0;
    const unsigned int WT_STANDING = 1;
    const unsigned int WT_START_FAKEDEATH = 2;
    const unsigned int WT_STOP_FAKEDEATH = 3;
	//
	if( pcls->usr.objectID == objectID )
	{
		switch( moveType )
		{
		case WT_SITTING: { pcls->usr.isSitting = 1; pcls->addChatToTab( CHAT_DMG, L"You sit" ); } break;
		case WT_STANDING: { pcls->usr.isSitting = 0; pcls->addChatToTab( CHAT_DMG, L"You stand" ); } break;
		case WT_START_FAKEDEATH: { pcls->usr.isFakeDeath = 1; pcls->addChatToTab( CHAT_DMG, L"You start FakeDeath" ); } break;
		case WT_STOP_FAKEDEATH: { pcls->usr.isFakeDeath = 0; pcls->addChatToTab( CHAT_DMG, L"You stop FakeDeath" ); } break;
		}
		pcls->postUpdateUI( UPDATE_USER );
		//log_error( LOG_OK, "User ChangeMoveType %u\n", moveType );
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
					switch( moveType )
					{
					case WT_SITTING: pcls->world_chars.chars_array[idx]->isSitting = 1; break;
					case WT_STANDING: pcls->world_chars.chars_array[idx]->isSitting = 0; break;
					}
				} break;
			/*case L2OT_NPC:
				{
					pcls->world_npcs.npcs_array[idx]->isRunning = run;
				} break;*/
			}
		}
		else log_error( LOG_ERROR, "WOT cannot find objectID while ChangeMoveType\n" );
	}
}
