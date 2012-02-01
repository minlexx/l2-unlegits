#include "stdafx.h"
#include "logger.h"
#include "UserParty.h"


void UserParty::clear()
{
	Lock();
	count = 0;
	leaderObjectId = 0;
	int i;
	for( i=0; i<MAX_PARTY; i++ )
	{
		party_player[i] = NULL;
		party_player_buffs[i] = NULL;
	}
	Unlock();
}

void UserParty::deleteAll()
{
	Lock();
	int i;
	for( i=0; i<MAX_PARTY; i++ )
	{
		if( party_player[i] ) delete party_player[i];
		if( party_player_buffs[i] ) delete party_player_buffs[i];
		party_player[i] = NULL;
		party_player_buffs[i] = NULL;
	}
	Unlock();
}

int UserParty::addPlayer( L2Player *pl )
{
	Lock();
	int idx = -1;
	int i;
	for( i=0; i<MAX_PARTY; i++ )
	{
		if( party_player[i] == NULL )
		{
			idx = i;
			break;
		}
	}
	if( idx == -1 )
	{
		Unlock();
		log_error( LOG_ERROR, "UserParty::addPlayer(): no free space!\n" );
		return -1;
	}
	// allocate new player
	party_player[idx] = new L2Player();
	if( !party_player[idx] )
	{
		Unlock();
		log_error( LOG_ERROR, "UserParty::addPlayer(): operator new() failed!\n" );
		return -1;
	}
	// from L2 Server for each party member we know only:
	// oid, name, level, class, race, hp/mp/cp
	party_player[idx]->objectID = pl->objectID;
	wcscpy( party_player[idx]->charName, pl->charName );
	party_player[idx]->level = pl->level;
	party_player[idx]->classID = pl->classID;
	party_player[idx]->race = pl->race;
	// hp, mp, cp
	party_player[idx]->curCp = pl->curCp;
	party_player[idx]->maxCp = pl->maxCp;
	party_player[idx]->curHp = pl->curHp;
	party_player[idx]->maxHp = pl->maxHp;
	party_player[idx]->curMp = pl->curMp;
	party_player[idx]->maxMp = pl->maxMp;
	// allocate also party player buffs object
	party_player_buffs[idx] = new UserBuffs();
	party_player_buffs[idx]->clear();
	// count
	count++;
	//
	Unlock();
	return idx;
}

bool UserParty::delPlayer( unsigned int objectID )
{
	int i;
	Lock();
	for( i=0; i<MAX_PARTY; i++ )
	{
		if( party_player[i] )
		{
			if( party_player[i]->objectID == objectID )
			{
				// delete player object
				delete party_player[i];
				party_player[i] = NULL;
				// delete player buffs
				if( party_player_buffs[i] ) delete party_player_buffs[i];
				party_player_buffs[i] = NULL;
				// dec count
				count--;
				shiftAfterDel( i );
				Unlock();
				return true;
			}
		}
	}
	Unlock();
	log_error( LOG_ERROR, "UserParty::delPlayer( %u ): failed!\n", objectID );
	return false;
}

bool UserParty::delPlayer( const wchar_t *playerName )
{
	if( !playerName ) return false;
	int i;
	Lock();
	for( i=0; i<MAX_PARTY; i++ )
	{
		if( party_player[i] )
		{
			if( wcscmp( party_player[i]->charName, playerName ) == 0 )
			{
				// delete player object
				delete party_player[i];
				party_player[i] = NULL;
				// delete player buffs
				if( party_player_buffs[i] ) delete party_player_buffs[i];
				party_player_buffs[i] = NULL;
				// dec count
				count--;
				shiftAfterDel( i );
				Unlock();
				return true;
			}
		}
	}
	Unlock();
	log_error( LOG_ERROR, "UserParty::delPlayer( \"%S\" ): failed!\n", playerName );
	return false;
}

bool UserParty::isInParty( unsigned int objectID, int *index )
{
	int i;
	if( index ) (*index) = -1;
	Lock();
	for( i=0; i<MAX_PARTY; i++ )
	{
		if( party_player[i] )
		{
			if( party_player[i]->objectID == objectID )
			{
				if( index ) (*index) = i;
				Unlock();
				return true;
			}
		}
	}
	Unlock();
	return false;
}

bool UserParty::getPartyPlayerStatsByObjectID(
                          unsigned int objectID,
                          int *pcurHp, int *pmaxHp,
                          int *pcurMp, int *pmaxMp,
                          int *pcurCp, int *pmaxCp,
                          unsigned int *pclassID,
						  unsigned int *targetObjectID
	                  )
{
	int idx = -1;
	Lock();
	if( !isInParty( objectID, &idx ) ) { Unlock(); return false; }
	if( idx == - 1 ) { Unlock(); return false; }
	//
	if( pcurHp ) (*pcurHp) = (int)party_player[idx]->curHp;
	if( pmaxHp ) (*pmaxHp) = (int)party_player[idx]->maxHp;
	if( pcurMp ) (*pcurMp) = (int)party_player[idx]->curMp;
	if( pmaxMp ) (*pmaxMp) = (int)party_player[idx]->maxMp;
	if( pcurCp ) (*pcurCp) = (int)party_player[idx]->curCp;
	if( pmaxCp ) (*pmaxCp) = (int)party_player[idx]->maxCp;
	if( pclassID ) (*pclassID) = party_player[idx]->classID;
	if( targetObjectID) (*targetObjectID) = party_player[idx]->targetObjectID;
	//
	Unlock();
	return true;
}

bool UserParty::setPlayerTarget( unsigned int objectID, unsigned int targetObjectID )
{
	int i;
	Lock();
	for( i=0; i<count; i++ )
	{
		if( !party_player[i] ) continue;
		if( party_player[i]->objectID == objectID )
		{
			party_player[i]->targetObjectID = targetObjectID;
			Unlock();
			return true;
		}
	}
	Unlock();
	return false;
}

bool UserParty::updatePlayer( L2Player *pl )
{
	if( !pl ) return false;
	int i;
	Lock();
	for( i=0; i<count; i++ )
	{
		if( !party_player[i] ) continue;
		if( party_player[i]->objectID == pl->objectID )
		{
			// during update we must update:
			// name, cp, hp, mp, level, classID
			wcscpy( party_player[i]->charName, pl->charName );
			party_player[i]->level = pl->level;
			party_player[i]->classID = pl->classID;
			party_player[i]->curCp = pl->curCp;
			party_player[i]->maxCp = pl->maxCp;
			party_player[i]->curHp = pl->curHp;
			party_player[i]->maxHp = pl->maxHp;
			party_player[i]->curMp = pl->curMp;
			party_player[i]->maxMp = pl->maxMp;
			Unlock();
			return true;
		}
	}
	Unlock();
	log_error( LOG_ERROR, "UserParty::updatePlayer(): ERROR: user %u (%S) not in party?\n", pl->objectID, pl->charName );
	return false;
}

L2Player *UserParty::getPartyPlayer( int idx )
{
	if( idx >= 0  &&  idx < MAX_PARTY ) return party_player[idx];
	return NULL;
}

UserBuffs *UserParty::getPartyPlayerBuffs( int idx )
{
	if( idx >= 0  &&  idx < MAX_PARTY ) return party_player_buffs[idx];
	return NULL;
}

void UserParty::shiftAfterDel( int deleted_pos )
{
	if( deleted_pos == (MAX_PARTY-1) ) return;
	int i;
	for( i = deleted_pos; i < MAX_PARTY - 1; i++ )
	{
		party_player[i] = party_player[i+1]; // 'move' player object pointer
		party_player_buffs[i] = party_player_buffs[i+1]; // 'move' player buffs object pointer
	}
	party_player[i] = NULL;
	party_player_buffs[i] = NULL;
}

void UserParty::parse_PartySpelled( L2GamePacket *p )
{
	if( !p ) return;
	p->getPacketType();
	//
	/*int activeCharType = */p->readInt();
	unsigned int objectID = p->readUInt();
	unsigned int i = 0;
	unsigned int effectCount = p->readUInt();
	int idx = 0;
	if( !isInParty( objectID, &idx ) )
	{
		log_error( LOG_DEBUG, "UserParty: PartySpelled: objectID is not in party!\n" );
		return;
	}
	// clear all buffs before parsing, because PartySpelled does not send updates,
	// ist sends all info about current buffs, their levels and times
	party_player_buffs[idx]->clear();
	for( i=0; i<effectCount; i++ )
	{
		UserBuff buf;
		buf.skillID  = p->readUInt();
		buf.skillLvl = p->readUShort();
		buf.duration = p->readUInt();
		// add this buff to party player buffs
		party_player_buffs[idx]->addBuff( &buf );
		//
		/*char skillName[256] = {0};
		L2Data_DB_GetSkillNameByID( skillID, skillName );
		log_error_np( LOG_OK, "skill %u: %u [%s], duration %u;   dat %08X\n",
			i, skillID, skillName, duration, (unsigned int)dat );*/
	}
}

void UserParty::process_BuffTick( unsigned int curTick )
{
	Lock();
	int i;
	for( i=0; i<UserParty::MAX_PARTY; i++ )
	{
		if( !party_player_buffs[i] ) continue;
		party_player_buffs[i]->process_BuffTick( curTick );
	}
	Unlock();
}
