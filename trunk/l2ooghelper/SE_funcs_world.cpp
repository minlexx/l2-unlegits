#include "stdafx.h"
#include "Logger.h"
#include "SE_funcs.h"
#include "DBLayer.h"

int l2c_getVisibleChars( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) return 0;
	CharArray *ca = cl->get_WorldChars();
	ca->Lock();
	unsigned int charCount = ca->GetCount();
	lua_createtable( L, 0, 0 );
	unsigned int i;
	unsigned int nPassed = 0;
	unsigned int tableIndex = 1;
	for( i=0; i<CharArray::CHARARRAY_MAX_CHARS; i++ )
	{
		if( ca->chars_array[i]->isUnused() ) continue;
		nPassed++;
		//
		lua_pushnumber( L, tableIndex );
		lua_pushnumber( L, ca->chars_array[i]->objectID );
		lua_settable( L, -3 );
		tableIndex++;
		//
		if( nPassed >= charCount ) break;
	}
	ca->Unlock();
	return 1;
}

int l2c_getVisibleMobs( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) return 0;
	NpcArray *na = cl->get_WorldNpcs();
	na->Lock();
	unsigned int count = na->getCount();
	lua_createtable( L, 0, 0 );
	unsigned int i;
	unsigned int nPassed = 0;
	unsigned int tableIndex = 1;
	for( i=0; i<NpcArray::NPCA_MAX_NPCS; i++ )
	{
		if( na->npcs_array[i]->isUnused() ) continue;
		nPassed++;
		if( !na->npcs_array[i]->isAttackable ) continue;
		//
		lua_pushnumber( L, tableIndex );
		lua_pushnumber( L, na->npcs_array[i]->objectID );
		lua_settable( L, -3 );
		tableIndex++;
		//
		if( nPassed >= count ) break;
	}
	na->Unlock();
	return 1;
}

int l2c_getVisibleNpcs( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) return 0;
	NpcArray *na = cl->get_WorldNpcs();
	na->Lock();
	unsigned int count = na->getCount();
	lua_createtable( L, 0, 0 );
	unsigned int i;
	unsigned int nPassed = 0;
	unsigned int tableIndex = 1;
	for( i=0; i<NpcArray::NPCA_MAX_NPCS; i++ )
	{
		if( na->npcs_array[i]->isUnused() ) continue;
		nPassed++;
		if( na->npcs_array[i]->isAttackable ) continue;
		//
		lua_pushnumber( L, tableIndex );
		lua_pushnumber( L, na->npcs_array[i]->objectID );
		lua_settable( L, -3 );
		tableIndex++;
		//
		if( nPassed >= count ) break;
	}
	na->Unlock();
	return 1;
}

int l2c_getVisibleItems( lua_State *L )
{
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) return 0;
	GIArray *a = cl->get_WorldItems();
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
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) { lua_pushnil( L ); return 1; }
	const char *aname = lua_tolstring( L, 1, NULL );
	if( !aname ) { lua_pushnil( L ); return 1; }
	wchar_t wname[128] = {0};
	memset( wname, 0, sizeof(wname) );
	MultiByteToWideChar( CP_ACP, 0, aname, -1, wname, 127 );
	CharArray *ca = cl->get_WorldChars();
	unsigned int nPassed = 0, i = 0, nChars = ca->GetCount();
	if( nChars > 0 )
	{
		ca->Lock();
		for( i=0; i<CharArray::CHARARRAY_MAX_CHARS; i++ )
		{
			if( ca->chars_array[i]->isUnused() ) continue;
			nPassed++;
			if( _wcsicmp( wname, ca->chars_array[i]->charName ) == 0 )
			{
				ca->Unlock();
				lua_pushnumber( L, ca->chars_array[i]->objectID );
				return 1;
			}
			if( nPassed >= nChars ) break;
		}
		ca->Unlock();
	}
	lua_pushnil( L );
	return 1;
}

int l2c_getNpcObjectIdByName( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) { lua_pushnil( L ); return 1; }
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) { lua_pushnil( L ); return 1; }
	const char *aname = lua_tolstring( L, 1, NULL );
	if( !aname ) { lua_pushnil( L ); return 1; }
	wchar_t wname[128] = {0};
	memset( wname, 0, sizeof(wname) );
	MultiByteToWideChar( CP_ACP, 0, aname, -1, wname, 127 );
	NpcArray *na = cl->get_WorldNpcs();
	unsigned int nPassed = 0, i = 0, nChars = na->getCount();
	if( nChars > 0 )
	{
		na->Lock();
		for( i=0; i<NpcArray::NPCA_MAX_NPCS; i++ )
		{
			if( na->npcs_array[i]->isUnused() ) continue;
			nPassed++;
			if( _wcsicmp( wname, na->npcs_array[i]->charName ) == 0 )
			{
				na->Unlock();
				lua_pushnumber( L, na->npcs_array[i]->objectID );
				return 1;
			}
			if( nPassed >= nChars ) break;
		}
		na->Unlock();
	}
	lua_pushnil( L );
	return 1;
}


int l2c_getObjectInfoByObjectId( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) { lua_pushnil( L ); return 1; }
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) { lua_pushnil( L ); return 1; }
	unsigned int objectID = (unsigned int)lua_tonumber( L, 1 );
	WorldObjectTree *wt = cl->get_WorldObjectTree();
	WorldObjectTreeNode wotNode;
	if( wt->GetInfoByObjectID( objectID, &wotNode ) )
	{
		int idx = wotNode.getArrayIdx();
		char ansi[256] = {0};
		switch( wotNode.getObjectType() )
		{
		case L2OT_PC:
			{
				CharArray *ca = cl->get_WorldChars();
				if( ca->chars_array[idx] == NULL ) { lua_pushnil( L ); return 1; }
				ca->Lock();
				lua_createtable( L, 0, 0 );
				//
				lua_pushstring( L, "type" );
				lua_pushstring( L, "pc" );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "objectID" );
				lua_pushnumber( L, ca->chars_array[idx]->objectID );
				lua_settable( L, -3 );
				// heading
				lua_pushstring( L, "heading" );
				lua_pushnumber( L, ca->chars_array[idx]->heading );
				lua_settable( L, -3 );
				lua_pushstring( L, "targetObjectID" );
				lua_pushnumber( L, ca->chars_array[idx]->targetObjectID );
				lua_settable( L, -3 );
				lua_pushstring( L, "pvpFlag" );
				lua_pushnumber( L, ca->chars_array[idx]->pvpFlag );
				lua_settable( L, -3 );
				lua_pushstring( L, "karma" );
				lua_pushnumber( L, ca->chars_array[idx]->karma );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "x" );
				lua_pushnumber( L, ca->chars_array[idx]->x );
				lua_settable( L, -3 );
				lua_pushstring( L, "y" );
				lua_pushnumber( L, ca->chars_array[idx]->y );
				lua_settable( L, -3 );
				lua_pushstring( L, "z" );
				lua_pushnumber( L, ca->chars_array[idx]->z );
				lua_settable( L, -3 );
				lua_pushstring( L, "xDst" );
				lua_pushnumber( L, ca->chars_array[idx]->xDst );
				lua_settable( L, -3 );
				lua_pushstring( L, "yDst" );
				lua_pushnumber( L, ca->chars_array[idx]->yDst );
				lua_settable( L, -3 );
				lua_pushstring( L, "zDst" );
				lua_pushnumber( L, ca->chars_array[idx]->zDst );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "name" );
				sprintf( ansi, "%S", ca->chars_array[idx]->charName );
				lua_pushstring( L, ansi );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "curHp" );
				lua_pushnumber( L, ca->chars_array[idx]->curHp );
				lua_settable( L, -3 );
				lua_pushstring( L, "maxHp" );
				lua_pushnumber( L, ca->chars_array[idx]->maxHp );
				lua_settable( L, -3 );
				lua_pushstring( L, "isAlikeDead" );
				lua_pushnumber( L, ca->chars_array[idx]->isAlikeDead );
				lua_settable( L, -3 );
				// player specific
				lua_pushstring( L, "classID" );
				lua_pushnumber( L, ca->chars_array[idx]->classID );
				lua_settable( L, -3 );
				ca->Unlock();
			} break;
		case L2OT_NPC:
			{
				NpcArray *na = cl->get_WorldNpcs();
				if( na->npcs_array[idx] == NULL ) { lua_pushnil( L ); return 1; }
				na->Lock();
				lua_createtable( L, 0, 0 );
				//
				lua_pushstring( L, "type" );
				if( na->npcs_array[idx]->isAttackable ) lua_pushstring( L, "mob" );
				else lua_pushstring( L, "npc" );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "objectID" );
				lua_pushnumber( L, na->npcs_array[idx]->objectID );
				lua_settable( L, -3 );
				// heading
				lua_pushstring( L, "heading" );
				lua_pushnumber( L, na->npcs_array[idx]->heading );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "x" );
				lua_pushnumber( L, na->npcs_array[idx]->x );
				lua_settable( L, -3 );
				lua_pushstring( L, "y" );
				lua_pushnumber( L, na->npcs_array[idx]->y );
				lua_settable( L, -3 );
				lua_pushstring( L, "z" );
				lua_pushnumber( L, na->npcs_array[idx]->z );
				lua_settable( L, -3 );
				lua_pushstring( L, "xDst" );
				lua_pushnumber( L, na->npcs_array[idx]->xDst );
				lua_settable( L, -3 );
				lua_pushstring( L, "yDst" );
				lua_pushnumber( L, na->npcs_array[idx]->yDst );
				lua_settable( L, -3 );
				lua_pushstring( L, "zDst" );
				lua_pushnumber( L, na->npcs_array[idx]->zDst );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "name" );
				sprintf( ansi, "%S", na->npcs_array[idx]->charName );
				lua_pushstring( L, ansi );
				lua_settable( L, -3 );
				//
				lua_pushstring( L, "curHp" );
				lua_pushnumber( L, na->npcs_array[idx]->curHp );
				lua_settable( L, -3 );
				lua_pushstring( L, "maxHp" );
				lua_pushnumber( L, na->npcs_array[idx]->maxHp );
				lua_settable( L, -3 );
				lua_pushstring( L, "isAlikeDead" );
				lua_pushnumber( L, na->npcs_array[idx]->isAlikeDead );
				lua_settable( L, -3 );
				// npc specific
				lua_pushstring( L, "templateID" );
				lua_pushnumber( L, na->npcs_array[idx]->templateID );
				lua_settable( L, -3 );
				na->Unlock();
			} break;
		case L2OT_ITEM:
			{
				GIArray *a = cl->get_WorldItems();
				if( !a->gi_array[idx] ) { lua_pushnil( L ); return 1; }
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
				// need to query cached DB; and convert from unicode to ansi >_<
				char aitemName[256] = {0};
				wchar_t witemName[256] = {0};
				DBLayer_getItemName( a->gi_array[idx]->itemID, witemName, 255 );
				WideCharToMultiByte( CP_ACP, 0, witemName, -1, aitemName, 255, NULL, NULL );
				lua_pushstring( L, aitemName ); // at last
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
	// get client interface
	IL2Client *cl = L2C_func_getClient( L );
	if( !cl ) { lua_pushnil( L ); return 1; }
	// convert 1st argument to number
	unsigned int objectID = (unsigned int)lua_tonumber( L, 1 );
	WorldObjectTree *wt = cl->get_WorldObjectTree();
	WorldObjectTreeNode wotNode;
	int return_value = 1; // default - dead
	// find objectID in world
	if( wt->GetInfoByObjectID( objectID, &wotNode ) )
	{
		switch( wotNode.getObjectType() )
		{
		case L2OT_PC:
			{
				CharArray *ca = cl->get_WorldChars();
				L2Player *pl = ca->chars_array[wotNode.getArrayIdx()];
				if( pl ) return_value = pl->isAlikeDead;
			} break;
		case L2OT_NPC:
			{
				NpcArray *na = cl->get_WorldNpcs();
				L2Npc *pnpc = na->npcs_array[wotNode.getArrayIdx()];
				if( pnpc ) return_value = pnpc->isAlikeDead;
			} break;
		default: return_value = 1; break; // items are always "dead"
		}
	}
	lua_pushboolean( L, return_value );
	return 1;
}
