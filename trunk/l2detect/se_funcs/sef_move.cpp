#include "stdafx.h"
#include "../Logger.h"
#include "../lua/lua.hpp"
#include "../ScriptEngine.h"
#include "../GameClient.h"
#include "../PacketInjector.h"
extern class GameClient *g_game_client;

int l2c_getPos( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs != 0 )
	{
		lua_pushnumber( L, 0 );
		lua_pushnumber( L, 0 );
		lua_pushnumber( L, 0 );
		return 3;
	}
	if( !g_game_client )
	{
		lua_pushnumber( L, 0 );
		lua_pushnumber( L, 0 );
		lua_pushnumber( L, 0 );
		return 3;
	}
	UserState *usr = &(g_game_client->ai.usr);
	lua_pushnumber( L, usr->x );
	lua_pushnumber( L, usr->y );
	lua_pushnumber( L, usr->z );
	return 3;
}

int l2c_moveTo( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 2 ) return 0;
	if( !g_game_client ) return 0;
	int x = (int)lua_tonumber( L, 1 );
	int y = (int)lua_tonumber( L, 2 );
	int z = g_game_client->ai.usr.z;
	if( nArgs == 3 ) z = (int)lua_tonumber( L, 3 );
	PGen_MoveBackwardToLocation( x, y, z, g_game_client->ai.usr.x, g_game_client->ai.usr.y, g_game_client->ai.usr.z );
	return 0;
}

int l2c_moveToDelta( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 2 ) return 0;
	if( !g_game_client ) return 0;
	int x = (int)lua_tonumber( L, 1 );
	int y = (int)lua_tonumber( L, 2 );
	int z = 0;
	if( nArgs == 3 ) z = (int)lua_tonumber( L, 3 );
	PGen_MoveBackwardToLocation(
		g_game_client->ai.usr.x + x,
		g_game_client->ai.usr.y + y,
		g_game_client->ai.usr.z + z,
		g_game_client->ai.usr.x,
		g_game_client->ai.usr.y,
		g_game_client->ai.usr.z );
	return 0;
}

int l2c_getSitRun( lua_State *L )
{
	if( !g_game_client )
	{
		lua_pushboolean( L, 0 );
		lua_pushboolean( L, 1 );
		return 2;
	}
	lua_pushboolean( L, g_game_client->ai.usr.isSitting );
	lua_pushboolean( L, g_game_client->ai.usr.isRunning );
	return 2;
}

int l2c_sitStand( lua_State *L )
{
	UNREFERENCED_PARAMETER(L);
	if( !g_game_client ) return 0;
	PGen_RequestActionUse( 0, false, false ); // actionId 0: sit/stand
	return 0;
}

int l2c_runWalk( lua_State *L )
{
	UNREFERENCED_PARAMETER(L);
	if( !g_game_client ) return 0;
	PGen_RequestActionUse( 1, false, false ); // actionId 1: run/walk
	return 0;
}
