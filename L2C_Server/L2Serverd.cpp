#include "pch.h"
#include "Resource.h"
#include "Log.h"
#include "GS_Cfg.h"
#include "LS_Connection.h"
#include "GS.h"
#include "utils/Debugging.h"
#include "utils/Exception.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hInst = NULL;

void Main_StartServer()
{
	GameServer *gs = GameServer::getInstance();
	if( !gs->start() ) LogError( L"GS Start failed!" );
}

void Main_StopServer()
{
	GameServer *gs = GameServer::getInstance();
	if( !gs->isRunning() ) return;
	if( !gs->stop() ) LogError( L"GS Stop failed!" );
	else Log( L"GS Stopped." );
}

void Main_OnTestExceptions()
{
	LogFull( true, true, RGB(255,128,128), RGB(255,255,255), L"===== Throwing an exception... =====" );
	try
	{
		throw Exception( "test exception: max players: %d", GameServer::getInstance()->getMaxPlayers() );
	}
	catch( Exception& e )
	{
		LogError( L"Got exception: %S", e.what() );
		e.logStackTrace();
	}
	LogFull( true, true, RGB(255,128,128), RGB(255,255,255), L"===== Exceptions test complete =====" );
}


void Main_OnInitDialog( HWND hDlg )
{
#ifdef _DEBUG
	SetWindowText( hDlg, TEXT("L2 Game Server (DEBUG)" ) );
#endif
	// rect
	RECT rc;
	GetClientRect( hDlg, &rc );
	HICON hIcon = LoadIcon( g_hInst, MAKEINTRESOURCE(IDI_MAIN) );
	SendMessage( hDlg, WM_SETICON, ICON_BIG,   (LPARAM)hIcon );
	SendMessage( hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon );
	// create richedit
	g_hWndLog = LogWnd_Create( hDlg );
	// start server
	Main_StartServer();
}

void Main_OnClose( HWND hDlg )
{
	EndDialog( hDlg, IDOK );
}

void Main_OnButtonServer( HWND hDlg )
{
	POINT pt;
	GetCursorPos( &pt );
	HMENU hMenu = CreatePopupMenu();
	AppendMenu( hMenu, MF_STRING, 1, TEXT("Start") );
	AppendMenu( hMenu, MF_STRING, 2, TEXT("Stop") );
	int cmd = TrackPopupMenu( hMenu, TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, 0, hDlg, NULL );
	DestroyMenu( hMenu );
	switch( cmd )
	{
	case 1: Main_StartServer(); break;
	case 2: Main_StopServer(); break;
	}
}

void Main_OnButtonDebug( HWND hDlg )
{
	POINT pt;
	GetCursorPos( &pt );
	HMENU hMenu = CreatePopupMenu();
	AppendMenu( hMenu, MF_STRING, 1, TEXT("Dump ClientPool") );
	AppendMenu( hMenu, MF_STRING, 2, TEXT("Test exceptions and stack trace") );
	int cmd = TrackPopupMenu( hMenu, TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, 0, hDlg, NULL );
	DestroyMenu( hMenu );
	switch( cmd )
	{
	case 1: 
		{
			if( GameServer::getInstance()->getClientPool() )
				GameServer::getInstance()->getClientPool()->dumpClientsToLog();
		} break;
	case 2:
		{
			Main_OnTestExceptions();
		} break;
	}
}

void Main_OnSize( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(wParam);
	int w = LOWORD(lParam);
	int h = HIWORD(lParam);
	const int controlswnd_h = 80;
	HWND hWnd = GetDlgItem( hDlg, IDC_STATIC_CONTROLS );
	MoveWindow( hWnd, 10, h-controlswnd_h-10, w-20, controlswnd_h, TRUE );
	hWnd = GetDlgItem( hDlg, IDC_B_SERVER );
	MoveWindow( hWnd, 20, h-controlswnd_h-10+20, 50, 25, TRUE );
	hWnd = GetDlgItem( hDlg, IDC_B_DEBUG );
	MoveWindow( hWnd, 80, h-controlswnd_h-10+20, 50, 25, TRUE );
	// log wnd
	MoveWindow( g_hWndLog, 10, 10, w-20, h-controlswnd_h-10-10-10, TRUE );
}

void Main_OnCommand( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam);
	switch( LOWORD(wParam) )
	{
	case IDC_B_SERVER: Main_OnButtonServer( hDlg ); break;
	case IDC_B_DEBUG: Main_OnButtonDebug( hDlg ); break;
	}
}

INT_PTR CALLBACK MainDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG: Main_OnInitDialog( hDlg ); break;
	case WM_CLOSE: Main_OnClose( hDlg ); break;
	case WM_SIZE: Main_OnSize( hDlg, wParam, lParam ); break;
	case WM_COMMAND: Main_OnCommand( hDlg, wParam, lParam ); break;
	default: return FALSE; break;
	}
	return TRUE;
}


int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpszCmdLine, int nShowCmd )
{
	g_hInst = hInstance;
	hPrevInstance = NULL;
	lpszCmdLine = NULL; // cmdline ignored...
	nShowCmd = 0; // ignored

	// WinXP visual styles / common controls
	INITCOMMONCONTROLSEX iccex;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccex.dwICC = ICC_TAB_CLASSES | ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx( &iccex );
	// winsock
	L2PNet_InitDefault();

	// richedit
	HINSTANCE hInstRiched = LoadLibrary( TEXT("Riched20") );
	if( !hInstRiched )
	{
		MessageBox( NULL, TEXT("Cannot load Riched20.dll"), TEXT("Load error!"), MB_ICONSTOP );
		return 1;
	}
	// setup logging
	CreateDirectoryW( L"./log", NULL );
	FILE *flog = _wfopen( L"log/gameserver.log", L"wt" );
	Log_EnableLogToFile( true, flog );
	// init OpenSSL
	SSL_library_init();
	SSL_load_error_strings();
	void *rand_buf = malloc( 1024 );
	if( rand_buf )
	{
		RAND_seed( rand_buf, 1024 );
		free( rand_buf );
		rand_buf = NULL;
	}
	srand( (unsigned)time( NULL ) );

	// initialize debugging
	Debug::getInstance();

	// run dialog!
	try
	{
		DialogBoxParam( hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, MainDlgProc, 0 );
	}
	catch( std::exception& stde ) {
		MessageBoxA( NULL, stde.what(), "WinMain: std::exception:", MB_ICONSTOP ); // warning C4702: unreachable code? WTF???
	} catch( Exception& e ) {
		MessageBoxA( NULL, e.what(), "WinMain: Exception:", MB_ICONSTOP );
	} catch( ... ) {
		MessageBox( NULL, L"catch( ... )", L"WinMain: unknown exception!", MB_ICONSTOP );
	}

	// close server
	Main_StopServer();
	GameServer::freeInstance();

	// free debugging
	Debug::freeInstance();

	// stop logging to file (close file)
	Log_EnableLogToFile( false, NULL );
	// cleanup winsock
	L2PNet_Cleanup();

	return 0;
}

