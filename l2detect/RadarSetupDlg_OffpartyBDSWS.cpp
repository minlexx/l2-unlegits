#include "stdafx.h"
#include "Resource.h"
#include "Logger.h"
#include "GameClient.h"
#include "UserAI_OffpartyBDSWS.h"

extern HINSTANCE g_radardll_hinst;
extern class GameClient *g_game_client; // in main.cpp

INT_PTR CALLBACK OFFBDSWS_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

HWND RadarSetupDlg_OFFBDSWS_Create( HWND hWndParent )
{
	return CreateDialogParam( g_radardll_hinst, MAKEINTRESOURCE(IDD_SR_OFFPARTYBDSWS),
		hWndParent, OFFBDSWS_DlgProc, 0 );
}

// =========================================================

UserAI_OffpartyBDSWS *rsobdsws;

void OFFBDSWS_Init( HWND hDlg );
void OFFBDSWS_OnDestroy( HWND hDlg );
void OFFBDSWS_OnApply( HWND hDlg );
void OFFBDSWS_OnSave( HWND hDlg );
void OFFBDSWS_OnEnable( HWND hDlg );
void OFFBDSWS_On_C_InvBD( HWND hDlg );
void OFFBDSWS_On_C_InvSWS( HWND hDlg );


INT_PTR CALLBACK OFFBDSWS_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	lParam;
	switch( uMsg )
	{
	case WM_INITDIALOG: OFFBDSWS_Init( hDlg ); break;
	case WM_DESTROY: { OFFBDSWS_OnDestroy( hDlg ); return FALSE; } break;
	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			case IDC_APPLY: OFFBDSWS_OnApply( hDlg ); break;
			case IDC_SAVE: OFFBDSWS_OnSave( hDlg ); break;
			case IDC_ENABLE: OFFBDSWS_OnEnable( hDlg ); break;
			case IDC_C_INVBD: OFFBDSWS_On_C_InvBD( hDlg ); break;
			case IDC_C_INVSWS: OFFBDSWS_On_C_InvSWS( hDlg ); break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

void OFFBDSWS_Init( HWND hDlg )
{
	//log_error( LOG_OK, "init\n" );
	rsobdsws = new UserAI_OffpartyBDSWS();
	rsobdsws->readFromFile( "L2Detect_offp_BDSWS.ini" );
	//
	HWND h;
	h = GetDlgItem( hDlg, IDC_ENABLE );
	if( rsobdsws->allEnabled ) SetWindowText( h, TEXT("Disable") );
	else SetWindowText( h, TEXT("Enable") );

	SetDlgItemText( hDlg, IDC_E_BDNAME, rsobdsws->nameBD );
	SetDlgItemText( hDlg, IDC_E_SWSNAME, rsobdsws->nameSWS );

	SetDlgItemInt( hDlg, IDC_E_INV_BD_SEC, rsobdsws->inv_bd_secs, TRUE );
	SetDlgItemInt( hDlg, IDC_E_INV_SWS_SEC, rsobdsws->inv_sws_secs, TRUE );

	SetDlgItemInt( hDlg, IDC_E_DISMISS_BD_SEC, rsobdsws->dismiss_bd_secs, TRUE );
	SetDlgItemInt( hDlg, IDC_E_DISMISS_SWS_SEC, rsobdsws->dismiss_sws_secs, TRUE );

	if( rsobdsws->inv_bd_enable ) CheckDlgButton( hDlg, IDC_C_INVBD, TRUE );
	else CheckDlgButton( hDlg, IDC_C_INVBD, FALSE );
	OFFBDSWS_On_C_InvBD( hDlg );

	if( rsobdsws->inv_sws_enable ) CheckDlgButton( hDlg, IDC_C_INVSWS, TRUE );
	else CheckDlgButton( hDlg, IDC_C_INVSWS, FALSE );
	OFFBDSWS_On_C_InvSWS( hDlg );
}


void OFFBDSWS_OnDestroy( HWND hDlg )
{
	hDlg;
	//log_error( LOG_OK, "close\n" );
	delete rsobdsws;
	rsobdsws = NULL;
}

void OFFBDSWS_OnApply( HWND hDlg )
{
	//log_error( LOG_OK, "OFFBDSWS_OnApply\n" );
	int c = IsDlgButtonChecked( hDlg, IDC_C_INVBD );
	if( c ) rsobdsws->inv_bd_enable = 1;
	else rsobdsws->inv_bd_enable = 0;
	c = IsDlgButtonChecked( hDlg, IDC_C_INVSWS );
	if( c ) rsobdsws->inv_sws_enable = 1;
	else rsobdsws->inv_sws_enable = 0;
	//
	GetDlgItemTextW( hDlg, IDC_E_BDNAME, rsobdsws->nameBD, 63 );
	GetDlgItemTextW( hDlg, IDC_E_SWSNAME, rsobdsws->nameSWS, 63 );
	//
	rsobdsws->inv_bd_secs = GetDlgItemInt( hDlg, IDC_E_INV_BD_SEC, NULL, TRUE );
	rsobdsws->inv_sws_secs = GetDlgItemInt( hDlg, IDC_E_INV_SWS_SEC, NULL, TRUE );
	//
	rsobdsws->dismiss_bd_secs = GetDlgItemInt( hDlg, IDC_E_DISMISS_BD_SEC, NULL, TRUE );
	rsobdsws->dismiss_sws_secs = GetDlgItemInt( hDlg, IDC_E_DISMISS_SWS_SEC, NULL, TRUE );
	//
	// apply setup immediately
	g_game_client->ai.setOffpartyBDSWS_Config( rsobdsws );
	//rsobdsws->saveToFile( "L2Detect_offp_BDSWS.ini" );
}

void OFFBDSWS_OnSave( HWND hDlg )
{
	hDlg;
	rsobdsws->saveToFile( "L2Detect_offp_BDSWS.ini" );
}

// apply enable state immediately
void OFFBDSWS_OnEnable( HWND hDlg )
{
	HWND h = GetDlgItem( hDlg, IDC_ENABLE );
	if( rsobdsws->allEnabled == 1 )
	{
		rsobdsws->allEnabled = 0;
		rsobdsws->saveToFile( "L2Detect_offp_BDSWS.ini" );
		g_game_client->ai.setOffpartyBDSWS_Enable( false );
		SetWindowText( h, TEXT("Enable") );
		return;
	}
	else
	{
		rsobdsws->allEnabled = 1;
		rsobdsws->saveToFile( "L2Detect_offp_BDSWS.ini" );
		g_game_client->ai.setOffpartyBDSWS_Enable( true );
		SetWindowText( h, TEXT("Disable") );
	}
}

void OFFBDSWS_On_C_InvBD( HWND hDlg )
{
	int c = IsDlgButtonChecked( hDlg, IDC_C_INVBD );
	if( c )
	{
		EnableWindow( GetDlgItem( hDlg, IDC_E_BDNAME ), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_E_INV_BD_SEC ), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_E_DISMISS_BD_SEC ), TRUE );
	}
	else
	{
		EnableWindow( GetDlgItem( hDlg, IDC_E_BDNAME ), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_E_INV_BD_SEC ), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_E_DISMISS_BD_SEC ), FALSE );
	}
}

void OFFBDSWS_On_C_InvSWS( HWND hDlg )
{
	int c = IsDlgButtonChecked( hDlg, IDC_C_INVSWS );
	if( c )
	{
		EnableWindow( GetDlgItem( hDlg, IDC_E_SWSNAME ), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_E_INV_SWS_SEC ), TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_E_DISMISS_SWS_SEC ), TRUE );
	}
	else
	{
		EnableWindow( GetDlgItem( hDlg, IDC_E_SWSNAME ), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_E_INV_SWS_SEC ), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_E_DISMISS_SWS_SEC ), FALSE );
	}
}
