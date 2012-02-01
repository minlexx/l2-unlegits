#include "stdafx.h"
#include "Logger.h"
#include "SE_funcs.h"

int l2c_is_INGAME( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs != 0 ) { lua_pushboolean( L, 0 ); return 1; }
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) { lua_pushboolean( L, 0 ); return 1; }
	int st = 0;
	st = cl->is_IN_GAME();
	lua_pushboolean( L, st );
	return 1;
}

int l2c_getHPMPCPWeight( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs != 0 ) { lua_pushnil( L ); return 1; }
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) { lua_pushnil( L ); return 1; }
	UserInfo *usr = cl->get_UserInfo();
	lua_pushnumber( L, usr->curHp );
	lua_pushnumber( L, usr->maxHp );
	lua_pushnumber( L, usr->curMp );
	lua_pushnumber( L, usr->maxMp );
	lua_pushnumber( L, usr->curCp );
	lua_pushnumber( L, usr->maxCp );
	lua_pushnumber( L, usr->curLoad );
	lua_pushnumber( L, usr->maxLoad );
	return 8;
}

int l2c_getStats( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) { lua_pushnil( L ); return 1; }
	UserInfo *usr = cl->get_UserInfo();
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
