#include "stdafx.h"
#include "Logger.h"
#include "SE_funcs.h"

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
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl )
	{
		lua_pushnumber( L, 0 );
		lua_pushnumber( L, 0 );
		lua_pushnumber( L, 0 );
		return 3;
	}
	UserInfo *usr = cl->get_UserInfo();
	lua_pushnumber( L, usr->x );
	lua_pushnumber( L, usr->y );
	lua_pushnumber( L, usr->z );
	return 3;
}

int l2c_moveTo( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 2 ) return 0;
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) return 0;
	double x = lua_tonumber( L, 1 );
	double y = lua_tonumber( L, 2 );
	int z = 0x7FFFFFFF;
	if( nArgs == 3 ) z = (int)lua_tonumber( L, 3 );
	//log_error_np( LOG_OK, "LUA: lua_moveto( %d, %d, %d ) [%d]\n", (int)x, (int)y, (int)z, nArgs );
	cl->game_MoveBackwardToLocation( (int)x, (int)y, z );
	return 0;
}

int l2c_moveToDelta( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 2 ) return 0;
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) return 0;
	double x = lua_tonumber( L, 1 );
	double y = lua_tonumber( L, 2 );
	double z = 0.0;
	if( nArgs == 3 ) z = lua_tonumber( L, 3 );
	//log_error_np( LOG_OK, "LUA: l2c_movetodelta( %d, %d, %d ) [%d]\n", (int)x, (int)y, (int)z, nArgs );
	cl->game_MoveToDelta( (int)x, (int)y, (int)z );
	return 0;
}

int l2c_getSitRun( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl )
	{
		lua_pushboolean( L, 0 );
		lua_pushboolean( L, 1 );
		return 2;
	}
	UserInfo *usr = cl->get_UserInfo();
	lua_pushboolean( L, usr->isSitting );
	lua_pushboolean( L, usr->isRunning );
	return 2;
}

int l2c_sitStand( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) return 0;
	cl->game_SitStand();
	return 0;
}

int l2c_runWalk( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) return 0;
	cl->game_RunWalk();
	return 0;
}
