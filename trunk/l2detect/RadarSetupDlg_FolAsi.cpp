#include "stdafx.h"
#include "Resource.h"
#include "Logger.h"
#include "GameClient.h"
#include "DlgPressKey.h"
#include "windowUtils.h"
#include "RadarSetupDlg_FolAsi.h"

extern HINSTANCE g_radardll_hinst;
extern class GameClient *g_game_client; // in main.cpp

INT_PTR CALLBACK RadarSetupDlg_FollowAssist_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

HWND RadarSetupDlg_FolAsi_Create( HWND hWndParent )
{
	return CreateDialogParam( g_radardll_hinst, MAKEINTRESOURCE(IDD_SR_FOLLOWASSIST),
		hWndParent, RadarSetupDlg_FollowAssist_DlgProc, 0 );
}

// =========================================================

void RadarSetupDlg_FollowAssist_Init( HWND hDlg );
void RadarSetupDlg_FollowAssist_OnDestroy( HWND hDlg );
void RadarSetupDlg_FollowAssist_OnApply( HWND hDlg );
// =====================================
void RadarSetupDlg_FollowAssist_UpdateDisabledWindows( HWND hDlg );


INT_PTR CALLBACK RadarSetupDlg_FollowAssist_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam);
	switch( uMsg )
	{
	case WM_INITDIALOG: RadarSetupDlg_FollowAssist_Init( hDlg ); break;
	case WM_DESTROY: { RadarSetupDlg_FollowAssist_OnDestroy( hDlg ); return FALSE; } break;
	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			case IDC_APPLY: RadarSetupDlg_FollowAssist_OnApply( hDlg ); break;
			case IDC_C_FOLLOW_ENABLE:
			case IDC_C_ASSIST_ENABLE:
				RadarSetupDlg_FollowAssist_UpdateDisabledWindows( hDlg );
				break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

void RadarSetupDlg_FollowAssist_Init( HWND hDlg )
{
	// force UserAI to reload config file
	g_game_client->ai.folAsiCfg.loadFromFile( "L2Detect_folAsi.ini" );
	// follow sets
	CheckDlgButton( hDlg, IDC_C_FOLLOW_ENABLE, g_game_client->ai.folAsiCfg.m_follow_enable );
	SetDlgItemInt( hDlg, IDC_E_FOLLOW_DISTANCE, g_game_client->ai.folAsiCfg.m_follow_dist, FALSE );
	SetDlgItemTextW( hDlg, IDC_E_NAME_FOLLOW, g_game_client->ai.folAsiCfg.m_follow_name );
	CheckDlgButton( hDlg, IDC_C_FOLLOW_ONLY_IN_PARTY, g_game_client->ai.folAsiCfg.m_follow_only_in_party );
	// assist sets
	CheckDlgButton( hDlg, IDC_C_ASSIST_ENABLE, g_game_client->ai.folAsiCfg.m_assist_enable );
	SetDlgItemTextW( hDlg, IDC_E_NAME_ASSIST, g_game_client->ai.folAsiCfg.m_assist_name );
	CheckDlgButton( hDlg, IDC_C_ASSIST_ONLY_IN_PARTY, g_game_client->ai.folAsiCfg.m_assist_only_in_party );
	CheckDlgButton( hDlg, IDC_C_DONT_ASSIST_ALLIES, g_game_client->ai.folAsiCfg.m_assist_dont_ally );
	//
	RadarSetupDlg_FollowAssist_UpdateDisabledWindows( hDlg );
}


void RadarSetupDlg_FollowAssist_OnDestroy( HWND hDlg )
{
	UNREFERENCED_PARAMETER(hDlg);
}

void RadarSetupDlg_FollowAssist_OnApply( HWND hDlg )
{
	// follow sets
	g_game_client->ai.folAsiCfg.m_follow_enable = IsDlgButtonChecked( hDlg, IDC_C_FOLLOW_ENABLE );
	g_game_client->ai.folAsiCfg.m_follow_dist = (int)GetDlgItemInt( hDlg, IDC_E_FOLLOW_DISTANCE, NULL, FALSE );
	GetDlgItemTextW( hDlg, IDC_E_NAME_FOLLOW, g_game_client->ai.folAsiCfg.m_follow_name, 127 );
	g_game_client->ai.folAsiCfg.m_follow_only_in_party = IsDlgButtonChecked( hDlg, IDC_C_FOLLOW_ONLY_IN_PARTY );
	// assist sets
	g_game_client->ai.folAsiCfg.m_assist_enable = IsDlgButtonChecked( hDlg, IDC_C_ASSIST_ENABLE );
	GetDlgItemTextW( hDlg, IDC_E_NAME_ASSIST, g_game_client->ai.folAsiCfg.m_assist_name, 127 );
	g_game_client->ai.folAsiCfg.m_assist_only_in_party = IsDlgButtonChecked( hDlg, IDC_C_ASSIST_ONLY_IN_PARTY );
	g_game_client->ai.folAsiCfg.m_assist_dont_ally = IsDlgButtonChecked( hDlg, IDC_C_DONT_ASSIST_ALLIES );
	// save config to file
	g_game_client->ai.folAsiCfg.saveToFile( "L2Detect_folAsi.ini" );
	// checks
	if( !g_game_client->ai.folAsiCfg.m_follow_enable )
		g_game_client->ai.followDisable(); // zero follow objectID
	if( !g_game_client->ai.folAsiCfg.m_assist_enable )
		g_game_client->ai.assistDisable(); // zero assist objectID
}

void RadarSetupDlg_FollowAssist_UpdateDisabledWindows( HWND hDlg )
{
	BOOL b = FALSE;
	if( IsDlgButtonChecked( hDlg, IDC_C_FOLLOW_ENABLE ) ) b = TRUE;
	EnableWindow( GetDlgItem( hDlg, IDC_E_NAME_FOLLOW ), b );
	EnableWindow( GetDlgItem( hDlg, IDC_C_FOLLOW_ONLY_IN_PARTY ), b );
	EnableWindow( GetDlgItem( hDlg, IDC_E_FOLLOW_DISTANCE ), b );
	b = FALSE;
	if( IsDlgButtonChecked( hDlg, IDC_C_ASSIST_ENABLE ) ) b = TRUE;
	EnableWindow( GetDlgItem( hDlg, IDC_E_NAME_ASSIST ), b );
	EnableWindow( GetDlgItem( hDlg, IDC_C_ASSIST_ONLY_IN_PARTY ), b );
	EnableWindow( GetDlgItem( hDlg, IDC_C_DONT_ASSIST_ALLIES ), b );
}
