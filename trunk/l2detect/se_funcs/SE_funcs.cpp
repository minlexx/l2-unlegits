#include "stdafx.h"
#include "../Logger.h"
#include "../lua/lua.hpp"
#include "SE_funcs.h"

// called by ScriptEngine on init()
void SE_funcs_register( lua_State *L )
{
	// sys
	lua_register( L, "sys_should_exit",       sys_should_exit );
	lua_register( L, "sys_register_onChat",   sys_register_onChat );
	// sys
	lua_register( L, "l2h_delay",             l2h_delay );
	lua_register( L, "l2h_print",             l2h_print );
	lua_register( L, "l2h_console_enable",    l2h_console_enable );
	lua_register( L, "l2h_soundAlert",        l2h_soundAlert );
	lua_register( L, "l2h_time",              l2h_time );
	lua_register( L, "l2h_timeMsec",          l2h_timeMsec );
	// user info
	lua_register( L, "l2c_is_INGAME",         l2c_is_INGAME );
	lua_register( L, "l2c_getHPMPCPWeight",   l2c_getHPMPCPWeight );
	lua_register( L, "l2c_getStats",          l2c_getStats );
	// chatting
	lua_register( L, "l2c_say",               l2c_say );
	lua_register( L, "l2c_sayPm",             l2c_sayPm );
	lua_register( L, "l2c_npcDlg",            l2c_npcDlg );
	lua_register( L, "l2c_npcDlgClear",       l2c_npcDlgClear );
	lua_register( L, "l2c_npcDlgExists",      l2c_npcDlgExists );
	lua_register( L, "l2c_RequestBypassToServer", l2c_RequestBypassToServer );
	lua_register( L, "l2c_RequestLinkHtml",    l2c_RequestLinkHtml );
	// Moving
	lua_register( L, "l2c_getPos",            l2c_getPos );
	lua_register( L, "l2c_moveTo",            l2c_moveTo );
	lua_register( L, "l2c_moveToDelta",       l2c_moveToDelta );
	lua_register( L, "l2c_getSitRun",         l2c_getSitRun );
	lua_register( L, "l2c_sitStand",          l2c_sitStand );
	lua_register( L, "l2c_runWalk",           l2c_runWalk );
	// Target,Attack
	lua_register( L, "l2c_action",            l2c_action );
	lua_register( L, "l2c_attack",            l2c_attack );
	lua_register( L, "l2c_targetByName",      l2c_targetByName );
	lua_register( L, "l2c_targetCancel",      l2c_targetCancel );
	lua_register( L, "l2c_getTarget",         l2c_getTarget );
	// Items
	lua_register( L, "l2c_autoSoulshot",      l2c_autoSoulshot );
	lua_register( L, "l2c_getItemCount",      l2c_getItemCount );
	lua_register( L, "l2c_useItem",           l2c_useItem );
	lua_register( L, "l2c_useItemByObjectId", l2c_useItemByObjectId );
	lua_register( L, "l2c_getPaperdollItem",  l2c_getPaperdollItem );
	lua_register( L, "l2c_getUserItems",      l2c_getUserItems );
	// Skills
	lua_register( L, "l2c_useSkill",          l2c_useSkill );
	lua_register( L, "l2c_getSkillLevel",     l2c_getSkillLevel );
	lua_register( L, "l2c_getSkillReuseLeft", l2c_getSkillReuseLeft );
	lua_register( L, "l2c_isCastingNow",      l2c_isCastingNow );
	lua_register( L, "l2c_getUserSkills",     l2c_getUserSkills );
	// Buffs
	lua_register( L, "l2c_getBuffs",          l2c_getBuffs );
	lua_register( L, "l2c_buffCancel",        l2c_buffCancel );
	// world information
	lua_register( L, "l2c_getVisibleChars",   l2c_getVisibleChars );
	lua_register( L, "l2c_getVisibleMobs",    l2c_getVisibleMobs );
	lua_register( L, "l2c_getVisibleNpcs",    l2c_getVisibleNpcs );
	lua_register( L, "l2c_getVisibleItems",   l2c_getVisibleItems );
	// individual world object information, based on objectID
	lua_register( L, "l2c_getCharObjectIdByName",   l2c_getCharObjectIdByName );
	lua_register( L, "l2c_getNpcObjectIdByName",    l2c_getNpcObjectIdByName );
	lua_register( L, "l2c_getObjectInfoByObjectId", l2c_getObjectInfoByObjectId );
	lua_register( L, "l2c_isCharDead",              l2c_isCharDead );
	// Party info
	lua_register( L, "l2c_getParty",            l2c_getParty );
	lua_register( L, "l2c_getPartyMemberBuffs", l2c_getPartyMemberBuffs );
	lua_register( L, "l2c_partySendInvite",     l2c_partySendInvite );
	lua_register( L, "l2c_partyLeave",          l2c_partyLeave );
	lua_register( L, "l2c_partyKickMember",     l2c_partyKickMember );
	lua_register( L, "l2c_partyChangeLeader",   l2c_partyChangeLeader );
	// bot config, not works here, just null-functions
	lua_register( L, "l2c_isCombatEnabled", l2c_isCombatEnabled );
	lua_register( L, "l2c_combatEnable",    l2c_combatEnable );
	lua_register( L, "l2ccfg_getInt",       l2ccfg_getInt );
	lua_register( L, "l2ccfg_setInt",       l2ccfg_setInt );
	lua_register( L, "l2ccfg_getStr",       l2ccfg_getStr );
	lua_register( L, "l2ccfg_setStr",       l2ccfg_setStr );
	lua_register( L, "l2ccfg_getAllConfig", l2ccfg_getAllConfig );
	// hack packet send func
	lua_register( L, "l2c_sendPacketHex", l2c_sendPacketHex );
}

