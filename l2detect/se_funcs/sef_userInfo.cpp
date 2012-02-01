#include "stdafx.h"
#include "../Logger.h"
#include "../lua/lua.hpp"
#include "../ScriptEngine.h"
#include "../GameClient.h"
extern class GameClient *g_game_client;

int l2c_is_INGAME( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs != 0  ||  g_game_client==NULL ) { lua_pushboolean( L, 0 ); return 1; }
	int st = g_game_client->getState();
	if( st == GCST_IN_GAME ) lua_pushboolean( L, 1 ); else lua_pushboolean( L, 0 );
	return 1;
}

int l2c_getHPMPCPWeight( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs != 0 ) { lua_pushnil( L ); return 1; }
	if( !g_game_client ) { lua_pushnil( L ); return 1; }
	UserState *usr = &(g_game_client->ai.usr);
	lua_pushnumber( L, usr->hp );
	lua_pushnumber( L, usr->hp_max );
	lua_pushnumber( L, usr->mp );
	lua_pushnumber( L, usr->mp_max );
	lua_pushnumber( L, usr->cp );
	lua_pushnumber( L, usr->cp_max );
	lua_pushnumber( L, usr->curLoad );
	lua_pushnumber( L, usr->maxLoad );
	return 8;
}

int l2c_getStats( lua_State *L )
{
	if( !g_game_client ) { lua_pushnil( L ); return 1; }
	UserState *usr = &(g_game_client->ai.usr);
	lua_createtable( L, 0, 0 );
	// name
	char aname[256] = {0};
	sprintf( aname, "%S", usr->charName );
	lua_pushstring( L, "name" );
	lua_pushstring( L, aname );
	lua_settable( L, -3 );
	// number stats:
	// pAtk, mAtk, pDef, mDef, pAtkSpd, mAtkSpd
#define LUA_PUSHTABLEKEY( sKeyName, keyName ) lua_pushstring( L, sKeyName ); lua_pushnumber( L, usr->keyName ); lua_settable( L, -3 );
	LUA_PUSHTABLEKEY( "pAtk", pAtk )
	LUA_PUSHTABLEKEY( "pDef", pDef )
	LUA_PUSHTABLEKEY( "mAtk", mAtk )
	LUA_PUSHTABLEKEY( "mDef", mDef )
	LUA_PUSHTABLEKEY( "pAtkSpd", pAtkSpd )
	LUA_PUSHTABLEKEY( "mAtkSpd", mAtkSpd )
	// self objectID
	LUA_PUSHTABLEKEY( "objectID", objectID )
	LUA_PUSHTABLEKEY( "heading", heading )
	LUA_PUSHTABLEKEY( "karma", karma )
#undef LUA_PUSHTABLEKEY
	//
	return 1;
}
