#include "stdafx.h"
#include "Logger.h"
#include "SE_funcs.h"
#include "DBLayer.h"

int l2c_getBuffs( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L ); if( !cl ) return 0;
	UserBuffs *b = cl->get_Buffs();
	int i;
	lua_createtable( L, 0, 0 );
	int tableIndex = 1;
	for( i=0; i<b->buffCount; i++ )
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
		wchar_t wstr[256] = {0};
		char skillName[256] = {0};
		DBLayer_getSkillName( b->buff[i].skillID, wstr, 256 );
		WideCharToMultiByte( CP_ACP, 0, wstr, -1, skillName, 255, NULL, NULL );
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
	IL2Client *cl = L2C_func_getClient( L ); if( !cl ) return 0;
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	unsigned int skillID = (unsigned int)lua_tonumber( L, 1 );
	cl->game_buffCancel( skillID );
	return 0;
}
