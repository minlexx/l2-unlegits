#include "stdafx.h"
#include "Resource.h"
#include "Logger.h"
#include "GameClient.h"

extern HINSTANCE g_radardll_hinst;
extern class GameClient *g_game_client;

INT_PTR CALLBACK RadarSetupDlg_LockTarget_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

HWND RadarSetupDlg_LockTarget_Create( HWND hWndParent )
{
	return CreateDialogParam( g_radardll_hinst, MAKEINTRESOURCE(IDD_SR_LOCKTARGET),
		hWndParent, RadarSetupDlg_LockTarget_DlgProc, 0 );
}

// =========================================================

void RadarSetupDlg_LockTarget_Init( HWND hDlg );
void RadarSetupDlg_LockTarget_OnDestroy( HWND hDlg );
void RadarSetupDlg_LockTarget_OnApply( HWND hDlg );


INT_PTR CALLBACK RadarSetupDlg_LockTarget_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	lParam;
	switch( uMsg )
	{
	case WM_INITDIALOG: RadarSetupDlg_LockTarget_Init( hDlg ); break;
	case WM_DESTROY: { RadarSetupDlg_LockTarget_OnDestroy( hDlg ); return FALSE; } break;
	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			case IDC_APPLY: RadarSetupDlg_LockTarget_OnApply( hDlg ); break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

void RadarSetupDlg_LockTarget_Init( HWND hDlg )
{
	int e = g_game_client->ai.lockTargetMgr.isEnabled() ? 1 : 0;
	CheckDlgButton( hDlg, IDC_C_LT_ENABLED, e );
}


void RadarSetupDlg_LockTarget_OnDestroy( HWND hDlg )
{
	UNREFERENCED_PARAMETER(hDlg);
}

void RadarSetupDlg_LockTarget_OnApply( HWND hDlg )
{
	int e = IsDlgButtonChecked( hDlg, IDC_C_LT_ENABLED );
	if( e == 0 )
		g_game_client->ai.lockTargetMgr.setEnable( false );
	else
		g_game_client->ai.lockTargetMgr.setEnable( true );
}

