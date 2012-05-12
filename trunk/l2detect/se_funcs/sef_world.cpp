#include "stdafx.h"
#include "../Logger.h"
#include "../lua/lua.hpp"
#include "../ScriptEngine.h"
#include "../GameClient.h"
#include "../PacketInjector.h"
extern class GameClient *g_game_client;

int l2c_getVisibleChars( lua_State *L )
{
	if( !g_game_client ) return 0;
	CharArray_Lock();
	unsigned int charCount = CharArray_GetCount();
	lua_createtable( L, 0, 0 );
	unsigned int i;
	unsigned int nPassed = 0;
	unsigned int tableIndex = 1;
	for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
	{
		if( chars_array[i]->isUnused() ) continue;
		nPassed++;
		//
		lua_pushnumber( L, tableIndex );
		lua_pushnumber( L, chars_array[i]->objectID );
		lua_settable( L, -3 );
		tableIndex++;
		//
		if( nPassed >= charCount ) break;
	}
	CharArray_Unlock();
	return 1;
}

int l2c_getVisibleMobs( lua_State *L )
{
	if( !g_game_client ) return 0;
	NpcArray_Lock();
	lua_createtable( L, 0, 0 );
	unsigned int i;
	unsigned int tableIndex = 1;
	for( i=0; i<NPCA_MAX_NPCS; i++ )
	{
		if( npc_array[i]->isUnused() ) continue;
		if( !npc_array[i]->isAttackable ) continue;
		//
		lua_pushnumber( L, tableIndex );
		lua_pushnumber( L, npc_array[i]->objectID );
		lua_settable( L, -3 );
		tableIndex++;
	}
	NpcArray_Unlock();
	return 1;
}

int l2c_getVisibleNpcs( lua_State *L )
{
	if( !g_game_client ) return 0;
	NpcArray_Lock();
	lua_createtable( L, 0, 0 );
	unsigned int i;
	unsigned int tableIndex = 1;
	for( i=0; i<NPCA_MAX_NPCS; i++ )
	{
		if( npc_array[i]->isUnused() ) continue;
		if( npc_array[i]->isAttackable ) continue;
		//
		lua_pushnumber( L, tableIndex );
		lua_pushnumber( L, npc_array[i]->objectID );
		lua_settable( L, -3 );
		tableIndex++;
	}
	NpcArray_Unlock();
	return 1;
}

int l2c_getVisibleItems( lua_State *L )
{
	if( !g_game_client ) return 0;
	GIArray *a = GIArray::getInstance();
	a->Lock();
	unsigned int count = a->getCount();
	lua_createtable( L, 0, 0 );
	unsigned int i;
	unsigned int nPassed = 0;
	unsigned int tableIndex = 1;
	for( i=0; i<GIArray::GA_MAX_ITEMS; i++ )
	{
		if( a->gi_array[i]->isUnused() ) continue;
		nPassed++;
		//
		lua_pushnumber( L, tableIndex );
		lua_pushnumber( L, a->gi_array[i]->objectID );
		lua_settable( L, -3 );
		tableIndex++;
		//
		if( nPassed >= count ) break;
	}
	a->Unlock();
	return 1;
}

int l2c_getCharObjectIdByName( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) { lua_pushnil( L ); return 1; }
	if( !g_game_client ) { lua_pushnil( L ); return 1; }
	const char *aname = lua_tolstring( L, 1, NULL );
	if( !aname ) { lua_pushnil( L ); return 1; }
	wchar_t wname[128] = {0};
	memset( wname, 0, sizeof(wname) );
	MultiByteToWideChar( CP_ACP, 0, aname, -1, wname, 127 );
	unsigned int nPassed = 0, i = 0, nChars = CharArray_GetCount();
	if( nChars > 0 )
	{
		CharArray_Lock();
		for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
		{
			if( chars_array[i]->isUnused() ) continue;
			nPassed++;
			if( _wcsicmp( wname, chars_array[i]->charName ) == 0 )
			{
				CharArray_Unlock();
				lua_pushnumber( L, chars_array[i]->objectID );
				return 1;
			}
			if( nPassed >= nChars ) break;
		}
		CharArray_Unlock();
	}
	lua_pushnil( L );
	return 1;
}

int l2c_getNpcObjectIdByName( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) { lua_pushnil( L ); return 1; }
	if( !g_game_client ) { lua_pushnil( L ); return 1; }
	const char *aname = lua_tolstring( L, 1, NULL );
	if( !aname ) { lua_pushnil( L ); return 1; }
	wchar_t wname[128] = {0};
	memset( wname, 0, sizeof(wname) );
	MultiByteToWideChar( CP_ACP, 0, aname, -1, wname, 127 );
	unsigned int i = 0;
	NpcArray_Lock();
	for( i=0; i<NPCA_MAX_NPCS; i++ )
	{
		if( npc_array[i]->isUnused() ) continue;
		if( _wcsicmp( wname, npc_array[i]->charName ) == 0 )
		{
			NpcArray_Unlock();
			lua_pushnumber( L, npc_array[i]->objectID );
			return 1;
		}
	}
	NpcArray_Unlock();
	lua_pushnil( L );
	return 1;
}


int l2c_getObjectInfoByObjectId( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) { lua_pushnil( L ); return 1; }
	if( !g_game_client ) { lua_pushnil( L ); return 1; }
	unsigned int objectID = (unsigned int)lua_tonumber( L, 1 );
	L2OBJECT_TYPE objType = L2OT_NONE;
	int idx = -1;
	if( WorldObjectTree_GetInfoByObjectID( objectID, &objType, &idx ) )
	{
		char ansi[256] = {0};
		switch( objType )
		{
		case L2OT_PC:
			{
				if( chars_array[idx] == NULL ) { lua_pushnil( L ); return 1; }
				CharArray_Lock();
				lua_createtable( L, 0, 0 );
				//
				lua_pushstring( L, "type" );
				lua_pushstring( L, "pc" );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "objectID" );
				lua_pushnumber( L, chars_array[idx]->objectID );
				lua_settable( L, -3 );
				// heading
				lua_pushstring( L, "heading" );
				lua_pushnumber( L, chars_array[idx]->heading );
				lua_settable( L, -3 );
				lua_pushstring( L, "targetObjectID" );
				lua_pushnumber( L, chars_array[idx]->targetObjectID );
				lua_settable( L, -3 );
				lua_pushstring( L, "pvpFlag" );
				lua_pushnumber( L, chars_array[idx]->pvpFlag );
				lua_settable( L, -3 );
				lua_pushstring( L, "karma" );
				lua_pushnumber( L, chars_array[idx]->karma );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "x" );
				lua_pushnumber( L, chars_array[idx]->x );
				lua_settable( L, -3 );
				lua_pushstring( L, "y" );
				lua_pushnumber( L, chars_array[idx]->y );
				lua_settable( L, -3 );
				lua_pushstring( L, "z" );
				lua_pushnumber( L, chars_array[idx]->z );
				lua_settable( L, -3 );
				lua_pushstring( L, "xDst" );
				lua_pushnumber( L, chars_array[idx]->xDst );
				lua_settable( L, -3 );
				lua_pushstring( L, "yDst" );
				lua_pushnumber( L, chars_array[idx]->yDst );
				lua_settable( L, -3 );
				lua_pushstring( L, "zDst" );
				lua_pushnumber( L, chars_array[idx]->zDst );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "name" );
				sprintf( ansi, "%S", chars_array[idx]->charName );
				lua_pushstring( L, ansi );
				lua_settable( L, -3 );
				lua_pushstring( L, "title" );
				sprintf( ansi, "%S", chars_array[idx]->charTitle );
				lua_pushstring( L, ansi );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "curHp" );
				lua_pushnumber( L, chars_array[idx]->curHp );
				lua_settable( L, -3 );
				lua_pushstring( L, "maxHp" );
				lua_pushnumber( L, chars_array[idx]->maxHp );
				lua_settable( L, -3 );
				lua_pushstring( L, "isAlikeDead" );
				lua_pushnumber( L, chars_array[idx]->isAlikeDead );
				lua_settable( L, -3 );
				// player specific
				lua_pushstring( L, "classID" );
				lua_pushnumber( L, chars_array[idx]->classID );
				lua_settable( L, -3 );
				CharArray_Unlock();
			} break;
		case L2OT_NPC:
			{
				if( npc_array[idx] == NULL ) { lua_pushnil( L ); return 1; }
				NpcArray_Lock();
				lua_createtable( L, 0, 0 );
				//
				lua_pushstring( L, "type" );
				if( npc_array[idx]->isAttackable ) lua_pushstring( L, "mob" );
				else lua_pushstring( L, "npc" );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "objectID" );
				lua_pushnumber( L, npc_array[idx]->objectID );
				lua_settable( L, -3 );
				// heading
				lua_pushstring( L, "heading" );
				lua_pushnumber( L, npc_array[idx]->heading );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "x" );
				lua_pushnumber( L, npc_array[idx]->x );
				lua_settable( L, -3 );
				lua_pushstring( L, "y" );
				lua_pushnumber( L, npc_array[idx]->y );
				lua_settable( L, -3 );
				lua_pushstring( L, "z" );
				lua_pushnumber( L, npc_array[idx]->z );
				lua_settable( L, -3 );
				lua_pushstring( L, "xDst" );
				lua_pushnumber( L, npc_array[idx]->xDst );
				lua_settable( L, -3 );
				lua_pushstring( L, "yDst" );
				lua_pushnumber( L, npc_array[idx]->yDst );
				lua_settable( L, -3 );
				lua_pushstring( L, "zDst" );
				lua_pushnumber( L, npc_array[idx]->zDst );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "name" );
				sprintf( ansi, "%S", npc_array[idx]->charName );
				lua_pushstring( L, ansi );
				lua_settable( L, -3 );
				lua_pushstring( L, "title" );
				sprintf( ansi, "%S", npc_array[idx]->charTitle );
				lua_pushstring( L, ansi );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "curHp" );
				lua_pushnumber( L, npc_array[idx]->curHp );
				lua_settable( L, -3 );
				lua_pushstring( L, "maxHp" );
				lua_pushnumber( L, npc_array[idx]->maxHp );
				lua_settable( L, -3 );
				lua_pushstring( L, "isAlikeDead" );
				lua_pushnumber( L, npc_array[idx]->isAlikeDead );
				lua_settable( L, -3 );
				// npc specific
				lua_pushstring( L, "templateID" );
				lua_pushnumber( L, npc_array[idx]->templateID );
				lua_settable( L, -3 );
				NpcArray_Unlock();
			} break;
		case L2OT_ITEM:
			{
				GIArray *a = GIArray::getInstance();
				if( a->gi_array[idx] == NULL ) { lua_pushnil( L ); return 1; }
				a->Lock();
				lua_createtable( L, 0, 0 );
				//
				lua_pushstring( L, "type" );
				lua_pushstring( L, "item" );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "objectID" );
				lua_pushnumber( L, a->gi_array[idx]->objectID );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "x" );
				lua_pushnumber( L, a->gi_array[idx]->x );
				lua_settable( L, -3 );
				lua_pushstring( L, "y" );
				lua_pushnumber( L, a->gi_array[idx]->y );
				lua_settable( L, -3 );
				lua_pushstring( L, "z" );
				lua_pushnumber( L, a->gi_array[idx]->z );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "name" );
				char itemName[512] = {0};
				L2Data_DB_GetItemNamePicByID( a->gi_array[idx]->itemID, itemName, NULL );
				lua_pushstring( L, itemName ); // at last
				lua_settable( L, -3 );
				// item specific
				lua_pushstring( L, "itemID" );
				lua_pushnumber( L, a->gi_array[idx]->itemID );
				lua_settable( L, -3 );
				lua_pushstring( L, "stackable" );
				lua_pushboolean( L, a->gi_array[idx]->stackable );
				lua_settable( L, -3 );
				lua_pushstring( L, "count" );
				lua_pushnumber( L, (lua_Number)a->gi_array[idx]->count );
				lua_settable( L, -3 );
				a->Unlock();
			} break;
		default: lua_pushnil( L ); break;
		}
	}
	else
	{
		lua_pushnil( L );
	}
	return 1;
}

// boolean l2c_isCharDead( int objectID )
int l2c_isCharDead( lua_State *L )
{
	// get number of arguments
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) { lua_pushnil( L ); return 1; }
	if( !g_game_client ) { lua_pushnil( L ); return 1; }
	// convert 1st argument to number
	unsigned int objectID = (unsigned int)lua_tonumber( L, 1 );
	L2OBJECT_TYPE objType = L2OT_NONE;
	int idx = -1;
	int return_value = 1; // default - dead
	// find objectID in world
	if( WorldObjectTree_GetInfoByObjectID( objectID, &objType, &idx ) )
	{
		switch( objType )
		{
		case L2OT_PC:
			{
				L2Player *pl = chars_array[idx];
				if( pl ) return_value = pl->isAlikeDead; // may be alive
			} break;
		case L2OT_NPC:
			{
				L2Npc *pnpc = npc_array[idx];
				if( pnpc ) return_value = pnpc->isAlikeDead; // may be alive
			} break;
		default: return_value = 1; break; // items are always "dead"
		}
	}
	lua_pushboolean( L, return_value );
	return 1;
}
