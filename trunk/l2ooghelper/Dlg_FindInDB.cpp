#include "stdafx.h"
#include "Dlg_FindInDB.h"
#include "Resource.h"

extern HINSTANCE g_hInst;

Dlg_FindInDB::Dlg_FindInDB( MODE mode )
{
	m_mode = mode;
	itemID = 0;
	itemName[0] = 0;
	itemNameW[0] = 0;
}

Dlg_FindInDB::~Dlg_FindInDB()
{
	itemID = 0;
	itemName[0] = 0;
	itemNameW[0] = 0;
}

INT_PTR CALLBACK Dlg_FindInDB_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

bool Dlg_FindInDB::runDialog( HWND hWndParent )
{
	INT_PTR ret = DialogBoxParam( g_hInst,
		MAKEINTRESOURCE(IDD_FINDINDB), hWndParent, Dlg_FindInDB_Proc, (LPARAM)this );
	if( ret == IDOK ) return true;
	return false;
}


void Dlg_FindInDB_OnInitDialog( HWND hDlg, LPARAM lParam );
void Dlg_FindInDB_OnOK( HWND hDlg );
void Dlg_FindInDB_OnFind( HWND hDlg );

INT_PTR CALLBACK Dlg_FindInDB_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG: Dlg_FindInDB_OnInitDialog( hDlg, lParam ); break;
	case WM_COMMAND:
		{
			switch( LOWORD( wParam ) )
			{
			case IDOK: Dlg_FindInDB_OnOK( hDlg ); break;
			case IDCANCEL: EndDialog( hDlg, IDCANCEL ); break;
			case IDC_FIND: Dlg_FindInDB_OnFind( hDlg ); break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

void Dlg_FindInDB_OnInitDialog( HWND hDlg, LPARAM lParam )
{
	SetWindowLongPtr( hDlg, GWLP_USERDATA, (LONG_PTR)lParam );
	class Dlg_FindInDB *cls = (class Dlg_FindInDB *)lParam;
	switch( cls->getMode() )
	{
	case Dlg_FindInDB::MODE_ITEM:
		{
			SetWindowText( hDlg, TEXT("Поиск предмета в базе") );
		} break;
	case Dlg_FindInDB::MODE_NPC:
		{
			SetWindowText( hDlg, TEXT("Поиск NPC в базе") );
		} break;
	case Dlg_FindInDB::MODE_SKILL:
		{
			SetWindowText( hDlg, TEXT("Поиск скилла в базе") );
		} break;
	}
	CheckDlgButton( hDlg, IDC_R_CONTAINS, BST_CHECKED );
	HWND hwndLV = GetDlgItem( hDlg, IDC_LISTITEMS );
	DWORD dwLvExStyle = LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	SendMessage( hwndLV, LVM_SETEXTENDEDLISTVIEWSTYLE, (WPARAM)dwLvExStyle, (LPARAM)dwLvExStyle );
	TCHAR text[256] = {0};
	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = 60;
	lvc.iSubItem = 0;
	lvc.pszText = text;
	lstrcpy( text, TEXT("id") );
	ListView_InsertColumn( hwndLV, 0, &lvc );
	lvc.cx = 200;
	lvc.iSubItem = 1;
	lstrcpy( text, TEXT("Имя") );
	ListView_InsertColumn( hwndLV, 1, &lvc );
}

void Dlg_FindInDB_OnOK( HWND hDlg )
{
	HWND hwndLV = GetDlgItem( hDlg, IDC_LISTITEMS );
	int iSel = ListView_GetNextItem( hwndLV, -1, LVNI_SELECTED );
	if( iSel == -1 ) return;
	class Dlg_FindInDB *cls = (class Dlg_FindInDB *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	if( !cls ) return;
	TCHAR text[256];
	LVITEM lvi;
	lvi.iItem = iSel;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_TEXT;
	lvi.pszText = text;
	lvi.cchTextMax = 255;
	ListView_GetItem( hwndLV, &lvi );
	_stscanf( lvi.pszText, _T("%d"), &(cls->itemID) );
	lvi.iSubItem = 1;
	lvi.mask = LVIF_TEXT;
	lvi.pszText = text;
	lvi.cchTextMax = 255;
	ListView_GetItem( hwndLV, &lvi );
#ifdef UNICODE
	wcscpy( cls->itemNameW, lvi.pszText );
	sprintf( cls->itemName, "%S", lvi.pszText );
#else
	strcpy( cls->itemName, lvi.pszText );
	wsprintfW( cls->itemNameW, L"%S", lvi.pszText );
#endif
	EndDialog( hDlg, IDOK );
}

void Dlg_FindInDB_OnFind( HWND hDlg )
{
	class Dlg_FindInDB *cls = (class Dlg_FindInDB *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	TCHAR text[256] = {0};
	char toFind[256] = {0};
	char queryString[256] = {0};
	GetDlgItemTextA( hDlg, IDC_EDITNAME, toFind, 255 );
	toFind[255] = 0;
	HWND hwndLV = GetDlgItem( hDlg, IDC_LISTITEMS );
	int starts_with = IsDlgButtonChecked( hDlg, IDC_R_STWITH );
	int contains = IsDlgButtonChecked( hDlg, IDC_R_CONTAINS );
	int ends_with = IsDlgButtonChecked( hDlg, IDC_R_ENWITH );
	if( contains ) sprintf( queryString, "%%%s%%", toFind );
	if( ends_with ) sprintf( queryString, "%%%s", toFind );
	if( starts_with ) sprintf( queryString, "%s%%", toFind );
	L2Data_DB_Result res;
	res.inMaxRowsToGet = 64;
	bool bRet = false;
	switch( cls->getMode() )
	{
	case Dlg_FindInDB::MODE_ITEM: bRet = L2Data_DB_Query_Items_ByName( queryString, &res ); break;
	case Dlg_FindInDB::MODE_NPC: bRet = L2Data_DB_Query_NPCs_ByName( queryString, &res ); break;
	case Dlg_FindInDB::MODE_SKILL: bRet = L2Data_DB_Query_Skills_ByName( queryString, &res ); break;
	}
	if( bRet )
	{
		ListView_DeleteAllItems( hwndLV );
		int nr = res.rowCount;
		int i;
		for( i=0; i<nr; i++ )
		{
			LVITEM lvi;
			lvi.mask = LVIF_TEXT;
			lvi.iItem = 999;
			lvi.iSubItem = 0;
			lvi.pszText = text;
			wsprintf( text, TEXT("%S"), res.rows[i][0] );
			int added_item = ListView_InsertItem( hwndLV, &lvi );
			lvi.iItem = added_item;
			lvi.iSubItem = 1;
			lvi.pszText = text;
			wsprintf( text, TEXT("%S"), res.rows[i][1] );
			ListView_SetItem( hwndLV, &lvi );
		}
		L2Data_DB_Free_Result( &res );
	}
}
