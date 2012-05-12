#pragma once

// called by ScriptEngine on init()
void SE_funcs_register( lua_State *L );

// "system" functions
int sys_should_exit( lua_State *L );
int sys_register_onChat( lua_State *L );
//
int l2h_print( lua_State *L );
int l2h_console_enable( lua_State *L );
int l2h_delay( lua_State *L );
int l2h_soundAlert( lua_State *L );
int l2h_time( lua_State *L );
int l2h_timeMsec( lua_State *L );

// User info
int l2c_is_INGAME( lua_State *L );
int l2c_getHPMPCPWeight( lua_State *L );
int l2c_getStats( lua_State *L );
// Chatting
int l2c_say( lua_State *L );
int l2c_sayPm( lua_State *L );
int l2c_npcDlg( lua_State *L );
int l2c_npcDlgClear( lua_State *L );
int l2c_npcDlgExists( lua_State *L );
// Moving
int l2c_getPos( lua_State *L );
int l2c_moveTo( lua_State *L );
int l2c_moveToDelta( lua_State *L );
int l2c_getSitRun( lua_State *L );
int l2c_sitStand( lua_State *L );
int l2c_runWalk( lua_State *L );
// Target,Attack
int l2c_action( lua_State *L );
int l2c_attack( lua_State *L );
int l2c_targetByName( lua_State *L );
int l2c_targetCancel( lua_State *L );
int l2c_getTarget( lua_State *L );
// Items
int l2c_autoSoulshot( lua_State *L );
int l2c_getItemCount( lua_State *L );
int l2c_useItem( lua_State *L );
int l2c_useItemByObjectId( lua_State *L );
int l2c_getPaperdollItem( lua_State *L );
int l2c_getUserItems( lua_State *L );
// Skills
int l2c_useSkill( lua_State *L );
int l2c_getSkillLevel( lua_State *L );
int l2c_getSkillReuseLeft( lua_State *L );
int l2c_isCastingNow( lua_State *L );
int l2c_getUserSkills( lua_State *L );
// Buffs
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
// Party info
int l2c_getParty( lua_State *L );
int l2c_getPartyMemberBuffs( lua_State *L );
int l2c_partySendInvite( lua_State *L );
int l2c_partyLeave( lua_State *L );
int l2c_partyKickMember( lua_State *L );
int l2c_partyChangeLeader( lua_State *L );
// bot config, not works here, just null-functions
int l2c_isCombatEnabled( lua_State *L );
int l2c_combatEnable( lua_State *L );
int l2ccfg_getInt( lua_State *L );
int l2ccfg_setInt( lua_State *L );
int l2ccfg_getStr( lua_State *L );
int l2ccfg_setStr( lua_State *L );
int l2ccfg_getAllConfig( lua_State *L );
// hack packet send func
int l2c_sendPacketHex( lua_State *L );

