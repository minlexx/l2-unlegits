#include "stdafx.h"
#include "Resource.h"
#include "Logger.h"
#include "ConfigIni.h"

extern HINSTANCE g_radardll_hinst;
extern CConfig g_cfg;

INT_PTR CALLBACK RadarSetupDlg_InvisGM_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

HWND RadarSetupDlg_InvisGM_Create( HWND hWndParent )
{
	return CreateDialogParam( g_radardll_hinst, MAKEINTRESOURCE(IDD_SR_INVISGM),
		hWndParent, RadarSetupDlg_InvisGM_DlgProc, 0 );
}

// =========================================================

void RadarSetupDlg_InvisGM_Init( HWND hDlg );
void RadarSetupDlg_InvisGM_OnDestroy( HWND hDlg );
void RadarSetupDlg_InvisGM_OnApply( HWND hDlg );


INT_PTR CALLBACK RadarSetupDlg_InvisGM_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	lParam;
	switch( uMsg )
	{
	case WM_INITDIALOG: RadarSetupDlg_InvisGM_Init( hDlg ); break;
	case WM_DESTROY: { RadarSetupDlg_InvisGM_OnDestroy( hDlg ); return FALSE; } break;
	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			case IDC_APPLY: RadarSetupDlg_InvisGM_OnApply( hDlg ); break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

void RadarSetupDlg_InvisGM_Init( HWND hDlg )
{
	CheckDlgButton( hDlg, IDC_C_INVIS_GM_ENABLE, g_cfg.InvisGMTrackEnable );
	SetDlgItemInt( hDlg, IDC_E_INVIS_GM_SPEED, g_cfg.InvisGMSpeed, TRUE );
}


void RadarSetupDlg_InvisGM_OnDestroy( HWND hDlg )
{
	UNREFERENCED_PARAMETER(hDlg);
}

void RadarSetupDlg_InvisGM_OnApply( HWND hDlg )
{
	g_cfg.InvisGMTrackEnable = IsDlgButtonChecked( hDlg, IDC_C_INVIS_GM_ENABLE );
	g_cfg.InvisGMSpeed = GetDlgItemInt( hDlg, IDC_E_INVIS_GM_SPEED, NULL, TRUE );
	g_cfg.SaveConfig();
}

