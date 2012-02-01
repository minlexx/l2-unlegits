#include "stdafx.h"
#include "Resource.h"
#include "Logger.h"
#include "HealItemsTable.h"
#include "GameClient.h"
#include "Dlg_FindInDB.h"

extern HINSTANCE g_radardll_hinst;
extern class GameClient *g_game_client; // in main.cpp

INT_PTR CALLBACK SelfHealSetup_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

HWND RadarSetupDlg_SelfHeal_Create( HWND hWndParent )
{
	return CreateDialogParam( g_radardll_hinst, MAKEINTRESOURCE(IDD_SR_SELFHEAL),
		hWndParent, SelfHealSetup_DlgProc, 0 );
}

HealItemsTable *rssh_hit;
int rssh_cur_sel;
//HWND SelfHealSetup_saveHWND;

void SelfHealSetup_Init( HWND hDlg );
void SelfHealSetup_OnDestroy( HWND hDlg );
void SelfHealSetup_OnCombo1Event( HWND hDlg, WPARAM wParam, LPARAM lParam );
void SelfHealSetup_fillList( HWND hDlg );
void SelfHealSetup_OnApply( HWND hDlg );
void SelfHealSetup_OnAdd( HWND hDlg );
void SelfHealSetup_OnDel( HWND hDlg );
void SelfHealSetup_OnEdit( HWND hDlg );

INT_PTR CALLBACK EditHealItem_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
HealItem *RS_StartHealItemAdd( HWND hWndParent );
BOOL RS_StartHealItemEdit( HWND hWndParent, HealItem *current );

INT_PTR CALLBACK SelfHealSetup_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG: SelfHealSetup_Init( hDlg ); break;
	case WM_DESTROY: { SelfHealSetup_OnDestroy( hDlg ); return FALSE; } break;
	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			case IDC_COMBO1: SelfHealSetup_OnCombo1Event( hDlg, wParam, lParam ); break;
			case IDC_APPLY: SelfHealSetup_OnApply( hDlg ); break;
			case IDC_DEL: SelfHealSetup_OnDel( hDlg ); break;
			case IDC_ADD: SelfHealSetup_OnAdd( hDlg ); break;
			case IDC_EDIT: SelfHealSetup_OnEdit( hDlg ); break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

void SelfHealSetup_Init( HWND hDlg )
{
	//log_error( LOG_OK, "init\n" );
	//SelfHealSetup_saveHWND = hDlg;
	//
	rssh_hit = new HealItemsTable();
	rssh_hit->clear();
	rssh_hit->LoadFromFile( "L2Detect_healItems.ini" );
	//
	HWND h = GetDlgItem( hDlg, IDC_COMBO1 );
	SendMessage( h, CB_ADDSTRING, 0, (LPARAM)TEXT("HP Healing") );
	SendMessage( h, CB_ADDSTRING, 0, (LPARAM)TEXT("MP Healing") );
	SendMessage( h, CB_ADDSTRING, 0, (LPARAM)TEXT("CP Healing") );
	SendMessage( h, CB_SETCURSEL, 0, 0 );
	rssh_cur_sel = 0;
	//
	h = GetDlgItem( hDlg, IDC_LISTITEMS );
	DWORD dwLvExStyle = LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	SendMessage( h, LVM_SETEXTENDEDLISTVIEWSTYLE, (WPARAM)dwLvExStyle, (LPARAM)dwLvExStyle );
	int i = 0;
	TCHAR text[256] = {0};
	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.cx = 180;
	lvc.fmt = LVCFMT_LEFT;
	lvc.iSubItem = i;
	lvc.pszText = text;
	lstrcpy( text, TEXT("Item name") );
	SendMessage( h, LVM_INSERTCOLUMN, i++, (LPARAM)&lvc );
	lvc.cx = 55;
	lvc.iSubItem = i;
	lvc.pszText = text;
	lstrcpy( text, TEXT("itemID") );
	SendMessage( h, LVM_INSERTCOLUMN, i++, (LPARAM)&lvc );
	lvc.cx = 40;
	lvc.iSubItem = i;
	lvc.pszText = text;
	lstrcpy( text, TEXT("pri") );
	SendMessage( h, LVM_INSERTCOLUMN, i++, (LPARAM)&lvc );
	lvc.cx = 50;
	lvc.iSubItem = i;
	lvc.pszText = text;
	lstrcpy( text, TEXT("% use") );
	SendMessage( h, LVM_INSERTCOLUMN, i++, (LPARAM)&lvc );
	SelfHealSetup_fillList( hDlg );
	lvc.cx = 70;
	lvc.iSubItem = i;
	lvc.pszText = text;
	lstrcpy( text, TEXT("delay,ms") );
	SendMessage( h, LVM_INSERTCOLUMN, i++, (LPARAM)&lvc );
	SelfHealSetup_fillList( hDlg );
}

//void SelfHealSetup_OnOK( HWND hDlg )
//{
//	EndDialog( hDlg, IDOK );
//}
//
//void SelfHealSetup_OnCancel( HWND hDlg )
//{
//	log_error( LOG_OK, "cancel\n" );
//	EndDialog( hDlg, IDCANCEL );
//}

void SelfHealSetup_OnDestroy( HWND hDlg )
{
	UNREFERENCED_PARAMETER(hDlg);
	rssh_hit->clear();
	delete rssh_hit;
	rssh_hit = NULL;
	//log_error( LOG_OK, "close\n" );
}

void SelfHealSetup_OnCombo1Event( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam);
	HWND h = GetDlgItem( hDlg, IDC_COMBO1 );
	int ctrlCode = LOWORD(wParam);
	int evt = HIWORD(wParam);
	if( (ctrlCode == IDC_COMBO1) && (evt == CBN_SELCHANGE) )
	{
		rssh_cur_sel = (int)SendMessage( h, CB_GETCURSEL, 0, 0 );
		//log_error( LOG_OK, "CBN_SELCHANGE %d\n", rssh_cur_sel );
		SelfHealSetup_fillList( hDlg );
	}
}

void SelfHealSetup_fillList( HWND hDlg )
{
	//CheckDlgButton( hDlg, IDC_C_ENABLED, 0 );
	switch( rssh_cur_sel )
	{
	case 0: CheckDlgButton( hDlg, IDC_C_ENABLED, rssh_hit->enableHealHP ); break;
	case 1: CheckDlgButton( hDlg, IDC_C_ENABLED, rssh_hit->enableHealMP ); break;
	case 2: CheckDlgButton( hDlg, IDC_C_ENABLED, rssh_hit->enableHealCP ); break;
	}
	HWND h;
	h = GetDlgItem( hDlg, IDC_LISTITEMS );
	SendMessage( h, LVM_DELETEALLITEMS, 0, 0 );
	HealItem hi;
	int i;
	bool bLoop = true;
	i = 0;
	LVITEM lvi;
	int added_item;
	TCHAR text[256] = {0};
	lvi.iItem = 0;
	lvi.iSubItem = 0;
	lvi.lParam = 0;
	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = text;
	while( bLoop )
	{
		if( rssh_cur_sel == 0 ) bLoop = rssh_hit->getHPItem( i, hi );
		if( rssh_cur_sel == 1 ) bLoop = rssh_hit->getMPItem( i, hi );
		if( rssh_cur_sel == 2 ) bLoop = rssh_hit->getCPItem( i, hi );
		if( !bLoop ) break;
		//
#ifdef UNICODE
		hi.GetItemNameW( text );
#else
		lstrcpy( text, hi.itemName );
#endif
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.pszText = text;
		lvi.iItem = 999;
		lvi.iSubItem = 0;
		lvi.lParam = i;
		added_item = (int)SendMessage( h, LVM_INSERTITEM, 0, (LPARAM)&lvi );
		//
		lvi.mask = LVIF_TEXT;
		wsprintf( text, TEXT("%u"), hi.itemID );
		lvi.lParam = 0;
		lvi.pszText = text;
		lvi.iItem = added_item;
		lvi.iSubItem = 1;
		SendMessage( h, LVM_SETITEM, 0, (LPARAM)&lvi );
		//
		wsprintf( text, TEXT("%d"), hi.priority );
		lvi.pszText = text;
		lvi.iItem = added_item;
		lvi.iSubItem = 2;
		SendMessage( h, LVM_SETITEM, 0, (LPARAM)&lvi );
		//
		wsprintf( text, TEXT("%d"), hi.percentUse );
		lvi.pszText = text;
		lvi.iItem = added_item;
		lvi.iSubItem = 3;
		SendMessage( h, LVM_SETITEM, 0, (LPARAM)&lvi );
		//
		wsprintf( text, TEXT("%u"), hi.reuseDelayMsec );
		lvi.pszText = text;
		lvi.iItem = added_item;
		lvi.iSubItem = 4;
		SendMessage( h, LVM_SETITEM, 0, (LPARAM)&lvi );
		//
		i++;
	}
}

void SelfHealSetup_OnApply( HWND hDlg )
{
	//log_error( LOG_OK, "SelfHealSetup_OnApply\n" );
	int enabled = IsDlgButtonChecked( hDlg, IDC_C_ENABLED );
	switch( rssh_cur_sel )
	{
	case 0: rssh_hit->enableHealHP = enabled; break;
	case 1: rssh_hit->enableHealMP = enabled; break;
	case 2: rssh_hit->enableHealCP = enabled; break;
	}
	switch( rssh_cur_sel )
	{
	case 0: g_game_client->ai.setEnableHealHP( enabled ); break;
	case 1: g_game_client->ai.setEnableHealMP( enabled ); break;
	case 2: g_game_client->ai.setEnableHealCP( enabled ); break;
	}
	rssh_hit->SaveToFile( "L2Detect_healItems.ini" );
	g_game_client->ai.setHealItemsTable( rssh_hit );
}

void SelfHealSetup_OnAdd( HWND hDlg )
{
	HealItemsTable::HEALITEM_TYPE hi_type;
	switch( rssh_cur_sel )
	{
	case 0: hi_type = HealItemsTable::HIT_HP; break;
	case 1: hi_type = HealItemsTable::HIT_MP; break;
	case 2: hi_type = HealItemsTable::HIT_CP; break;
	default: return; break;
	}
	HealItem *hi = RS_StartHealItemAdd( hDlg );
	if( hi )
	{
		hi->lastUseTime = GetTickCount() - hi->reuseDelayMsec - 1000; // :)
		rssh_hit->addHealItem( hi_type, hi ); // TODO!
		SelfHealSetup_fillList( hDlg );
		delete hi;
	}
}

void SelfHealSetup_OnDel( HWND hDlg )
{
	HWND hwndLV = GetDlgItem( hDlg, IDC_LISTITEMS );
	int cur_sel = ListView_GetNextItem( hwndLV, -1, LVNI_SELECTED );
	if( cur_sel == -1 ) return;
	LVITEM lvi;
	lvi.iItem = cur_sel;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_PARAM;
	lvi.lParam = 0;
	ListView_GetItem( hwndLV, &lvi );
	ListView_DeleteItem( hwndLV, cur_sel );
	if( rssh_cur_sel == 0 ) rssh_hit->delItemFromTable( HealItemsTable::HIT_HP, lvi.lParam );
	if( rssh_cur_sel == 1 ) rssh_hit->delItemFromTable( HealItemsTable::HIT_MP, lvi.lParam );
	if( rssh_cur_sel == 2 ) rssh_hit->delItemFromTable( HealItemsTable::HIT_CP, lvi.lParam );
	SelfHealSetup_fillList( hDlg );
}

void SelfHealSetup_OnEdit( HWND hDlg )
{
	HealItemsTable::HEALITEM_TYPE hi_type;
	switch( rssh_cur_sel )
	{
	case 0: hi_type = HealItemsTable::HIT_HP; break;
	case 1: hi_type = HealItemsTable::HIT_MP; break;
	case 2: hi_type = HealItemsTable::HIT_CP; break;
	default: return; break;
	}
	HWND hwndLV = GetDlgItem( hDlg, IDC_LISTITEMS );
	int cur_sel = ListView_GetNextItem( hwndLV, -1, LVNI_SELECTED );
	if( cur_sel == -1 ) return;
	LVITEM lvi;
	lvi.iItem = cur_sel;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_PARAM;
	lvi.lParam = 0;
	ListView_GetItem( hwndLV, &lvi );
	int idx = lvi.lParam;
	HealItem current;
	switch( rssh_cur_sel )
	{
	case 0: rssh_hit->getHPItem( idx, current ); break;
	case 1: rssh_hit->getMPItem( idx, current ); break;
	case 2: rssh_hit->getCPItem( idx, current ); break;
	}
	if( RS_StartHealItemEdit( hDlg, &current ) )
	{
		rssh_hit->setHealItem( hi_type, idx, &current );
		SelfHealSetup_fillList( hDlg );
	}
}

struct RS_HealItemEditStruct
{
	BOOL edit_mode;
	HealItem *toEdit;
};

HealItem *RS_StartHealItemAdd( HWND hWndParent )
{
	HealItem *current = new HealItem();
	RS_HealItemEditStruct st;
	st.edit_mode = FALSE;
	st.toEdit = current;
	INT_PTR ret = DialogBoxParam( g_radardll_hinst, MAKEINTRESOURCE(IDD_SR_SELFHEAL_EDITOR),
		hWndParent, EditHealItem_DlgProc, (LPARAM)&st );
	if( ret == IDOK ) return current;
	delete current;
	return NULL;
}

BOOL RS_StartHealItemEdit( HWND hWndParent, HealItem *current )
{
	RS_HealItemEditStruct st;
	st.edit_mode = TRUE;
	st.toEdit = current;
	INT_PTR ret = DialogBoxParam( g_radardll_hinst, MAKEINTRESOURCE(IDD_SR_SELFHEAL_EDITOR),
		hWndParent, EditHealItem_DlgProc, (LPARAM)&st );
	if( ret == IDOK ) return TRUE;
	return FALSE;
}

INT_PTR CALLBACK EditHealItem_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			SetWindowLongPtr( hDlg, GWLP_USERDATA, (LONG_PTR)lParam );
			struct RS_HealItemEditStruct *st = (struct RS_HealItemEditStruct *)lParam;
			if( st )
			{
				if( st->edit_mode )
				{
					SetWindowText( hDlg, TEXT("Edit item") );
					SetDlgItemTextA( hDlg, IDC_E_ITEMNAME, st->toEdit->itemName );
					SetDlgItemInt( hDlg, IDC_E_ITEMID, st->toEdit->itemID, FALSE );
					SetDlgItemInt( hDlg, IDC_E_ITEMPRI, st->toEdit->priority, TRUE );
					SetDlgItemInt( hDlg, IDC_E_PERCENTUSE, st->toEdit->percentUse, TRUE );
					SetDlgItemInt( hDlg, IDC_E_REUSEDELAY, st->toEdit->reuseDelayMsec, TRUE );
				}
				else
				{
					SetWindowText( hDlg, TEXT("Add item") );
				}
			}
		} break;
	case WM_COMMAND:
		{
			switch( LOWORD( wParam ) )
			{
			case IDCANCEL: EndDialog( hDlg, IDCANCEL ); break;
			case IDOK:
				{
					struct RS_HealItemEditStruct *st = 
						(struct RS_HealItemEditStruct *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
					if( !st )
					{
						EndDialog( hDlg, IDCANCEL );
					}
					else
					{
						GetDlgItemTextA( hDlg, IDC_E_ITEMNAME, st->toEdit->itemName,
							sizeof(st->toEdit->itemName)-1 );
						st->toEdit->itemName[sizeof(st->toEdit->itemName)-1] = 0;
						st->toEdit->itemID = GetDlgItemInt( hDlg, IDC_E_ITEMID, NULL, FALSE );
						st->toEdit->priority = GetDlgItemInt( hDlg, IDC_E_ITEMPRI, NULL, TRUE );
						st->toEdit->percentUse = GetDlgItemInt( hDlg, IDC_E_PERCENTUSE, NULL, TRUE );
						st->toEdit->reuseDelayMsec = GetDlgItemInt( hDlg, IDC_E_REUSEDELAY, NULL, FALSE );
						EndDialog( hDlg, IDOK );
					}
				} break;
			case IDC_FIND:
				{
					Dlg_FindInDB *dlg = new Dlg_FindInDB( Dlg_FindInDB::MODE_ITEM );
					if( dlg->runDialog( hDlg ) )
					{
						//TCHAR tstr[256];
						//wsprintf( tstr, TEXT("%u: %s"), dlg->itemID, dlg->itemNameW );
						//MessageBox( hDlg, tstr, TEXT("Result"), 0 );
						SetDlgItemTextA( hDlg, IDC_E_ITEMNAME, dlg->itemName );
						SetDlgItemInt( hDlg, IDC_E_ITEMID, dlg->itemID, FALSE );
					}
					delete dlg;
				} break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}
