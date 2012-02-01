#include "stdafx.h"
#include "../Logger.h"
#include "../lua/lua.hpp"
#include "../ScriptEngine.h"
#include "../GameClient.h"
extern class GameClient *g_game_client;
#include "../PacketInjector.h"

int l2c_getBuffs( lua_State *L )
{
	if( !g_game_client ) return 0;
	UserBuffs *b = &(g_game_client->ai.buffs);
	int i;
	lua_createtable( L, 0, 0 );
	int tableIndex = 1;
	// usual buffs
	for( i=0; i<b->USER_MAX_BUFFS; i++ )
	{
		if( b->buff[i].skillID == 0 ) continue;
		lua_pushnumber( L, tableIndex );
		//
		lua_createtable( L, 0, 0 );
		//
		lua_pushstring( L, "skillID" );
		lua_pushnumber( L, b->buff[i].skillID );
		lua_settable( L, -3 );
		lua_pushstring( L, "skillLvl" );
		lua_pushnumber( L, b->buff[i].skillLvl );
		lua_settable( L, -3 );
		lua_pushstring( L, "durationSecs" );
		lua_pushnumber( L, b->buff[i].duration );
		lua_settable( L, -3 );
		// skill name
		char skillName[256] = {0};
		L2Data_DB_GetSkillNameByID( b->buff[i].skillID, skillName );
		lua_pushstring( L, "skillName" );
		lua_pushstring( L, skillName );
		lua_settable( L, -3 );
		//
		lua_settable( L, -3 );
		//
		tableIndex++;
	}
	// short buffs
	for( i=0; i<b->USER_MAX_SHORT_BUFFS; i++ )
	{
		if( b->short_buff[i].skillID == 0 ) continue;
		lua_pushnumber( L, tableIndex );
		//
		lua_createtable( L, 0, 0 );
		//
		lua_pushstring( L, "skillID" );
		lua_pushnumber( L, b->short_buff[i].skillID );
		lua_settable( L, -3 );
		lua_pushstring( L, "skillLvl" );
		lua_pushnumber( L, b->short_buff[i].skillLvl );
		lua_settable( L, -3 );
		lua_pushstring( L, "durationSecs" );
		lua_pushnumber( L, b->short_buff[i].duration );
		lua_settable( L, -3 );
		// skill name
		char skillName[256] = {0};
		L2Data_DB_GetSkillNameByID( b->short_buff[i].skillID, skillName );
		lua_pushstring( L, "skillName" );
		lua_pushstring( L, skillName );
		lua_settable( L, -3 );
		//
		lua_settable( L, -3 );
		//
		tableIndex++;
	}
	return 1;
}

int l2c_buffCancel( lua_State *L )
{
	if( !g_game_client ) return 0;
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	unsigned int skillID = (unsigned int)lua_tonumber( L, 1 );
	PGen_RequestDispel( skillID );
	return 0;
}
