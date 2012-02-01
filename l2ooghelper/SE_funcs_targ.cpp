#include "stdafx.h"
#include "Logger.h"
#include "SE_funcs.h"

int l2c_action( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) return 0;
	unsigned int objectID = (unsigned int)lua_tonumber( L, 1 );
	cl->game_Action( objectID );
	return 0;
}

int l2c_attack( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) return 0;
	cl->game_Attack();
	return 0;
}

int l2c_targetByName( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) return 0;
	const char *aname = lua_tolstring( L, 1, NULL );
	wchar_t wname[256];
	memset( wname, 0, sizeof(wname) );
	MultiByteToWideChar( CP_ACP, 0, aname, -1, wname, 255 );
	cl->game_TargetByName( wname );
	return 0;
}

int l2c_targetCancel( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) return 0;
	cl->game_TargetCancel();
	return 0;
}

int l2c_getTarget( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl )
	{
		lua_pushnumber( L, 0 );
		lua_pushnumber( L, 0 );
		lua_pushnumber( L, 0 );
		return 3;
	}
	UserInfo *pusr = cl->get_UserInfo();
	lua_pushnumber( L, pusr->targetObjectID );
	lua_pushnumber( L, pusr->targetCurHp );
	lua_pushnumber( L, pusr->targetMaxHp );
	return 3;
}

