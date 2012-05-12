#include "stdafx.h"
#include "../Logger.h"
#include "../lua/lua.hpp"
#include "../ScriptEngine.h"
#include "../GameClient.h"
#include "../PacketInjector.h"
extern class GameClient *g_game_client;

// l2c_useSkill( int skillID, [bool force = false] )
int l2c_useSkill( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	if( !g_game_client ) return 0;
	unsigned int skillID = (unsigned int)lua_tonumber( L, 1 );
	unsigned int force = 0;
	if( nArgs >= 2 ) force = lua_toboolean( L, 2 );
	PGen_RequestMagicSkillUse( skillID, force, 0 );
	return 0;
}

// int l2c_getSkillLevel( int skillID )
int l2c_getSkillLevel( lua_State *L )
{
	if( !g_game_client ) { lua_pushnumber( L, 0 ); return 1; }
	if( lua_gettop( L ) < 1 ) { lua_pushnumber( L, 0 ); return 1; }
	unsigned int skillID = (unsigned int)lua_tonumber( L, 1 );
	UserSkill skill;
	g_game_client->ai.skills.getSkillInfoBySkillId( skillID, &skill );
	lua_pushnumber( L, (lua_Number)(skill.level) );
	return 1;
}

// int l2c_getSkillReuseLeft( int skillID )
int l2c_getSkillReuseLeft( lua_State *L )
{
	if( !g_game_client ) { lua_pushnumber( L, 0 ); return 1; }
	if( lua_gettop( L ) < 1 ) { lua_pushnumber( L, 0 ); return 1; }
	unsigned int skillID = (unsigned int)lua_tonumber( L, 1 );
	UserSkills *sk = &(g_game_client->ai.skills);
	UserSkill skill;
	sk->getSkillInfoBySkillId( skillID, &skill );
	lua_pushnumber( L, (lua_Number)skill.coolTimeRemaining );
	return 1;
}

// boolean l2c_isCastingNow()
int l2c_isCastingNow( lua_State *L )
{
	if( !g_game_client ) { lua_pushboolean( L, 0 ); return 1; }
	UserSkills *sk = &(g_game_client->ai.skills);
	int b = 0;
	if( sk->isCasting() ) b = 1;
	lua_pushboolean( L, b );
	return 1;
}


// table l2c_getUserSkills()
/*
local sk = l2c_getUserSkills();
local sks = #sk;
l2h_print( sks, " skills found.\n" );
local i = 0;
for i=1,sks do
	l2h_print( i, ": ", sk[i].skillId, " - ", sk[i].skillName, "\n" );
end
*/
int l2c_getUserSkills( lua_State *L )
{
	if( !g_game_client ) { lua_pushboolean( L, 0 ); return 1; }
	UserSkills *sk = &(g_game_client->ai.skills);
	//
	int tableIndex = 1;
	lua_createtable( L, 0, 0 );
	int i = 0;
	char skillName[255] = {0};
	for( i=0; i<UserSkills::USERSKILL_MAX_SKILLS; i++ )
	{
		if( sk->skill[i].isUnused() ) continue;
		sk->skill[i].getSkillName( skillName, sizeof(skillName)-1 );
		lua_pushnumber( L, tableIndex );
		lua_createtable( L, 0, 0 );
		//
		lua_pushstring( L, "skillId" );
		lua_pushinteger( L, sk->skill[i].skillID );
		lua_settable( L, -3 );
		//
		lua_pushstring( L, "skillName" );
		lua_pushstring( L, skillName );
		lua_settable( L, -3 );
		//
		lua_pushstring( L, "isActive" );
		lua_pushboolean( L, sk->skill[i].isPassive ? 0 : 1 );
		lua_settable( L, -3 );
		//
		lua_pushstring( L, "level" );
		lua_pushinteger( L, sk->skill[i].level );
		lua_settable( L, -3 );
		//
		lua_settable( L, -3 );
		tableIndex++;
	}
	//
	return 1;
}
