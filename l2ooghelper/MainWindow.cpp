#include "stdafx.h"
#include "Resource.h"
#include "AccountDlg.h"
#include "Tabs.h"
#include "MainWindow.h"
#include "ScriptEngine.h"
#include "GlobalMapImages.h"
#include "DBLayer.h"
#include "SEH_global.h"
#include "HWID.h"
#include "bot_interaction/BotIPC.h"

#ifdef _DEBUG
#include "Logger.h"
#endif

#define MAX_LOADSTRING 256

// Глобальные переменные:
HINSTANCE g_hInst;								// текущий экземпляр
HWND g_hWnd;                                    // главное окно приложения
HWND g_hWndTab;                                 // TabControl приложения
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна
GlobalMapImages g_mapImg;                       // карты

// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass( HINSTANCE hInstance );
BOOL				InitInstance( HINSTANCE, int );
LRESULT CALLBACK	WndProc( HWND, UINT, WPARAM, LPARAM );
INT_PTR CALLBACK	About( HWND, UINT, WPARAM, LPARAM );

// функции обработки сообщений окна
LRESULT OnCreate();
LRESULT OnClose();
LRESULT OnDestroy();
LRESULT OnCommand( WPARAM wParam, LPARAM lParam );
LRESULT OnNotify( WPARAM wParam, LPARAM lParam );
LRESULT OnPaint();
LRESULT OnSize( WPARAM wParam, LPARAM lParam );

int APIENTRY _tWinMain( HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );
	MSG msg;
	HACCEL hAccelTable;
	//
#ifdef _DEBUG
	FILE *error_logger_file = fopen( "L2OOGHelper.log", "wt" );
	ErrorLogger_Init();
	ErrorLogger_SetWarnMessageLevel( LOG_DEBUG );
	ErrorLogger_SetAutoPrependErrorType( true );
	ErrorLogger_Enable( true );
	ErrorLogger_SetLogFile( error_logger_file );
	log_error( LOG_OK, "L2OOGHelper: Debug build start\n" );
#endif // _DEBUG

	// init Comctl32.dll
	InitCommonControls();

#ifdef USE_SEH
	// set default error mode and global exception handler
	SetErrorMode(0); // Use the system default, which is to display all error dialog boxes.
	SetUnhandledExceptionFilter( GlobalUnhandledExceptionFilter );
#ifdef _DEBUG
	SEH_loadMap();
#endif
#endif // USE_SEH

#ifdef CHAT_USE_RICHEDIT // если определено, загружает Riched20.dll (Для RichEdit контрола)
	HINSTANCE hInstDllRichEdit = LoadLibrary( TEXT("Riched20") );
	if( !hInstDllRichEdit )
	{
#ifdef _DEBUG
		log_error( LOG_ERROR, "_tWinMain: cannot load Riched20.dll!" );
#endif
		MessageBox( NULL, TEXT("RichEdit control class DLL cannot be loaded\n")
			TEXT("(Riched20.dll not found)"),
			TEXT("L2OogHelper: FATAL ERROR"), MB_ICONSTOP );
		return -1;
	}
#ifdef _DEBUG
	log_error( LOG_DEBUG, "_tWinMain: Riched20.dll loaded OK\n" );
#endif
#endif // CHAT_USE_RICHEDIT

	// verify HWID
	if( !verifyHWID() ) return 0;
	if( !verifyTimeLimit() ) return 0;

	// init WinSock
	WSADATA wsadata;
	WSAStartup( MAKEWORD(2,0), &wsadata );

	// init L2Packets
	L2PNet_InitDefault();
	L2PacketNames_Init();
	L2Data_init();
	L2Data_SM_init();
	// init caching database layer (based on L2Packets L2Data_*)
	DBLayer_init();

	// init global map images maps directory
	char aCurDir[256] = {0};
	GetCurrentDirectoryA( 255, aCurDir );
	strcat( aCurDir, "\\maps" );
	g_mapImg.setMapsDir( aCurDir );
	// init recipe data
	L2Data_InitRecipes();

	// init Bot interprocess communications
	BotIPC::getInstance();

	// Инициализация глобальных строк
	LoadString( hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING );
	LoadString( hInstance, IDC_L2OOGHELPER, szWindowClass, MAX_LOADSTRING );
	MyRegisterClass( hInstance );
	// Выполнить инициализацию приложения:
	if( !InitInstance( hInstance, nCmdShow ) )
	{
		return FALSE;
	}
	hAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE(IDC_L2OOGHELPER) );
	// Цикл основного сообщения:
	while( GetMessage( &msg, NULL, 0, 0) )
	{
		if( !TranslateAccelerator( msg.hwnd, hAccelTable, &msg ) )
		{
			//if( !IsDialogMessage( msg.hwnd, &msg ) ) // косячок тута, но могут быть проблемы с дочерними диалогами
			//{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			//}
		}
	}

	BotIPC::getInstance()->delAllMyProcessBots(); // safe cleanup
	BotIPC::freeInstance();

	L2Data_SM_free();
	DBLayer_free();

	WSACleanup();

	g_mapImg.unloadAllSquares();
	if( g_mapImg.getLoadedCount() > 0 )
	{
		MessageBox( NULL, TEXT("GlobalMapImages: not all images are unloaded!"),
			TEXT("Warning! Potential memory leaks!"), MB_ICONWARNING );
	}

#ifdef _DEBUG
	log_error( LOG_OK, "Closing log; last message... Bye!\n" );
	ErrorLogger_SetLogFile( stdout );
	ErrorLogger_Enable( false );
	if( error_logger_file ) fclose( error_logger_file );
	error_logger_file = NULL;
#endif

	return (int)msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
//  КОММЕНТАРИИ:
//
//    Эта функция и ее использование необходимы только в случае, если нужно, чтобы данный код
//    был совместим с системами Win32, не имеющими функции RegisterClassEx'
//    которая была добавлена в Windows 95. Вызов этой функции важен для того,
//    чтобы приложение получило "качественные" мелкие значки и установило связь
//    с ними.
//
ATOM MyRegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_OWNDC; // | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon( hInstance, MAKEINTRESOURCE(IDI_L2OOGHELPER) );
	wcex.hCursor		= LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground	= (HBRUSH)( COLOR_WINDOW + 1 );
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_L2OOGHELPER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon( wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL) );
	return RegisterClassEx( &wcex );
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
	HWND hWnd;
	g_hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной
	hWnd = CreateWindow( szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 960 /*CW_USEDEFAULT*/, 695, NULL, NULL, hInstance, NULL );
	if( !hWnd ) return FALSE;
	ShowWindow( hWnd, nCmdShow );
	UpdateWindow( hWnd );
	return TRUE;
}

LRESULT OnCreate()
{
	RECT r;
	GetClientRect( g_hWnd, &r );
	// init common controls :)
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_TAB_CLASSES | ICC_LISTVIEW_CLASSES | ICC_TREEVIEW_CLASSES |
		ICC_BAR_CLASSES | ICC_STANDARD_CLASSES | ICC_PROGRESS_CLASS;
	InitCommonControlsEx( &icex );
	// create tab control
	g_hWndTab = CreateWindowEx( 0, WC_TABCONTROL, NULL,
		WS_CHILD | WS_VISIBLE | TCS_MULTILINE | TCS_TABS,
		0, 0, r.right - r.left, r.bottom - r.top, g_hWnd, NULL, g_hInst, NULL );
	//
	Tabs_Init();
	return 0;
}

LRESULT OnClose()
{
	return 0;
}

LRESULT OnDestroy()
{
	PostQuitMessage( 0 );
	return 0;
}

LRESULT OnCommand( WPARAM wParam, LPARAM lParam )
{
	int wmId, wmEvent;
	wmId    = LOWORD( wParam );
	wmEvent = HIWORD( wParam );
	switch( wmId )
	{
	case IDM_ABOUT:
		DialogBox( g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), g_hWnd, About );
		break;
	case IDM_EXIT:
		PostMessage( g_hWnd, WM_CLOSE, 0, 0 );
		break;
	case IDM_LINEAGE_ADD_ACC:
		{
			AccountDlg *dlg = new AccountDlg();
			if( dlg->run( g_hWnd ) )
			{
				Account acc = dlg->getAccount();
				Tabs_AddAccount( &acc );
			}
			delete dlg;
		} break;
#ifdef _DEBUG
	case ID_DEBUG_ENABLECON: ErrorLogger_EnableLoggingToConsole( true ); break;
	case ID_DEBUG_DISABLECON: ErrorLogger_EnableLoggingToConsole( false ); break;
	case ID_LOGLEVEL_OK: ErrorLogger_SetWarnMessageLevel( LOG_OK ); break;
	case ID_LOGLEVEL_ERROR: ErrorLogger_SetWarnMessageLevel( LOG_ERROR ); break;
	case ID_LOGLEVEL_WARN: ErrorLogger_SetWarnMessageLevel( LOG_WARNING ); break;
	case ID_LOGLEVEL_USERAI: ErrorLogger_SetWarnMessageLevel( LOG_USERAI ); break;
	case ID_LOGLEVEL_PACKETNAME: ErrorLogger_SetWarnMessageLevel( LOG_PACKETNAME ); break;
	case ID_LOGLEVEL_DEBUG: ErrorLogger_SetWarnMessageLevel( LOG_DEBUG ); break;
	case ID_LOGLEVEL_DEBUGDUMP: ErrorLogger_SetWarnMessageLevel( LOG_DEBUGDUMP ); break;
	/*case ID_DEBUG_RUNTESTSCRIPT:
		{
			ScriptEngine se;
			se.init();
			se.startScript( "test.lua" );
			se.deinit();
		} break;*/
#endif
	default:
		return DefWindowProc( g_hWnd, WM_COMMAND, wParam, lParam );
	}
	return 0;
}

LRESULT OnNotify( WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(wParam);
	LPNMHDR lpnmhdr = (LPNMHDR)lParam;
	switch( lpnmhdr->code )
	{
	case TCN_SELCHANGING: Tabs_OnSelChanging( lpnmhdr ); break;
	case TCN_SELCHANGE: Tabs_OnSelChange( lpnmhdr ); break;
	}
	return 0;
}

LRESULT OnPaint()
{
	//RECT wr;
	//GetWindowRect( hWnd, &wr );
	//TCHAR tstr[256];
	//wsprintf( tstr, TEXT("%d x %d"), wr.right - wr.left, wr.bottom - wr.top );
	PAINTSTRUCT ps;
	HDC hdc;
	hdc = BeginPaint( g_hWnd, &ps );
	// TODO: добавьте любой код отрисовки...
	//TextOut( hdc, 0, 0, tstr, lstrlen(tstr) );
	EndPaint( g_hWnd, &ps );
	Tabs_OnPaint();
	return 0;
}

LRESULT OnSize( WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(wParam);
	/*lParam:
	The low-order word of lParam specifies the new width of the client area. 
	The high-order word of lParam specifies the new height of the client area.*/
	int w = LOWORD( lParam );
	int h = HIWORD( lParam );
	// resize tab
	MoveWindow( g_hWndTab, 0, 0, w, h, TRUE );
	return 0;
}

LRESULT OnMyDeleteTab( LPARAM lParam )
{
	Tabs_DeleteTab( (int)lParam );
	return 0;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_CREATE:
		{
			g_hWnd = hWnd;
			return OnCreate();
		} break;
	case WM_COMMAND: return OnCommand( wParam, lParam ); break;
	case WM_PAINT: return OnPaint(); break;
	case WM_DESTROY: return OnDestroy(); break;
	case WM_SIZE: return OnSize( wParam, lParam ); break;
	case WM_NOTIFY: return OnNotify( wParam, lParam ); break;
	case WMMY_DELETETAB: return OnMyDeleteTab( lParam ); break;
	}
	return DefWindowProc( hWnd, message, wParam, lParam );
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam);
	switch( message )
	{
	case WM_INITDIALOG:	return (INT_PTR)TRUE;
	case WM_COMMAND:
		if( LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL )
		{
			EndDialog( hDlg, LOWORD(wParam) );
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
