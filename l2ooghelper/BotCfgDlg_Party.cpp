#include "stdafx.h"
#include "Resource.h"
#include "windowUtils.h"
#include "logger.h"
#include "L2Client.h"
#include "IL2Client.h"
#include "BotCfgDlg.h"

extern HINSTANCE g_hInst;

INT_PTR CALLBACK BotCfg_Party_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

HWND BotCfgDlg_Party_Create( HWND hWndParent, BotConfig *pcfg )
{
	return CreateDialogParam( g_hInst, MAKEINTRESOURCE(IDD_BOTCFG_PARTY),
		hWndParent, BotCfg_Party_DlgProc, (LPARAM)pcfg );
}

void BotCfg_Party_init( HWND hDlg, BotConfig *cfg );
void BotCfg_Party_apply( HWND hDlg, BotConfig *cfg );

INT_PTR CALLBACK BotCfg_Party_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			BotConfig *cfg = (BotConfig *)lParam;
			SetWindowLongPtrW( hDlg, GWLP_USERDATA, (LONG_PTR)cfg );
			BotCfg_Party_init( hDlg, cfg );
		} break;
	case WM_COMMAND:
		{
			BotConfig *cfg = (BotConfig *)GetWindowLongPtrW( hDlg, GWLP_USERDATA );
			int ctrlID = LOWORD(wParam);
			switch( ctrlID )
			{
			case IDC_APPLY: BotCfg_Party_apply( hDlg, cfg ); break;
			case IDC_LOAD: BotCfg_Party_init( hDlg, cfg ); break;
			default: return FALSE; break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

void BotCfg_Party_init( HWND hDlg, BotConfig *cfg )
{
	// init loot combobox
	HWND hwndCB = GetDlgItem( hDlg, IDC_CB_PARTY_LOOT );
	ComboBox_ResetContent( hwndCB );
	ComboBox_AddString( hwndCB, TEXT("Нашедшему") );
	ComboBox_AddString( hwndCB, TEXT("Случайно") );
	ComboBox_AddString( hwndCB, TEXT("Случайно, включая спойл") );
	ComboBox_AddString( hwndCB, TEXT("По очереди") );
	ComboBox_AddString( hwndCB, TEXT("По очереди, включая спойл") );
	int loot = cfg->getValInt( "party_loot" );
	ComboBox_SetCurSel( hwndCB, loot );
	//
	CheckDlgButton( hDlg, IDC_C_PARTY_AUTO_ACCEPT, cfg->getValInt( "party_auto_accept_enable" ) );
	SetDlgItemTextA( hDlg, IDC_E_PARTY_AUTO_ACCEPT, cfg->getValStr( "party_auto_accept_names" ) );
	CheckDlgButton( hDlg, IDC_C_PARTY_AUTO_INVITE, cfg->getValInt( "party_auto_invite_enable" ) );
	SetDlgItemTextA( hDlg, IDC_E_PARTY_AUTO_INVITE, cfg->getValStr( "party_auto_invite_names" ) );
	//
	CheckDlgButton( hDlg, IDC_C_FOLLOW_ENABLE, cfg->getValInt( "follow_enable" ) );
	SetDlgItemTextA( hDlg, IDC_E_FOLLOW_NAME, cfg->getValStr( "follow_name" ) );
	SetDlgItemInt( hDlg, IDC_E_FOLLOW_DIST, cfg->getValInt( "follow_dist" ), TRUE );
	//
	CheckDlgButton( hDlg, IDC_C_PARTY_ASSIST, cfg->getValInt( "party_assist_enable" ) );
}

void BotCfg_Party_apply( HWND hDlg, BotConfig *cfg )
{
	char atext[512] = {0};
	// loot CB
	HWND hwndCB = GetDlgItem( hDlg, IDC_CB_PARTY_LOOT );
	cfg->setValInt( "party_loot", ComboBox_GetCurSel( hwndCB ) );
	//
	cfg->setValInt( "party_auto_accept_enable", IsDlgButtonChecked( hDlg, IDC_C_PARTY_AUTO_ACCEPT ) );
	GetDlgItemTextA( hDlg, IDC_E_PARTY_AUTO_ACCEPT, atext, 255 );
	cfg->setValStr( "party_auto_accept_names", atext );
	cfg->setValInt( "party_auto_invite_enable", IsDlgButtonChecked( hDlg, IDC_C_PARTY_AUTO_INVITE ) );
	GetDlgItemTextA( hDlg, IDC_E_PARTY_AUTO_INVITE, atext, 255 );
	cfg->setValStr( "party_auto_invite_names", atext );
	//
	cfg->setValInt( "follow_enable", IsDlgButtonChecked( hDlg, IDC_C_FOLLOW_ENABLE ) );
	GetDlgItemTextA( hDlg, IDC_E_FOLLOW_NAME, atext, 255 );
	cfg->setValStr( "follow_name", atext );
	cfg->setValInt( "follow_dist", GetDlgItemInt( hDlg, IDC_E_FOLLOW_DIST, NULL, TRUE ) );
	//
	cfg->setValInt( "party_assist_enable", IsDlgButtonChecked( hDlg, IDC_C_PARTY_ASSIST ) );
}
