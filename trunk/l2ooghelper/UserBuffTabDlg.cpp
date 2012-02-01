#include "stdafx.h"
#include "Resource.h"
#include "UserBuffs.h"
#include "IL2Client.h"
#include "windowUtils.h"

extern HINSTANCE g_hInst;

INT_PTR CALLBACK UserBuffTabDlg_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );


HWND UserBuffTabDlg_Create( HWND hWndParent, IL2Client *pcl )
{
	HWND hDlg = CreateDialogParam( g_hInst, MAKEINTRESOURCE(IDD_USERBUFF),
		hWndParent, UserBuffTabDlg_Proc, 0 );
	SetWindowLongPtrW( hDlg, GWLP_USERDATA, (LONG_PTR)pcl );
	return hDlg;
}

void format_bufftimeW( wchar_t *out, int nSecs )
{
	if( nSecs <= 59 ) // only seconds
	{
		wsprintfW( out, L"%d", nSecs );
		return;
	}
	int nMins = nSecs / 60;
	nSecs -= nMins * 60;
	int nHrs = nMins / 60;
	if( nHrs > 0 ) // have hours
	{
		nMins -= nHrs * 60;
		wsprintfW( out, L"%02d:%02d:%02d", nHrs, nMins, nSecs );
		return;
	}
	// only min:sec
	wsprintfW( out, L"%02d:%02d", nMins, nSecs );
}

INT_PTR CALLBACK UserBuffTabDlg_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	TCHAR text[128] = {0};
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
			WULV_SetExtStyle( hwndLV, true, true );
			int i = 0;
			WULV_InsertColumn( hwndLV, TEXT("Αΰττ"), 150, i++ );
			WULV_InsertColumn( hwndLV, TEXT("lvl"), 50, i++ );
			WULV_InsertColumn( hwndLV, TEXT("time"), 60, i++ );
			WULV_InsertColumn( hwndLV, TEXT("skillID"), 50, i++ );
		} break;
	case WM_COMMAND:
		{
			IL2Client *pcl = (IL2Client *)GetWindowLongPtrW( hDlg, GWLP_USERDATA );
			HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
			int buttonId = LOWORD( wParam );
			if( buttonId == IDC_CANCELBUFF )
			{
				int cur_sel = ListView_GetNextItem( hwndLV, -1, LVNI_SELECTED );
				if( cur_sel >= 0 )
				{
					unsigned int skillID = (unsigned int)WULV_GetItemLPARAM( hwndLV, cur_sel );
					pcl->game_buffCancel( skillID );
				}
			}
		} break;
	case WM_USER:
		{
			UserBuffs *buffs = (UserBuffs *)lParam;
			if( !buffs ) return TRUE;
			wsprintf( text, TEXT("%d buffs"), buffs->buffCount );
			SetDlgItemText( hDlg, IDC_ST_NBUFFS, text );
			HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
			ListView_DeleteAllItems( hwndLV );
			int i = 0;
			int added_idx = 0;
			for( i=0; i<UserBuffs::USER_MAX_BUFFS; i++ )
			{
				if( buffs->buff[i].skillID == 0 ) continue;
				// skill name (lParam - skillID)
				buffs->buff[i].getSkillName( text, 128 );
				added_idx = WULV_InsertItem( hwndLV, text, 0, buffs->buff[i].skillID );
				if( added_idx == -1 ) continue;
				// skill lvl
				wsprintf( text, TEXT("%d"), buffs->buff[i].skillLvl );
				WULV_SetItem( hwndLV, text, added_idx, 1 );
				// time
				format_bufftimeW( text, buffs->buff[i].duration );
				WULV_SetItem( hwndLV, text, added_idx, 2 );
				// skillID
				wsprintf( text, TEXT("%u"), buffs->buff[i].skillID );
				WULV_SetItem( hwndLV, text, added_idx, 3 );
			}
			for( i=0; i<UserBuffs::USER_MAX_SHORT_BUFFS; i++ )
			{
				if( buffs->short_buff[i].skillID == 0 ) continue;
				// skill name (lParam - skillID)
				buffs->short_buff[i].getSkillName( text, 128 );
				added_idx = WULV_InsertItem( hwndLV, text, 0, buffs->short_buff[i].skillID );
				if( added_idx == -1 ) continue;
				// skill lvl
				wsprintf( text, TEXT("%d"), buffs->short_buff[i].skillLvl );
				WULV_SetItem( hwndLV, text, added_idx, 1 );
				// time
				format_bufftimeW( text, buffs->short_buff[i].duration );
				WULV_SetItem( hwndLV, text, added_idx, 2 );
				// skillID
				wsprintf( text, TEXT("%u"), buffs->short_buff[i].skillID );
				WULV_SetItem( hwndLV, text, added_idx, 3 );
			}
		} break;
	case (WM_USER+1):
		{
			// update buff times
			UserBuffs *buffs = (UserBuffs *)lParam;
			if( !buffs ) return TRUE;
			wsprintf( text, TEXT("%d buffs"), buffs->buffCount );
			SetDlgItemText( hDlg, IDC_ST_NBUFFS, text );
			HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
			int i = 0;
			int count = ListView_GetItemCount( hwndLV );
			if( count < 1 ) break;
			for( i=0; i<count; i++ )
			{
				unsigned int skillID = (unsigned int)WULV_GetItemLPARAM( hwndLV, i );
				if( skillID == 0 ) continue;
				UserBuff buf;
				buffs->getBuffnfoBySkillId( skillID, &buf );
				format_bufftimeW( text, buf.duration );
				WULV_SetItem( hwndLV, text, i, 2 );
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}
