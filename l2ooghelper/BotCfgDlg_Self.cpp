#include "stdafx.h"
#include "Resource.h"
#include "windowUtils.h"
#include "logger.h"
#include "L2Client.h"
#include "IL2Client.h"
#include "BotCfgDlg.h"

extern HINSTANCE g_hInst;

INT_PTR CALLBACK BotCfg_Self_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

HWND BotCfgDlg_Self_Create( HWND hWndParent, BotConfig *pcfg )
{
	return CreateDialogParam( g_hInst, MAKEINTRESOURCE(IDD_BOTCFG_SELF),
		hWndParent, BotCfg_Self_DlgProc, (LPARAM)pcfg );
}

void BotCfg_Self_init( HWND hDlg, BotConfig *cfg );
void BotCfg_Self_apply( HWND hDlg, BotConfig *cfg );

INT_PTR CALLBACK BotCfg_Self_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			BotConfig *cfg = (BotConfig *)lParam;
			SetWindowLongPtrW( hDlg, GWLP_USERDATA, (LONG_PTR)cfg );
			//
			HWND hwndCB = GetDlgItem( hDlg, IDC_CB_AI_MODEL );
			ComboBox_ResetContent( hwndCB );
			ComboBox_AddString( hwndCB, TEXT("Обобщенный") );
			ComboBox_AddString( hwndCB, TEXT("Боец ближнего боя") );
			ComboBox_AddString( hwndCB, TEXT("Лучник") );
			ComboBox_AddString( hwndCB, TEXT("Нюкер") );
			ComboBox_AddString( hwndCB, TEXT("Хилер/баффер/речаржер") );
			ComboBox_SetCurSel( hwndCB, 0 );
			//
			BotCfg_Self_init( hDlg, cfg );
		} break;
	case WM_COMMAND:
		{
			BotConfig *cfg = (BotConfig *)GetWindowLongPtrW( hDlg, GWLP_USERDATA );
			int ctrlID = LOWORD(wParam);
			switch( ctrlID )
			{
			case IDC_APPLY: BotCfg_Self_apply( hDlg, cfg ); break;
			case IDC_LOAD: BotCfg_Self_init( hDlg, cfg ); break;
			default: return FALSE; break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

void BotCfg_Self_init( HWND hDlg, BotConfig *cfg )
{
	HWND hwndCB = GetDlgItem( hDlg, IDC_CB_AI_MODEL );
	ComboBox_SetCurSel( hwndCB, cfg->getValInt( "ai_model" ) );
}

void BotCfg_Self_apply( HWND hDlg, BotConfig *cfg )
{
	HWND hwndCB = GetDlgItem( hDlg, IDC_CB_AI_MODEL );
	cfg->setValInt( "ai_model", ComboBox_GetCurSel( hwndCB ) );
}
