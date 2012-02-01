#ifndef CLANLIST_H_
#define CLANLIST_H_

void ClanList_Init();
void ClanList_Free();

void ClanList_Add( unsigned int clanID, const wchar_t *clanName, const wchar_t *allyName );
void ClanList_Delete( unsigned int clanID );
void ClanList_DeleteAll();

void ClanList_Display( FILE *f );

void ClanList_SetUpdateCommand( HWND hWnd, UINT uMsg );

unsigned int ClanList_GetCount();

void *ClanList_GetClansArray();
void ClanList_FreeClansArray( void *p );

void ClanList_GetClanNameByID( unsigned int clanID, TCHAR *out );

#endif /* CLANLIST_H_ */
