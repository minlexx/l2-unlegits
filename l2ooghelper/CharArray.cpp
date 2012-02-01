#include "stdafx.h"
#include "Logger.h"
#include "CharArray.h"

void CharArray::Init()
{
	charArray_count = 0;
	charArray_updateHWND = NULL;
	charArray_updateMSG = 0;
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

void CharArray::Lock() { EnterCriticalSection( &charArray_cs ); }
void CharArray::Unlock() { LeaveCriticalSection( &charArray_cs ); }

void CharArray::SetUpdateCommand( HWND hWnd, UINT uMsg )
{
	charArray_updateHWND = hWnd;
	charArray_updateMSG = uMsg;
}

void CharArray::PostUpdateMessage()
{
	if( !charArray_updateHWND ) return;
	if( charArray_updateMSG < WM_USER ) return;
	PostMessage( charArray_updateHWND, charArray_updateMSG, 0, 0 );
}

void CharArray::Free()
{
	int i;
	Lock();
	for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
	{
		if( chars_array[i] ) delete chars_array[i];
		chars_array[i] = NULL;
	}
	Unlock();
	charArray_count = 0;
}


int CharArray::AddCharInfo( L2Player *cha )
{
	if( !cha ) return -1;
	Lock();
	int idx = FindFreeIndex();
	if( idx >= 0 )
	{
		memcpy( chars_array[idx], cha, sizeof(class L2Player) );
		charArray_count++;
		//log_error( LOG_USERAI, "Char: added new [%S] to idx[%d]\n", cha->charName, idx );
	}
	else
	{
		log_error( LOG_ERROR, "Error adding character [%S] to chars array! idx = %d\n", cha->getName(), idx );
	}
	Unlock();
	PostUpdateMessage();
	return idx;
}

void CharArray::UpdateCharInfo( int idx, L2Player *cha )
{
	Lock();
	// update info on existing
	if( !chars_array[idx] )
	{
		log_error( LOG_ERROR, "CharArray_AddCharInfo(): "
			"char oid [%S] [%u] found in L2World, but its index [%d] is empty!\n",
			cha->charName, cha->objectID, idx );
		Unlock();
		return;
	}
	// save old move vectors (since by default unused CharInfo is not moving)
	int xDst = chars_array[idx]->xDst;
	int yDst = chars_array[idx]->yDst;
	int zDst = chars_array[idx]->zDst;
	//
	memcpy( chars_array[idx], cha, sizeof(class L2Player) );
	// restore old move vectors (since by default unused CharInfo is not moving)
	chars_array[idx]->xDst = xDst;
	chars_array[idx]->yDst = yDst;
	chars_array[idx]->zDst = zDst;
	//
	Unlock();
	PostUpdateMessage();
	//log_error( LOG_USERAI, "Char: Updated idx [%d] [%S]\n", idx, cha->name() );
}

/*void CharArray::DeleteCharByObjectID( unsigned int objectID )
{
	Lock();
	BOOL bDelOK = 0;
	int idx = FindCharByObjectID( objectID );
	if( (idx>=0) && (idx<CHARARRAY_MAX_CHARS) )
	{
		chars_array[idx]->setUnused();
		bDelOK = TRUE;
		charArray_count--;
	}
	Unlock();
	if( bDelOK ) PostUpdateMessage();
}*/

void CharArray::DeleteCharByArrayIdx( int idx )
{
	BOOL bDelOK = FALSE;
	Lock();
	if( (idx>=0) && (idx<CHARARRAY_MAX_CHARS) )
	{
		if( chars_array[idx] )
		{
			//unsigned int objectID = chars_array[idx]->objectID;
			//log_error( LOG_USERAI, "Char: Deleted [%S] from [%d]\n", chars_array[idx]->charName, idx );
			chars_array[idx]->setUnused();
			bDelOK = TRUE;
			charArray_count--;
		}
		else log_error( LOG_ERROR, "Char: index[%d] is NULL while deleting char ny idx\n", idx );
	}
	Unlock();
	if( bDelOK ) PostUpdateMessage();
}

void CharArray::DeleteAll()
{
	Lock();
	int i;
	for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
	{
		if( chars_array[i] ) chars_array[i]->setUnused();
	}
	charArray_count = 0;
	Unlock();
	PostUpdateMessage();
}

unsigned int CharArray::GetCount() { return charArray_count; }

int CharArray::FindCharByObjectID( unsigned int objectID )
{
	int ret = -1;
	int i;
	Lock();
	for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
	{
		if( chars_array[i] == NULL ) continue;
		if( chars_array[i]->objectID == objectID )
		{
			ret = i;
			break;
		}
	}
	Unlock();
	return ret;
}

int CharArray::FindFreeIndex()
{
	int i;
	Lock();
	for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
	{
		if( chars_array[i] == NULL )
		{
			chars_array[i] = new L2Player(); // allocate new
			chars_array[i]->setUnused(); // and set as unused
			Unlock();
			return i;
		}
		if( chars_array[i]->isUnused() )
		{
			Unlock(); // just return index...
			return i;
		}
	}
	Unlock();
	log_error( LOG_ERROR, "CharArray_FindFreeIndex(): cannot find free index! limit is %d\n",
		CHARARRAY_MAX_CHARS );
	return -1;
}

void CharArray::Display( FILE *f )
{
	if( !f ) return;
	Lock();
	int i;
	for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
	{
		if( chars_array[i]->isUnused() ) continue;
		fprintf( f, "%d: %S\n", i, chars_array[i]->charName );
	}
	Unlock();
}

void CharArray::DumpToFile( const char *filename )
{
	FILE *f = fopen( filename, "wt" );
	if( !f ) return;
	Display( f );
	fclose( f );
}

void CharArray::DisplayToConsole()
{
	int i = 0;
	unsigned int nCharsDisplayed = 0;
	log_error( LOG_USERAI, "=== Chars: %d ===\n", this->charArray_count );
	if( this->charArray_count > 0 )
	{
		Lock();
		for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
		{
			if( chars_array[i]->isUnused() )
				log_error( LOG_USERAI, "Chars: %2d: <unused>\n", i );
			else
			{
				log_error( LOG_USERAI, "Chars: %2d: [%S] [%S] [%s]\n", i,
					chars_array[i]->charName, chars_array[i]->charTitle,
					L2Data_getClass( chars_array[i]->classID ) );
				nCharsDisplayed++;
			}
			if( nCharsDisplayed >= this->charArray_count ) break;
		}
		Unlock();
	}
	log_error( LOG_USERAI, "=== Chars End ===\n" );
}
