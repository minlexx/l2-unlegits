#include "stdafx.h"
#include "Logger.h"
#include "CharArray.h"
#include "WorldObjectTree.h"
#include "RadarDllWnd.h" // to post updates

class L2Player *chars_array[CHARARRAY_MAX_CHARS];
CRITICAL_SECTION charArray_cs;

//HWND charArray_updateHWND;
//UINT charArray_updateMSG;
UINT charArray_count;

void CharArray_Init()
{
	charArray_count = 0;
	//charArray_updateHWND = NULL;
	//charArray_updateMSG = 0;
	InitializeCriticalSection( &charArray_cs );
	EnterCriticalSection( &charArray_cs );
	int i;
	for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
	{
		chars_array[i] = new L2Player();
		chars_array[i]->setUnused();
	}
	LeaveCriticalSection( &charArray_cs );
}

void CharArray_Lock() { EnterCriticalSection( &charArray_cs ); }
void CharArray_Unlock() { LeaveCriticalSection( &charArray_cs ); }

//void CharArray_SetUpdateCommand( HWND hWnd, UINT uMsg )
//{
//	charArray_updateHWND = hWnd;
//	charArray_updateMSG = uMsg;
//}
//
//void CharArray_PostUpdateMessage()
//{
//	if( !charArray_updateHWND ) return;
//	if( charArray_updateMSG < WM_USER ) return;
//	PostMessage( charArray_updateHWND, charArray_updateMSG, 0, 0 );
//}

void CharArray_Free()
{
	int i;
	CharArray_Lock();
	for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
	{
		if( chars_array[i] ) delete chars_array[i];
		chars_array[i] = NULL;
	}
	CharArray_Unlock();
	charArray_count = 0;
}


void CharArray_AddCharInfo( L2Player *cha )
{
	if( !cha ) return;
	//log_error( LOG_DEBUG, "CharList_Add: [%S] (oid %u) %s %s %s  [%d,%d,%d -> %u]... ",
	//	name, oid,
	//	L2Data_getRace( race ), L2Data_getSex( sex ), L2Data_getClass( baseClass ),
	//	x,y,z, heading );
	CharArray_Lock();

	// first try to find existing char in world object tree
	int idx = -1;
	L2OBJECT_TYPE objType = L2OT_NONE;
	WorldObjectTree_GetInfoByObjectID( cha->objectID, &objType, &idx );
	if( (idx>=0) && (idx<CHARARRAY_MAX_CHARS) && (objType == L2OT_PC) )
	{
		// update info on existing
		if( !chars_array[idx] )
		{
			CharArray_Unlock();
			return;
		}
		memcpy( chars_array[idx], cha, sizeof(class L2Player) );
		//log_error( LOG_USERAI, "Char: Updated idx [%d] [%S]\n", idx, cha->name() );
		// update in radar window
		RadarWnd_UpdChar( cha->objectID );
	}
	else // add new
	{
		idx = CharArray_FindFreeIndex();
		if( idx >= 0 )
		{
			//chars_array[idx]->setFromOther( cha );
			memcpy( chars_array[idx], cha, sizeof(class L2Player) );
			charArray_count++;
			// add char to world object tree
			WorldObjectTree_AddObject( cha->objectID, L2OT_PC, idx );
			//log_error( LOG_USERAI, "Char: added new [%S] to idx[%d]\n", cha->name(), idx );
			// add to radar window
			RadarWnd_AddChar( cha->objectID );
		}
		else  log_error( LOG_ERROR, "Error adding character [%S] to chars array! idx = %d\n", cha->getName(), idx );
	}
	CharArray_Unlock();
}

void CharArray_DeleteCharByObjectID( unsigned int objectID )
{
	CharArray_Lock();
	BOOL bDelOK = 0;
	int idx = CharArray_FindCharByObjectID( objectID );
	if( (idx>=0) && (idx<CHARARRAY_MAX_CHARS) )
	{
		chars_array[idx]->setUnused();
		bDelOK = TRUE;
		charArray_count--;
	}
	CharArray_Unlock();
	if( bDelOK ) //CharArray_PostUpdateMessage();
		RadarWnd_DelChar( objectID );
}

void CharArray_DeleteCharByIdx( int idx )
{
	BOOL bDelOK = FALSE;
	CharArray_Lock();
	if( (idx>=0) && (idx<CHARARRAY_MAX_CHARS) )
	{
		if( chars_array[idx] )
		{
			unsigned int objectID = chars_array[idx]->objectID;
			//log_error( LOG_USERAI, "Char: Deleted [%S] from [%d]\n", chars_array[idx]->name(), idx );
			chars_array[idx]->setUnused();
			bDelOK = TRUE;
			charArray_count--;
			// delete from WorldObjectTree
			WorldObjectTree_DelObject( objectID );
			RadarWnd_DelChar( objectID );
		}
	}
	CharArray_Unlock();
	//if( bDelOK ) CharArray_PostUpdateMessage();
}

void CharArray_DeleteAll()
{
	CharArray_Lock();
	int i;
	for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
	{
		if( chars_array[i] ) chars_array[i]->setUnused();
	}
	charArray_count = 0;
	CharArray_Unlock();
}

/*void CharList_Display( FILE *f )
{
	if( !f || !bt_chars ) return;
	CharList_Lock();
	fprintf( f, "===== Chars list =====\n" );
	BinTreeNode *root = bt_chars->getRoot();
	if( !root ) fprintf( f, "Chars list empty!\n" );
	else CharList_displayNode( f, root );
	fprintf( f, "======================\n" );
	//bt_chars->printTree( f );
	CharList_Unlock();
}*/

unsigned int CharArray_GetCount() { return charArray_count; }

int CharArray_FindCharByObjectID( unsigned int objectID )
{
	int ret = -1;
	int i;
	CharArray_Lock();
	for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
	{
		if( chars_array[i] == NULL ) continue;
		if( chars_array[i]->objectID == objectID )
		{
			ret = i;
			break;
		}
	}
	CharArray_Unlock();
	return ret;
}

int CharArray_FindFreeIndex()
{
	int i;
	CharArray_Lock();
	for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
	{
		if( chars_array[i] == NULL )
		{
			chars_array[i] = new L2Player(); // allocate new 
			chars_array[i]->setUnused(); // and set as unused
			CharArray_Unlock();
			return i;
		}
		if( chars_array[i]->isUnused() )
		{
			CharArray_Unlock(); // just return index...
			return i;
		}
	}
	CharArray_Unlock();
	return -1;
}
