#include "stdafx.h"
#include "DBLayer.h"
#include "Logger.h"

// limits

//sqlite> select max(id) from items:  20062
//sqlite> select max(id) from skills: 26073
//sqlite> select max(id) from mobs:   36599

const unsigned int dbl_max_npc_id   = 40000;
const unsigned int dbl_max_skill_id = 27000;
const unsigned int dbl_max_item_id  = 25000;

TCHAR **dbl_npc_names = NULL;
TCHAR **dbl_npc_titles = NULL;
TCHAR **dbl_skill_names = NULL;
TCHAR **dbl_item_names = NULL;

int dbl_cache_hits = 0;
int dbl_cache_misses = 0;
int dbl_npcs_cached = 0;
int dbl_skills_cached = 0;
int dbl_items_cached = 0;

void DBLayer_init()
{
	log_error( LOG_DEBUG, "DB Layer: init database...\n" );
	//
	char errmsg[256] = {0};
	bool open_ok = L2Data_DB_Init( errmsg,
		".\\data\\L2Detect_items.sqlite3db",
		".\\data\\L2Detect_mobs.sqlite3db",
		".\\data\\L2Detect_skills.sqlite3db" );
	if( errmsg[0] )
	{
		log_error( LOG_ERROR, "L2Data_DB_Init(): says: %s\n", errmsg );
		MessageBoxA( NULL, errmsg, "L2Data_DB_Init():", MB_ICONWARNING );
	}
	if( !open_ok )
	{
		log_error( LOG_ERROR, "DB Layer: database open error!\n" );
		return;
	}
	//
	log_error( LOG_OK, "DB Layer: database opened\n" );
	//
	dbl_cache_hits = 0;
	dbl_cache_misses = 0;
	dbl_npcs_cached = 0;
	dbl_skills_cached = 0;
	dbl_items_cached = 0;
	// allocate mem
	// NULL
	dbl_npc_names = NULL;
	dbl_npc_titles = NULL;
	dbl_skill_names = NULL;
	dbl_item_names = NULL;
	// malloc npc names
	dbl_npc_names  = (TCHAR **)malloc( sizeof(TCHAR *) * dbl_max_npc_id );
	if( !dbl_npc_names )
	{
		log_error( LOG_ERROR, "DB Layer: cannot allocate mem (1)\n" );
		return;
	}
	// malloc npc titles
	dbl_npc_titles = (TCHAR **)malloc( sizeof(TCHAR *) * dbl_max_npc_id );
	if( !dbl_npc_titles )
	{
		free( dbl_npc_names );
		dbl_npc_names = NULL;
		log_error( LOG_ERROR, "DB Layer: cannot allocate mem (2)\n" );
		return;
	}
	// malloc skill names
	dbl_skill_names = (TCHAR **)malloc( sizeof(TCHAR *) * dbl_max_skill_id );
	if( !dbl_skill_names )
	{
		free( dbl_npc_names );
		free( dbl_npc_titles );
		dbl_npc_names = NULL;
		dbl_npc_titles = NULL;
		log_error( LOG_ERROR, "DB Layer: cannot allocate mem (3)\n" );
		return;
	}
	// malloc item names
	dbl_item_names = (TCHAR **)malloc( sizeof(TCHAR *) * dbl_max_item_id );
	if( !dbl_item_names )
	{
		free( dbl_npc_names );
		free( dbl_npc_titles );
		free( dbl_skill_names );
		dbl_npc_names = NULL;
		dbl_npc_titles = NULL;
		dbl_skill_names = NULL;
		log_error( LOG_ERROR, "DB Layer: cannot allocate mem (4)\n" );
		return;
	}
	//
	log_error( LOG_DEBUG, "DB cache created\n" );
	int i;
	for( i=0; i<dbl_max_npc_id; i++ )
	{
		dbl_npc_names[i] = NULL;
		dbl_npc_titles[i] = NULL;
	}
	for( i=0; i<dbl_max_item_id; i++ ) dbl_item_names[i] = NULL;
	for( i=0; i<dbl_max_skill_id; i++ ) dbl_skill_names[i] = NULL;
	//
	log_error( LOG_DEBUG, "DB Layer: init database end.\n" );
}

void DBLayer_free()
{
	log_error( LOG_DEBUG, "DB Layer: closing database..\n" );
	int i = 0;
	int nfreed = 0;
	// free npc names
	if( dbl_npc_names )
	{
		nfreed = 0;
		for( i=0; i<dbl_max_npc_id; i++ )
		{
			if( dbl_npc_names[i] )
			{
				free( dbl_npc_names[i] );
				dbl_npc_names[i] = NULL;
				nfreed++;
			}
		}
		free( dbl_npc_names );
		dbl_npc_names = NULL;
		log_error( LOG_OK, "DB Layer: freed %d cached npc names\n", nfreed );
	}
	// free npc titles
	if( dbl_npc_titles )
	{
		nfreed = 0;
		for( i=0; i<dbl_max_npc_id; i++ )
		{
			if( dbl_npc_titles[i] )
			{
				free( dbl_npc_titles[i] );
				dbl_npc_titles[i] = NULL;
				nfreed++;
			}
		}
		free( dbl_npc_titles );
		dbl_npc_titles = NULL;
		log_error( LOG_OK, "DB Layer: freed %d cached npc titles\n", nfreed );
	}
	// free skill names
	if( dbl_skill_names )
	{
		nfreed = 0;
		for( i=0; i<dbl_max_skill_id; i++ )
		{
			if( dbl_skill_names[i] )
			{
				free( dbl_skill_names[i] );
				dbl_skill_names[i] = NULL;
				nfreed++;
			}
		}
		free( dbl_skill_names );
		dbl_skill_names = NULL;
		log_error( LOG_OK, "DB Layer: freed %d cached skill names\n", nfreed );
	}
	// free item names
	if( dbl_item_names )
	{
		nfreed = 0;
		for( i=0; i<dbl_max_item_id; i++ )
		{
			if( dbl_item_names[i] )
			{
				free( dbl_item_names[i] );
				dbl_item_names[i] = NULL;
				nfreed++;
			}
		}
		free( dbl_item_names );
		dbl_item_names = NULL;
		log_error( LOG_OK, "DB Layer: freed %d cached item names\n", nfreed );
	}
	// close db files
	L2Data_DB_Free();
	log_error( LOG_DEBUG, "DB Layer: database files closed.\n" );
}

#if defined(UNICODE) || defined(_UNICODE)
// needed as analog to _strdup/_wcsdup but converting from ansi string to unicode
wchar_t *ansi_to_unicode_dup( const char *ansi )
{
	if( !ansi ) return NULL;
	size_t len = strlen( ansi );
	wchar_t *wstr = (wchar_t *)malloc( len*2 + 2 );
	if( !wstr ) return NULL;
	swprintf( wstr, len*2 + 2, L"%S", ansi );
	wstr[len] = 0;
	return wstr;
}
#endif

void DBLayer_getItemName( unsigned int itemID, TCHAR *out, size_t maxCount )
{
	if( itemID >= dbl_max_item_id )
	{
		log_error( LOG_ERROR, "DBLayer_getItemName(): item ID %u is above max limit %u\n",
			itemID, dbl_max_item_id );
		return;
	}
	if( dbl_item_names )
	{
		// check cache
		if( dbl_item_names[itemID] )
		{
			// found in cache
			_tcsncpy( out, dbl_item_names[itemID], maxCount );
			out[maxCount-1] = 0;
			dbl_cache_hits++;
			return;
		}
		// not in cache :( request DB
		char aname[256] = {0};
		if( L2Data_DB_GetItemNamePicByID( itemID, aname, NULL ) )
		{
			dbl_cache_misses++;
			dbl_items_cached++;
#if defined(UNICODE) || defined(_UNICODE)
			dbl_item_names[itemID] = ansi_to_unicode_dup( aname );
#else
			dbl_item_names[itemID] = _strdup( aname );
#endif
			_tcsncpy( out, dbl_item_names[itemID], maxCount );
			out[maxCount-1] = 0;
		}
		else log_error( LOG_ERROR, "DB: item %u not in DB!\n", itemID );
	}
	else log_error( LOG_ERROR, "DB Layer: no mem allocated for item names, DB init error?\n" );
}

void DBLayer_getSkillName( unsigned int skillID, TCHAR *out, size_t maxCount )
{
	if( skillID >= dbl_max_skill_id )
	{
		log_error( LOG_ERROR, "DBLayer_getSkillName(): skill ID %u is above max limit %u\n",
			skillID, dbl_max_skill_id );
		return;
	}
	if( dbl_skill_names )
	{
		// check cache
		if( dbl_skill_names[skillID] )
		{
			// found in cache
			_tcsncpy( out, dbl_skill_names[skillID], maxCount );
			out[maxCount-1] = 0;
			dbl_cache_hits++;
			return;
		}
		// not in cache :( request DB
		char aname[256] = {0};
		if( L2Data_DB_GetSkillNameByID( skillID, aname ) )
		{
			dbl_cache_misses++;
			dbl_skills_cached++;
#if defined(UNICODE) || defined(_UNICODE)
			dbl_skill_names[skillID] = ansi_to_unicode_dup( aname );
#else
			dbl_skill_names[skillID] = _strdup( aname );
#endif
			_tcsncpy( out, dbl_skill_names[skillID], maxCount );
			out[maxCount-1] = 0;
		}
		else log_error( LOG_ERROR, "DB: skill %u not in DB!\n", skillID );
	}
	else log_error( LOG_ERROR, "DB Layer: no mem allocated for skill names, DB init error?\n" );
}

void DBLayer_getNpcNameTitle(
	unsigned int npcID,
	TCHAR *npcName,  size_t npcNameMaxCount,
	TCHAR *npcTitle, size_t npcTitleMaxCount )
{
	if( npcID >= dbl_max_npc_id )
	{
		log_error( LOG_ERROR, "DBLayer_getNpcNameTitle(): npc ID %u is above max limit %u\n",
			npcID, dbl_max_npc_id );
		return;
	}
	if( dbl_npc_names && dbl_npc_titles )
	{
		// check cache
		if( dbl_npc_names[npcID] )
		{
			// found in cache
			_tcsncpy( npcName, dbl_npc_names[npcID], npcNameMaxCount );
			npcName[npcNameMaxCount-1] = 0;
			npcTitle[0] = 0;
			// npc title can be NULL
			if( dbl_npc_titles[npcID] )
			{
				_tcsncpy( npcTitle, dbl_npc_titles[npcID], npcTitleMaxCount );
				npcTitle[npcTitleMaxCount-1] = 0;
			}
			dbl_cache_hits++;
			return;
		}
		// not in cache :( request DB
		char aname[256] = {0};
		char atitle[256] = {0};
		if( L2Data_DB_GetNPCNameTitleByID( npcID, aname, atitle ) )
		{
			dbl_cache_misses++;
			dbl_npcs_cached++;
#if defined(UNICODE) || defined(_UNICODE)
			dbl_npc_names[npcID] = ansi_to_unicode_dup( aname );
			if( atitle[0] )
				dbl_npc_titles[npcID] = ansi_to_unicode_dup( atitle );
#else
			dbl_npc_names[npcID] = _strdup( aname );
			if( atitle[0] )
				dbl_npc_titles[npcID] = _strdup( atitle );
#endif
			// out
			_tcsncpy( npcName, dbl_npc_names[npcID], npcNameMaxCount );
			npcName[npcNameMaxCount-1] = 0;
			npcTitle[0] = 0;
			// npc title can be NULL
			if( dbl_npc_titles[npcID] )
			{
				_tcsncpy( npcTitle, dbl_npc_titles[npcID], npcTitleMaxCount );
				npcTitle[npcTitleMaxCount-1] = 0;
			}
		}
		else log_error( LOG_ERROR, "DB: NPC %u not in DB!\n", npcID );
	}
	else log_error( LOG_ERROR, "DB Layer: no mem allocated for "
		"NPC names & titles, DB init error?\n" );
}

void DBLayer_printStats()
{
	log_error_np( LOG_OK, "[DB] ===== Stats info: ======\n" );
	log_error_np( LOG_OK, "[DB] cached (NPCs, Skills, Items) : %d, %d, %d\n",
		dbl_npcs_cached, dbl_skills_cached, dbl_items_cached );
	log_error_np( LOG_OK, "[DB] Cache: MISS / HIT :  %d / %d\n",
		dbl_cache_misses, dbl_cache_hits );
	log_error_np( LOG_OK, "[DB] ===== Stats info end =====\n" );
}
