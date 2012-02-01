#ifndef CLANLIST_H_
#define CLANLIST_H_

#include "ClanTreeNode.h"

class ClanList
{
public:
	ClanList() { bt_clans = NULL; Init(); }
	~ClanList() { Free(); }
public:
	void Init();
	void Free();
public:
	void Lock();
	void Unlock();
public:
	void Add( unsigned int clanID, const wchar_t *clanName, const wchar_t *allyName );
	void Delete( unsigned int clanID );
	void DeleteAll();
public:
	unsigned int GetCount() { return clanList_count; }
	//void *GetClansArray();
	//void FreeClansArray( void *p );
public:
	bool GetClanNameByID( unsigned int clanID, TCHAR *out );
public:
	void Display( FILE *f );
	void DisplayConsole();
	void SetUpdateCommand( HWND hWnd, UINT uMsg );
protected:
	void PostUpdate();
protected:
	BinTree *bt_clans;
	HWND clanList_updateHWND;
	UINT clanList_updateMSG;
	CRITICAL_SECTION clanList_cs;
	UINT clanList_count;
};

#endif /* CLANLIST_H_ */
