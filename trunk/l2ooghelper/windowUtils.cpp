#include "stdafx.h"
#include "windowUtils.h"

void WULV_SetExtStyle( HWND hwndLV, bool fullRowSelect, bool gridLines )
{
	DWORD dwExStyle = 0;
	if( fullRowSelect ) dwExStyle |= LVS_EX_FULLROWSELECT;
	if( gridLines ) dwExStyle |= LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyleEx( hwndLV, dwExStyle, dwExStyle );
}

void WULV_InsertColumn( HWND hwndLV, LPTSTR colName, int width, int idx, int fmt )
{
	LVCOLUMN lvc;
	lvc.mask = LVCF_WIDTH | LVCF_FMT | LVCF_TEXT;
	lvc.fmt = fmt;
	lvc.cx = width;
	lvc.iSubItem = idx;
	lvc.pszText = colName;
	ListView_InsertColumn( hwndLV, idx, &lvc );
}

int  WULV_InsertItem( HWND hwndLV, LPTSTR itemText, int iItem, LPARAM lParam )
{
	LVITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.pszText = itemText;
	lvi.lParam = lParam;
	return ListView_InsertItem( hwndLV, &lvi );
}

int  WULV_InsertItem2( HWND hwndLV, LPTSTR itemText, int iItem, int iImage, LPARAM lParam )
{
	LVITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.pszText = itemText;
	lvi.lParam = lParam;
	lvi.iImage = iImage;
	return ListView_InsertItem( hwndLV, &lvi );
}

void WULV_SetItem( HWND hwndLV, LPTSTR itemText, int iItem, int iSubItem )
{
	LVITEM lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = iSubItem;
	lvi.pszText = itemText;
	ListView_SetItem( hwndLV, &lvi );
}

LPARAM WULV_GetItemLPARAM( HWND hwndLV, int iItem )
{
	LVITEM lvi;
	lvi.mask = LVIF_PARAM;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.lParam = 0;
	ListView_GetItem( hwndLV, &lvi );
	return lvi.lParam;
}

void WULV_GetItemText( HWND hwndLV, int iItem, int iSubItem, LPTSTR out, int cchTextMax )
{
	LVITEM lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = iSubItem;
	lvi.pszText = out;
	lvi.cchTextMax = cchTextMax;
	SendMessage( hwndLV, LVM_GETITEM, 0, (LPARAM)&lvi );
}

int WULV_FindItemByString( HWND hwndLV, int start_iItem, LPCTSTR string, bool exactMatch )
{
	LVFINDINFO info;
	memset( &info, 0, sizeof(info) );
	info.flags = LVFI_PARTIAL;
	info.psz = string;
	if( exactMatch ) info.flags = LVFI_STRING;
	return ListView_FindItem( hwndLV, start_iItem, &info );
}

int WULV_GetCurSelItem( HWND hwndLV )
{
	int cur_sel = ListView_GetNextItem( hwndLV, -1, LVNI_SELECTED );
	return cur_sel;
}



void WUTab_InsertItem( HWND hwndTab, LPTSTR text, int idx )
{
	TCITEM tci;
	tci.mask = TCIF_TEXT;
	tci.pszText = text;
	TabCtrl_InsertItem( hwndTab, idx, &tci );
}



HTREEITEM WUTV_InsertItem( HWND hwndTV, HTREEITEM hParent, HTREEITEM hInsertAfter, LPTSTR text, LPARAM lParam,
	bool hasChildren, bool expanded, bool bold )
{
	TVINSERTSTRUCT tvis;
	tvis.hParent = hParent;
	tvis.hInsertAfter = hInsertAfter;
	tvis.item.mask = TVIF_CHILDREN | TVIF_PARAM | TVIF_STATE | TVIF_TEXT;
	tvis.item.stateMask = TVIS_BOLD | TVIS_EXPANDED;
	tvis.item.lParam = lParam;
	if( hasChildren ) tvis.item.cChildren = 1; else tvis.item.cChildren = 0;
	tvis.item.state = 0;
	if( bold ) tvis.item.state |= TVIS_BOLD;
	if( expanded ) tvis.item.state |= TVIS_EXPANDED;
	tvis.item.pszText = text;
	HTREEITEM htiReturn = TreeView_InsertItem( hwndTV, &tvis );
	return htiReturn;
}

BOOL WU_SetDlgItemInt64( HWND hDlg, int nIDDlgItem, unsigned long long int value, BOOL bSigned )
{
	TCHAR text[32] = {0};
	if( bSigned )
		_sntprintf( text, 32, _T("%lld"), value );
	else
		_sntprintf( text, 32, _T("%llu"), value );
	text[31] = 0;
	return SetDlgItemText( hDlg, nIDDlgItem, text );
}

unsigned long long int WU_GetDlgItemInt64( HWND hDlg, int nIDDlgItem, BOOL bSigned )
{
	TCHAR text[32] = {0};
	unsigned long long int i64 = 0;
	GetDlgItemText( hDlg, nIDDlgItem, text, 32 );
	text[31] = 0;
	if( bSigned )
		_stscanf( text, _T("%lld"), &i64 );
	else
		_stscanf( text, _T("%llu"), &i64 );
	return i64;
}
