#ifndef H_USERPARTY
#define H_USERPARTY

#include "UserBuffs.h"

class UserParty
{
public:
	UserParty()                                     { InitializeCriticalSection( &cs ); clear(); }
	~UserParty()                                    { clear(); DeleteCriticalSection( &cs ); }
public:
	static const int  MAX_PARTY = 8;
public:
	void              clear();
	int               addPlayer( L2Player *pl );
	bool              updatePlayer( L2Player *pl );
	bool              delPlayer( unsigned int objectID );
	bool              delPlayer( const wchar_t *playerName );
	void              deleteAll();
	bool              isInParty( unsigned int objectID, int *index );
	bool              getPartyPlayerStatsByObjectID(
                          unsigned int objectID,
                          int *pcurHp, int *pmaxHp,
                          int *pcurMp, int *pmaxMp,
                          int *pcurCp, int *pmaxCp,
                          unsigned int *pclassID,
						  unsigned int *targetObjectID
	                  );
	bool              setPlayerTarget( unsigned int objectID, unsigned int targetObjectID );
public:
	void              Lock()                               { EnterCriticalSection( &cs ); }
	void              Unlock()                             { LeaveCriticalSection( &cs ); }
public:
	int               getCount()                           { return count; }
	L2Player         *getPartyPlayer( int idx );
	UserBuffs        *getPartyPlayerBuffs( int idx );
	unsigned int      getLeaderObjectId() const            { return leaderObjectId; }
	unsigned int      getItemDistribution() const          { return itemDistribution; }
	void              setLeaderObjectId( unsigned oid )    { leaderObjectId = oid; }
	void              setItemDistribution( unsigned type ) { itemDistribution = type; }
public: // packet handlers
	void              parse_PartySpelled( L2GamePacket *p );
public: // world tick processor for party buffs
	void              process_BuffTick( unsigned int curTick );
protected:
	void              shiftAfterDel( int deleted_pos );
protected:
	unsigned int      leaderObjectId;
	unsigned int      itemDistribution;
	CRITICAL_SECTION  cs;
	int               count;
	L2Player         *party_player[MAX_PARTY];
	UserBuffs        *party_player_buffs[MAX_PARTY];
};

#endif
