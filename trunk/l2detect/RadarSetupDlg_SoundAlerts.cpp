#include "stdafx.h"
#include "Resource.h"
#include "Logger.h"
#include "GameClient.h"

extern HINSTANCE g_radardll_hinst;
extern class GameClient *g_game_client; // in main.cpp

INT_PTR CALLBACK SoundAlerts_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

HWND RadarSetupDlg_SoundAlerts_Create( HWND hWndParent )
{
	return CreateDialogParam( g_radardll_hinst, MAKEINTRESOURCE(IDD_SR_SOUNDALERTS),
		hWndParent, SoundAlerts_DlgProc, 0 );
}

// =========================================================

void SoundAlerts_Init( HWND hDlg );
void SoundAlerts_OnDestroy( HWND hDlg );
void SoundAlerts_OnApply( HWND hDlg );
void SoundAlerts_OnFind( HWND hDlg, int N );
void SoundAlerts_PlayFile( HWND hDlg, int N );


INT_PTR CALLBACK SoundAlerts_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	lParam;
	switch( uMsg )
	{
	case WM_INITDIALOG: SoundAlerts_Init( hDlg ); break;
	case WM_DESTROY: { SoundAlerts_OnDestroy( hDlg ); return FALSE; } break;
	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			case IDC_APPLY: SoundAlerts_OnApply( hDlg ); break;
			case IDC_B_FIND1: SoundAlerts_OnFind( hDlg, 1 ); break;
			case IDC_B_FIND2: SoundAlerts_OnFind( hDlg, 2 ); break;
			case IDC_B_FIND3: SoundAlerts_OnFind( hDlg, 3 ); break;
			case IDC_B_PLAY1: SoundAlerts_PlayFile( hDlg, 1 ); break;
			case IDC_B_PLAY2: SoundAlerts_PlayFile( hDlg, 2 ); break;
			case IDC_B_PLAY3: SoundAlerts_PlayFile( hDlg, 3 ); break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

void SoundAlerts_Init( HWND hDlg )
{
	// force UserAI to reload config file
	g_game_client->ai.loadSoundAlertsCfgFromFile();
	//
	HWND hwndCB = GetDlgItem( hDlg, IDC_CB_SOUNDTYPECW );
	SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)TEXT("Info") );
	SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)TEXT("Warning") );
	SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)TEXT("Critical") );
	hwndCB = GetDlgItem( hDlg, IDC_CB_SOUNDTYPETARGET );
	SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)TEXT("Info") );
	SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)TEXT("Warning") );
	SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)TEXT("Critical") );
	hwndCB = GetDlgItem( hDlg, IDC_CB_SOUNDTYPEINVISGM );
	SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)TEXT("Info") );
	SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)TEXT("Warning") );
	SendMessage( hwndCB, CB_ADDSTRING, 0, (LPARAM)TEXT("Critical") );
	// set initial CB checks
	if( g_game_client->ai.soundAlert_alertOnClanWar > 0 )
	{
		CheckDlgButton( hDlg, IDC_C_SOUNDENEMYENTER, g_game_client->ai.soundAlert_alertOnClanWar );
		hwndCB = GetDlgItem( hDlg, IDC_CB_SOUNDTYPECW );
		ComboBox_SetCurSel( hwndCB, g_game_client->ai.soundAlert_alertOnClanWar - 1 );
	}
	if( g_game_client->ai.soundAlert_alertOnTargeted > 0 )
	{
		CheckDlgButton( hDlg, IDC_C_SOUNDWHENTARGETED, g_game_client->ai.soundAlert_alertOnTargeted );
		hwndCB = GetDlgItem( hDlg, IDC_CB_SOUNDTYPETARGET );
		ComboBox_SetCurSel( hwndCB, g_game_client->ai.soundAlert_alertOnTargeted - 1 );
	}
	if( g_game_client->ai.soundAlert_alertOnInvisGM > 0 )
	{
		CheckDlgButton( hDlg, IDC_C_SOUNDONINVISGM, g_game_client->ai.soundAlert_alertOnTargeted );
		hwndCB = GetDlgItem( hDlg, IDC_CB_SOUNDTYPEINVISGM );
		ComboBox_SetCurSel( hwndCB, g_game_client->ai.soundAlert_alertOnInvisGM - 1 );
	}
	// no clan/party alert, screen message
	CheckDlgButton( hDlg, IDC_C_NOPARTYTARGETALERT, g_game_client->ai.soundAlert_noPartyTargetAlert );
	CheckDlgButton( hDlg, IDC_C_NOCLANTARGETALERT,  g_game_client->ai.soundAlert_noClanTargetAlert );
	CheckDlgButton( hDlg, IDC_C_TARGETALERTONSCREEN, g_game_client->ai.soundAlert_screenMessage );
	// set file names
	SetDlgItemTextA( hDlg, IDC_E_VERYCRITICALFILE, g_game_client->ai.soundAlert_fileNameCritical );
	SetDlgItemTextA( hDlg, IDC_E_WARNINGFILE,      g_game_client->ai.soundAlert_fileNameWarning );
	SetDlgItemTextA( hDlg, IDC_E_INFOFILE,         g_game_client->ai.soundAlert_fileNameInfo );
}


void SoundAlerts_OnDestroy( HWND hDlg )
{
	hDlg;
	//log_error( LOG_OK, "SoundAlerts_OnDestroy\n" );
}

void SoundAlerts_OnApply( HWND hDlg )
{
	//log_error( LOG_OK, "SoundAlerts_OnApply\n" );
	// apply dialog config to UserAI class
	HWND hwndcb = NULL;
	if( IsDlgButtonChecked( hDlg, IDC_C_SOUNDENEMYENTER ) )
	{
		hwndcb = GetDlgItem( hDlg, IDC_CB_SOUNDTYPECW );
		g_game_client->ai.soundAlert_alertOnClanWar = 1 + ComboBox_GetCurSel( hwndcb );
	}
	else g_game_client->ai.soundAlert_alertOnClanWar = 0;
	//
	if( IsDlgButtonChecked( hDlg, IDC_C_SOUNDWHENTARGETED ) )
	{
		hwndcb = GetDlgItem( hDlg, IDC_CB_SOUNDTYPETARGET );
		g_game_client->ai.soundAlert_alertOnTargeted = 1 + ComboBox_GetCurSel( hwndcb );
	}
	else g_game_client->ai.soundAlert_alertOnTargeted = 0;
	//
	if( IsDlgButtonChecked( hDlg, IDC_C_SOUNDONINVISGM ) )
	{
		hwndcb = GetDlgItem( hDlg, IDC_CB_SOUNDTYPEINVISGM );
		g_game_client->ai.soundAlert_alertOnInvisGM = 1 + ComboBox_GetCurSel( hwndcb );
	}
	else g_game_client->ai.soundAlert_alertOnInvisGM = 0;
	// no clan/party alert, screen message
	g_game_client->ai.soundAlert_noPartyTargetAlert = IsDlgButtonChecked( hDlg, IDC_C_NOPARTYTARGETALERT );
	g_game_client->ai.soundAlert_noClanTargetAlert = IsDlgButtonChecked( hDlg, IDC_C_NOCLANTARGETALERT );
	g_game_client->ai.soundAlert_screenMessage   = IsDlgButtonChecked( hDlg, IDC_C_TARGETALERTONSCREEN );
	// file names
	GetDlgItemTextA( hDlg, IDC_E_VERYCRITICALFILE, g_game_client->ai.soundAlert_fileNameCritical, 255 );
	GetDlgItemTextA( hDlg, IDC_E_WARNINGFILE,      g_game_client->ai.soundAlert_fileNameWarning, 255 );
	GetDlgItemTextA( hDlg, IDC_E_INFOFILE,         g_game_client->ai.soundAlert_fileNameInfo, 255 );
	// save to file
	g_game_client->ai.saveSoundAlertsCfgToFile();
}

void SoundAlerts_OnFind( HWND hDlg, int N )
{
	TCHAR fileName[256] = {0};
	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof(ofn) );
	ofn.lStructSize = sizeof(ofn);
	ofn.Flags = OFN_EXPLORER | OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
	ofn.hInstance = g_radardll_hinst;
	ofn.hwndOwner = hDlg;
	ofn.lpstrFile = fileName;
	ofn.lpstrTitle = TEXT("Choose sound file:");
	ofn.nMaxFile = 255;
	if( GetOpenFileName( &ofn ) )
	{
		//MessageBox( hDlg, fileName, TEXT("User choosed"), 0 );
		switch( N )
		{
		case 1: SetDlgItemText( hDlg, IDC_E_VERYCRITICALFILE, fileName ); break;
		case 2: SetDlgItemText( hDlg, IDC_E_WARNINGFILE, fileName ); break;
		case 3: SetDlgItemText( hDlg, IDC_E_INFOFILE, fileName ); break;
		}
	}
}

void SoundAlerts_PlayFile( HWND hDlg, int N )
{
	TCHAR text[256] = {0};
	if( N<1 || N>3 ) return;
	switch( N )
	{
	case 1: GetDlgItemText( hDlg, IDC_E_VERYCRITICALFILE, text, 255 ); break;
	case 2: GetDlgItemText( hDlg, IDC_E_WARNINGFILE, text, 255 ); break;
	case 3: GetDlgItemText( hDlg, IDC_E_INFOFILE, text, 255 ); break;
	}
	if( !PlaySound( text, NULL, SND_FILENAME | SND_ASYNC ) )
	{
		ErrorLogger_LogLastError( "PlaySoundW", GetLastError() );
		MessageBox( hDlg, TEXT("PlaySound() failed"), text, MB_ICONSTOP );
	}
}
