#ifndef H_GI_ARRAY
#define H_GI_ARRAY
#include "GroundItem.h"

class GIArray
{
public:
	GIArray() { Init(); }
	~GIArray() { Free(); }
public:
	static const int GA_MAX_ITEMS = 512;
	GroundItem *gi_array[GA_MAX_ITEMS];
public:
	void Init();
	void Free();
public:
	int  AddGIInfo( GroundItem *pGIInfo );
	void UpdateGIInfo( int idx, GroundItem *pGIInfo );
	void DelGIByArrayIdx( int idx );
	void DeleteAll();
public:
	void Lock();
	void Unlock();
public:
	int FindGIByObjectID( unsigned int objectID );
	int FindFreeIndex();
public:
	void SetUpdateCommand( HWND hWnd, UINT uMsg );
	unsigned int getCount() const { return giArray_count; }
	void DisplayToConsole();
protected:
	void PostUpdateMessage();
protected:
	unsigned int     giArray_count;
	CRITICAL_SECTION cs_gi_array;
	HWND             giArray_updateHWND;
	UINT             giArray_updateMSG;
};

#endif
