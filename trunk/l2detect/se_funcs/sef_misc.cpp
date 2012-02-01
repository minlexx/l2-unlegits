#include "stdafx.h"
#include "../Logger.h"
#include "../lua/lua.hpp"
#include "../ScriptEngine.h"
#include "../GameClient.h"
#include "../PacketInjector.h"
extern class GameClient *g_game_client;

// bool l2c_isCombatEnabled()
// here always returns true
int l2c_isCombatEnabled( lua_State *L )
{
	UNREFERENCED_PARAMETER(L);
	lua_pushboolean( L, 1 );
	return 1;
}

// l2c_combatEnable( bool bEnable = true )
int l2c_combatEnable( lua_State *L )
{
	UNREFERENCED_PARAMETER(L);
	return 0;
}

// int l2ccfg_getInt( string varName )
int l2ccfg_getInt( lua_State *L )
{
	lua_pushnumber( L, 0 );
	return 1;
}

// int l2ccfg_setInt( string varName, int val )
int l2ccfg_setInt( lua_State *L )
{
	UNREFERENCED_PARAMETER(L);
	return 0;
}

// string l2ccfg_getStr( string varName )
int l2ccfg_getStr( lua_State *L )
{
	lua_pushstring( L, "" );
	return 1;
}

// int l2ccfg_setStr( string varName, string val )
int l2ccfg_setStr( lua_State *L )
{
	UNREFERENCED_PARAMETER(L);
	return 0;
}

// table l2ccfg_getAllConfig()
int l2ccfg_getAllConfig( lua_State *L )
{
	//int tableIndex = 1;
	lua_createtable( L, 0, 0 );
	return 1;
}



// l2c_sendPacketHex( string packetHex )
int l2c_sendPacketHex( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	if( !g_game_client ) return 0;
	const char *hex = lua_tolstring( L, 1, NULL );
	if( !hex ) return 0;
	PGen_send_hackPacketHex_toServer( hex );
	return 0;
}

