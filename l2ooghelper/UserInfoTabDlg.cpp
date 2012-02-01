#include "stdafx.h"
#include "Resource.h"
#include "UserInfo_c.h"

extern HINSTANCE g_hInst;

INT_PTR CALLBACK UserInfoTabDlg_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

HWND UserInfoTabDlg_Create( HWND hWndParent )
{
	HWND hDlg = CreateDialogParam( g_hInst, MAKEINTRESOURCE(IDD_USERINFO),
		hWndParent, UserInfoTabDlg_Proc, 0 );
	return hDlg;
}

INT_PTR CALLBACK UserInfoTabDlg_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(wParam);
	switch( uMsg )
	{
	case WM_INITDIALOG: break;
	case WM_USER:
		{
			UserInfo *usr = (UserInfo *)lParam;
			if( !usr ) return TRUE;
			TCHAR text[256] = {0};
			// exp
			wsprintf( text, TEXT("%I64u"), usr->experience );
			SetDlgItemText( hDlg, IDC_ST_EXP, text );
			// SP
			SetDlgItemInt( hDlg, IDC_ST_SP, usr->skill_points, FALSE );
			//
			SetDlgItemInt( hDlg, IDC_ST_PATK, usr->pAtk, FALSE );
			SetDlgItemInt( hDlg, IDC_ST_MATK, usr->mAtk, FALSE );
			SetDlgItemInt( hDlg, IDC_ST_PDEF, usr->pDef, FALSE );
			SetDlgItemInt( hDlg, IDC_ST_MDEF, usr->mDef, FALSE );
			SetDlgItemInt( hDlg, IDC_ST_ATKSPD, usr->pAtkSpd, FALSE );
			SetDlgItemInt( hDlg, IDC_ST_CASTSPEED, usr->mAtkSpd, FALSE );
			SetDlgItemInt( hDlg, IDC_ST_ACCURACY, usr->accuracy, FALSE );
			SetDlgItemInt( hDlg, IDC_ST_EVASION, usr->evasion, FALSE );
			SetDlgItemInt( hDlg, IDC_ST_CRITICAL, usr->critical, FALSE );
			if( usr->isRunning ) SetDlgItemInt( hDlg, IDC_ST_SPEED, usr->runSpeed, FALSE );
			else SetDlgItemInt( hDlg, IDC_ST_SPEED, usr->walkSpeed, FALSE );
			// str dex con int wit men
			SetDlgItemInt( hDlg, IDC_ST_STR, usr->s_STR, FALSE );
			SetDlgItemInt( hDlg, IDC_ST_DEX, usr->s_DEX, FALSE );
			SetDlgItemInt( hDlg, IDC_ST_CON, usr->s_CON, FALSE );
			SetDlgItemInt( hDlg, IDC_ST_INT, usr->s_INT, FALSE );
			SetDlgItemInt( hDlg, IDC_ST_WIT, usr->s_WIT, FALSE );
			SetDlgItemInt( hDlg, IDC_ST_MEN, usr->s_MEN, FALSE );
			// social
			SetDlgItemInt( hDlg, IDC_ST_KARMA, usr->karma, FALSE );
			wsprintf( text, TEXT("%d/%d"), usr->pvpKills, usr->pkKills );
			SetDlgItemText( hDlg, IDC_ST_PVPPK, text );
			wsprintf( text, TEXT("%d/%d"), usr->recomHave, usr->recomLeft );
			SetDlgItemText( hDlg, IDC_ST_RECOM, text );
			SetDlgItemInt( hDlg, IDC_ST_FAME, usr->fame, FALSE );
			// attributes
			SetDlgItemInt( hDlg, IDC_ST_ATKATTRIB, usr->elements.attackElementType, TRUE );
			SetDlgItemInt( hDlg, IDC_ST_ATTRIBVAL, usr->elements.attackElementValue, TRUE );
			SetDlgItemInt( hDlg, IDC_ST_FIRE, usr->elements.defAttrFire, TRUE );
			SetDlgItemInt( hDlg, IDC_ST_WATER, usr->elements.defAttrWater, TRUE );
			SetDlgItemInt( hDlg, IDC_ST_WIND, usr->elements.defAttrWind, TRUE );
			SetDlgItemInt( hDlg, IDC_ST_EARTH, usr->elements.defAttrEarth, TRUE );
			SetDlgItemInt( hDlg, IDC_ST_HOLY, usr->elements.defAttrHoly, TRUE );
			SetDlgItemInt( hDlg, IDC_ST_DARK, usr->elements.defAttrUnholy, TRUE );
			// vitality level
			SetDlgItemInt( hDlg, IDC_ST_VITALITYLEVEL, usr->vitalityLevel, FALSE );
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}
