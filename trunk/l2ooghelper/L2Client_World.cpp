#include "stdafx.h"
#include "Resource.h"
#include "logger.h"
#include "L2Client.h"

void L2Client::worldTick()
{
	static int countCalled = 0;
	unsigned int i = 0;
	unsigned int nProcessed = 0;
	unsigned int curTick = GetTickCount();
	// user
	usr.processMoveTick();
	// send ValidatePosition?
	countCalled++;
	if( usr.isMoving() )
	{
		if( countCalled % 8 == 0 ) // every 8th worldTick() function call (once in 2 sec)
		{
			//log_error( LOG_OK, "Validating position: %d, %d, %d, %d\n", usr.x, usr.y, usr.z, usr.heading );
			send_ValidatePosition();
		}
	}
	// current casting progress is calculated and updated here
	skills.processCasting( curTick );
	postUpdateUI( UPDATE_USER_CASTING );
	// npc
	if( world_npcs.getCount() > 0 )
	{
		nProcessed = 0;
		for( i=0; i<world_npcs.getCount(); i++ )
		{
			if( world_npcs.npcs_array[i]->isUnused() ) continue;
			world_npcs.npcs_array[i]->processMoveTick();
			nProcessed++;
			if( nProcessed >= world_npcs.getCount() ) break;
		}
	}
	// pc
	if( world_chars.GetCount() > 0 )
	{
		nProcessed = 0;
		for( i=0; i<world_chars.GetCount(); i++ )
		{
			if( world_chars.chars_array[i]->isUnused() ) continue;
			world_chars.chars_array[i]->processMoveTick();
			nProcessed++;
			if( nProcessed >= world_chars.GetCount() ) break;
		}
	}
	// update skills reuse timers
	skills.processCoolTimes( curTick );
	OnUiUpdateSkillCoolTimes();
	// update user buffs times
	buffs.process_BuffTick( curTick );
	OnUiUpdateBuffsDuration();
	// update party buffs times
	party.process_BuffTick( curTick );
	//OnUiUpdatePartyBuffsDuration();
	// no really need to update party buffs duration - UI does not display duration, only skill names
	// post user interface updates
	postUpdateMap();
}
