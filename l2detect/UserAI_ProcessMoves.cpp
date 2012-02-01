#include "stdafx.h"
#include "UserAI.h"
#include "Logger.h"

void UserAI::UAI_ProcessWorld( class UserAI *cls )
{
	unsigned int curTick = (unsigned int)GetTickCount();
	// moves
	UAI_ProcessMoves( cls );
	// buffs
	cls->buffs.process_BuffTick( curTick );
	// party buffs
	int i;
	for( i=0; i<UserParty::MAX_PARTY; i++ )
	{
		UserBuffs *buffs = cls->party.getPartyPlayerBuffs( i );
		if( buffs )
			buffs->process_BuffTick( curTick );
	}
	// skill reuses & casting
	cls->skills.processCoolTimes( curTick );
	cls->skills.processCasting( curTick );
}

void UserAI::UAI_ProcessMoves( class UserAI *cls )
{
	int i;
	// user move
	cls->usr.processMoveTick();

	// NPC moves
	NpcArray_Lock();
	for( i=0; i<NPCA_MAX_NPCS; i++ )
	{
		if( npc_array[i]->isUnused() ) continue;
		if( npc_array[i]->isMoving() )
		{
			npc_array[i]->processMoveTick();
		}
	}
	NpcArray_Unlock();

	// PC moves
	CharArray_Lock();
	for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
	{
		if( chars_array[i]->isUnused() ) continue;
		if( chars_array[i]->isMoving() )
		{
			chars_array[i]->processMoveTick();
		}
	}
	CharArray_Unlock();
}
