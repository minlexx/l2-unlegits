#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**
writeC(0x2f);
writeD(_charObjId);
writeD(_xDst);
writeD(_yDst);
writeD(_zDst);
writeD(_x);
writeD(_y);
writeD(_z); **/
void L2Client::ph_MoveToLocation( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	unsigned int objectID = p->readUInt();
	int xDst = p->readInt();
	int yDst = p->readInt();
	int zDst = p->readInt();
	int x = p->readInt();
	int y = p->readInt();
	int z = p->readInt();
	//
	if( objectID == pcls->usr.objectID )
	{
		pcls->usr.startMoveTo( xDst, yDst, zDst, x, y, z );
	}
	else
	{
		WorldObjectTreeNode wotNode;
		if( pcls->world_tree.GetInfoByObjectID( objectID, &wotNode ) )
		{
			int idx = wotNode.getArrayIdx();
			switch( wotNode.getObjectType() )
			{
			case L2OT_PC: pcls->world_chars.chars_array[idx]->startMoveTo( xDst, yDst, zDst, x, y, z ); break;
			case L2OT_NPC:
				{
					//log_error( LOG_OK, "MoveToLocation NPC %S\n", pcls->world_npcs.npcs_array[idx]->charName );
					pcls->world_npcs.npcs_array[idx]->startMoveTo( xDst, yDst, zDst, x, y, z );
				} break;
			}
		}
		else
		{
			log_error( LOG_ERROR, "WOT cannot find objectID while MoveToLocation\n" );
			pcls->addChatToTab( CHAT_DMG, L"WARN: possible invisible GM nearby!" );
		}
	}
}
