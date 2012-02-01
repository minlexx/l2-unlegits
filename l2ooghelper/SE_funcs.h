#ifndef H_SE_FUNCS
#define H_SE_FUNCS

#include "lib/lua/lua.hpp"
#include "IL2Client.h"

void SE_funcs_register( lua_State *L );
void SE_register_client_funcs( lua_State *L, void *pvL2Client, void *ptrScriptEngine );

// function registration
void SE_regclientfunc( lua_State *L, void *pvL2Client, lua_CFunction funcPtr, const char *funcName );
void SE_regScriptEngineFunc( lua_State *L, void *ptrScriptEngine, lua_CFunction funcPtr, const char *funcName );

// "system" functions
int l2h_print( lua_State *L );
int l2h_console_enable( lua_State *L );
int l2h_delay( lua_State *L );
int l2h_soundAlert( lua_State *L );
int l2h_time( lua_State *L );
int l2h_timeMsec( lua_State *L );

IL2Client *L2C_func_getClient( lua_State *L );

// general user info
int l2c_is_INGAME( lua_State *L );
int l2c_getHPMPCPWeight( lua_State *L );
int l2c_getStats( lua_State *L );

// chatting
int l2c_say( lua_State *L );
int l2c_sayPm( lua_State *L );
int l2c_npcDlg( lua_State *L );
int l2c_npcDlgClear( lua_State *L );
int l2c_npcDlgExists( lua_State *L );

// movement
int l2c_getPos( lua_State *L );
int l2c_moveTo( lua_State *L );
int l2c_moveToDelta( lua_State *L );
int l2c_getSitRun( lua_State *L );
int l2c_sitStand( lua_State *L );
int l2c_runWalk( lua_State *L );

// target, attack
int l2c_action( lua_State *L );
int l2c_attack( lua_State *L );
int l2c_targetByName( lua_State *L );
int l2c_targetCancel( lua_State *L );
int l2c_getTarget( lua_State *L );

// skills
int l2c_useSkill( lua_State *L );
int l2c_getSkillLevel( lua_State *L );
int l2c_getSkillReuseLeft( lua_State *L );
int l2c_isCastingNow( lua_State *L );

// items (in inventory)
int l2c_autoSoulshot( lua_State *L );
int l2c_getItemCount( lua_State *L );
int l2c_useItem( lua_State *L );
int l2c_useItemByObjectId( lua_State *L );
int l2c_getPaperdollItem( lua_State *L );

// buffs
int l2c_getBuffs( lua_State *L );
int l2c_buffCancel( lua_State *L );

// world information
int l2c_getVisibleChars( lua_State *L );
int l2c_getVisibleMobs( lua_State *L );
int l2c_getVisibleNpcs( lua_State *L );
int l2c_getVisibleItems( lua_State *L );
// individual world object information, based on objectID
int l2c_getCharObjectIdByName( lua_State *L );
int l2c_getNpcObjectIdByName( lua_State *L );
int l2c_getObjectInfoByObjectId( lua_State *L );
int l2c_isCharDead( lua_State *L );

// party
int l2c_getParty( lua_State *L );
int l2c_getPartyMemberBuffs( lua_State *L );
int l2c_partySendInvite( lua_State *L );
int l2c_partyLeave( lua_State *L );
int l2c_partyKickMember( lua_State *L );
int l2c_partyChangeLeader( lua_State *L );

// botting config
int l2ccfg_getInt( lua_State *L );
int l2ccfg_setInt( lua_State *L );
int l2ccfg_getStr( lua_State *L );
int l2ccfg_setStr( lua_State *L );
int l2ccfg_getAllConfig( lua_State *L );
// botting combat state
int l2c_isCombatEnabled( lua_State *L );
int l2c_combatEnable( lua_State *L );

// handlers
int sys_register_onChat( lua_State *L );

// dirty hacks
int l2c_sendPacketHex( lua_State *L );

#endif
