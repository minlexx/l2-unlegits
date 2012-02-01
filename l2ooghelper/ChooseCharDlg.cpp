#include "stdafx.h"
#include "Resource.h"
#include "windowUtils.h"

extern HINSTANCE g_hInst;

INT_PTR CALLBACK ChooseCharDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

typedef struct ChooseChar_st
{
	int sel;
	L2Game_CharSelectionInfoBlock *chars;
	int nCount;
} CHOOSECHAR;

// -1 on cancel
int ChooseChar( HWND hWndParent, L2Game_CharSelectionInfoBlock *chars, int nCount )
{
	CHOOSECHAR st;
	st.nCount = nCount;
	st.chars = chars;
	st.sel = -1;
	INT_PTR dlgRet = DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_CHOOSECHAR),
		hWndParent, ChooseCharDlgProc, (LPARAM)&st );
	if( dlgRet == IDOK ) return st.sel;
	return -1;
}

INT_PTR CALLBACK ChooseCharDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			SetWindowLongPtr( hDlg, GWLP_USERDATA, (LONG_PTR)lParam );
			CHOOSECHAR *st = (CHOOSECHAR *)lParam;
			TCHAR text[256];
			HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
			WULV_SetExtStyle( hwndLV, true, true );
			// insert columns
			WULV_InsertColumn( hwndLV, TEXT("#"), 30, 0 );
			WULV_InsertColumn( hwndLV, TEXT("name"), 130, 1 );
			WULV_InsertColumn( hwndLV, TEXT("level"), 40, 2 );
			WULV_InsertColumn( hwndLV, TEXT("class"), 100, 3 );
			WULV_InsertColumn( hwndLV, TEXT("del_secs"), 60, 4 );
			// populate data
			int i;
			for( i=0; i<st->nCount; i++ )
			{
				// add item
				// #
				wsprintf( text, TEXT("%d"), i );
				int added_item = WULV_InsertItem( hwndLV, text, 0, i );
				if( added_item == -1 ) continue;
				// name
				WULV_SetItem( hwndLV, st->chars[i].charName, added_item, 1 );
				// level
				wsprintf( text, TEXT("%u"), st->chars[i].level );
				WULV_SetItem( hwndLV, text, added_item, 2 );
				// class
				wsprintf( text, TEXT("%S"), L2Data_getClass( st->chars[i].classID ) );
				WULV_SetItem( hwndLV, text, added_item, 3 );
				// del days
				if( st->chars[i].deleteSeconds > 0 )
					wsprintf( text, TEXT("%u"), st->chars[i].deleteSeconds );
				else
					text[0] = 0;
				WULV_SetItem( hwndLV, text, added_item, 4 );
			}
		} break;
	case WM_COMMAND:
		{
			CHOOSECHAR *st = (CHOOSECHAR *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
			HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
			switch( LOWORD( wParam ) )
			{
			case IDOK:
				{
					int cur_sel = WULV_GetCurSelItem( hwndLV );
					if( cur_sel >= 0 )
					{
						st->sel = WULV_GetItemLPARAM( hwndLV, cur_sel );
						EndDialog( hDlg, IDOK );
					}
				} break;
			case IDCANCEL: EndDialog( hDlg, IDCANCEL ); break;
			case IDC_CHARDEL: // request delete char
				{
					int cur_sel = WULV_GetCurSelItem( hwndLV );
					if( cur_sel >= 0 )
					{
						st->sel = WULV_GetItemLPARAM( hwndLV, cur_sel ) + 1000;
						EndDialog( hDlg, IDOK );
					}
				} break;
			case IDC_CHARLDELCANCEL: // request cancel character deletion
				{
					int cur_sel = WULV_GetCurSelItem( hwndLV );
					if( cur_sel >= 0 )
					{
						st->sel = WULV_GetItemLPARAM( hwndLV, cur_sel ) + 2000;
						EndDialog( hDlg, IDOK );
					}
				} break;
			case IDC_CHARCREATE: // request create char
				{
					st->sel = 10000;
					EndDialog( hDlg, IDOK );
				} break;
			}
		} break;
	case WM_NOTIFY:
		{
			CHOOSECHAR *st = (CHOOSECHAR *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
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
