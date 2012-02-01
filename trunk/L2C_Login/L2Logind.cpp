#include "pch.h"
#include "Resource.h"
#include "Log.h"
#include "LoginServer.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hInst = NULL;
LoginServer *g_server = NULL;

void Main_StartServer()
{
	if( !g_server )
	{
		g_server = new LoginServer();
	}
	g_server->start();
}

void Main_StopServer()
{
	if( g_server ) g_server->stop();
	Log( L"Login server stopped." );
}

void Main_OnInitDialog( HWND hDlg )
{
#ifdef _DEBUG
	SetWindowText( hDlg, TEXT("L2 Login Server (DEBUG)" ) );
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
	case 1:
		{
			if( g_server )
				if( !g_server->start() ) LogError( L"Start failed." );
		} break;
	case 2:
		{
			if( g_server )
				if( !g_server->stop() ) LogError( L"Stop failed." );
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
	// log wnd
	MoveWindow( g_hWndLog, 10, 10, w-20, h-controlswnd_h-10-10-10, TRUE );
}

void Main_OnCommand( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam);
	switch( LOWORD(wParam) )
	{
	case IDC_B_SERVER: Main_OnButtonServer( hDlg ); break;
	}
}

INT_PTR CALLBACK MainDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG: Main_OnInitDialog( hDlg ); break;
	case WM_CLOSE: Main_OnClose( hDlg ); break;
	case WM_SIZE: Main_OnSize( hDlg, wParam, lParam ); break;
	//case WM_SIZING: Main_OnSize( hDlg, wParam, lParam ); break; // invalid
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
	FILE *flog = _wfopen( L"log/loginserver.log", L"wt" );
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

	// run dialog!
	DialogBoxParam( hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, MainDlgProc, 0 );
	// close server
	Main_StopServer();
	if( g_server ) delete g_server;
	g_server = NULL;

	// stop logging to file (close file)
	Log_EnableLogToFile( false, NULL );
	// cleanup winsock
	L2PNet_Cleanup();

	return 0;
}
