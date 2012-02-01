#include "stdafx.h"
#include "../Logger.h"
#include "../lua/lua.hpp"
#include "../ScriptEngine.h"
#include "../GameClient.h"
#include "../PacketInjector.h"
extern class GameClient *g_game_client;

// table l2c_getParty()
int l2c_getParty( lua_State *L )
{
	if( !g_game_client )
	{
		lua_createtable( L, 0, 0 );
		return 1;
	}
	UserParty *pty = &g_game_client->ai.party;
	if( !pty )
	{
		lua_createtable( L, 0, 0 );
		return 1;
	}
	pty->Lock(); // really?
	int cnt = pty->getCount();
	int i;
	lua_createtable( L, 0, 0 );
	int tableIndex = 1;
	char aname[256] = {0};
	for( i=0; i<cnt; i++ )
	{
		L2Player *pl = pty->getPartyPlayer( i );
		if( !pl ) continue;
		//
		lua_pushnumber( L, tableIndex );
		//
		lua_createtable( L, 0, 0 );
		//
		lua_pushstring( L, "objectID" );
		lua_pushnumber( L, pl->objectID );
		lua_settable( L, -3 );
		//
		lua_pushstring( L, "name" );
		sprintf( aname, "%S", pl->charName );
		lua_pushstring( L, aname );
		lua_settable( L, -3 );
		//
		lua_pushstring( L, "level" );
		lua_pushnumber( L, pl->level );
		lua_settable( L, -3 );
		//
		lua_pushstring( L, "curHp" );
		lua_pushnumber( L, pl->curHp );
		lua_settable( L, -3 );
		lua_pushstring( L, "maxHp" );
		lua_pushnumber( L, pl->maxHp );
		lua_settable( L, -3 );
		lua_pushstring( L, "curMp" );
		lua_pushnumber( L, pl->curMp );
		lua_settable( L, -3 );
		lua_pushstring( L, "maxMp" );
		lua_pushnumber( L, pl->maxMp );
		lua_settable( L, -3 );
		lua_pushstring( L, "curCp" );
		lua_pushnumber( L, pl->curCp );
		lua_settable( L, -3 );
		lua_pushstring( L, "maxCp" );
		lua_pushnumber( L, pl->maxCp );
		lua_settable( L, -3 );
		//
		lua_pushstring( L, "classID" );
		lua_pushnumber( L, pl->classID );
		lua_settable( L, -3 );
		//
		lua_pushstring( L, "targetObjectID" );
		lua_pushnumber( L, pl->targetObjectID );
		lua_settable( L, -3 );
		//
		lua_settable( L, -3 );
		tableIndex++;
	}
	pty->Unlock();
	return 1;
}

// table l2c_getPartyMemberBuffs( int objectID )
int l2c_getPartyMemberBuffs( lua_State *L )
{
	if( !g_game_client )
	{
		lua_createtable( L, 0, 0 );
		return 1;
	}
	int nArgs = lua_gettop( L );
	if( nArgs < 1 )
	{
		lua_createtable( L, 0, 0 );
		return 1;
	}
	unsigned int objectID = (unsigned int)lua_tonumber( L, 1 );
	if( objectID == 0 )
	{
		lua_createtable( L, 0, 0 );
		return 1;
	}
	UserParty *pty = &g_game_client->ai.party;
	if( !pty )
	{
		lua_createtable( L, 0, 0 );
		return 1;
	}
	if( pty->getCount() < 1 )
	{
		lua_createtable( L, 0, 0 );
		return 1;
	}
	int idx = -1;
	pty->Lock();
	if( !pty->isInParty( objectID, &idx ) )
	{
		pty->Unlock();
		lua_createtable( L, 0, 0 );
		return 1;
	}
	if( idx == -1 )
	{
		pty->Unlock();
		lua_createtable( L, 0, 0 );
		return 1;
	}
	UserBuffs *buffs = pty->getPartyPlayerBuffs( idx );
	if( !buffs )
	{
		pty->Unlock();
		lua_createtable( L, 0, 0 );
		return 1;
	}
	// at last, we can push buffs :)
	lua_createtable( L, 0, 0 );
	int tableIndex = 1;
	int i = 0;
	for( i=0; i<UserBuffs::USER_MAX_BUFFS; i++ )
	{
		if( buffs->buff[i].skillID == 0 ) continue;
		//
		lua_pushnumber( L, tableIndex );
		//
		lua_createtable( L, 0, 0 );
		//
		lua_pushstring( L, "skillID" );
		lua_pushnumber( L, buffs->buff[i].skillID );
		lua_settable( L, -3 );
		lua_pushstring( L, "skillLvl" );
		lua_pushnumber( L, buffs->buff[i].skillLvl );
		lua_settable( L, -3 );
		lua_pushstring( L, "durationSecs" );
		lua_pushnumber( L, buffs->buff[i].duration );
		lua_settable( L, -3 );
		// skill name
		char skillName[256] = {0};
		L2Data_DB_GetSkillNameByID( buffs->buff[i].skillID, skillName );
		lua_pushstring( L, "skillName" );
		lua_pushstring( L, skillName );
		lua_settable( L, -3 );
		//
		lua_settable( L, -3 );
		//
		tableIndex++;
	}
	for( i=0; i<UserBuffs::USER_MAX_SHORT_BUFFS; i++ )
	{
		if( buffs->short_buff[i].skillID == 0 ) continue;
		//
		lua_pushnumber( L, tableIndex );
		//
		lua_createtable( L, 0, 0 );
		//
		lua_pushstring( L, "skillID" );
		lua_pushnumber( L, buffs->short_buff[i].skillID );
		lua_settable( L, -3 );
		lua_pushstring( L, "skillLvl" );
		lua_pushnumber( L, buffs->short_buff[i].skillLvl );
		lua_settable( L, -3 );
		lua_pushstring( L, "durationSecs" );
		lua_pushnumber( L, buffs->short_buff[i].duration );
		lua_settable( L, -3 );
		// skill name
		char skillName[256] = {0};
		L2Data_DB_GetSkillNameByID( buffs->buff[i].skillID, skillName );
		lua_pushstring( L, "skillName" );
		lua_pushstring( L, skillName );
		lua_settable( L, -3 );
		//
		lua_settable( L, -3 );
		//
		tableIndex++;
	}
	//
	pty->Unlock();
	return 1;
}

// l2c_partySendInvite( string name, int loot )
int l2c_partySendInvite( lua_State *L )
{
	if( !g_game_client ) return 0;
	int nArgs = lua_gettop( L );
	if( nArgs < 2 ) return 0;
	const char *aname = lua_tolstring( L, 1, NULL );
	int loot = (int)lua_tonumber( L, 2 );
	wchar_t name[256] = {0};
	MultiByteToWideChar( CP_ACP, 0, aname, -1, name, 255 );
	PGen_RequestJoinParty( name, (unsigned int)loot );
	return 0;
}

// l2c_partyLeave()
int l2c_partyLeave( lua_State *L )
{
	UNREFERENCED_PARAMETER(L);
	if( !g_game_client ) return 0;
	UserParty *pty = &g_game_client->ai.party;
	if( pty->getCount() < 1 ) return 0; // no party?
	PGen_RequestWithdrawalParty(); // ok, leave party
	return 0;
}

// l2c_partyKickMember( string name )
int l2c_partyKickMember( lua_State *L )
{
	if( !g_game_client ) return 0;
	UserParty *pty = &g_game_client->ai.party;
	if( pty->getCount() < 1 ) return 0; // no party?
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	const char *aname = lua_tolstring( L, 1, NULL );
	wchar_t name[256] = {0};
	MultiByteToWideChar( CP_ACP, 0, aname, -1, name, 255 );
	PGen_RequestOustPartyMember( name );
	return 0;
}

// l2c_partyChangeLeader( string name )
int l2c_partyChangeLeader( lua_State *L )
{
	if( !g_game_client ) return 0;
	UserParty *pty = &g_game_client->ai.party;
	if( pty->getCount() < 1 ) return 0; // no party?
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	const char *aname = lua_tolstring( L, 1, NULL );
	wchar_t name[256] = {0};
	MultiByteToWideChar( CP_ACP, 0, aname, -1, name, 255 );
	PGen_RequestChangePartyLeader( name );
	return 0;
}
