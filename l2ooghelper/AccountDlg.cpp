#include "StdAfx.h"
#include "AccountDlg.h"
#include "Resource.h"

extern HINSTANCE g_hInst;

bool AccountDlg::run( HWND hWndParent )
{
	INT_PTR ret = DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_ACCOUNT), hWndParent,
		AccountDlgProc, (LPARAM)&(this->account) );
	if( ret == IDOK ) return true;
	return false;
}

void AccountDlg_SetMembers( HWND hDlg, class Account *acc )
{
	//class Account *acc = (class Account *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	if( !acc ) return;
	HWND hwndCB = NULL;
	SetDlgItemTextW( hDlg, IDC_E_LOGIN, acc->login );
	SetDlgItemTextW( hDlg, IDC_E_PASS, acc->pass );
	SetDlgItemTextA( hDlg, IDC_E_IP, acc->loginServerIP );
	SetDlgItemInt( hDlg, IDC_E_PORT, acc->loginServerPort, TRUE );
	SetDlgItemInt( hDlg, IDC_E_GSID, acc->gameserverID, FALSE );
	SetDlgItemTextW( hDlg, IDC_E_CHARNAME, acc->charSelectName );
	if( acc->gameserverSelectManual )
	{
		CheckDlgButton( hDlg, IDC_C_GAME_MANUAL, TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_E_GSID ), FALSE );
	}
	else
	{
		CheckDlgButton( hDlg, IDC_C_GAME_MANUAL, FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_E_GSID ), TRUE );
	}
	if( acc->charSelectManual )
	{
		CheckDlgButton( hDlg, IDC_C_CHAR_MANUAL, TRUE );
		EnableWindow( GetDlgItem( hDlg, IDC_E_CHARNAME ), FALSE );
	}
	else
	{
		CheckDlgButton( hDlg, IDC_C_CHAR_MANUAL, FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_E_CHARNAME ), TRUE );
	}
	// combobox server version
	hwndCB = GetDlgItem( hDlg, IDC_CB_L2VER );
	ComboBox_ResetContent( hwndCB );
	ComboBox_AddString( hwndCB, TEXT("CT1 The Kamael") );
	ComboBox_AddString( hwndCB, TEXT("CT1.5 Hellbound") );
	ComboBox_AddString( hwndCB, TEXT("CT2 Gracia") );
	ComboBox_AddString( hwndCB, TEXT("CT2.2 Gracia Part 2") );
	ComboBox_AddString( hwndCB, TEXT("CT2.3 Gracia Final") );
	ComboBox_AddString( hwndCB, TEXT("CT2.4 Gracia Epilogue") );
	ComboBox_SetCurSel( hwndCB, acc->serverVersion );
	if( acc->useDefaultProtocolVersion )
	{
		EnableWindow( GetDlgItem( hDlg, IDC_E_PROTO ), FALSE );
		SetDlgItemText( hDlg, IDC_E_PROTO, TEXT("auto") );
		CheckDlgButton( hDlg, IDC_C_PROTOAUTO, TRUE );
	}
	else
	{
		CheckDlgButton( hDlg, IDC_C_PROTOAUTO, FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_E_PROTO ), TRUE );
		SetDlgItemInt( hDlg, IDC_E_PROTO, acc->protocolVersion, FALSE );
	}
	// config files
	hwndCB = GetDlgItem( hDlg, IDC_CB_CONFIG );
	ComboBox_ResetContent( hwndCB );
	ComboBox_AddString( hwndCB, TEXT("<none>") );
	if( acc->configFileName[0] == 0 ) ComboBox_SetCurSel( hwndCB, 0 );
	// scan folder "..\configs" for pattern "*.config"
	WIN32_FIND_DATA fdata;
	HANDLE hFind = FindFirstFile( TEXT(".\\configs\\*.config"), &fdata );
	if( hFind != INVALID_HANDLE_VALUE )
	{
		int add_idx = ComboBox_AddString( hwndCB, fdata.cFileName );
		if( _tcscmp( fdata.cFileName, acc->configFileName ) == 0 ) ComboBox_SetCurSel( hwndCB, add_idx );
		while( FindNextFile( hFind, &fdata ) )
		{
			add_idx = ComboBox_AddString( hwndCB, fdata.cFileName );
			if( _tcscmp( fdata.cFileName, acc->configFileName ) == 0 ) ComboBox_SetCurSel( hwndCB, add_idx );
		}
		FindClose( hFind );
	}
}

Account AccountDlg_GetMembers( HWND hDlg )
{
	HWND hwndCB = NULL;
	Account acc;
	GetDlgItemTextW( hDlg, IDC_E_LOGIN, acc.login, 31 );
	GetDlgItemTextW( hDlg, IDC_E_PASS, acc.pass, 31 );
	GetDlgItemTextA( hDlg, IDC_E_IP, acc.loginServerIP, 31 );
	acc.loginServerPort = GetDlgItemInt( hDlg, IDC_E_PORT, NULL, TRUE );
	acc.gameserverID = GetDlgItemInt( hDlg, IDC_E_GSID, NULL, FALSE );
	GetDlgItemTextW( hDlg, IDC_E_CHARNAME, acc.charSelectName, 31 );
	acc.gameserverSelectManual = IsDlgButtonChecked( hDlg, IDC_C_GAME_MANUAL );
	acc.charSelectManual = IsDlgButtonChecked( hDlg, IDC_C_CHAR_MANUAL );
	hwndCB = GetDlgItem( hDlg, IDC_CB_L2VER );
	acc.serverVersion = ComboBox_GetCurSel( hwndCB );
	if( IsDlgButtonChecked( hDlg, IDC_C_PROTOAUTO ) ) acc.useDefaultProtocolVersion = 1;
	else
	{
		acc.useDefaultProtocolVersion = 0;
		acc.protocolVersion = GetDlgItemInt( hDlg, IDC_E_PROTO, NULL, FALSE );
	}
	// config file name
	acc.configFileName[0] = 0;
	hwndCB = GetDlgItem( hDlg, IDC_CB_CONFIG );
	int cur_sel = ComboBox_GetCurSel( hwndCB );
	// >= 1 because string 0 is always "<none>", meaning that config file is not set
	if( cur_sel >= 1 ) GetWindowText( hwndCB, acc.configFileName, 511 );
	return acc;
}

void AccountDlg_UpdateEnabledWindows( HWND hDlg )
{
	if( IsDlgButtonChecked( hDlg, IDC_C_GAME_MANUAL ) ) EnableWindow( GetDlgItem( hDlg, IDC_E_GSID ), FALSE );
	else EnableWindow( GetDlgItem( hDlg, IDC_E_GSID ), TRUE );
	if( IsDlgButtonChecked( hDlg, IDC_C_CHAR_MANUAL ) ) EnableWindow( GetDlgItem( hDlg, IDC_E_CHARNAME ), FALSE );
	else EnableWindow( GetDlgItem( hDlg, IDC_E_CHARNAME ), TRUE );
	if( IsDlgButtonChecked( hDlg, IDC_C_PROTOAUTO ) )
	{
		EnableWindow( GetDlgItem( hDlg, IDC_E_PROTO ), FALSE );
		SetDlgItemText( hDlg, IDC_E_PROTO, TEXT("auto") );
	}
	else
	{
		EnableWindow( GetDlgItem( hDlg, IDC_E_PROTO ), TRUE );
		SetDlgItemInt( hDlg, IDC_E_PROTO, 12, FALSE );
	}
}

INT_PTR CALLBACK AccountDlg::AccountDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			SetWindowLongPtr( hDlg, GWLP_USERDATA, (LONG_PTR)lParam );
			AccountDlg_SetMembers( hDlg, (class Account *)lParam );
		} break;
	case WM_COMMAND:
		{
			switch( LOWORD( wParam ) )
			{
			case IDCANCEL: EndDialog( hDlg, IDCANCEL ); break;
			case IDOK:
				{
					class Account *acc = (class Account *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
					Account accCurrent = AccountDlg_GetMembers( hDlg );
					(*acc) = accCurrent;
					EndDialog( hDlg, IDOK );
				} break;
			case IDC_C_CHAR_MANUAL: AccountDlg_UpdateEnabledWindows( hDlg ); break;
			case IDC_C_GAME_MANUAL: AccountDlg_UpdateEnabledWindows( hDlg ); break;
			case IDC_C_PROTOAUTO:   AccountDlg_UpdateEnabledWindows( hDlg ); break;
			case IDC_LOAD:
				{
					// change subdir to '.\accounts'
					wchar_t curDir[256] = {0};
					wchar_t newDir[256] = {0};
					GetCurrentDirectoryW( 200, curDir );
					wsprintfW( newDir, L"%s\\accounts", curDir );
					SetCurrentDirectoryW( newDir );
					//
					wchar_t fileName[256] = {0};
					OPENFILENAMEW ofn;
					memset( &ofn, 0, sizeof(ofn) );
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hDlg;
					ofn.hInstance = g_hInst;
					ofn.lpstrFilter = TEXT("Acc files\0*.account\0All files\0*.*\0\0");
					ofn.lpstrFile = fileName;
					ofn.nMaxFile = 255;
					ofn.lpstrTitle = TEXT("Откуда читать?");
					ofn.lpstrInitialDir = newDir;
					ofn.Flags = OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
					if( GetOpenFileNameW( &ofn ) )
					{
						// restore cur dir (required to scan for configs, for example)
						SetCurrentDirectoryW( curDir );
						Account accLoaded;
						if( !accLoaded.loadFromFile( fileName ) )
						{
							MessageBox( hDlg, TEXT("Load failed!"), TEXT("Error!"), MB_ICONSTOP );
						}
						AccountDlg_SetMembers( hDlg, &accLoaded );
					}
					// restore cur dir (required to load maps, for example)
					SetCurrentDirectoryW( curDir );
				} break;
			case IDC_SAVE:
				{
					// change subdir to '.\accounts'
					wchar_t curDir[256] = {0};
					wchar_t newDir[256] = {0};
					GetCurrentDirectoryW( 200, curDir );
					wsprintfW( newDir, L"%s\\accounts", curDir );
					SetCurrentDirectoryW( newDir );
					//
					wchar_t fileName[256] = {0};
					OPENFILENAMEW ofn;
					memset( &ofn, 0, sizeof(ofn) );
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hDlg;
					ofn.hInstance = g_hInst;
					ofn.lpstrFilter = TEXT("Acc files\0*.account\0All files\0*.*\0\0");
					ofn.lpstrFile = fileName;
					ofn.nMaxFile = 255;
					ofn.lpstrTitle = TEXT("Куда писать?");
					ofn.lpstrInitialDir = newDir;
					ofn.Flags = OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
					if( GetSaveFileNameW( &ofn ) )
					{
						// restore cur dir (required to load maps, for example)
						SetCurrentDirectoryW( curDir );
						if( wcsstr( fileName, L".account" ) == NULL ) wcscat( fileName, L".account" );
						Account accCurrent = AccountDlg_GetMembers( hDlg );
						if( !accCurrent.saveToFile( fileName ) )
						{
							MessageBox( hDlg, TEXT("Save failed!"), TEXT("Error!"), MB_ICONSTOP );
						}
					}
					// restore cur dir (required to load maps, for example)
					SetCurrentDirectoryW( curDir );
				} break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

