#include "stdafx.h"
#include "../Logger.h"
#include "../lua/lua.hpp"
#include "../ScriptEngine.h"
#include "../GameClient.h"
extern class GameClient *g_game_client;
#include "../PacketInjector.h"

//l2c_autoSoulshot( int itemID, [boolean enable = true] )
int l2c_autoSoulshot( lua_State *L )
{
	if( !g_game_client ) return 0;
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	unsigned int itemID = (unsigned int)lua_tonumber( L, 1 );
	bool bEnable = true;
	if( nArgs >= 2 ) bEnable = lua_toboolean( L, 2 ) ? true : false;
	PGen_RequestAutoSoulshot( itemID, bEnable );
	return 0;
}

// int l2c_getItemCount( int itemID )
int l2c_getItemCount( lua_State *L )
{
	if( !g_game_client ) return 0;
	int nArgs = lua_gettop( L );
	if( nArgs < 1 )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}
	unsigned int itemID = (unsigned int)lua_tonumber( L, 1 );
	UserInventoryItem it;
	g_game_client->ai.inv.getItemInfoByItemId( itemID, it );
	lua_Number cnt = (lua_Number)it.count;
	lua_pushnumber( L, cnt );
	return 1;
}

// l2c_useItem( int itemID )
int l2c_useItem( lua_State *L )
{
	if( !g_game_client ) return 0;
	if( lua_gettop( L ) < 1 ) return 0;
	UserInventoryItem it;
	unsigned int itemID = (unsigned int)lua_tonumber( L, 1 );
	g_game_client->ai.inv.getItemInfoByItemId( itemID, it );
	if( it.objectID > 0  &&  it.count > 0 )
		PGen_UseItem( it.objectID );
	return 0;
}

// l2c_useItemByObjectId( int objectID )
int l2c_useItemByObjectId( lua_State *L )
{
	if( !g_game_client ) return 0;
	if( lua_gettop( L ) < 1 ) return 0;
	PGen_UseItem( (unsigned int)lua_tonumber( L, 1 ) );
	return 0;
}

// int, int l2c_getPaperdollItem( int slot )
int l2c_getPaperdollItem( lua_State *L )
{
	if( !g_game_client ) return 0;
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	unsigned int slot = (unsigned int)lua_tonumber( L, 1 );
	UserState *pusr = &(g_game_client->ai.usr);
	lua_pushnumber( L, pusr->paperdoll_iid[ slot ] );
	lua_pushnumber( L, pusr->paperdoll_oid[ slot ] );
	return 2;
}
