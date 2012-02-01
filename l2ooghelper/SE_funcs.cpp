#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"
#include "SE_funcs.h"
#include "ScriptEngine.h"

void SE_funcs_register( lua_State *L )
{
	//lua_register( L, "test_func", test_func );
	lua_register( L, "l2h_delay", l2h_delay );
	lua_register( L, "l2h_print", l2h_print );
	lua_register( L, "l2h_console_enable", l2h_console_enable );
	lua_register( L, "l2h_soundAlert", l2h_soundAlert );
	lua_register( L, "l2h_time", l2h_time );
	lua_register( L, "l2h_timeMsec", l2h_timeMsec );
}

int l2h_print( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs > 0 )
	{
		int i;
		for( i=1; i<=nArgs; i++ )
		{
			int typ = lua_type( L, i );
			switch( typ )
			{
			case LUA_TNIL: log_error_np( LOG_OK, "NULL" ); break;
			case LUA_TNUMBER:
				{
					double d = lua_tonumber( L, i );
					double floor_d = floor( d * 100 ); // 10012.00
					__int64 int_d = (__int64)floor_d; // 10012
					__int64 eds_cnt = int_d % 100; // 12
					if( eds_cnt ) log_error_np( LOG_OK, "%0.2f", d ); // original double
					else log_error_np( LOG_OK, "%I64d", int_d/100 ); // rounded integer
				} break;
			case LUA_TBOOLEAN:
				{
					lua_toboolean( L, i ) ? log_error_np( LOG_OK, "true" ) : log_error_np( LOG_OK, "false" );
				} break;
			case LUA_TSTRING: log_error_np( LOG_OK, "%s", lua_tostring( L, i ) ); break;
			case LUA_TTABLE: { log_error_np( LOG_OK, "(table)" ); /*lua_pop( L, i );*/ } break;
			case LUA_TFUNCTION: { log_error_np( LOG_OK, "(function)" ); /*lua_pop( L, i );*/ } break;
			default: lua_pop( L, i ); break;
			}
		}
	}
	return 0;
}


int l2h_console_enable( lua_State *L )
{
	int n = lua_gettop( L );    // number of arguments
	if( n > 0 )
	{
		if( lua_type( L, 1 ) == LUA_TBOOLEAN )
		{
			int enable = lua_toboolean( L, 1 );
			ErrorLogger_EnableLoggingToConsole( enable ? true : false );
		}
	}
	return 0;
}

int l2h_delay( lua_State *L )
{
	int n = lua_gettop( L );    // number of arguments
	//log_error_np( LOG_OK, "delay_lua: %d args\n", n );
	if( n >= 1 )
	{
		if( lua_type( L, 1 ) == LUA_TNUMBER )
		{
			int msec = (int)lua_tonumber( L, 1 );
			//log_error_np( LOG_OK, "delay_lua( %d );\n", msec );
			Sleep( (DWORD)msec );
		}
	}
	return 0;
}

int l2h_soundAlert( lua_State *L )
{
	int nArgs = lua_gettop( L );
	TCHAR fileName[256] = {0};
	if( nArgs == 0 )
	{
		GetWindowsDirectory( fileName, 255 );
		_tcscat( fileName, _T("\\Media\\ringin.wav") );
	}
	else
	{
		const char *astr = lua_tolstring( L, 1, NULL );
		MultiByteToWideChar( CP_ACP, 0, astr, -1, fileName, 255 );
	}
	PlaySound( fileName, NULL, SND_FILENAME|SND_ASYNC ); 
	return 0;
}

int l2h_time( lua_State *L )
{
	unsigned int secs = (unsigned int)time( NULL );
	lua_pushnumber( L, secs );
	return 1;
}

int l2h_timeMsec( lua_State *L )
{
	unsigned int msecs = GetTickCount();
	lua_pushnumber( L, msecs );
	return 1;
}

IL2Client *L2C_func_getClient( lua_State *L )
{
	int typ = lua_type( L, lua_upvalueindex( 1 ) );
	if( typ == LUA_TLIGHTUSERDATA )
	{
		IL2Client *cl = (IL2Client *)lua_touserdata( L, lua_upvalueindex( 1 ) );
		return cl;
	}
	return NULL;
}

int sys_should_exit( lua_State *L )
{
	int typ = lua_type( L, lua_upvalueindex( 1 ) );
	if( typ != LUA_TLIGHTUSERDATA )
	{
		log_error( LOG_ERROR, "sys_should_exit(): no light user data!\n" );
		lua_pushboolean( L, 1 );
		return 1;
	}
	ScriptEngine *se = (ScriptEngine *)lua_touserdata( L, lua_upvalueindex( 1 ) );
	//log_error( LOG_DEBUG, "sys_should_exit(): se = 0x%p\n", se );
	if( !se )
	{
		log_error( LOG_ERROR, "sys_should_exit(): ScriptEngine ptr == NULL!\n" );
		lua_pushboolean( L, 1 );
		return 1;
	}
	if( se->should_stop ) lua_pushboolean( L, 1 );
	else lua_pushboolean( L, 0 );
	return 1;
}


void SE_regclientfunc( lua_State *L, void *pvL2Client, lua_CFunction funcPtr, const char *funcName )
{
	lua_pushlightuserdata( L, pvL2Client ); // store class pointer as user data to function
	lua_pushcclosure( L, funcPtr, 1 ); // 1 - userdata objects count
	lua_setfield( L, LUA_GLOBALSINDEX, funcName );
}

void SE_regScriptEngineFunc( lua_State *L, void *ptrScriptEngine, lua_CFunction funcPtr, const char *funcName )
{
	lua_pushlightuserdata( L, ptrScriptEngine ); // store scriptEngine pointer as light user data
	lua_pushcclosure( L, funcPtr, 1 ); // 1 - user data objects count
	lua_setfield( L, LUA_GLOBALSINDEX, funcName );
}

void SE_register_client_funcs( lua_State *L, void *pvL2Client, void *ptrScriptEngine )
{
	if( !L || !pvL2Client ) return;
	L2Client *pcl = (L2Client *)pvL2Client;
	IL2Client *pInterface = pcl->getInterface();
	// script engine special func
	SE_regScriptEngineFunc( L, ptrScriptEngine, sys_should_exit,     "sys_should_exit" );
	SE_regScriptEngineFunc( L, ptrScriptEngine, sys_register_onChat, "sys_register_onChat" );
	//
	SE_regclientfunc( L, pInterface, l2c_is_INGAME,              "l2c_is_INGAME" );
	// user info
	SE_regclientfunc( L, pInterface, l2c_getHPMPCPWeight,        "l2c_getHPMPCPWeight" );
	SE_regclientfunc( L, pInterface, l2c_getPos,                 "l2c_getPos" );
	SE_regclientfunc( L, pInterface, l2c_getStats,               "l2c_getStats" );
	// chatting
	SE_regclientfunc( L, pInterface, l2c_say,                    "l2c_say" );
	SE_regclientfunc( L, pInterface, l2c_sayPm,                  "l2c_sayPm" );
	SE_regclientfunc( L, pInterface, l2c_npcDlg,                 "l2c_npcDlg" );
	SE_regclientfunc( L, pInterface, l2c_npcDlgClear,            "l2c_npcDlgClear" );
	SE_regclientfunc( L, pInterface, l2c_npcDlgExists,           "l2c_npcDlgExists" );
	// moving
	SE_regclientfunc( L, pInterface, l2c_moveTo,                 "l2c_moveTo" );
	SE_regclientfunc( L, pInterface, l2c_moveToDelta,            "l2c_moveToDelta" );
	SE_regclientfunc( L, pInterface, l2c_getSitRun,              "l2c_getSitRun" );
	SE_regclientfunc( L, pInterface, l2c_sitStand,               "l2c_sitStand" );
	SE_regclientfunc( L, pInterface, l2c_runWalk,                "l2c_runWalk" );
	// targeting, attack
	SE_regclientfunc( L, pInterface, l2c_action,                 "l2c_action" );
	SE_regclientfunc( L, pInterface, l2c_attack,                 "l2c_attack" );
	SE_regclientfunc( L, pInterface, l2c_targetByName,           "l2c_targetByName" );
	SE_regclientfunc( L, pInterface, l2c_targetCancel,           "l2c_targetCancel" );
	SE_regclientfunc( L, pInterface, l2c_getTarget,              "l2c_getTarget" );
	// skills
	SE_regclientfunc( L, pInterface, l2c_useSkill,               "l2c_useSkill" );
	SE_regclientfunc( L, pInterface, l2c_getSkillLevel,          "l2c_getSkillLevel" );
	SE_regclientfunc( L, pInterface, l2c_getSkillReuseLeft,      "l2c_getSkillReuseLeft" );
	SE_regclientfunc( L, pInterface, l2c_isCastingNow,           "l2c_isCastingNow" );
	// items in inventory, auto SS
	SE_regclientfunc( L, pInterface, l2c_autoSoulshot,           "l2c_autoSoulshot" );
	SE_regclientfunc( L, pInterface, l2c_getItemCount,           "l2c_getItemCount" );
	SE_regclientfunc( L, pInterface, l2c_useItem,                "l2c_useItem" );
	SE_regclientfunc( L, pInterface, l2c_useItemByObjectId,      "l2c_useItemByObjectId" );
	SE_regclientfunc( L, pInterface, l2c_getPaperdollItem,       "l2c_getPaperdollItem" );
	// buffs
	SE_regclientfunc( L, pInterface, l2c_getBuffs,               "l2c_getBuffs" );
	SE_regclientfunc( L, pInterface, l2c_buffCancel,             "l2c_buffCancel" );
	// get world objects information
	SE_regclientfunc( L, pInterface, l2c_getVisibleChars,        "l2c_getVisibleChars" );
	SE_regclientfunc( L, pInterface, l2c_getVisibleMobs,         "l2c_getVisibleMobs" );
	SE_regclientfunc( L, pInterface, l2c_getVisibleNpcs,         "l2c_getVisibleNpcs" );
	SE_regclientfunc( L, pInterface, l2c_getVisibleItems,        "l2c_getVisibleItems" );
	// party information
	SE_regclientfunc( L, pInterface, l2c_getParty,               "l2c_getParty" );
	SE_regclientfunc( L, pInterface, l2c_getPartyMemberBuffs,    "l2c_getPartyMemberBuffs" );
	SE_regclientfunc( L, pInterface, l2c_partySendInvite,        "l2c_partySendInvite" );
	SE_regclientfunc( L, pInterface, l2c_partyLeave,             "l2c_partyLeave" );
	SE_regclientfunc( L, pInterface, l2c_partyKickMember,        "l2c_partyKickMember" );
	SE_regclientfunc( L, pInterface, l2c_partyChangeLeader,      "l2c_partyChangeLeader" );
	// object information
	SE_regclientfunc( L, pInterface, l2c_getCharObjectIdByName,  "l2c_getCharObjectIdByName" );
	SE_regclientfunc( L, pInterface, l2c_getNpcObjectIdByName,   "l2c_getNpcObjectIdByName" );
	SE_regclientfunc( L, pInterface, l2c_getObjectInfoByObjectId,"l2c_getObjectInfoByObjectId" );
	SE_regclientfunc( L, pInterface, l2c_isCharDead,             "l2c_isCharDead" );
	// bot config
	SE_regclientfunc( L, pInterface, l2ccfg_getInt,              "l2ccfg_getInt" );
	SE_regclientfunc( L, pInterface, l2ccfg_setInt,              "l2ccfg_setInt" );
	SE_regclientfunc( L, pInterface, l2ccfg_getStr,              "l2ccfg_getStr" );
	SE_regclientfunc( L, pInterface, l2ccfg_setStr,              "l2ccfg_setStr" );
	SE_regclientfunc( L, pInterface, l2ccfg_getAllConfig,        "l2ccfg_getAllConfig" );
	// botting combat state
	SE_regclientfunc( L, pInterface, l2c_isCombatEnabled,        "l2c_isCombatEnabled" );
	SE_regclientfunc( L, pInterface, l2c_combatEnable,           "l2c_combatEnable" );
	// dirty hacks
	SE_regclientfunc( L, pInterface, l2c_sendPacketHex,          "l2c_sendPacketHex" );
}
