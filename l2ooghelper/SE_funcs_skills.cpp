#include "stdafx.h"
#include "Logger.h"
#include "SE_funcs.h"

// l2c_useSkill( int skillID )
int l2c_useSkill( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) return 0;
	unsigned int skillID = (unsigned int)lua_tonumber( L, 1 );
	unsigned int force = 0;
	if( nArgs >= 2 ) force = lua_toboolean( L, 2 );
	cl->game_SkillUse( skillID, force, 0 );
	return 0;
}

// int l2c_getSkillLevel( int skillID )
int l2c_getSkillLevel( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) { lua_pushnumber( L, 0 ); return 1; }
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) { lua_pushnumber( L, 0 ); return 1; }
	unsigned int skillID = (unsigned int)lua_tonumber( L, 1 );
	int skillLevel = cl->game_getSkillLevel( skillID );
	lua_pushnumber( L, skillLevel );
	return 1;
}

// int l2c_getSkillReuseLeft( int skillID )
int l2c_getSkillReuseLeft( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) { lua_pushnumber( L, 0 ); return 1; }
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) { lua_pushnumber( L, 0 ); return 1; }
	unsigned int skillID = (unsigned int)lua_tonumber( L, 1 );
	UserSkills *sk = cl->get_Skills();
	UserSkill skill;
	sk->getSkillInfoBySkillId( skillID, &skill );
	lua_pushnumber( L, skill.coolTimeRemaining );
	return 1;
}

// boolean l2c_isCastingNow()
int l2c_isCastingNow( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) { lua_pushboolean( L, 0 ); return 1; }
	UserSkills *sk = cl->get_Skills();
	int b = 0;
	if( sk->isCasting() ) b = 1;
	lua_pushboolean( L, b );
	return 1;
}
