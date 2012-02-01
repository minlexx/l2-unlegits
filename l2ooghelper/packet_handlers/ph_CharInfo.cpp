#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_CharInfo( class L2Client *pcls, L2GamePacket *p )
{
	L2Player *cha = new L2Player();
	
	cha->parse_CharInfo( p, pcls->account.getL2Version() );

	// we should request PledgeInfo for every clan that we do not know
	if( cha->clanID != 0 )
	{
		TCHAR tszClanName[256] = {0};
		if( !pcls->world_clans.GetClanNameByID( cha->clanID, tszClanName ) )
		{
			L2GamePacket *pack = new L2GamePacket();
			pack->writeReset();
			pack->setPacketType( 0x65 ); // RequestPledgeInfo
			pack->writeUInt( cha->clanID );
			pcls->sendPacket( pack, true );
			delete pack; pack = NULL;
			log_error( LOG_USERAI, "CharInfo: Clan %u unknown, sent RequestPledgeInfo\n",
				cha->clanID );
		}
	}

	// set last time when chars coordinates were known exactly
	// cha->lastMoveTickTime = GetTickCount(); // done by parse()

	// first try to find
	WorldObjectTreeNode wotNode;
	if( pcls->world_tree.GetInfoByObjectID( cha->objectID, &wotNode ) )
	{
		// already exists, update info
		pcls->world_chars.UpdateCharInfo( wotNode.getArrayIdx(), cha );
	}
	else
	{
		// add new char
		int idx = pcls->world_chars.AddCharInfo( cha );
		if( idx >= 0 )
		{
			wotNode.setPlayer( cha );
			wotNode.setArrayIdx( idx );
			pcls->world_tree.AddObject( cha->objectID, &wotNode );
		}
		else
			log_error( LOG_ERROR, "Character [%S] failed to add to world and to chars list, possible chars limit exceeded\n",
				cha->charName );
	}
	delete cha;
	pcls->postUpdateUI( UPDATE_MAP_PLAYERS );
}
