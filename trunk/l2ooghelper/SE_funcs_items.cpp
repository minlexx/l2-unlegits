#include "stdafx.h"
#include "Logger.h"
#include "SE_funcs.h"

int l2c_autoSoulshot( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) return 0;
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	unsigned int itemID = (unsigned int)lua_tonumber( L, 1 );
	bool bEnable = true;
	if( nArgs >= 2 ) bEnable = lua_toboolean( L, 2 ) ? true : false;
	cl->game_autoSoulshot( itemID, bEnable );
	return 0;
}

// int l2c_getItemCount( int itemID )
int l2c_getItemCount( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) return 0;
	int nArgs = lua_gettop( L );
	if( nArgs < 1 )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}
	unsigned int itemID = (unsigned int)lua_tonumber( L, 1 );
	lua_Number cnt = (lua_Number)cl->game_getItemCount( itemID );
	lua_pushnumber( L, cnt );
	return 1;
}

// l2c_useItem( int itemID )
int l2c_useItem( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L ); if( !cl ) return 0;
	cl->game_UseItemByItemId( (unsigned int)lua_tonumber( L, 1 ) );
	return 0;
}

// l2c_useItemByObjectId( int objectID )
int l2c_useItemByObjectId( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L ); if( !cl ) return 0;
	cl->game_UseItemByObjectId( (unsigned int)lua_tonumber( L, 1 ) );
	return 0;
}

// int, int l2c_getPaperdollItem( int slot )
int l2c_getPaperdollItem( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L ); if( !cl ) return 0;
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	unsigned int slot = (unsigned int)lua_tonumber( L, 1 );
	UserInfo *pusr = cl->get_UserInfo();
	lua_pushnumber( L, pusr->paperdoll_iid[ slot ] );
	lua_pushnumber( L, pusr->paperdoll_oid[ slot ] );
	return 2;
}
