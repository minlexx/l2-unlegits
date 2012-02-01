#include "stdafx.h"
#include "Logger.h"
#include "Resource.h"
#include "windowUtils.h"
#include "GameClient.h"

extern class GameClient *g_game_client;
extern HINSTANCE         g_radardll_hinst;

struct rsdqtsdp_RANGE_INFO
{
	int min;
	int max;
	int pri;
};

INT_PTR CALLBACK rsdqtsdp_EditRange_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

bool rsdqtsdp_launch_edit_dlg( HWND hDlg, rsdqtsdp_RANGE_INFO *st )
{
	INT_PTR ret = DialogBoxParam( g_radardll_hinst, MAKEINTRESOURCE(IDD_SR_DISTPRI_EDITPRI), hDlg,
		rsdqtsdp_EditRange_DlgProc, (LPARAM)st );
	if( ret == IDOK ) return true;
	return false;
}

void rsdqtsdp_add_range( HWND hDlg )
{
	HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
	int cur_sel = WULV_GetCurSelItem( hwndLV );
	if( cur_sel == -1 ) cur_sel = 999;
	rsdqtsdp_RANGE_INFO inf = { 0, 0, 0 };
	if( rsdqtsdp_launch_edit_dlg( hDlg, &inf ) )
	{
		TCHAR txt[32] = {0};
		swprintf( txt, 31, L"%d", inf.min );
		int added_idx = WULV_InsertItem( hwndLV, txt, cur_sel );
		if( added_idx >= 0 )
		{
			swprintf( txt, 31, L"%d", inf.max );
			WULV_SetItem( hwndLV, txt, added_idx, 1 );
			swprintf( txt, 31, L"%d", inf.pri );
			WULV_SetItem( hwndLV, txt, added_idx, 2 );
		}
	}
}

void rsdqtsdp_edit_range( HWND hDlg )
{
	HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
	int cur_sel = WULV_GetCurSelItem( hwndLV );
	if( cur_sel == -1 ) return;
	rsdqtsdp_RANGE_INFO inf;
	TCHAR txt[32] = {0};
	WULV_GetItemText( hwndLV, cur_sel, 0, txt, 31 );
	swscanf( txt, L"%d", &(inf.min) );
	WULV_GetItemText( hwndLV, cur_sel, 1, txt, 31 );
	swscanf( txt, L"%d", &(inf.max) );
	WULV_GetItemText( hwndLV, cur_sel, 2, txt, 31 );
	swscanf( txt, L"%d", &(inf.pri) );
	if( rsdqtsdp_launch_edit_dlg( hDlg, &inf ) )
	{
		swprintf( txt, 31, L"%d", inf.min );
		WULV_SetItem( hwndLV, txt, cur_sel, 0 );
		swprintf( txt, 31, L"%d", inf.max );
		WULV_SetItem( hwndLV, txt, cur_sel, 1 );
		swprintf( txt, 31, L"%d", inf.pri );
		WULV_SetItem( hwndLV, txt, cur_sel, 2 );
	}
}

void rsdqtsdp_del_range( HWND hDlg )
{
	HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
	int cur_sel = WULV_GetCurSelItem( hwndLV );
	if( cur_sel == -1 ) return;
	ListView_DeleteItem( hwndLV, cur_sel );
}

void rsdqtsdp_fill_lv( HWND hDlg )
{
	HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
	SendMessage( hwndLV, LVM_DELETEALLITEMS, 0, 0 );
	int i;
	DistancePriorities *dpri = g_game_client->ai.cfgQuickTarg.getDistancePriorities();
	for( i=0; i<dpri->MAXN; i++ )
	{
		int min=-1, max=-1, pri=0;
		dpri->getRangeInfo( i, &min, &max, &pri );
		if( min>=0 && max>=0 )
		{
			TCHAR txt[32];
			swprintf( txt, 31, L"%d", min );
			int added_item = WULV_InsertItem( hwndLV, txt, 999 );
			if( added_item >= 0 )
			{
				swprintf( txt, 31, L"%d", max );
				WULV_SetItem( hwndLV, txt, added_item, 1 );
				swprintf( txt, 31, L"%d", pri );
				WULV_SetItem( hwndLV, txt, added_item, 2 );
			}
		}
	}
}

INT_PTR CALLBACK RadarSetupDlg_QuickTarget_SetupDistancePriorities_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
			WULV_SetExtStyle( hwndLV, true, true );
			WULV_InsertColumn( hwndLV, TEXT("[min"), 55, 0 );
			WULV_InsertColumn( hwndLV, TEXT("max)"), 55, 1 );
			WULV_InsertColumn( hwndLV, TEXT("pri_mod"), 60, 2 );
			rsdqtsdp_fill_lv( hDlg );
			lParam = 0; // fix warn unref. param
		} break;
	case WM_COMMAND:
		switch( LOWORD( wParam ) )
		{
		case IDC_B_ADD: rsdqtsdp_add_range( hDlg ); break;
		case IDC_B_EDIT: rsdqtsdp_edit_range( hDlg ); break;
		case IDC_B_DEL: rsdqtsdp_del_range( hDlg ); break;
		case IDOK:
			{
				DistancePriorities *dpri = g_game_client->ai.cfgQuickTarg.getDistancePriorities();
				dpri->removeAll();
				HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
				TCHAR txt[32] = {0};
				int min, max, pri;
				int iItem = -1;
				iItem = ListView_GetNextItem( hwndLV, iItem, LVNI_ALL );
				while( iItem >= 0 )
				{
					WULV_GetItemText( hwndLV, iItem, 0, txt, 31 );
					swscanf( txt, L"%d", &min );
					WULV_GetItemText( hwndLV, iItem, 1, txt, 31 );
					swscanf( txt, L"%d", &max );
					WULV_GetItemText( hwndLV, iItem, 2, txt, 31 );
					swscanf( txt, L"%d", &pri );
					if( dpri->addRangePri( min, max, pri ) < 0 )
						log_error( LOG_ERROR, "RadarSetupDlg_QuickTarget_SetupDistancePriorities_DlgProc "
							"on OK: failed to add dist pri range [%d-%d,%d]\n", min, max, pri );
					iItem = ListView_GetNextItem( hwndLV, iItem, LVNI_ALL );
				}
				EndDialog( hDlg, IDOK );
			} break;
		case IDCANCEL: EndDialog( hDlg, IDCANCEL ); break;
		}
		break;
	case WM_CLOSE: EndDialog( hDlg, IDCANCEL ); break;
	default: return FALSE; break;
	}
	return TRUE;
}

void RadarSetupDlg_QuickTarget_SetupDistancePriorities( HWND hWndParent )
{
	DialogBoxParam( g_radardll_hinst, MAKEINTRESOURCE(IDD_SR_DISTPRI), hWndParent,
		RadarSetupDlg_QuickTarget_SetupDistancePriorities_DlgProc, 0 );
}



INT_PTR CALLBACK rsdqtsdp_EditRange_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			SetWindowLongPtr( hDlg, GWLP_USERDATA, lParam );
			rsdqtsdp_RANGE_INFO *st = (rsdqtsdp_RANGE_INFO *)lParam;
			SetDlgItemInt( hDlg, IDC_E_MIN, st->min, TRUE );
			SetDlgItemInt( hDlg, IDC_E_MAX, st->max, TRUE );
			SetDlgItemInt( hDlg, IDC_E_PRI, st->pri, TRUE );
		} break;
	case WM_COMMAND:
		{
			switch( LOWORD( wParam ) )
			{
			case IDOK:
				{
					rsdqtsdp_RANGE_INFO *st = (rsdqtsdp_RANGE_INFO *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
					st->min = (int)GetDlgItemInt( hDlg, IDC_E_MIN, NULL, TRUE );
					st->max = (int)GetDlgItemInt( hDlg, IDC_E_MAX, NULL, TRUE );
					st->pri = (int)GetDlgItemInt( hDlg, IDC_E_PRI, NULL, TRUE );
					EndDialog( hDlg, IDOK );
				} break;
			case IDCANCEL: EndDialog( hDlg, IDCANCEL ); break;
			default: return FALSE; break;
			}
		} break;
	case WM_CLOSE: EndDialog( hDlg, IDCANCEL ); break;
	default: return FALSE; break;
	}
	return TRUE;
}
