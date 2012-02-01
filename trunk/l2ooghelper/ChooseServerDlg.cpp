#include "stdafx.h"
#include "Resource.h"
#include "windowUtils.h"

extern HINSTANCE g_hInst;

INT_PTR CALLBACK ChooseServerDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

typedef struct ChooseServer_st
{
	int sel;
	L2GameServerInfo *servers;
	int nCount;
} CHOOSESERVER;

// -1 on cancel
int ChooseServer( HWND hWndParent, L2GameServerInfo *servers, int nCount )
{
	CHOOSESERVER st;
	st.nCount = nCount;
	st.servers = servers;
	st.sel = -1;
	INT_PTR dlgRet = DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_CHOOSESERVER),
		hWndParent, ChooseServerDlgProc, (LPARAM)&st );
	if( dlgRet == IDOK ) return st.sel;
	return -1;
}

INT_PTR CALLBACK ChooseServerDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			SetWindowLongPtr( hDlg, GWLP_USERDATA, (LONG_PTR)lParam );
			CHOOSESERVER *st = (CHOOSESERVER *)lParam;
			TCHAR text[256];
			HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
			WULV_SetExtStyle( hwndLV, true, true );
			// insert columns
			WULV_InsertColumn( hwndLV, TEXT("id"), 50, 0 );
			WULV_InsertColumn( hwndLV, TEXT("addr"), 150, 1 );
			WULV_InsertColumn( hwndLV, TEXT("online"), 80, 2 );
			// populate
			int i;
			for( i=0; i<st->nCount; i++ )
			{
				// #
				wsprintf( text, TEXT("%d"), (int)st->servers[i].gsID );
				int added_item = WULV_InsertItem( hwndLV, text, 0, i );
				if( added_item == -1 ) continue;
				// addr
				wsprintf( text, TEXT("%d.%d.%d.%d:%d"),
					(int)st->servers[i].gsIP[0], (int)st->servers[i].gsIP[1],
					(int)st->servers[i].gsIP[2], (int)st->servers[i].gsIP[3],
					(int)st->servers[i].gsPort );
				WULV_SetItem( hwndLV, text, added_item, 1 );
				// online
				wsprintf( text, TEXT("%d / %d"),
					(int)st->servers[i].gsPlayersOnline, (int)st->servers[i].gsPlayersMax );
				WULV_SetItem( hwndLV, text, added_item, 2 );
			}
		} break;
	case WM_COMMAND:
		{
			switch( LOWORD( wParam ) )
			{
			case IDOK:
				{
					CHOOSESERVER *st = (CHOOSESERVER *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
					HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
					int cur_sel = WULV_GetCurSelItem( hwndLV );
					if( cur_sel >= 0 )
					{
						st->sel = WULV_GetItemLPARAM( hwndLV, cur_sel );
						EndDialog( hDlg, IDOK );
					}
				} break;
			case IDCANCEL: EndDialog( hDlg, IDCANCEL ); break;
			}
		} break;
	case WM_NOTIFY:
		{
			CHOOSESERVER *st = (CHOOSESERVER *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
			HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
			LPNMHDR pnmh = (LPNMHDR)lParam;
			if( pnmh->code == NM_DBLCLK )
			{
				LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
				st->sel = WULV_GetItemLPARAM( hwndLV, lpnmitem->iItem );
				EndDialog( hDlg, IDOK );
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}
