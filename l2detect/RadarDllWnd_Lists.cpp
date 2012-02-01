#include "stdafx.h"
#include "Logger.h"
#include "RadarDllWnd.h"
#include "WorldObjectTree.h"
#include "CharArray.h"
#include "NpcArray.h"
#include "utils.h"
#include "GameClient.h"
extern GameClient *g_game_client;
#include "ConfigIni.h"
extern CConfig g_cfg;
#include "windowUtils.h"

void RadarWnd_AddChar( unsigned int objectId ) { PostMessage( g_radardll_hwnd, WMMY_ADD_OBJECT, MSG_PC, (LPARAM)objectId ); }

void RadarWnd_UpdChar( unsigned int objectId ) { PostMessage( g_radardll_hwnd, WMMY_UPDATE_OBJECT, MSG_PC, (LPARAM)objectId ); }

void RadarWnd_DelChar( unsigned int objectId ) { PostMessage( g_radardll_hwnd, WMMY_DEL_OBJECT, MSG_PC, (LPARAM)objectId ); }

void RadarWnd_AddNpc( unsigned int objectId ) { PostMessage( g_radardll_hwnd, WMMY_ADD_OBJECT, MSG_NPC, (LPARAM)objectId ); }

void RadarWnd_UpdNpc( unsigned int objectId ) { PostMessage( g_radardll_hwnd, WMMY_UPDATE_OBJECT, MSG_NPC, (LPARAM)objectId ); }

void RadarWnd_DelNpc( unsigned int objectId ) { PostMessage( g_radardll_hwnd, WMMY_DEL_OBJECT, MSG_NPC, (LPARAM)objectId ); }

// forwards
void RW_DeleteCharFromEnemiesList( unsigned int objectID );
void RW_UpdateCharInEnemiesList( L2Player *cha );

extern HWND g_rw_hWndCL;      // chars ListView window
extern HWND g_rw_hWndNPC;     // npcs ListView window
extern HWND g_rw_hWndEnemies; // enemies ListView window
extern bool g_radarwnd_npc_displayEnable;

LRESULT RW_OnAddObject( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(hWnd);
	// do not add npc if not displaying
	if( wParam == MSG_NPC && !g_radarwnd_npc_displayEnable ) return 0;

	unsigned int objectId = (unsigned int)lParam;
	L2OBJECT_TYPE objType = L2OT_NONE;
	int index = -1;
	if( !WorldObjectTree_GetInfoByObjectID( objectId, &objType, &index ) )
	{
		log_error( LOG_WARNING, "RW_OnAddObject(): cannot get info about object [%u] to add\n", objectId );
		return 0;
	}

	int user_z  = g_game_client->ai.usr.z;

	switch( wParam )
	{
	case MSG_PC:
		{
			if( objType != L2OT_PC )
			{
				log_error( LOG_WARNING, "RW_OnAddObject(): object [%u]: adding player, but in WOT incorrect type %d\n",
					objectId, (int)objType );
				return 0;
			}
			L2Player *cha = chars_array[index];
			if( !cha )
			{
				log_error( LOG_ERROR, "RW_OnAddObject(): player found in WOT, but is NULL in array? O_o\n" );
				return 0;
			}
			if( cha->isUnused() )
			{
				log_error( LOG_ERROR, "RW_OnAddObject(): player is unused in array! O_o\n" );
				return 0;
			}

			// ok, adding
			bool isEnemy = false;
			bool is2sidewar = false;
			TCHAR colText[128] = {0};
			TCHAR szBuf1[128] = {0};
			TCHAR szBuf2[128] = {0};

			// detect clan war
			utils_detectClanWar( cha->relation, &isEnemy, &is2sidewar );
			
			// name
			lstrcpy( colText, cha->getName() );
			int iImage = -1;
			if( isEnemy )
			{
				iImage = 0;
				if( !is2sidewar ) iImage = 3;
			}
			if( g_game_client->ai.party.isInParty( cha->objectID, NULL ) ) iImage = 1;
			if( cha->isAlikeDead ) iImage = 2;
			int added_item = -1;
			int added_item2 = -1;
			added_item = WULV_InsertItem2( g_rw_hWndCL, colText, 999, iImage, (LPARAM)cha->objectID );
			if( added_item == -1 ) return 0;
			if( isEnemy ) // add enemies also to enemies tab
				added_item2 = WULV_InsertItem2( g_rw_hWndEnemies, colText, 999, iImage, (LPARAM)cha->objectID );

			// baseClass
			szBuf2[0] = szBuf1[0] = 0;
			cha->getClassStr( szBuf1 );
			if( cha->classID != cha->baseClassID )
			{
				cha->getBaseClassStr( szBuf2 );
				wsprintfW( colText, L"%s / %s", szBuf1, szBuf2 );
			}
			else wsprintfW( colText, L"%s", szBuf1 );
			WULV_SetItem( g_rw_hWndCL, colText, added_item, 1 );
			if( isEnemy ) // add enemies info also to enemies tab
				WULV_SetItem( g_rw_hWndEnemies, colText, added_item2, 1 );

			// clan
			ClanList_GetClanNameByID( cha->clanID, colText ); // max 127 chars
			WULV_SetItem( g_rw_hWndCL, colText, added_item, 2 );
			if( isEnemy ) // add enemies info also to enemies tab
				WULV_SetItem( g_rw_hWndEnemies, colText, added_item2, 2 );

			// dZ
			wsprintf( colText, TEXT("%d"), (user_z - cha->z) );
			WULV_SetItem( g_rw_hWndCL, colText, added_item, 3 );
			if( isEnemy ) // add enemies info also to enemies tab
				WULV_SetItem( g_rw_hWndEnemies, colText, added_item2, 3 );

			// clanwar state
			colText[0] = 0;
			if( isEnemy && !is2sidewar ) lstrcpy( colText, TEXT("<") );
			if( isEnemy && is2sidewar ) lstrcpy( colText, TEXT("<>") );
			WULV_SetItem( g_rw_hWndCL, colText, added_item, 4 );
			if( isEnemy ) // add enemies info also to enemies tab
				WULV_SetItem( g_rw_hWndEnemies, colText, added_item2, 4 );
		} break;

	case MSG_NPC:
		{
			if( g_radarwnd_npc_displayEnable == 0 ) return 0; // NPC display disabled
			if( objType != L2OT_NPC )
			{
				log_error( LOG_WARNING, "RW_OnAddObject(): object [%u]: adding NPC, but in WOT incorrect type %d\n",
					objectId, (int)objType );
				return 0;
			}

			L2Npc *npc = npc_array[index];
			if( !npc )
			{
				log_error( LOG_ERROR, "RW_OnAddObject(): NPC found in WOT, but is NULL in array? O_o\n" );
				return 0;
			}
			if( npc->isUnused() )
			{
				log_error( LOG_ERROR, "RW_OnAddObject(): NPC is unused in array! O_o\n" );
				return 0;
			}

			TCHAR colText[128] = {0};
			wchar_t wName[128] = {0};  char aname[128] = {0};
			wchar_t wTitle[128] = {0}; char atitle[128] = {0};

			// NPC name
			aname[0] = atitle[0] = 0;
			wName[0] = wTitle[0] = 0;
			lstrcpy( colText, npc->getName() );
			if( colText[0] == 0 )
			{
				if( L2Data_DB_GetNPCNameTitleByID( npc->templateID, aname, atitle ) )
				{
					MultiByteToWideChar( CP_ACP, 0, aname, -1, wName, 127 );
					MultiByteToWideChar( CP_ACP, 0, atitle, -1, wTitle, 127 );
					npc->setName( wName );
					lstrcpy( colText, wName );
				}
			}
			int iImage = -1;
			if( npc->isAlikeDead ) iImage = 2;
			int added_item = WULV_InsertItem2( g_rw_hWndNPC, colText, 999, iImage, (LPARAM)npc->objectID );
			if( added_item == -1 ) return 0;

			// NPC title
			wcscpy( colText, npc->getTitle() );
			if( (colText[0] == 0) && (wTitle[0]) )
			{
				lstrcpy( colText, wTitle );
				npc->setTitle( wTitle );
			}
			WULV_SetItem( g_rw_hWndNPC, colText, added_item, 1 );

			// NPC dZ
			wsprintf( colText, TEXT("%d"), (user_z - npc->z) );
			WULV_SetItem( g_rw_hWndNPC, colText, added_item, 2 );

			// NPC templateID
			wsprintf( colText, TEXT("%d"), npc->templateID );
			WULV_SetItem( g_rw_hWndNPC, colText, added_item, 3 );
		} break;
	}
	return 0;
}






LRESULT RW_OnUpdObject( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(hWnd);
	// do not update npc if not displaying
	if( wParam == MSG_NPC && !g_radarwnd_npc_displayEnable ) return 0;

	unsigned int objectId = (unsigned int)lParam;
	L2OBJECT_TYPE objType = L2OT_NONE;
	int index = -1;
	if( !WorldObjectTree_GetInfoByObjectID( objectId, &objType, &index ) )
	{
		log_error( LOG_WARNING, "RW_OnUpdObject(): cannot get info about object [%u] to add\n", objectId );
		return 0;
	}

	int user_z  = g_game_client->ai.usr.z;

	HWND hWndLV = g_rw_hWndCL; // default: from char list
	if( wParam == MSG_NPC ) hWndLV = g_rw_hWndNPC;

	// find listview itemId to update
	int iItem = -1;
	LVITEM lvi;
	memset( &lvi, 0, sizeof(lvi) );
	int idx = -1;
	do
	{
		idx = ListView_GetNextItem( hWndLV, idx, LVNI_ALL );
		if( idx == -1 ) break;
		lvi.iItem = idx;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_PARAM;
		if( ListView_GetItem( hWndLV, &lvi ) )
		{
			if( ((unsigned int)lvi.lParam) == objectId )
			{
				iItem = lvi.iItem;
				break;
			}
		}
	} while( idx >= 0 );

	if( iItem == -1 )
	{
		// not found
		switch( wParam )
		{
		case MSG_PC:  log_error( LOG_WARNING, "RW_OnUpdObject(): cannot find player objectId [%u] in LV to update\n", objectId ); break;
		case MSG_NPC: log_error( LOG_WARNING, "RW_OnUpdObject(): cannot find NPC objectId [%u] in LV to update\n", objectId ); break;
		}
		return 0;
	}

	memset( &lvi, 0, sizeof(lvi) );
	lvi.iItem = iItem;
	lvi.lParam = (LPARAM)objectId;
	switch( wParam )
	{
	case MSG_PC:
		{
			if( objType != L2OT_PC )
			{
				log_error( LOG_WARNING, "RW_OnUpdObject(): object [%u]: update player, but in WOT incorrect type %d\n",
					objectId, (int)objType );
				return 0;
			}
			L2Player *cha = chars_array[index];
			if( !cha )
			{
				log_error( LOG_ERROR, "RW_OnUpdObject(): player found in WOT, but is NULL in array? O_o\n" );
				return 0;
			}
			if( cha->isUnused() )
			{
				log_error( LOG_ERROR, "RW_OnUpdObject(): player is unused in array! O_o\n" );
				return 0;
			}

			// ok, adding
			bool isEnemy = false;
			bool is2sidewar = false;
			TCHAR colText[128] = {0};
			TCHAR szBuf1[128] = {0};
			TCHAR szBuf2[128] = {0};
			memset( &lvi, 0, sizeof(lvi) );
			lvi.cchTextMax = 128;
			lvi.pszText = colText;

			// detect clan war
			utils_detectClanWar( cha->relation, &isEnemy, &is2sidewar );

			// name
			lstrcpy( colText, cha->getName() );
			lvi.pszText = colText;
			lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
			lvi.iItem = iItem;
			lvi.iSubItem = 0;
			lvi.lParam = cha->objectID;
			// image
			lvi.iImage = -1;
			if( isEnemy )
			{
				lvi.iImage = 0; // RED CIRC
				if( !is2sidewar ) lvi.iImage = 3; // HALF RED CIRC
			}
			if( g_game_client->ai.party.isInParty( cha->objectID, NULL ) ) lvi.iImage = 1;
			if( cha->isAlikeDead ) lvi.iImage = 2;
			SendMessage( g_rw_hWndCL, LVM_SETITEM, 0, (LPARAM)&lvi );
			lvi.iImage = 0;

			// baseClass
			szBuf2[0] = szBuf1[0] = 0;
			cha->getClassStr( szBuf1 );
			if( cha->classID != cha->baseClassID )
			{
				cha->getBaseClassStr( szBuf2 );
				wsprintfW( colText, L"%s / %s", szBuf1, szBuf2 );
			}
			else wsprintfW( colText, L"%s", szBuf1 );

			lvi.mask = LVIF_TEXT;
			lvi.iItem = iItem;
			lvi.iSubItem = 1;
			lvi.pszText = colText;
			SendMessage( g_rw_hWndCL, LVM_SETITEMTEXT, (WPARAM)iItem, (LPARAM)&lvi );

			// clan
			ClanList_GetClanNameByID( cha->clanID, colText ); // max 127 chars
			lvi.mask = LVIF_TEXT;
			lvi.iItem = iItem;
			lvi.iSubItem = 2;
			lvi.pszText = colText;
			SendMessage( g_rw_hWndCL, LVM_SETITEMTEXT, (WPARAM)iItem, (LPARAM)&lvi );

			// dZ
			wsprintf( colText, TEXT("%d"), (user_z - cha->z) );
			lvi.mask = LVIF_TEXT;
			lvi.iItem = iItem;
			lvi.iSubItem = 3;
			lvi.pszText = colText;
			SendMessage( g_rw_hWndCL, LVM_SETITEMTEXT, (WPARAM)iItem, (LPARAM)&lvi );

			// clanwar state
			lvi.mask = LVIF_TEXT;
			lvi.iItem = iItem;
			lvi.iSubItem = 4;
			lvi.pszText = colText;
			if( isEnemy )
			{
				colText[0] = 0;
				if( !is2sidewar ) lstrcpy( colText, TEXT("<") );
				if( is2sidewar ) lstrcpy( colText, TEXT("<>") );
			}
			SendMessage( g_rw_hWndCL, LVM_SETITEMTEXT, (WPARAM)iItem, (LPARAM)&lvi );

			// update enemies also in enemies list
			if( isEnemy ) RW_UpdateCharInEnemiesList( cha );
		} break;

	case MSG_NPC:
		{
			L2Npc *npc = npc_array[index];
			if( !npc )
			{
				log_error( LOG_ERROR, "RW_OnUpdObject(): NPC found in WOT, but is NULL in array? O_o\n" );
				return 0;
			}
			if( npc->isUnused() )
			{
				log_error( LOG_ERROR, "RW_OnUpdObject(): NPC is unused in array! O_o\n" );
				return 0;
			}

			TCHAR colText[128] = {0};
			wchar_t wName[128] = {0};  char aname[128] = {0};
			wchar_t wTitle[128] = {0}; char atitle[128] = {0};
			memset( &lvi, 0, sizeof(lvi) );
			lvi.cchTextMax = 128;
			lvi.pszText = colText;

			// NPC name
			aname[0] = atitle[0] = 0;
			wName[0] = wTitle[0] = 0;
			lstrcpy( colText, npc->getName() );
			if( colText[0] == 0 )
			{
				if( L2Data_DB_GetNPCNameTitleByID( npc->templateID, aname, atitle ) )
				{
					MultiByteToWideChar( CP_ACP, 0, aname, -1, wName, 127 );
					MultiByteToWideChar( CP_ACP, 0, atitle, -1, wTitle, 127 );
					npc->setName( wName );
					lstrcpy( colText, wName );
				}
			}
			lvi.pszText = colText;
			lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
			lvi.iItem = iItem;
			lvi.iSubItem = 0;
			lvi.lParam = npc->objectID;
			lvi.iImage = -1;
			if( npc->isAlikeDead ) lvi.iImage = 2;
			//log_error_np( LOG_OK, "Updating NPC %S, setting iImage = %d\n", npc->charName, lvi.iImage );
			SendMessage( g_rw_hWndNPC, LVM_SETITEM, 0, (LPARAM)&lvi );
			lvi.iImage = 0;

			// NPC title
			wcscpy( colText, npc->getTitle() );
			if( (colText[0] == 0) && (wTitle[0]) )
			{
				lstrcpy( colText, wTitle );
				npc->setTitle( wTitle );
			}
			lvi.mask = LVIF_TEXT;
			lvi.iItem = iItem;
			lvi.iSubItem = 1;
			lvi.pszText = colText;
			SendMessage( g_rw_hWndNPC, LVM_SETITEMTEXT, (WPARAM)iItem, (LPARAM)&lvi );

			// NPC dZ
			wsprintf( colText, TEXT("%d"), (user_z - npc->z) );
			lvi.mask = LVIF_TEXT;
			lvi.iItem = iItem;
			lvi.iSubItem = 2;
			lvi.pszText = colText;
			SendMessage( g_rw_hWndNPC, LVM_SETITEMTEXT, (WPARAM)iItem, (LPARAM)&lvi );

			// NPC templateID
			wsprintf( colText, TEXT("%d"), npc->templateID );
			lvi.mask = LVIF_TEXT;
			lvi.iItem = iItem;
			lvi.iSubItem = 3;
			lvi.pszText = colText;
			SendMessage( g_rw_hWndNPC, LVM_SETITEMTEXT, (WPARAM)iItem, (LPARAM)&lvi );
		} break;
	}
	return 0;
}






LRESULT RW_OnDelObject( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(hWnd);
	// do not del npc if do not display
	if( wParam == MSG_NPC && !g_radarwnd_npc_displayEnable ) return 0;

	bool deleteOK = false;
	HWND hWndLV = g_rw_hWndCL; // by default: delete from PC list
	if( wParam == MSG_NPC ) hWndLV = g_rw_hWndNPC; // if NPC, delete from NPC list
	unsigned int objectId = (unsigned int)lParam;
	LVITEM lvi;
	memset( &lvi, 0, sizeof(lvi) );

	int idx = -1;
	do
	{
		idx = ListView_GetNextItem( hWndLV, idx, LVNI_ALL );
		if( idx == -1 ) break;
		lvi.iItem = idx;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_PARAM;
		if( ListView_GetItem( hWndLV, &lvi ) )
		{
			if( ((unsigned int)lvi.lParam) == objectId )
			{
				ListView_DeleteItem( hWndLV, lvi.iItem );
				deleteOK = true;
				if( wParam == MSG_NPC ) return 0;
				if( wParam == MSG_PC ) break;
			}
		}
	} while( idx >= 0 );

	if( deleteOK )
	{
		// delete also enemies from enemies list (if enemy)
		if( wParam == MSG_PC )
		{
			//log_error( LOG_OK, "RW_OnDelObject(): MSG_PC: deleting PC\n" );
			//unsigned int RELATION_MUTUAL_WAR = 0x08000;
			//unsigned int RELATION_1SIDED_WAR = 0x10000;
			//if( g_cfg.L2_version == L2_VERSION_T23 )
			//{
			//	RELATION_MUTUAL_WAR = 0x04000; // gracia final constants changed
			//	RELATION_1SIDED_WAR = 0x08000;
			//}
			//L2OBJECT_TYPE objType = L2OT_NONE;
			//int index = -1;
			//if( WorldObjectTree_GetInfoByObjectID( objectId, &objType, &index ) )
			//{
			//	int relation = chars_array[index]->relation;
			//	log_error( LOG_OK, "RW_OnDelObject(): MSG_PC: deleting PC %S relation 0x%08X\n",
			//		chars_array[index]->charName, chars_array[index]->relation );
			//	if( (relation & RELATION_1SIDED_WAR) || (relation & RELATION_MUTUAL_WAR) )
					RW_DeleteCharFromEnemiesList( objectId );
			//}
		}
		return 0;
	}

	// if we here, objectId not found
	log_error( LOG_WARNING, "RW_OnDelObject(): cannot find oid [%u] in List-View\n", objectId );
	return 0;
}

void RW_DeleteCharFromEnemiesList( unsigned int objectID )
{
	LVITEM lvi;
	memset( &lvi, 0, sizeof(lvi) );

	int idx = -1;
	do
	{
		idx = ListView_GetNextItem( g_rw_hWndEnemies, idx, LVNI_ALL );
		if( idx == -1 ) break;
		lvi.iItem = idx;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_PARAM;
		if( ListView_GetItem( g_rw_hWndEnemies, &lvi ) )
		{
			if( ((unsigned int)lvi.lParam) == objectID )
			{
				ListView_DeleteItem( g_rw_hWndEnemies, lvi.iItem );
				log_error( LOG_OK, "RW_DeleteCharFromEnemiesList(): oid [%u] del OK\n", objectID );
				return;
			}
		}
	} while( idx >= 0 );

	// if we here, objectId not found
	// this is not error
	//log_error( LOG_WARNING, "RW_DeleteCharFromEnemiesList(): cannot find char oid [%u] in List-View\n",
	//	cha->objectID, cha->charName );
	return;
}

void RW_UpdateCharInEnemiesList( L2Player *cha )
{
	int user_z  = g_game_client->ai.usr.z;

	// find listview itemId to update
	int iItem = -1;
	LVITEM lvi;
	memset( &lvi, 0, sizeof(lvi) );
	int idx = -1;
	do
	{
		idx = ListView_GetNextItem( g_rw_hWndEnemies, idx, LVNI_ALL );
		if( idx == -1 ) break;
		lvi.iItem = idx;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_PARAM;
		if( ListView_GetItem( g_rw_hWndEnemies, &lvi ) )
		{
			if( ((unsigned int)lvi.lParam) == cha->objectID )
			{
				iItem = lvi.iItem;
				break;
			}
		}
	} while( idx >= 0 );

	if( iItem == -1 )
	{
		// not found
		log_error( LOG_WARNING, "RW_UpdateCharInEnemiesList(): cannot find objectId [%u] in Enemies LV to update\n",
			cha->objectID );
		return;
	}

	memset( &lvi, 0, sizeof(lvi) );
	lvi.iItem = iItem;
	lvi.lParam = (LPARAM)(cha->objectID);

	// ok, adding
	bool isEnemy = false;
	bool is2sidewar = false;
	TCHAR colText[128] = {0};
	TCHAR szBuf1[128] = {0};
	TCHAR szBuf2[128] = {0};
	memset( &lvi, 0, sizeof(lvi) );
	lvi.cchTextMax = 128;
	lvi.pszText = colText;

	// detect clan war
	utils_detectClanWar( cha->relation, &isEnemy, &is2sidewar );

	// name
	lstrcpy( colText, cha->getName() );
	lvi.pszText = colText;
	lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.lParam = cha->objectID;
	// image
	lvi.iImage = -1;
	if( isEnemy )
	{
		lvi.iImage = 0;
		if( !is2sidewar ) lvi.iImage = 3;
	}
	if( g_game_client->ai.party.isInParty( cha->objectID, NULL ) ) lvi.iImage = 1;
	if( cha->isAlikeDead ) lvi.iImage = 2;
	SendMessage( g_rw_hWndEnemies, LVM_SETITEM, 0, (LPARAM)&lvi );
	lvi.iImage = 0;

	// baseClass
	szBuf2[0] = szBuf1[0] = 0;
	cha->getClassStr( szBuf1 );
	if( cha->classID != cha->baseClassID )
	{
		cha->getBaseClassStr( szBuf2 );
		wsprintfW( colText, L"%s / %s", szBuf1, szBuf2 );
	}
	else wsprintfW( colText, L"%s", szBuf1 );

	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = 1;
	lvi.pszText = colText;
	SendMessage( g_rw_hWndEnemies, LVM_SETITEMTEXT, (WPARAM)iItem, (LPARAM)&lvi );

	// clan
	ClanList_GetClanNameByID( cha->clanID, colText ); // max 127 chars
	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = 2;
	lvi.pszText = colText;
	SendMessage( g_rw_hWndEnemies, LVM_SETITEMTEXT, (WPARAM)iItem, (LPARAM)&lvi );

	// dZ
	wsprintf( colText, TEXT("%d"), (user_z - cha->z) );
	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = 3;
	lvi.pszText = colText;
	SendMessage( g_rw_hWndEnemies, LVM_SETITEMTEXT, (WPARAM)iItem, (LPARAM)&lvi );

	// clanwar state
	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = 4;
	lvi.pszText = colText;
	if( isEnemy )
	{
		colText[0] = 0;
		if( !is2sidewar ) lstrcpy( colText, TEXT("<") );
		if( is2sidewar ) lstrcpy( colText, TEXT("<>") );
	}
	SendMessage( g_rw_hWndEnemies, LVM_SETITEMTEXT, (WPARAM)iItem, (LPARAM)&lvi );
}





void RadarWnd_ForceUpdateCharsList()
{
	PostMessage( g_radardll_hwnd, WMMY_FORCE_UPDATE_LIST, FORCE_UPDATE_CHARS_LIST, 0 );
}

void RadarWnd_ForceUpdateNpcsList()
{
	PostMessage( g_radardll_hwnd, WMMY_FORCE_UPDATE_LIST, FORCE_UPDATE_NPCS_LIST, 0 );
}

LRESULT RW_OnUpdateCharsList( HWND hWnd )
{
	UNREFERENCED_PARAMETER(hWnd);
	// window is visible?
	//if( bRadarWindowIsVisible == FALSE ) return 0;
	// check too fast updates timeout
	//DWORD dwCurTickTime = GetTickCount();
	//if( dwCurTickTime < (dwRadarDllLastCharListUpdateTime + 1000) ) return 0;
	//dwRadarDllLastCharListUpdateTime = dwCurTickTime;
	// IN_GAME?
	//if( g_game_client->getState() != GCST_IN_GAME ) return 0;
	// delete all items from list-view
	SendMessage( g_rw_hWndCL, LVM_DELETEALLITEMS, 0, 0 );
	// delete all items from enemies list-view
	SendMessage( g_rw_hWndEnemies, LVM_DELETEALLITEMS, 0, 0 );
	//
	int user_z;
	user_z = g_game_client->ai.usr.z;
	int i;
	TCHAR colText[128] = {0};
	TCHAR szBuf1[128] = {0};
	TCHAR szBuf2[128] = {0};
	LVITEM lvi;
	memset( &lvi, 0, sizeof(lvi) );
	lvi.cchTextMax = 128;
	lvi.pszText = colText;
	bool isEnemy = false;
	bool is2sidewar = false;
	for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
	{
		L2Player *cha = chars_array[i];
		if( !cha ) continue;
		if( cha->isUnused() ) continue;
		isEnemy = false;
		is2sidewar = false;
		utils_detectClanWar( cha->relation, &isEnemy, &is2sidewar );
		// name
		lstrcpy( colText, cha->getName() );
		lvi.pszText = colText;
		lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		lvi.iItem = 999;
		lvi.iSubItem = 0;
		lvi.lParam = cha->objectID;
		// image
		lvi.iImage = -1;
		if( isEnemy )
		{
			lvi.iImage = 0;
			if( !is2sidewar ) lvi.iImage = 3;
		}
		if( g_game_client->ai.party.isInParty( cha->objectID, NULL ) ) lvi.iImage = 1;
		if( cha->isAlikeDead ) lvi.iImage = 2;
		int added_item = -1;
		int added_item2 = -1;
		added_item = (int)SendMessage( g_rw_hWndCL, LVM_INSERTITEM, 0, (LPARAM)&lvi );
		if( added_item == -1 ) continue;
		if( isEnemy )
		{
			// add enemies also to enemies tab
			added_item2 = (int)SendMessage( g_rw_hWndEnemies, LVM_INSERTITEM, 0, (LPARAM)&lvi );
		}
		lvi.iImage = 0;

		// baseClass
#ifdef UNICODE
		szBuf2[0] = szBuf1[0] = 0;
		cha->getClassStr( szBuf1 );
		if( cha->classID != cha->baseClassID )
		{
			cha->getBaseClassStr( szBuf2 );
			wsprintfW( colText, L"%s / %s", szBuf1, szBuf2 );
		}
		else wsprintfW( colText, L"%s", szBuf1 );
#else
		//wsprintfA( colText, "%s/%s", cha->classStr(), cha->baseClassStr() );
#endif
		lvi.mask = LVIF_TEXT;
		lvi.iItem = added_item;
		lvi.iSubItem = 1;
		lvi.pszText = colText;
		SendMessage( g_rw_hWndCL, LVM_SETITEM, 0, (LPARAM)&lvi );
		if( isEnemy )
		{
			// add enemies info also to enemies tab
			lvi.iItem = added_item2;
			SendMessage( g_rw_hWndEnemies, LVM_SETITEM, 0, (LPARAM)&lvi );
		}

		// clan
		ClanList_GetClanNameByID( cha->clanID, colText ); // max 127 chars
		lvi.mask = LVIF_TEXT;
		lvi.iItem = added_item;
		lvi.iSubItem = 2;
		lvi.pszText = colText;
		SendMessage( g_rw_hWndCL, LVM_SETITEM, 0, (LPARAM)&lvi );
		if( isEnemy )
		{
			// add enemies info also to enemies tab
			lvi.iItem = added_item2;
			SendMessage( g_rw_hWndEnemies, LVM_SETITEM, 0, (LPARAM)&lvi );
		}

		// dZ
		wsprintf( colText, TEXT("%d"), (user_z - cha->z) );
		lvi.mask = LVIF_TEXT;
		lvi.iItem = added_item;
		lvi.iSubItem = 3;
		lvi.pszText = colText;
		SendMessage( g_rw_hWndCL, LVM_SETITEM, 0, (LPARAM)&lvi );
		if( isEnemy )
		{
			// add enemies info also to enemies tab
			lvi.iItem = added_item2;
			SendMessage( g_rw_hWndEnemies, LVM_SETITEM, 0, (LPARAM)&lvi );
		}

		// clanwar state
		if( isEnemy )
		{
			lvi.mask = LVIF_TEXT;
			lvi.iItem = added_item;
			lvi.iSubItem = 4;
			lvi.pszText = colText;
			colText[0] = 0;
			if( isEnemy && !is2sidewar ) lstrcpy( colText, TEXT("<") );
			if( isEnemy &&  is2sidewar ) lstrcpy( colText, TEXT("<>") );
			if( colText[0] )
			{
				SendMessage( g_rw_hWndCL, LVM_SETITEM, 0, (LPARAM)&lvi );
				// add enemies info also to enemies tab
				lvi.iItem = added_item2;
				SendMessage( g_rw_hWndEnemies, LVM_SETITEM, 0, (LPARAM)&lvi );
			}
		}
	}
	return 0;
}

LRESULT RW_OnUpdateNpcsList( HWND hWnd )
{
	UNREFERENCED_PARAMETER(hWnd);
	// in game?
	if( g_game_client->getState() != GCST_IN_GAME ) return 0;
	// clear ListView
	SendMessage( g_rw_hWndNPC, LVM_DELETEALLITEMS, 0, 0 );
	if( g_radarwnd_npc_displayEnable == 0 ) return 0;
	//
	int user_z;
	user_z = g_game_client->ai.usr.z;
	int i;
	TCHAR colText[128] = {0};
	wchar_t wName[128] = {0}; char aname[128] = {0};
	wchar_t wTitle[128] = {0}; char atitle[128] = {0};
	LVITEM lvi;
	memset( &lvi, 0, sizeof(lvi) );
	lvi.cchTextMax = 128;
	lvi.pszText = colText;
	for( i=0; i<NPCA_MAX_NPCS; i++ )
	{
		//
		aname[0] = atitle[0] = 0;
		wName[0] = wTitle[0] = 0;
		//
		L2Npc *npc = npc_array[i];
		if( !npc ) continue;
		if( npc->isUnused() ) continue;
		// name
		lstrcpy( colText, npc->getName() );
		if( colText[0] == 0 )
		{
			if( L2Data_DB_GetNPCNameTitleByID( npc->templateID, aname, atitle ) )
			{
				MultiByteToWideChar( CP_ACP, 0, aname, -1, wName, 127 );
				MultiByteToWideChar( CP_ACP, 0, atitle, -1, wTitle, 127 );
				npc->setName( wName );
				//npc->setTitle( wTitle );
				lstrcpy( colText, wName );
			}
		}
		lvi.pszText = colText;
		lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		lvi.iItem = 999;
		lvi.iSubItem = 0;
		lvi.lParam = npc->objectID;
		lvi.iImage = -1;
		if( npc->isAlikeDead ) lvi.iImage = 2;
		int added_item = (int)SendMessage( g_rw_hWndNPC, LVM_INSERTITEM, 0, (LPARAM)&lvi );
		if( added_item == -1 ) continue;
		lvi.iImage = 0;

		// title
#ifdef UNICODE
		wcscpy( colText, npc->getTitle() );
		if( (colText[0] == 0) && (wTitle[0]) )
		{
			lstrcpy( colText, wTitle );
			npc->setTitle( wTitle );
		}
#else
		wsprintfA( colText, "%S", npc->getTitle() );
#endif
		lvi.mask = LVIF_TEXT;
		lvi.iItem = added_item;
		lvi.iSubItem = 1;
		lvi.pszText = colText;
		SendMessage( g_rw_hWndNPC, LVM_SETITEM, 0, (LPARAM)&lvi );
		// dZ
		wsprintf( colText, TEXT("%d"), (user_z - npc->z) );
		lvi.mask = LVIF_TEXT;
		lvi.iItem = added_item;
		lvi.iSubItem = 2;
		lvi.pszText = colText;
		SendMessage( g_rw_hWndNPC, LVM_SETITEM, 0, (LPARAM)&lvi );
		// templateID
		wsprintf( colText, TEXT("%d"), npc->templateID );
		lvi.mask = LVIF_TEXT;
		lvi.iItem = added_item;
		lvi.iSubItem = 3;
		lvi.pszText = colText;
		SendMessage( g_rw_hWndNPC, LVM_SETITEM, 0, (LPARAM)&lvi );
	}
	return 0;
}
