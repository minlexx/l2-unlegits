#include "stdafx.h"
#include "L2Data.h"
#include "sqlite3.h"

//char l2data_db_filename
int      l2data_sqlite3_was_init    = 0;
sqlite3 *l2data_sqlite3_conn_items  = NULL;
sqlite3 *l2data_sqlite3_conn_npcs   = NULL;
sqlite3 *l2data_sqlite3_conn_skills = NULL;

// synchronisation
#ifdef L2PACKETS_WINDOWS
CRITICAL_SECTION l2data_db_cs;
#endif


bool L2Data_DB_Init( char *outErrMsg,
					const char *items_db_filename,
					const char *mobs_db_filename,
					const char *skills_db_filename )
{
	if( outErrMsg ) outErrMsg[0] = 0;
	if( !l2data_sqlite3_was_init )
	{
		// test for threadsafe
		if( sqlite3_threadsafe() == 0 )
		{
			fprintf( stderr, "WARNING: sqlite3 is not configured to use thread-safe operations!\n" );
			if( outErrMsg ) strcat( outErrMsg, "sqlite3 is not configured threadsafe! " );
		}
		//
		int r = 0;
		const char *errmsg = NULL;
		// items
		r = sqlite3_open( items_db_filename, &l2data_sqlite3_conn_items );
		if( r != SQLITE_OK )
		{
			errmsg = sqlite3_errmsg( l2data_sqlite3_conn_items );
			if( outErrMsg && errmsg ) strcat( outErrMsg, errmsg );
			sqlite3_close( l2data_sqlite3_conn_items );
			l2data_sqlite3_conn_items = NULL;
			return false;
		}
		// npcs
		r = sqlite3_open( mobs_db_filename, &l2data_sqlite3_conn_npcs );
		if( r != SQLITE_OK )
		{
			errmsg = sqlite3_errmsg( l2data_sqlite3_conn_npcs );
			if( outErrMsg && errmsg ) strcat( outErrMsg, errmsg );
			sqlite3_close( l2data_sqlite3_conn_items );
			l2data_sqlite3_conn_items = NULL;
			sqlite3_close( l2data_sqlite3_conn_npcs );
			l2data_sqlite3_conn_npcs = NULL;
			return false;
		}
		// skills
		r = sqlite3_open( skills_db_filename, &l2data_sqlite3_conn_skills );
		if( r != SQLITE_OK )
		{
			errmsg = sqlite3_errmsg( l2data_sqlite3_conn_skills );
			if( outErrMsg && errmsg ) strcat( outErrMsg, errmsg );
			sqlite3_close( l2data_sqlite3_conn_items );
			l2data_sqlite3_conn_items = NULL;
			sqlite3_close( l2data_sqlite3_conn_npcs );
			l2data_sqlite3_conn_npcs = NULL;
			sqlite3_close( l2data_sqlite3_conn_skills );
			l2data_sqlite3_conn_skills = NULL;
			return false;
		}
		//
		l2data_sqlite3_was_init = 1;
#ifdef L2PACKETS_WINDOWS
		InitializeCriticalSection( &l2data_db_cs );
#endif
		return true;
	}
	return true;
}

#ifdef L2PACKETS_WINDOWS
void L2Data_DB_Lock() { EnterCriticalSection( &l2data_db_cs ); }
void L2Data_DB_UnLock() { LeaveCriticalSection( &l2data_db_cs ); }
#endif

#ifdef L2PACKETS_LINUX
// TODO: lock/unlock semaphore (mutex?)
// FIXED: (?) uses SQLITE_THREADSAFE definition to sqlite3 library
void L2Data_DB_Lock()
{
}

void L2Data_DB_UnLock()
{
}
#endif

void L2Data_DB_Free()
{
	if( l2data_sqlite3_was_init )
	{
		L2Data_DB_Lock();
		sqlite3_close( l2data_sqlite3_conn_items );
		l2data_sqlite3_conn_items = NULL;
		sqlite3_close( l2data_sqlite3_conn_npcs );
		l2data_sqlite3_conn_npcs = NULL;
		sqlite3_close( l2data_sqlite3_conn_skills );
		l2data_sqlite3_conn_skills = NULL;
		l2data_sqlite3_was_init = 0;
		L2Data_DB_UnLock();
#ifdef L2PACKETS_WINDOWS
		DeleteCriticalSection( &l2data_db_cs );
#endif
#ifdef L2PACKETS_LINUX
		// TODO: linux: delete sync object
#endif
	}
}

typedef struct st_L2DATA_2PARAM
{
	char *param1;
	char *param2;
} L2DATA_2PARAM, *PL2DATA_2PARAM;

int L2Data_sqlite_callback_2param( void *arg, int argc, char **argv, char **argvCol )
{
	argvCol = NULL; // unreferenced formal parameter
	PL2DATA_2PARAM pst = (PL2DATA_2PARAM)arg;
	if( !pst ) return 0;
	if( !argv ) return 0;
	if( argc == 1 )
	{
		if( pst->param1 ) pst->param1[0] = 0;
		if( argv[0] && pst->param1 ) strcpy( pst->param1, argv[0] );
	}
	else if( argc == 2 )
	{
		if( pst->param1 ) pst->param1[0] = 0;
		if( pst->param2 ) pst->param2[0] = 0;
		if( argv[0] && pst->param1 ) strcpy( pst->param1, argv[0] );
		if( argv[1] && pst->param2 ) strcpy( pst->param2, argv[1] );
	}
	return 0;
}

// title can be NULL
bool L2Data_DB_GetNPCNameTitleByID( unsigned int npcID, char *name, char *title )
{
	if( !l2data_sqlite3_conn_npcs ) return false;
	L2Data_DB_Lock(); // lock
	char *errmsg = NULL;
	char query[128];
	L2DATA_2PARAM st;
	st.param1 = name;
	st.param2 = title;
	sprintf( query, "SELECT name,title FROM mobs WHERE id=%u", npcID );
	int r = sqlite3_exec( l2data_sqlite3_conn_npcs, query, L2Data_sqlite_callback_2param, (void *)&st, &errmsg );
	if( errmsg )
	{
#if defined(_DEBUG) && defined(L2PACKETS_WINDOWS)
		MessageBoxA( NULL, errmsg, "sqlite3 error (get NPC by ID)", MB_ICONSTOP );
#endif
#if defined(_DEBUG) && defined(L2PACKETS_LINUX)
		fprintf( stderr, "sqlite3 error (get NPC by ID): %s\n", errmsg );
#endif
		sqlite3_free( errmsg );
	}
	L2Data_DB_UnLock(); // unlock
	return (r == SQLITE_OK);
}

// desc can be NULL
bool L2Data_DB_GetItemNamePicByID( unsigned int itemID, char *name, char *pic )
{
	if( !l2data_sqlite3_conn_items ) return false;
	L2Data_DB_Lock(); // lock
	char *errmsg = NULL;
	char query[128];
	L2DATA_2PARAM st;
	st.param1 = name;
	st.param2 = pic;
	sprintf( query, "SELECT name,pic FROM items WHERE id=%u", itemID );
	int r = sqlite3_exec( l2data_sqlite3_conn_items, query, L2Data_sqlite_callback_2param, (void *)&st, &errmsg );
	if( errmsg )
	{
#if defined(_DEBUG) && defined(L2PACKETS_WINDOWS)
		MessageBoxA( NULL, errmsg, "sqlite3 error (get item by id)", MB_ICONSTOP );
#endif
#if defined(_DEBUG) && defined(L2PACKETS_LINUX)
		fprintf( stderr, "sqlite3 error (get item by ID): %s\n", errmsg );
#endif
		sqlite3_free( errmsg );
	}
	L2Data_DB_UnLock(); // unlock
	return (r == SQLITE_OK);
}

bool L2Data_DB_GetSkillNameByID( unsigned int skillID, char *name )
{
	if( !l2data_sqlite3_conn_skills ) return false;
	L2Data_DB_Lock(); // lock
	char *errmsg = NULL;
	char query[128];
	L2DATA_2PARAM st;
	st.param1 = name;
	st.param2 = NULL;
	sprintf( query, "SELECT name FROM skills WHERE id=%u", skillID );
	int r = sqlite3_exec( l2data_sqlite3_conn_skills, query, L2Data_sqlite_callback_2param, (void *)&st, &errmsg );
	if( errmsg )
	{
#if defined(_DEBUG) && defined(L2PACKETS_WINDOWS)
		MessageBoxA( NULL, errmsg, "sqlite3 error (get skill by ID)", MB_ICONSTOP );
#endif
#if defined(_DEBUG) && defined(L2PACKETS_LINUX)
		fprintf( stderr, "sqlite3 error (get skill by ID): %s\n", errmsg );
#endif
		sqlite3_free( errmsg );
	}
	L2Data_DB_UnLock(); // unlock
	return (r == SQLITE_OK);
}


//#define L2DATA_RESULT_MAX_ROWS 64

int L2Data_sqlite_result_callback( void *pvResult, int colCount, char **colData, char **colName )
{
	colName = NULL; // unreferenced formal parameter
	L2Data_DB_Result *pResult = (L2Data_DB_Result *)pvResult;
	if( !pResult ) return 0;
	if( pResult->rowCount == 0 ) // current row number is 0, func called first time
	{
		pResult->colCount = colCount; // set column count in result set
		// allocate row pointers array (maximum rows count limited :( )
		pResult->rows = (char ***)malloc( pResult->inMaxRowsToGet * sizeof(char **) );
		if( !pResult->rows ) return 0; // malloc() failed!
		int i;
		for( i=0; i<pResult->inMaxRowsToGet; i++ ) pResult->rows[i] = NULL;
	}
	if( pResult->rowCount >= pResult->inMaxRowsToGet ) return 0; // maximum reached, no more place
	// allocate space for current row (colCount columns)
	pResult->rows[ pResult->rowCount ] = (char **)malloc( colCount * sizeof( char* ) );
	if( !pResult->rows[ pResult->rowCount ] ) return 0; // malloc failed! :(
	// copy all coumns data to current row
	int iCol;
	for( iCol = 0; iCol < colCount; iCol++ )
	{
		if( colData[iCol] != NULL )
#ifdef L2PACKETS_WINDOWS
			pResult->rows[ pResult->rowCount ][ iCol ] = _strdup( colData[iCol] ); // copy string to result set
#else // write own strdup() :)
			pResult->rows[ pResult->rowCount ][ iCol ] = (char *)malloc( strlen( colData[iCol] ) + 1 );
			if( pResult->rows[ pResult->rowCount ][ iCol ] ) strcpy( pResult->rows[ pResult->rowCount ][ iCol ], colData[iCol] );
#endif
		else
			pResult->rows[ pResult->rowCount ][ iCol ] = NULL; // NULL column value
	}
	//
	pResult->rowCount++; // inc current row count in result set
	return 0;
}



bool L2Data_DB_Query_NPCs_ByName( const char *request, L2Data_DB_Result *pResult )
{
	if( !request || !pResult ) return false;
	L2Data_DB_Lock(); // lock
	char *errmsg = NULL;
	char query[256];
	sprintf( query, "SELECT id,name,title,level FROM mobs WHERE name LIKE '%s' ORDER BY name", request );
	// zero result set
	pResult->colCount = 0;
	pResult->rowCount = 0;
	pResult->rows = NULL;
	// run query
	int r = sqlite3_exec( l2data_sqlite3_conn_npcs, query,
		L2Data_sqlite_result_callback,
		(void *)pResult, // pass pointer to result set to callback
		&errmsg );
	if( errmsg )
	{
#if defined(_DEBUG) && defined(L2PACKETS_WINDOWS)
		MessageBoxA( NULL, errmsg, "sqlite3 error (L2Data_DB_Query_NPCs_ByName)", MB_ICONSTOP );
#endif
#if defined(_DEBUG) && defined(L2PACKETS_LINUX)
		fprintf( stderr, "sqlite3 error (L2Data_DB_Query_NPCs_ByName): %s\n", errmsg );
#endif
		sqlite3_free( errmsg );
	}
	L2Data_DB_UnLock(); // unlock
	return (r == SQLITE_OK);
}

bool L2Data_DB_Query_Items_ByName( const char *request, L2Data_DB_Result *pResult )
{
	if( !request || !pResult ) return false;
	L2Data_DB_Lock(); // lock
	char *errmsg = NULL;
	char query[256];
	sprintf( query, "SELECT id,name,pic FROM items WHERE name LIKE '%s' ORDER BY name", request );
	// zero result set
	pResult->colCount = 0;
	pResult->rowCount = 0;
	pResult->rows = NULL;
	// run query
	int r = sqlite3_exec( l2data_sqlite3_conn_items, query,
		L2Data_sqlite_result_callback,
		(void *)pResult, // pass pointer to result set to callback
		&errmsg );
	if( errmsg )
	{
#if defined(_DEBUG) && defined(L2PACKETS_WINDOWS)
		MessageBoxA( NULL, errmsg, "sqlite3 error (L2Data_DB_Query_Items_ByName)", MB_ICONSTOP );
#endif
#if defined(_DEBUG) && defined(L2PACKETS_LINUX)
		fprintf( stderr, "sqlite3 error (L2Data_DB_Query_Items_ByName): %s\n", errmsg );
#endif
		sqlite3_free( errmsg );
	}
	L2Data_DB_UnLock(); // unlock
	return (r == SQLITE_OK);
}

bool L2Data_DB_Query_Skills_ByName( const char *request, L2Data_DB_Result *pResult )
{
	if( !request || !pResult ) return false;
	L2Data_DB_Lock(); // lock
	char *errmsg = NULL;
	char query[256];
	sprintf( query, "SELECT id,name FROM skills WHERE name LIKE '%s' ORDER BY name", request );
	// zero result set
	pResult->colCount = 0;
	pResult->rowCount = 0;
	pResult->rows = NULL;
	// run query
	int r = sqlite3_exec( l2data_sqlite3_conn_skills, query,
		L2Data_sqlite_result_callback,
		(void *)pResult, // pass pointer to result set to callback
		&errmsg );
	if( errmsg )
	{
#if defined(_DEBUG) && defined(L2PACKETS_WINDOWS)
		MessageBoxA( NULL, errmsg, "sqlite3 error (L2Data_DB_Query_Skills_ByName)", MB_ICONSTOP );
#endif
#if defined(_DEBUG) && defined(L2PACKETS_LINUX)
		fprintf( stderr, "sqlite3 error (L2Data_DB_Query_Skills_ByName): %s\n", errmsg );
#endif
		sqlite3_free( errmsg );
	}
	L2Data_DB_UnLock(); // unlock
	return (r == SQLITE_OK);
}

void L2Data_DB_Free_Result( L2Data_DB_Result *pResult )
{
	if( !pResult ) return;
	int rowCount = pResult->rowCount;
	int colCount = pResult->colCount;
	int ir, ic;
	for( ir = 0; ir < rowCount; ir++ )
	{
		if( pResult->rows[ir] )
		{
			for( ic = 0; ic < colCount; ic++ )
			{
				if( pResult->rows[ir][ic] ) free( pResult->rows[ir][ic] );
			}
			free( pResult->rows[ir] );
		}
	}
	free( pResult->rows );
	pResult->rows = NULL;
	pResult->colCount = 0;
	pResult->rowCount = 0;
	pResult->inMaxRowsToGet = 0;
}
