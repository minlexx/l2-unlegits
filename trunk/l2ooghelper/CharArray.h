#ifndef CHARLIST_H_
#define CHARLIST_H_

class CharArray
{
public:
	CharArray() { Init(); }
	~CharArray() { Free(); }
public:
	static const int CHARARRAY_MAX_CHARS = 512;
	L2Player *chars_array[CHARARRAY_MAX_CHARS];
public:
	void Init();
	void Free();
public:
	void Lock();
	void Unlock();
public:
	int  AddCharInfo( L2Player *cha );
	void UpdateCharInfo( int idx, L2Player *cha );
	//void DeleteCharByObjectID( unsigned int objectID );
	void DeleteCharByArrayIdx( int idx );
	void DeleteAll();
public:
	int FindCharByObjectID( unsigned int objectID );
	int FindFreeIndex();
public:
	void SetUpdateCommand( HWND hWnd, UINT uMsg );
	unsigned int GetCount();
public:
	void Display( FILE *f );
	void DumpToFile( const char *filename );
	void DisplayToConsole();
protected:
	void PostUpdateMessage();
protected:
	CRITICAL_SECTION charArray_cs;
	HWND charArray_updateHWND;
	UINT charArray_updateMSG;
	UINT charArray_count;
};

#endif /* CHARLIST_H_ */
