#ifndef CHARLIST_H_
#define CHARLIST_H_

#define CHARARRAY_MAX_CHARS 1024

extern class L2Player *chars_array[CHARARRAY_MAX_CHARS];

void CharArray_Init();
void CharArray_Free();

void CharArray_Lock();
void CharArray_Unlock();

void CharArray_AddCharInfo( L2Player *cha );
void CharArray_DeleteCharByObjectID( unsigned int objectID );
void CharArray_DeleteCharByIdx( int idx );
void CharArray_DeleteAll();

int CharArray_FindCharByObjectID( unsigned int objectID );
int CharArray_FindFreeIndex();

//void CharArray_Display( FILE *f );
//void CharArray_SetUpdateCommand( HWND hWnd, UINT uMsg );
unsigned int CharArray_GetCount();

#endif /* CHARLIST_H_ */
