#include "stdafx.h"
#include "Resource.h"
#include "windowUtils.h"
#include "UserSkills.h"
#include "logger.h"
#include "IL2Client.h"
#include "DBLayer.h"

extern HINSTANCE g_hInst;

INT_PTR CALLBACK UserSkillsTabDlg_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

HWND UserSkillsTabDlg_Create( HWND hWndParent, IL2Client *pcl )
{
	HWND hDlg = CreateDialogParam( g_hInst, MAKEINTRESOURCE(IDD_USERSKILLS),
		hWndParent, UserSkillsTabDlg_Proc, 0 );
	SetWindowLongPtr( hDlg, GWLP_USERDATA, (LONG_PTR)pcl );
	return hDlg;
}

void UserSkillsTabDlg_UpdateAllSkills( HWND hDlg, UserSkills *sk );
void UserSkillsTabDlg_UpdateSkillsCoolTimes( HWND hDlg, UserSkills *sk );
void UserSkillsTabDlg_OnNotify( HWND hDlg, WPARAM wParam, LPARAM lParam );

INT_PTR CALLBACK UserSkillsTabDlg_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			HWND hWndTab = GetDlgItem( hDlg, IDC_TAB2 );
			WUTab_InsertItem( hWndTab, TEXT("Активные"), 0 );
			WUTab_InsertItem( hWndTab, TEXT("Пассивные"), 1 );
			//
			RECT rc;
			GetClientRect( hWndTab, &rc );
			HWND hwndLV = CreateWindowEx( WS_EX_CLIENTEDGE,
				WC_LISTVIEW, NULL,
				WS_VSCROLL | WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SORTASCENDING | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
				2, 24, rc.right - rc.left - 5, rc.bottom - rc.top - 30,
				hWndTab, (HMENU)100, g_hInst, NULL );
			WULV_SetExtStyle( hwndLV, true, true );
			WULV_InsertColumn( hwndLV, TEXT("Скилл"), 150, 0 );
			WULV_InsertColumn( hwndLV, TEXT("lv"), 40, 1 );
			WULV_InsertColumn( hwndLV, TEXT("cool"), 60, 2 );
			WULV_InsertColumn( hwndLV, TEXT("skillID"), 50, 3 );
			//
			hwndLV = CreateWindowEx( WS_EX_CLIENTEDGE,
				WC_LISTVIEW, NULL,
				WS_VSCROLL | WS_CHILD | /*WS_VISIBLE |*/ LVS_REPORT | LVS_SORTASCENDING | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
				2, 24, rc.right - rc.left - 5, rc.bottom - rc.top - 30,
				hWndTab, (HMENU)101, g_hInst, NULL );
			WULV_SetExtStyle( hwndLV, true, true );
			WULV_InsertColumn( hwndLV, TEXT("Скилл"), 150, 0 );
			WULV_InsertColumn( hwndLV, TEXT("lv"), 50, 1 );
			WULV_InsertColumn( hwndLV, TEXT("skillID"), 50, 2 );
		} break;
	case WM_NOTIFY: UserSkillsTabDlg_OnNotify( hDlg, wParam, lParam ); break;
	case WM_USER:
		{
			UserSkills *sk = (UserSkills *)lParam;
			if( !sk ) return TRUE;
			UserSkillsTabDlg_UpdateAllSkills( hDlg, sk );
		} break;
	case (WM_USER+1):
		{
			UserSkills *sk = (UserSkills *)lParam;
			if( !sk ) return TRUE;
			UserSkillsTabDlg_UpdateSkillsCoolTimes( hDlg, sk );
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

void UserSkillsTabDlg_UpdateAllSkills( HWND hDlg, UserSkills *sk )
{
	TCHAR text[256] = {0}; // ?
	HWND hWndTab = GetDlgItem( hDlg, IDC_TAB2 );
	HWND hwndLVA = GetDlgItem( hWndTab, 100 );
	HWND hwndLVP = GetDlgItem( hWndTab, 101 );
	int i;
	ListView_DeleteAllItems( hwndLVA );
	ListView_DeleteAllItems( hwndLVP );
	int added_item;
	for( i=0; i<UserSkills::USERSKILL_MAX_SKILLS; i++ )
	{
		if( sk->skill[i].skillID == 0 ) continue;
		// skill name (lParam is skillID)
		sk->skill[i].getSkillName( text, 256 );
		if( sk->skill[i].isPassive == 0 )
			added_item = WULV_InsertItem( hwndLVA, text, 0, sk->skill[i].skillID );
		else
			added_item = WULV_InsertItem( hwndLVP, text, 0, sk->skill[i].skillID );
		// lvl
		wsprintf( text, TEXT("%d"), sk->skill[i].level );
		if( sk->skill[i].isPassive == 0 )
			WULV_SetItem( hwndLVA, text, added_item, 1 );
		else
			WULV_SetItem( hwndLVP, text, added_item, 1 );
		// coolTime for active
		if( sk->skill[i].isPassive == 0 )
		{
			wsprintf( text, TEXT("%d / %d"), sk->skill[i].coolTimeRemaining, sk->skill[i].coolTimeReuse );
			WULV_SetItem( hwndLVA, text, added_item, 2 );
		}
		// skill id
		wsprintf( text, TEXT("%d"), sk->skill[i].skillID );
		if( sk->skill[i].isPassive == 0 )
			WULV_SetItem( hwndLVA, text, added_item, 3 );
		else
			WULV_SetItem( hwndLVP, text, added_item, 2 );
	}
}

void UserSkillsTabDlg_UpdateSkillsCoolTimes( HWND hDlg, UserSkills *sk )
{
	TCHAR text[256] = {0}; // ?
	HWND hWndTab = GetDlgItem( hDlg, IDC_TAB2 );
	HWND hwndLV = GetDlgItem( hWndTab, 100 ); // only for active skills cooltime
	int cnt = ListView_GetItemCount( hwndLV );
	if( cnt < 1 ) return;
	int i;
	for( i=0; i<cnt; i++ )
	{
		unsigned int skillID = (unsigned int)WULV_GetItemLPARAM( hwndLV, i );
		if( skillID == 0 ) continue;
		UserSkill skill;
		sk->getSkillInfoBySkillId( skillID, &skill );
		if( skill.skillID == 0 ) continue;
		// update item text if cooltime in progress :)
		if( skill.coolTimeReuse > 0 )
		{
			// print new coolTime
			wsprintf( text, TEXT("%d / %d"), skill.coolTimeRemaining, skill.coolTimeReuse );
			WULV_SetItem( hwndLV, text, i, 2 );
			// debug
			//log_error_np( LOG_OK, "UpdateSkillsCoolTimes(): updated cooltime for skill %u [%S]\n",
			//	skill.skillID, skill.getSkillNameW() );
		}
	}
}

void UserSkillsTabDlg_OnNotify( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(wParam);
	IL2Client *pcl = (IL2Client *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	LPNMHDR phdr = (LPNMHDR)lParam;
	HWND hWndTab = GetDlgItem( hDlg, IDC_TAB2 );
	if( phdr->hwndFrom == hWndTab )
	{
		int cur_sel = TabCtrl_GetCurSel( hWndTab );
		HWND hwndLVA = GetDlgItem( hWndTab, 100 );
		HWND hwndLVP = GetDlgItem( hWndTab, 101 );
		switch( phdr->code )
		{
		case TCN_SELCHANGING:
			{
				if( cur_sel == 0 ) ShowWindow( hwndLVA, SW_HIDE );
				if( cur_sel == 1 ) ShowWindow( hwndLVP, SW_HIDE );
			} break;
		case TCN_SELCHANGE:
			{
				if( cur_sel == 0 ) ShowWindow( hwndLVA, SW_SHOW );
				if( cur_sel == 1 ) ShowWindow( hwndLVP, SW_SHOW );
			} break;
		}
		return;
	}
	HWND hwndLVA = GetDlgItem( hWndTab, 100 ); // active skills LV
	if( phdr->hwndFrom == hwndLVA )
	{
		if( phdr->code == NM_DBLCLK )
		{
			LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE) lParam;
			unsigned int skillID = (unsigned int)WULV_GetItemLPARAM( hwndLVA, lpnmitem->iItem );
			if( skillID > 0 )
			{
				//log_error( LOG_OK, "Use skill %u\n", skillID );
				pcl->game_SkillUse( skillID );
			}
		}
	}
}
