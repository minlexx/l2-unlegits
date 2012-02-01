#include "stdafx.h"
#include "Logger.h"
#include "SE_funcs.h"

// int l2ccfg_getInt( string varName )
int l2ccfg_getInt( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L ); if( !cl ) return 0;
	int nArgs = lua_gettop( L ); if( nArgs < 1 ) return 0;
	const char *varName = lua_tolstring( L, 1, NULL ); if( !varName ) return 0;
	BotConfig *cfg = cl->get_BotConfig();
	lua_pushnumber( L, cfg->getValInt( varName ) );
	return 1;
}

// int l2ccfg_setInt( string varName, int val )
int l2ccfg_setInt( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L ); if( !cl ) return 0;
	return 0;
}

// string l2ccfg_getStr( string varName )
int l2ccfg_getStr( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L ); if( !cl ) return 0;
	int nArgs = lua_gettop( L ); if( nArgs < 1 ) return 0;
	const char *varName = lua_tolstring( L, 1, NULL ); if( !varName ) return 0;
	BotConfig *cfg = cl->get_BotConfig();
	lua_pushstring( L, cfg->getValStr( varName ) );
	return 1;
}

// int l2ccfg_setStr( string varName, string val )
int l2ccfg_setStr( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L ); if( !cl ) return 0;
	return 0;
}

// table l2ccfg_getAllConfig()
int l2ccfg_getAllConfig( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L ); if( !cl ) return 0;
	BotConfig *cfg = cl->get_BotConfig();
	int i = 0;
	//int tableIndex = 1;
	lua_createtable( L, 0, 0 );
	for( i=0; i<BotConfig::MAX_VALUES; i++ )
	{
		BotConfigValue val = cfg->getValByIdx( i );
		if( val.getType() == CFG_TYPE_NONE ) continue;
		const char *valName = val.name();
		lua_pushstring( L, valName );
		switch( val.getType() )
		{
		case CFG_TYPE_INT: lua_pushnumber( L, (lua_Number)val.getInt() ); break;
		case CFG_TYPE_FLT: lua_pushnumber( L, (lua_Number)val.getFlt() ); break;
		case CFG_TYPE_STR: lua_pushstring( L, val.getStr() ); break;
		}
		lua_settable( L, -3 );
		//tableIndex++;
	}
	return 1;
}


// bool l2c_isCombatEnabled()
int l2c_isCombatEnabled( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) { lua_pushboolean( L, 0 ); return 1; }
	int e = 0;
	if( cl->isBotCombatEnabled() ) e = 1;
	lua_pushboolean( L, e );
	return 1;
}

// l2c_combatEnable( bool bEnable = true )
int l2c_combatEnable( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L ); if( !cl ) return 0;
	bool e = true;
	int nArgs = lua_gettop( L );
	if( nArgs >= 1 ) { e = lua_toboolean( L, 1 ) ? true : false; }
	cl->botCombatEnable( e );
	return 0;
}

