#include "stdafx.h"
#include "Resource.h"
#include "ConfigIni.h"
#include "ConfigDlg.h"
#include "Logger.h"
// listeners
#include "LoginListener.h"
#include "GameListener.h"

// EXTERNALS
extern HINSTANCE      g_radardll_hinst;
// global app configuration
extern CConfig g_cfg;
extern LoginListener *g_plogin;
extern GameListener *g_pgame;
extern char g_error_logger_strtype[LOG_LEVELS][16]; // Warning!!! HACK!!!

void ConfigDlg_OnInitDialog( HWND hDlg );
void ConfigDlg_OnOK( HWND hDlg, BOOL bEndDialog );
void ConfigDlg_OnApply( HWND hDlg );
void ConfigDlg_UpateEnabledControls( HWND hDlg );

INT_PTR CALLBACK ConfigDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	lParam;
	switch( uMsg )
	{
	case WM_INITDIALOG: ConfigDlg_OnInitDialog( hDlg ); break;
	case WM_COMMAND:
		{
			switch( LOWORD( wParam ) )
			{
			case IDOK: ConfigDlg_OnOK( hDlg, TRUE ); break;
			case IDCANCEL: EndDialog( hDlg, IDCANCEL ); break;
			case IDC_APPLY: ConfigDlg_OnApply( hDlg ); break;
			}
			ConfigDlg_UpateEnabledControls( hDlg );
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

void ConfigDialogStart( HWND hWndParent )
{
	DialogBoxParam( g_radardll_hinst, MAKEINTRESOURCE(IDD_CONFIG), hWndParent, ConfigDlgProc, 0 );
}

void ConfigDlg_OnInitDialog( HWND hDlg )
{
	HWND h;
	int i;

	// L2 Version
	h = GetDlgItem( hDlg, IDC_CB_L2VER );
	SendMessage( h, CB_ADDSTRING, 0, (LPARAM)TEXT("Chaotic Throne 1 - The Kamael") );
	SendMessage( h, CB_ADDSTRING, 0, (LPARAM)TEXT("Chaotic Throne 1.5 - Hellbound") );
	SendMessage( h, CB_ADDSTRING, 0, (LPARAM)TEXT("Chaotic Throne 2 - Gracia Part 1") );
	SendMessage( h, CB_ADDSTRING, 0, (LPARAM)TEXT("Chaotic Throne 2.2 - Gracia Part 2") );
	SendMessage( h, CB_ADDSTRING, 0, (LPARAM)TEXT("Chaotic Throne 2.3 - Gracia Final") );
	SendMessage( h, CB_ADDSTRING, 0, (LPARAM)TEXT("Chaotic Throne 2.4 - Gracia Epilogue") );
	SendMessage( h, CB_SETCURSEL, (WPARAM)g_cfg.L2_version, 0 );
	// L2 Client Version
	h = GetDlgItem( hDlg, IDC_CB_L2CVER );
	SendMessage( h, CB_ADDSTRING, 0, (LPARAM)TEXT("Chaotic Throne 1 - The Kamael") );
	SendMessage( h, CB_ADDSTRING, 0, (LPARAM)TEXT("Chaotic Throne 1.5 - Hellbound") );
	SendMessage( h, CB_ADDSTRING, 0, (LPARAM)TEXT("Chaotic Throne 2 - Gracia Part 1") );
	SendMessage( h, CB_ADDSTRING, 0, (LPARAM)TEXT("Chaotic Throne 2.2 - Gracia Part 2") );
	SendMessage( h, CB_ADDSTRING, 0, (LPARAM)TEXT("Chaotic Throne 2.3 - Gracia Final") );
	SendMessage( h, CB_ADDSTRING, 0, (LPARAM)TEXT("Chaotic Throne 2.4 - Gracia Epilogue") );
	SendMessage( h, CB_SETCURSEL, (WPARAM)g_cfg.L2_client_version, 0 );

	// full mode enable
	CheckDlgButton( hDlg, IDC_C_ENABLE_MODGT, g_cfg.EnableModifyGameTraffic );
	// override game protocolversion
	SetDlgItemInt( hDlg, IDC_E_OVERRIDE_GPV, g_cfg.OverrideGameProtocolVersion, FALSE );
	// thread priority raise enable
	CheckDlgButton( hDlg, IDC_C_ENABLE_THREADPRI, g_cfg.ThreadProirityRaiseEnable );
	// reply L2J GameGuardQuery
	CheckDlgButton( hDlg, IDC_C_GAMEGUARDREPLY, g_cfg.ReplyL2JGameGuardQuery );
	// Gracia Epilogue protocol 148 hacks
	CheckDlgButton( hDlg, IDC_C_EPILOGUE_148_146, g_cfg.GraciaEpilogueProtocol_148_hacks );

	// L2Walker_DropRequestGMList
	CheckDlgButton( hDlg, IDC_C_L2WDROPGMLIST, g_cfg.L2Walker_DropRequestGMList );
	// L2Walker_FixMoveBackwardToLocation
	CheckDlgButton( hDlg, IDC_C_L2WMOVEFIX, g_cfg.L2Walker_FixMoveBackwardToLocation );
	// L2Walker_FixChangeWaitType2
	CheckDlgButton( hDlg, IDC_C_L2WSITFIX, g_cfg.L2Walker_FixChangeWaitType2 );
	// L2Walker_InjectStatusUpdate
	CheckDlgButton( hDlg, IDC_C_L2WALKER_INJECTSTATUSUPDATE, g_cfg.L2Walker_InjectStatusUpdate );

	// log level
	h = GetDlgItem( hDlg, IDC_CB_LOGLEVEL );
	for( i=LOG_OK; i<=LOGLEVEL_LAST; i++ )
	{
		char *pstr = g_error_logger_strtype[i];
		SendMessageA( h, CB_ADDSTRING, 0, (LPARAM)pstr );
	}
	SendMessage( h, CB_SETCURSEL, (WPARAM)(int)g_cfg.WarnMessageLevel, 0 );

	// Log Game Packets
	CheckDlgButton( hDlg, IDC_C_LOGGAMEP, g_cfg.LogGamePackets );
	// Log packets FileName Prefix
	SetDlgItemTextA( hDlg, IDC_E_LOGFNPREFIX, g_cfg.LogGameFileNamePrefix );
	// warn unknown packets
	CheckDlgButton( hDlg, IDC_C_WARNUNKP, g_cfg.WarnUnknownPacketsToStdout );

	// FakeListenLoginPort
	SetDlgItemInt( hDlg, IDC_E_FLPORT, g_cfg.FakeListenLoginPort, TRUE );
	// FakeListenGamePort
	SetDlgItemInt( hDlg, IDC_E_FGPORT, g_cfg.FakeListenGamePort, TRUE );
	// RealLoginServerIP
	SetDlgItemTextA( hDlg, IDC_E_REALIP, g_cfg.RealLoginServerIP );
	// RealLoginServerPort
	SetDlgItemInt( hDlg, IDC_E_REALPORT, g_cfg.RealLoginServerPort, TRUE );
	// PlayGameServerNo
	//SetDlgItemInt( hDlg, IDC_E_PLAYGSNO, g_cfg.PlayGameServerNo, TRUE );

	// forced game server select setting
	SetDlgItemTextA( hDlg, IDC_E_FORCEGSIP, g_cfg.ForceGameServerIP );
	SetDlgItemInt( hDlg, IDC_E_FORCEGSPORT, g_cfg.ForceGameServerPort, TRUE );

	ConfigDlg_UpateEnabledControls( hDlg );
}

void ConfigDlg_OnOK( HWND hDlg, BOOL bEndDialog )
{
	// L2 version
	g_cfg.L2_version = (int)SendMessage( GetDlgItem( hDlg, IDC_CB_L2VER ), CB_GETCURSEL, 0, 0 );
	// L2 client version
	g_cfg.L2_client_version = (int)SendMessage( GetDlgItem( hDlg, IDC_CB_L2CVER ), CB_GETCURSEL, 0, 0 );

	// full mode enable
	g_cfg.EnableModifyGameTraffic = IsDlgButtonChecked( hDlg, IDC_C_ENABLE_MODGT );
	// override game protocolversion
	g_cfg.OverrideGameProtocolVersion = (int)GetDlgItemInt( hDlg, IDC_E_OVERRIDE_GPV, NULL, TRUE );
	// thread priority raise enable
	g_cfg.ThreadProirityRaiseEnable = IsDlgButtonChecked( hDlg, IDC_C_ENABLE_THREADPRI );
	// reply L2J GameGuardQuery
	g_cfg.ReplyL2JGameGuardQuery = IsDlgButtonChecked( hDlg, IDC_C_GAMEGUARDREPLY );
	// Gracia Epilogue protocol 148 hacks
	g_cfg.GraciaEpilogueProtocol_148_hacks = IsDlgButtonChecked( hDlg, IDC_C_EPILOGUE_148_146 );

	// L2Walker_DropRequestGMList
	g_cfg.L2Walker_DropRequestGMList = IsDlgButtonChecked( hDlg, IDC_C_L2WDROPGMLIST );
	// L2Walker_FixMoveBackwardToLocation
	g_cfg.L2Walker_FixMoveBackwardToLocation = IsDlgButtonChecked( hDlg, IDC_C_L2WMOVEFIX );
	// L2Walker_FixChangeWaitType2
	g_cfg.L2Walker_FixChangeWaitType2 = IsDlgButtonChecked( hDlg, IDC_C_L2WSITFIX );
	// L2Walker_InjectStatusUpdate
	g_cfg.L2Walker_InjectStatusUpdate = IsDlgButtonChecked( hDlg, IDC_C_L2WALKER_INJECTSTATUSUPDATE );
	// log level
	HWND h = GetDlgItem( hDlg, IDC_CB_LOGLEVEL );
	g_cfg.WarnMessageLevel = (int)SendMessage( h, CB_GETCURSEL, 0, 0 );

	//
	// Log Game Packets
	g_cfg.LogGamePackets = IsDlgButtonChecked( hDlg, IDC_C_LOGGAMEP );
	// Log packets FileName Prefix
	GetDlgItemTextA( hDlg, IDC_E_LOGFNPREFIX, g_cfg.LogGameFileNamePrefix,
		sizeof(g_cfg.LogGameFileNamePrefix)-1 );
	// warn unknown packets
	g_cfg.WarnUnknownPacketsToStdout = IsDlgButtonChecked( hDlg, IDC_C_WARNUNKP );

	//
	// FakeListenLoginPort
	g_cfg.FakeListenLoginPort = GetDlgItemInt( hDlg, IDC_E_FLPORT, NULL, TRUE );
	// FakeListenGamePort
	g_cfg.FakeListenGamePort = GetDlgItemInt( hDlg, IDC_E_FGPORT, NULL, TRUE );
	// RealLoginServerIP
	GetDlgItemTextA( hDlg, IDC_E_REALIP, g_cfg.RealLoginServerIP, sizeof(g_cfg.RealLoginServerIP) );
	// RealLoginServerPort
	g_cfg.RealLoginServerPort = GetDlgItemInt( hDlg, IDC_E_REALPORT, NULL, TRUE );
	// PlayGameServerNo
	//g_cfg.PlayGameServerNo = GetDlgItemInt( hDlg, IDC_E_PLAYGSNO, NULL, TRUE );
	// forced GS settings
	GetDlgItemTextA( hDlg, IDC_E_FORCEGSIP, g_cfg.ForceGameServerIP, sizeof(g_cfg.ForceGameServerIP) );
	g_cfg.ForceGameServerPort = GetDlgItemInt( hDlg, IDC_E_FORCEGSPORT, NULL, TRUE );

	//
	g_cfg.SaveConfig();
	if( bEndDialog == TRUE ) EndDialog( hDlg, IDOK );
}

void ConfigDlg_OnApply( HWND hDlg )
{
	if( !g_plogin || !g_pgame ) return;
	// get SOME config values and save config
	ConfigDlg_OnOK( hDlg, FALSE );
	//
	// check in client is in game
	bool running = false;
	if( g_plogin )
		if( g_plogin->isRunning() ) running = true;
	if( g_pgame )
		if( g_pgame->isRunning() ) running = true;
	if( running )
	{
		log_error( LOG_WARNING, "ConfigDlg_OnApply(): warning: game is running!\n" );
		int answer = MessageBox( hDlg,
			TEXT("Warning! Listeners are running now!\n")
			TEXT("If you press 'Yes', connection will be terminated.\n")
			TEXT("If you press 'No', changes will no take effect until next connection.\n")	,
			TEXT("Warning! Game running!"), MB_YESNO | MB_ICONWARNING );
		if( answer == IDNO ) return;
		log_error( LOG_WARNING, "ConfigDlg_OnApply(): connections will be terminated.\n" );
	}
	//
	if( g_plogin ) g_plogin->signalStop(); // LoginListener may not be started in ingame mode at all
	g_pgame->signalStop();
	if( g_plogin ) // LoginListener may not be started in ingame mode at all
	{
		if( !g_plogin->waitStopped( 5000 ) )
		{
			ErrorLogger_FlushLogFile();
			MessageBox( hDlg, TEXT("Login Listener stop wait failed! Terminated..."), TEXT("Warning!"), MB_ICONWARNING );
			g_plogin->terminateThread();
		}
	} // LoginListener may not be started in ingame mode at all

	if( !g_pgame->waitStopped( 5000 ) )
	{
		ErrorLogger_FlushLogFile();
		MessageBox( hDlg, TEXT("Game Listener stop wait failed! Terminated..."), TEXT("Warning!"), MB_ICONWARNING );
		g_pgame->terminateThread();
	}
	if( g_plogin ) // LoginListener may not be started in ingame mode at all
	{
		if( !g_plogin->start() )
		{
			ErrorLogger_FlushLogFile();
			MessageBox( hDlg, TEXT("Login Listener start failed! O_o"), TEXT("Warning!"), MB_ICONWARNING );
		}
	} // LoginListener may not be started in ingame mode at all

	if( !g_pgame->start() )
	{
		ErrorLogger_FlushLogFile();
		MessageBox( hDlg, TEXT("Game Listener start failed! O_o"), TEXT("Warning!"), MB_ICONWARNING );
	}
	ErrorLogger_FlushLogFile();
}

void ConfigDlg_UpateEnabledControls( HWND hDlg )
{
	HWND hs, hc;
	hs = GetDlgItem( hDlg, IDC_CB_L2VER );
	hc = GetDlgItem( hDlg, IDC_CB_L2CVER );
	int srv_ver = ComboBox_GetCurSel( hs );
	int cl_ver = ComboBox_GetCurSel( hc );
	if( srv_ver == 5 && cl_ver == 5 )
		EnableWindow( GetDlgItem( hDlg, IDC_C_EPILOGUE_148_146 ), TRUE );
	else
	{
		CheckDlgButton( hDlg, IDC_C_EPILOGUE_148_146, 0 );
		EnableWindow( GetDlgItem( hDlg, IDC_C_EPILOGUE_148_146 ), FALSE );
	}

	// disable some controls in ingame mode
	if( g_cfg.isInGameMode == true )
	{
		EnableWindow( GetDlgItem( hDlg, IDC_C_L2WMOVEFIX ), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_C_L2WDROPGMLIST ), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_C_L2WSITFIX ), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_C_L2WALKER_INJECTSTATUSUPDATE ), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_E_FLPORT ), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_E_REALIP ), FALSE );
		EnableWindow( GetDlgItem( hDlg, IDC_E_REALPORT ), FALSE );
		//EnableWindow( GetDlgItem( hDlg, IDC_E_PLAYGSNO ), FALSE );
	}
	// also temporarily disable some controls
	EnableWindow( GetDlgItem( hDlg, IDC_C_ENABLE_MODGT ), FALSE );
}
