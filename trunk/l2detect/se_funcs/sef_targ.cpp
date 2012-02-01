#include "stdafx.h"
#include "../Logger.h"
#include "../lua/lua.hpp"
#include "../ScriptEngine.h"
#include "../GameClient.h"
#include "../PacketInjector.h"
extern class GameClient *g_game_client;

int l2c_action( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	if( !g_game_client ) return 0;
	unsigned int objectID = (unsigned int)lua_tonumber( L, 1 );
	PGen_Action( objectID, g_game_client->ai.usr.x, g_game_client->ai.usr.y, g_game_client->ai.usr.z, 0 );
	return 0;
}

int l2c_attack( lua_State *L )
{
	UNREFERENCED_PARAMETER(L);
	if( !g_game_client ) return 0;
	if( g_game_client->ai.usr.targetObjectID == 0 ) return 0;
	PGen_AttackRequest(
		g_game_client->ai.usr.targetObjectID,
		g_game_client->ai.usr.x,
		g_game_client->ai.usr.y,
		g_game_client->ai.usr.z, 0 );
	return 0;
}

int l2c_targetByName( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	if( !g_game_client ) return 0;
	const char *aname = lua_tolstring( L, 1, NULL );
	wchar_t name[256];
	memset( name, 0, sizeof(name) );
	MultiByteToWideChar( CP_ACP, 0, aname, -1, name, 255 );
	UserState *usr = &(g_game_client->ai.usr);
	unsigned int nPassed = 0, i = 0, count = CharArray_GetCount();
	unsigned int objectID = 0;
	if( _wcsicmp( name, usr->charName ) == 0 )
	{
		objectID = usr->objectID;
	}
	if( objectID == 0 )
	{
		// try chars
		count = CharArray_GetCount();
		nPassed = 0;
		if( count > 0 )
		{
			CharArray_Lock();
			for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
			{
				if( chars_array[i]->isUnused() ) continue;
				nPassed++;
				if( _wcsicmp( name, chars_array[i]->charName ) == 0 )
				{
					objectID = chars_array[i]->objectID;
					break;
				}
				if( nPassed >= count ) break;
			}
			CharArray_Unlock();
		}
	}
	if( objectID == 0 )
	{
		// try NPCs
		NpcArray_Lock();
		for( i=0; i<NPCA_MAX_NPCS; i++ )
		{
			if( npc_array[i]->isUnused() ) continue;
			if( _wcsicmp( name, npc_array[i]->charName ) == 0 )
			{
				objectID = npc_array[i]->objectID;
				break;
			}
			NpcArray_Unlock();
		}
	}
	if( objectID == 0 ) return 0; // name not found
	// check current target
	if( g_game_client->ai.usr.targetObjectID == objectID ) return 0; // already on target
	PGen_Action( objectID, g_game_client->ai.usr.x, g_game_client->ai.usr.y, g_game_client->ai.usr.z, 0 );
	return 0;
}

int l2c_targetCancel( lua_State *L )
{
	UNREFERENCED_PARAMETER(L);
	if( !g_game_client ) return 0;
	PGen_RequestTargetCanceld( false );
	return 0;
}

int l2c_getTarget( lua_State *L )
{
	if( !g_game_client )
	{
		lua_pushnumber( L, 0 );
		lua_pushnumber( L, 0 );
		lua_pushnumber( L, 0 );
		return 3;
	}
	UserState *pusr = &(g_game_client->ai.usr);
	lua_pushnumber( L, pusr->targetObjectID );
	lua_pushnumber( L, pusr->targetCurHP );
	lua_pushnumber( L, pusr->targetMaxHP );
	return 3;
}
