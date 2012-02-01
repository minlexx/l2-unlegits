#include "stdafx.h"
#include "Resource.h"
#include "windowUtils.h"
#include "logger.h"
#include "DBLayer.h"
//
#include "CharArray.h"
#include "NpcArray.h"
#include "GIArray.h"
#include "IL2Client.h"

extern HINSTANCE g_hInst;

INT_PTR CALLBACK MapItemsTabDlg_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

HWND MapItemsTabDlg_Create( HWND hWndParent, IL2Client *pcl )
{
	HWND hDlg = CreateDialogParam( g_hInst, MAKEINTRESOURCE(IDD_MAPINFO),
		hWndParent, MapItemsTabDlg_Proc, (LPARAM)pcl );
	return hDlg;
}

#define MAP_TABS 4

typedef struct stMapTab
{
	int nTabs;
	HWND hwndLV[MAP_TABS];
	IL2Client *cl;
} MapTab;

void MapItem_updatePlayers( HWND hDlg, LPARAM lParam );
void MapItem_updateNpcs( HWND hDlg, LPARAM lParam, bool mobs );
void MapItem_updateItems( HWND hDlg, LPARAM lParam );

INT_PTR CALLBACK MapItemsTabDlg_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			IL2Client *pcl = (IL2Client *)lParam;
			//
			MapTab *st = (MapTab *)malloc( sizeof(MapTab) );
			SetWindowLongPtr( hDlg, GWLP_USERDATA, (LONG_PTR)st );
			st->nTabs = MAP_TABS;
			st->cl = pcl;
			//
			HWND hWndTab = GetDlgItem( hDlg, IDC_TAB2 );
			WUTab_InsertItem( hWndTab, TEXT("Чары"), 0 );
			WUTab_InsertItem( hWndTab, TEXT("Мобы"), 1 );
			WUTab_InsertItem( hWndTab, TEXT("НПЦ"), 2 );
			WUTab_InsertItem( hWndTab, TEXT("Предметы"), 3 );
			//
			RECT rc;
			GetClientRect( hWndTab, &rc );
			// players
			HWND hwndLV = CreateWindowEx( WS_EX_CLIENTEDGE,
				WC_LISTVIEW, NULL,
				WS_VSCROLL | WS_CHILD | WS_VISIBLE |
					LVS_REPORT | LVS_SORTASCENDING | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
				2, 24, rc.right - rc.left - 5, rc.bottom - rc.top - 30,
				hWndTab, (HMENU)100, g_hInst, NULL );
			st->hwndLV[0] = hwndLV;
			WULV_SetExtStyle( hwndLV, true, true );
			WULV_InsertColumn( hwndLV, TEXT("Имя"), 100, 0 );
			WULV_InsertColumn( hwndLV, TEXT("класс"), 100, 1 );
			WULV_InsertColumn( hwndLV, TEXT("dz"), 50, 2 );
			WULV_InsertColumn( hwndLV, TEXT("war"), 50, 3 );
			WULV_InsertColumn( hwndLV, TEXT("Клан"), 100, 4 );
			// mobs
			hwndLV = CreateWindowEx( WS_EX_CLIENTEDGE,
				WC_LISTVIEW, NULL,
				WS_VSCROLL | WS_CHILD | /*WS_VISIBLE |*/
					LVS_REPORT | LVS_SORTASCENDING | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
				2, 24, rc.right - rc.left - 5, rc.bottom - rc.top - 30,
				hWndTab, (HMENU)101, g_hInst, NULL );
			st->hwndLV[1] = hwndLV;
			WULV_SetExtStyle( hwndLV, true, true );
			WULV_InsertColumn( hwndLV, TEXT("Моб"), 100, 0 );
			WULV_InsertColumn( hwndLV, TEXT("Титул"), 80, 1 );
			WULV_InsertColumn( hwndLV, TEXT("npcID"), 50, 2 );
			WULV_InsertColumn( hwndLV, TEXT("dz"), 50, 3 );
			// NPC
			hwndLV = CreateWindowEx( WS_EX_CLIENTEDGE,
				WC_LISTVIEW, NULL,
				WS_VSCROLL | WS_CHILD | /*WS_VISIBLE |*/
					LVS_REPORT | LVS_SORTASCENDING | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
				2, 24, rc.right - rc.left - 5, rc.bottom - rc.top - 30,
				hWndTab, (HMENU)102, g_hInst, NULL );
			st->hwndLV[2] = hwndLV;
			WULV_SetExtStyle( hwndLV, true, true );
			WULV_InsertColumn( hwndLV, TEXT("НПЦ"), 130, 0 );
			WULV_InsertColumn( hwndLV, TEXT("Титул"), 80, 1 );
			WULV_InsertColumn( hwndLV, TEXT("npcID"), 50, 2 );
			WULV_InsertColumn( hwndLV, TEXT("dz"), 50, 3 );
			// Items
			hwndLV = CreateWindowEx( WS_EX_CLIENTEDGE,
				WC_LISTVIEW, NULL,
				WS_VSCROLL | WS_CHILD | /*WS_VISIBLE |*/
					LVS_REPORT | LVS_SORTASCENDING | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
				2, 24, rc.right - rc.left - 5, rc.bottom - rc.top - 30,
				hWndTab, (HMENU)103, g_hInst, NULL );
			st->hwndLV[3] = hwndLV;
			WULV_SetExtStyle( hwndLV, true, true );
			WULV_InsertColumn( hwndLV, TEXT("Предмет"), 130, 0 );
			WULV_InsertColumn( hwndLV, TEXT("itemID"), 50, 1 );
			WULV_InsertColumn( hwndLV, TEXT("кол-во"), 50, 2 );
		} break;
	case WM_DESTROY:
		{
			MapTab *st = (MapTab *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
			if( st ) free( st );
			SetWindowLongPtr( hDlg, GWLP_USERDATA, 0 );
		} break;
	case WM_NOTIFY:
		{
			LPNMHDR phdr = (LPNMHDR)lParam;
			HWND hWndTab = GetDlgItem( hDlg, IDC_TAB2 );
			MapTab *st = (MapTab *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
			if( phdr->hwndFrom == hWndTab )
			{
				int cur_sel = TabCtrl_GetCurSel( hWndTab );
				switch( phdr->code )
				{
				case TCN_SELCHANGING:
					{
						if( (cur_sel >= 0) && (cur_sel < st->nTabs) ) ShowWindow( st->hwndLV[cur_sel], SW_HIDE );
					} break;
				case TCN_SELCHANGE:
					{
						if( (cur_sel >= 0) && (cur_sel < st->nTabs) ) ShowWindow( st->hwndLV[cur_sel], SW_SHOW );
					} break;
				}
			}
			if( phdr->code == NM_DBLCLK )
			{
				LPNMITEMACTIVATE lpnmitemactivate = (LPNMITEMACTIVATE)lParam;
				int i;
				for( i=0; i<st->nTabs; i++ )
				{
					if( phdr->hwndFrom == st->hwndLV[i] )
					{
						unsigned int objectID = (unsigned int)WULV_GetItemLPARAM( st->hwndLV[i], lpnmitemactivate->iItem );
						if( objectID > 0 )
						{
							//log_error( LOG_OK, "Map LV action to oid %u\n", objectID );
							st->cl->game_Action( objectID );
						}
					}
				}
			}
		} break;
	case WM_USER:
		{
			int tabId = (int)wParam;
			log_error( LOG_DEBUG, "MapItemsTabDlgProc(): update tab %d\n", tabId );
			switch( tabId )
			{
			case 0: MapItem_updatePlayers( hDlg, lParam ); break;
			case 1: MapItem_updateNpcs( hDlg, lParam, true ); break;
			case 2: MapItem_updateNpcs( hDlg, lParam, false ); break;
			case 3: MapItem_updateItems( hDlg, lParam ); break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

void MapItem_updatePlayers( HWND hDlg, LPARAM lParam )
{
	MapTab *st = (MapTab *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	if( !st ) return;
	IL2Client *pcl = (IL2Client *)lParam;
	if( !pcl ) return;
	CharArray *ca = pcl->get_WorldChars();
	ClanList *cl = pcl->get_WorldClans();
	UserInfo *usr = pcl->get_UserInfo();
	// Lock !!!
	ca->Lock();
	cl->Lock();
	///
	HWND hwndLV = st->hwndLV[0];
	ListView_DeleteAllItems( hwndLV );
	int i;
	int added_item;
	TCHAR text[256] = {0};
	int dz;
	unsigned int nDisplayed = 0;
	for( i=0; i<CharArray::CHARARRAY_MAX_CHARS; i++ )
	{
		if( ca->chars_array[i]->isUnused() ) continue;
		nDisplayed++;
		added_item = WULV_InsertItem( hwndLV, ca->chars_array[i]->charName, 0, ca->chars_array[i]->objectID );
		// class
		if( ca->chars_array[i]->classID != ca->chars_array[i]->baseClassID )
		{
			wsprintf( text, TEXT("%S / %S"),
				L2Data_getClass( ca->chars_array[i]->classID ),
				L2Data_getClass( ca->chars_array[i]->baseClassID ) );
		}
		else
		{
			wsprintf( text, TEXT("%S"), L2Data_getClass( ca->chars_array[i]->classID ) );
		}
		WULV_SetItem( hwndLV, text, added_item, 1 );
		// dz
		dz = usr->z - ca->chars_array[i]->z;
		wsprintf( text, TEXT("%d"), dz );
		WULV_SetItem( hwndLV, text, added_item, 2 );
		// war state
		text[0] = 0;
		switch( ca->chars_array[i]->relation )
		{
		case L2Player::RELATION_1SIDED_WAR: lstrcpy( text, TEXT("<") ); break;
		case L2Player::RELATION_MUTUAL_WAR: lstrcpy( text, TEXT("<>") ); break;
		}
		WULV_SetItem( hwndLV, text, added_item, 3 );
		// clan
		if( ca->chars_array[i]->clanID )
			cl->GetClanNameByID( ca->chars_array[i]->clanID, text );
		WULV_SetItem( hwndLV, text, added_item, 4 );
		//
		if( nDisplayed >= ca->GetCount() ) break;
	}
	// Unlock!!!
	ca->Unlock();
	cl->Unlock();
}

void MapItem_updateNpcs( HWND hDlg, LPARAM lParam, bool mobs )
{
	MapTab *st = (MapTab *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	if( !st ) return;
	IL2Client *pcl = (IL2Client *)lParam;
	if( !pcl ) return;
	NpcArray *na = pcl->get_WorldNpcs();
	UserInfo *usr = pcl->get_UserInfo();
	//
	na->Lock();
	//
	HWND hwndLV = st->hwndLV[1];
	if( !mobs ) hwndLV = st->hwndLV[2];
	ListView_DeleteAllItems( hwndLV );
	int i;
	int added_item;
	TCHAR text[256] = {0};
	wchar_t npcName[256] = {0}, npcTitle[256] = {0};
	int dz;
	unsigned int nDisplayed = 0;
	for( i=0; i<NpcArray::NPCA_MAX_NPCS; i++ )
	{
		if( na->npcs_array[i]->isUnused() ) continue;
		nDisplayed++; // not really displayed, but passed index in array
		if( mobs && !na->npcs_array[i]->isAttackable ) continue;
		if( !mobs && na->npcs_array[i]->isAttackable ) continue;
		npcName[0] = 0, npcTitle[0] = 0;
		text[0] = 0;
		// mob name, title, npcID, dz
		// name
		if( na->npcs_array[i]->charName[0] == 0 )
		{
			DBLayer_getNpcNameTitle( na->npcs_array[i]->templateID, npcName, 256, npcTitle, 256 );
			na->npcs_array[i]->setName( npcName );
		}
		added_item = WULV_InsertItem( hwndLV, na->npcs_array[i]->charName, 0, na->npcs_array[i]->objectID );
		// title
		if( na->npcs_array[i]->charTitle[0] == 0 )
			if( npcTitle[0] != 0 ) na->npcs_array[i]->setTitle( npcTitle );
		WULV_SetItem( hwndLV, na->npcs_array[i]->charTitle, added_item, 1 );
		// npcID
		wsprintf( text, TEXT("%u"), na->npcs_array[i]->templateID );
		WULV_SetItem( hwndLV, text, added_item, 2 );
		// dz
		dz = usr->z - na->npcs_array[i]->z;
		wsprintf( text, TEXT("%d"), dz );
		WULV_SetItem( hwndLV, text, added_item, 3 );
		//
		if( nDisplayed >= na->getCount() ) break;
	}
	na->Unlock();
}

void MapItem_updateItems( HWND hDlg, LPARAM lParam )
{
	MapTab *st = (MapTab *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	if( !st ) return;
	IL2Client *pcl = (IL2Client *)lParam;
	if( !pcl ) return;
	GIArray *ga = pcl->get_WorldItems();
	// lock
	ga->Lock();
	//
	HWND hwndLV = st->hwndLV[3];
	ListView_DeleteAllItems( hwndLV );
	int i;
	int added_item;
	TCHAR text[256] = {0};
	unsigned int nDisplayed = 0;
	for( i=0; i<GIArray::GA_MAX_ITEMS; i++ )
	{
		if( ga->gi_array[i]->isUnused() ) continue;
		nDisplayed++; // not really displayed, but passed index in array
		text[0] = 0;
		// item name, itemID, count
		// item name
		DBLayer_getItemName( ga->gi_array[i]->itemID, text, 255 );
		added_item = WULV_InsertItem( hwndLV, text, 0, ga->gi_array[i]->objectID );
		// itemID
		wsprintf( text, TEXT("%u"), ga->gi_array[i]->itemID );
		WULV_SetItem( hwndLV, text, added_item, 1 );
		// count
		wsprintf( text, TEXT("%d"), ga->gi_array[i]->count );
		WULV_SetItem( hwndLV, text, added_item, 2 );
		//
		if( nDisplayed >= ga->getCount() ) break;
	}
	ga->Unlock();
}
