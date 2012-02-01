#include "stdafx.h"
#include "UserBuffs.h"
#include "logger.h"

wchar_t *UserBuff::getSkillNameW()
{
	if( this->cached_skillName[0] == 0 )
	{
		char skillname[256];
		memset( skillname, 0, sizeof(skillname) );
		L2Data_DB_GetSkillNameByID( skillID, skillname );
		if( skillname[0] == 0 ) strcpy( skillname, "<unknown>" );
		MultiByteToWideChar( CP_ACP, 0, skillname, -1, cached_skillName, 255 );
	}
	return this->cached_skillName;
}

void UserBuff::getSkillName( wchar_t *out, size_t maxCount )
{
	if( this->cached_skillName[0] == 0 )
	{
		char skillname[256];
		memset( skillname, 0, sizeof(skillname) );
		L2Data_DB_GetSkillNameByID( skillID, skillname );
		if( skillname[0] == 0 ) strcpy( skillname, "<unknown>" );
		MultiByteToWideChar( CP_ACP, 0, skillname, -1, cached_skillName, 255 );
	}
	if( out ) wcsncpy( out, cached_skillName, maxCount );
}



UserBuffs::UserBuffs() { clear(); clearShortBuffs(); }
UserBuffs::~UserBuffs() { clear(); clearShortBuffs(); }

void UserBuffs::clear()
{
	buffCount = 0;
	int i;
	for( i=0; i<USER_MAX_BUFFS; i++ ) buff[i].clear();
	lastBuffKnownTick = GetTickCount();
}

void UserBuffs::clearShortBuffs()
{
	shortBuffCount = 0;
	int i;
	for( i=0; i<USER_MAX_SHORT_BUFFS; i++ ) short_buff[i].clear();
	lastShortBuffKnownTick = GetTickCount();
}


int UserBuffs::addBuff( UserBuff *addbuff )
{
	int add_idx = -1;
	int i;
	for( i=0; i<USER_MAX_BUFFS; i++ )
	{
		if( buff[i].skillID == 0 )
		{
			add_idx = i;
			break;
		}
	}
	if( add_idx == -1 ) return add_idx;
	buffCount++;
	buff[add_idx] = (*addbuff);
	//log_error( LOG_USERAI, "Added buff %u [%S] lvl %d (%d sec)\n", addbuff->skillID,
	//	addbuff->getSkillNameW(), addbuff->skillLvl, addbuff->duration );
	return add_idx;
}

int UserBuffs::addShortBuff( UserBuff *addbuff )
{
	int add_idx = -1;
	int i;
	// find existing buff by skillID
	for( i=0; i<USER_MAX_SHORT_BUFFS; i++ )
	{
		if( short_buff[i].skillID == addbuff->skillID )
		{
			add_idx = i;
			break;
		}
	}
	// find free place to add in new slot
	if( add_idx == -1 )
	{
		for( i=0; i<USER_MAX_SHORT_BUFFS; i++ )
		{
			if( short_buff[i].skillID == 0 )
			{
				add_idx = i;
				break;
			}
		}
	}
	if( add_idx == -1 ) return add_idx;
	shortBuffCount++;
	short_buff[add_idx] = (*addbuff);
	//log_error( LOG_USERAI, "Added short buff %u [%S] lvl %d (%d sec)\n", addbuff->skillID,
	//	addbuff->getSkillNameW(), addbuff->skillLvl, addbuff->duration );
	return add_idx;
}

int UserBuffs::getBuffnfoBySkillId( unsigned int skillID, UserBuff *outbuff )
{
	int i;
	int idx = -1;
	for( i=0; i<USER_MAX_BUFFS; i++ )
	{
		if( buff[i].skillID == skillID )
		{
			idx = i;
			break;
		}
	}
	if( idx == -1 ) // try to find short buff
	{
		for( i=0; i<USER_MAX_SHORT_BUFFS; i++ )
		{
			if( short_buff[i].skillID == skillID )
			{
				idx = i;
				break;
			}
		}
		if( idx == -1 ) return -1;
		(*outbuff) = short_buff[idx];
		return idx;
	}
	(*outbuff) = buff[idx];
	return idx;
}

/**
Server: 85 AbnormalStatusUpdate: len 25
19 00
85
02 00             // nEffects = 2
3E 14 00 00       // skillID 5182  \
01 00             // level 1        > 1
BF 1E 00 00       // duration 7871 /
*/

void UserBuffs::parse_AbnormalStatusUpdate( void *l2_game_packet )
{
	if( !l2_game_packet ) return;
	this->clear();
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	p->getPacketType();
	int nEffects = p->readUShort();
	int i;
	for( i=0; i<nEffects; i++ )
	{
		UserBuff b;
		b.skillID = p->readUInt();
		b.skillLvl = p->readUShort();
		b.duration = p->readInt();
		addBuff( &b );
	}
	lastBuffKnownTick = GetTickCount();
}

/** writeC(0xfa);
writeD(_skillId);
writeD(_skillLvl);
writeD(_duration); **/
void UserBuffs::parse_ShortBuffStatusUpdate( void *l2_game_packet )
{
	if( !l2_game_packet ) return;
	//this->clearShortBuffs();
	L2GamePacket *pack = (L2GamePacket *)l2_game_packet;
	pack->getPacketType();
	unsigned int skillID = pack->readUInt();
	int skillLvl = pack->readInt();
	int duration = pack->readInt(); // in seconds!
	if( skillID > 0 )
	{
		UserBuff buf;
		buf.skillID = skillID;
		buf.skillLvl = skillLvl;
		buf.duration = duration; // in seconds
		addShortBuff( &buf ); // updates existing
		//
		//char skillName[256] = {0};
		//L2Data_DB_GetSkillNameByID( skillID, skillName );
		//log_error( LOG_USERAI, "ShortBuffStatusUpdate: %u [%s] lvl %d, duration %d\n",
		//	skillID, skillName, skillLvl, duration );
	}
	lastShortBuffKnownTick = GetTickCount();
}

void UserBuffs::process_BuffTick( unsigned int curTick )
{
	unsigned int ticksPassed = curTick - lastBuffKnownTick;
	unsigned int ticksPassedS = curTick - lastShortBuffKnownTick;
	if( (ticksPassed < 1000) || (ticksPassedS < 1000) ) return; // only update buff times once a second
	lastBuffKnownTick = curTick;
	lastShortBuffKnownTick = curTick;
	int i;
	for( i=0; i<buffCount; i++ )
	{
		if( buff[i].skillID != 0 )
		{
			buff[i].duration--; // minus 1 second for each buff :)
			// buff stopped?
			if( buff[i].duration <= 0 ) { buff[i].clear(); buffCount--; }
		}
	}
	for( i=0; i<shortBuffCount; i++ )
	{
		if( short_buff[i].skillID != 0 )
		{
			short_buff[i].duration--; // minus 1 second for each buff :)
			// buff stopped?
			if( short_buff[i].duration <= 0 ) { short_buff[i].clear(); shortBuffCount--; }
		}
	}
}

/*
int buffCount;
UserBuff buff[USER_MAX_BUFFS];
*/

