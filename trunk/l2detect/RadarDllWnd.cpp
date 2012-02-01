#include "stdafx.h"
#include "Resource.h"
#include "ConfigIni.h"
#include "RadarDllWnd.h"
#include "Logger.h"
#include "GameClient.h"

extern class CConfig g_cfg;
extern class GameClient *g_game_client;

#define RADARDLLWND_CLASS TEXT("RadarDllWindow_2.0")

// globals
HINSTANCE g_radardll_hinst = NULL;
DWORD g_radardll_thread_id = 0;
HWND  g_hWndL2 = NULL;
DWORD g_l2_thread_id = 0;
HWND g_radardll_hwnd = NULL;

// display globals
int g_radarWnd_drawOnClient = 0;

LPDIRECTDRAW lpdd = NULL;
LPDIRECTDRAW7 lpdd7 = NULL;
LPDIRECTDRAWSURFACE7 lpdds7 = NULL;

// hook procs defs
LRESULT CALLBACK KeyboardProc( int nCode, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam );

 void InitDirectDraw( HWND hWnd );
 void CleanupDirectDraw();

BOOL RadarDllWindowRegClass( HINSTANCE hInst )
{
	g_radardll_hinst = hInst;
	WNDCLASSEX wc;
	memset( &wc, 0, sizeof(WNDCLASSEX) );
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.hInstance = hInst;
	wc.hIcon = wc.hIconSm = LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_MFC) );
	wc.lpszClassName = RADARDLLWND_CLASS;
	wc.style = CS_OWNDC | /*CS_VREDRAW | CS_HREDRAW |*/ CS_DBLCLKS;
	wc.lpfnWndProc = RadarDllWindowProc;
	if( !RegisterClassEx( &wc ) )
	{
		MessageBox( 0, TEXT("RegisterClassEx() failed!"), TEXT("Error!"), MB_ICONSTOP );
		return FALSE;
	}
	return TRUE;
}

HWND RadarDllWindowCreate()
{
	if( g_radardll_hinst == NULL ) return NULL;
	DWORD windowExStyle = WS_EX_LAYERED | WS_EX_TOPMOST; // | WS_EX_TOOLWINDOW /*| WS_EX_NOACTIVATE*/;
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	if( g_cfg.isInGameMode == false )
	{
		windowExStyle = WS_EX_LAYERED; // NO WS_EX_TOPMOST; // :)
		windowStyle |= ( WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX );
	}
	HWND hWndRadar = NULL;
	hWndRadar = CreateWindowEx( windowExStyle, RADARDLLWND_CLASS, TEXT("L2Detect"), windowStyle,
		0, 0, 300, 800, NULL, NULL, g_radardll_hinst, NULL );
	if( !hWndRadar )
	{
		log_error( LOG_ERROR, "Radar window creation failed, trying to init common controls...\n" );
		MyInitCommonControlsEx();
		hWndRadar = CreateWindowEx( windowExStyle, RADARDLLWND_CLASS, TEXT("L2Detect"), windowStyle,
			0, 0, 300, 800, NULL, NULL, g_radardll_hinst, NULL );
		if( !hWndRadar )
		{
			log_error( LOG_ERROR, "Radar window creation failed again. Radar will not function!\n" );
			return NULL;
		}
	}
	SetLayeredWindowAttributes( hWndRadar, RGB(0,0,0), 255, /*LWA_COLORKEY |*/ LWA_ALPHA );
	return hWndRadar;
}

void RadarDllWindowStart( HINSTANCE hInst )
{
	RadarDllWindowRegClass( hInst );
	HANDLE hThread = NULL;
	DWORD dwTID = 0;
	hThread = (HANDLE)_beginthreadex( NULL, 0,
		(unsigned int (__stdcall *)(void *))RadarDllWindowThread, NULL, 0,
		(unsigned int *)&dwTID );
	if( hThread == NULL )
	{
		MessageBox( 0, TEXT("RadarDllWindowThread create error!"), TEXT("Error!"), MB_ICONSTOP );
		return;
	}
	CloseHandle( hThread );
}

// вызывается каждый раз когда найдено какое-то окно функцией EnumWindows
// описатель окна попадает в hWnd
BOOL CALLBACK RadarDllWindowThread_EnumWindowsProc( HWND hWnd, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam);
	DWORD myPid = GetCurrentProcessId(); // pID текущего процесса
	DWORD dwPid = 0;
	DWORD dwTid = GetWindowThreadProcessId( hWnd, &dwPid ); // pID процесса, владеющего анализируемым окном
	// защита на случай запуска 2х окон л2
	if( dwPid == myPid ) // только если это окно принадлежит нашему процессу! а не другому окну л2
	{
		log_error( LOG_DEBUGDUMP, "RadarDllWindowThread_EnumWindowsProc(): got window pid,tid = (%u,%u)\n",
			(unsigned int)dwPid, (unsigned int)dwTid );
		// window is running in the same process as we are
		// check window class or name to check is it is main Lineage II window
		bool isL2Window = false;
		TCHAR wndTitle[256];
		memset( wndTitle, 0, sizeof(wndTitle) );
		GetWindowText( hWnd, wndTitle, 255 );
		log_error( LOG_DEBUGDUMP, ".. got window title [%S]\n", wndTitle );
		if( _tcsicmp( wndTitle, _T("Lineage II") ) == 0 )
		{
			isL2Window = true;
			log_error( LOG_DEBUG, "... found L2 window by title!\n" );
		}
		// title doesn't match? check window class name!
		if( !isL2Window )
		{
			log_error( LOG_DEBUGDUMP, "... window title doesn't match, trying by class name...\n" );
			TCHAR buffer_for_class_name[256] = {0};
			if( GetClassName( hWnd, buffer_for_class_name, 255 ) )
			{
				log_error( LOG_DEBUGDUMP, ".... got window class name = [%S]\n", buffer_for_class_name );
				if( _tcsicmp( buffer_for_class_name, _T("l2UnrealWWindowsViewportWindow") ) == 0 )
				{
					isL2Window = true;
					log_error( LOG_DEBUG, ".... found L2 window by class name!\n" );
				}
			}
			else
				ErrorLogger_LogLastError( "GetClassName()", GetLastError() );
		}
		// checks...
		if( isL2Window ) // found
		{
			g_hWndL2 = hWnd; // save in global var
			g_l2_thread_id = dwTid;
			return FALSE; // stop EnumWindows
		}
	}
	return TRUE;
}

HHOOK WINAPI RadarDllWindowThread_installKeyboardHook()
{
	HHOOK hook = NULL;
	unsigned int my_process_id = (unsigned int)GetCurrentProcessId();
	log_error( LOG_DEBUG, "We're running in process id #%u\n", my_process_id );
	g_hWndL2 = NULL; // global handle to L2 Window
	g_l2_thread_id = 0;
	int tries = 80; // wait 20 seconds for L2 widnow
	log_error( LOG_OK, "RadarDllWindowThread_installKeyboardHook(): looking for L2 window ... " );
	while( g_hWndL2 == NULL )
	{
		EnumWindows( RadarDllWindowThread_EnumWindowsProc, 0 );
		tries--;
		if( g_hWndL2 == NULL )
			log_error_np( LOG_OK, "%d, ", tries );
		if( tries <= 0 ) break;
		Sleep( 250 ); // wait 0.25 seconds, maybe L2 window will appear
	}
	log_error_np( LOG_OK, "\n" );
	// check if window is found
	if( g_hWndL2 )
	{
		hook = SetWindowsHookEx( WH_KEYBOARD, KeyboardProc, g_radardll_hinst, g_l2_thread_id );
		if( hook == NULL )
		{
			DWORD le = GetLastError();
			ErrorLogger_LogLastError( "SetWindowsHookEx() failed", le );
		}
		else
			log_error( LOG_OK, "Window keyboard hook installed.\n" );
	}
	else
	{
		log_error( LOG_ERROR, "RadarDllWindowThread_installKeyboardHook(): cannot find Lineage II window!!!\n" );
		g_hWndL2 = NULL;
	}
	return hook;
}


DWORD WINAPI RadarDllWindowThread( LPVOID lpParam )
{
	UNREFERENCED_PARAMETER(lpParam);
	g_radardll_thread_id = GetCurrentThreadId();

	// are we in ingame or outgame?
	TCHAR tszEvtName[256];
	wsprintf( tszEvtName, TEXT("L2Detect Outgame Event_%d"), (unsigned)GetCurrentProcessId() );
	HANDLE hEvtDbg = OpenEvent( EVENT_MODIFY_STATE, FALSE, tszEvtName );

	HHOOK hook = NULL;

	if( hEvtDbg == NULL ) // ingame, wait for L2 Window
	{
		g_cfg.isInGameMode = true;
		Sleep( 5000 );
		// install keyboard hook
		hook = RadarDllWindowThread_installKeyboardHook();
		// wait more till L2 window initialises
		Sleep( 4000 );
		//MyInitCommonControlsEx();
		//InitDirectDraw( hWndL2 );
	}
	else // outgame. do not wait for anything
	{
		g_cfg.isInGameMode = false;
	}
	//

	ErrorLogger_FlushLogFile();
	//
	HWND hWnd = RadarDllWindowCreate();
	if( hWnd == NULL )
	{
		DWORD le = GetLastError();
		log_error( LOG_ERROR, "RadarDllWindowCreate() failed! last error %u\n", (unsigned int)le );
		ErrorLogger_LogLastError( "RadarDllWindowCreate()", le );
		return le;
	}
	g_radardll_hwnd = hWnd; // save radar window handle in global var
	// hide radar window ONLY if is INGAME mode AND Lineage II window is found!
	if( (g_cfg.isInGameMode == true) && (g_hWndL2 != NULL) )
		ShowWindow( hWnd, SW_HIDE );
	// fix: show radar window again in INGAME mode and if Lineage II window was not found
	if( (g_cfg.isInGameMode == true) && (g_hWndL2 == NULL) )
		ShowWindow( hWnd, SW_SHOWNORMAL );

	// radar wnd message loop
	MSG msg;
	while( GetMessage( &msg, NULL, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	//
	log_error( LOG_DEBUG, "Window is closed. Cleanup...\n" );
	ErrorLogger_FlushLogFile();

	// Cleanup
	if( g_cfg.isInGameMode )
	{
		// unhook hook installed in ingame mode
		UnhookWindowsHookEx( hook );
		CleanupDirectDraw();
	}
	else
	{
		// signal EXE in outgame mode to stop
		SetEvent( hEvtDbg );
		CloseHandle( hEvtDbg );
	}
	return 0;
}


// local thread's keyboard hook
LRESULT CALLBACK KeyboardProc( int nCode, WPARAM wParam, LPARAM lParam )
{
	if( nCode < 0 ) return CallNextHookEx( NULL, nCode, wParam, lParam );
	if( nCode == HC_ACTION || nCode == HC_NOREMOVE )
	{
		//wParam [in] Specifies the virtual-key code of the key that generated the keystroke message.
		//lParam [in] Specifies the repeat count, scan code, extended-key flag, context code,
		//            previous key-state flag, and transition-state flag
		//lParam bit 31 is set if key is pressed, and not set if released
		bool keyIsPushed = ((lParam & 0x80000000) != 0);
		if( keyIsPushed ) // ТОЛЬКО если кнопка была НАЖАТА, а не отпущена
		{
			switch( wParam ) // wParam - код клавиши
			{
			case VK_PAUSE: // нажали Pause/Break?
				PostMessage( g_radardll_hwnd, WMMY_WNDSHOWTOGGLE, 0, 0 );
				break;
			default: // любая другая кнопка
				if( g_game_client )
					g_game_client->ai.notifyUIKeyUp( (int)wParam );
				break;
			}
		}
	}
	// [DBG] KeyboardProc( 0, '' 19, 0xC0450001 ) cur thread: 5088 5088: l2 thread
	//log_error( LOG_DEBUG, "KeyboardProc( %d, '%c' %d, 0x%08X ) cur thread: %u %u: l2 thread\n",
	//	nCode, (char)wParam, (int)wParam, (unsigned int)lParam,
	//	(unsigned int)GetCurrentThreadId(), (unsigned int)g_l2_thread_id );
	return CallNextHookEx( NULL, nCode, wParam, lParam );
}

// global keyboard hook
LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam )
{
	if( nCode < 0 ) return CallNextHookEx( NULL, nCode, wParam, lParam );
	//
	//char szMes[32] = {0};
	//switch( wParam )
	//{
	//case WM_KEYDOWN:    strcpy( szMes, "WM_KEYDOWN   " ); break;
	//case WM_KEYUP:      strcpy( szMes, "WM_KEYUP     " ); break;
	//case WM_SYSKEYDOWN: strcpy( szMes, "WM_SYSKEYDOWN" ); break;
	//case WM_SYSKEYUP:   strcpy( szMes, "WM_SYSKEYUP  " ); break;
	//default: sprintf( szMes, "mes_%u", (unsigned int)wParam ); break;
	//}
	//
	KBDLLHOOKSTRUCT *phs = (KBDLLHOOKSTRUCT *)lParam;
	if( !phs ) return CallNextHookEx( NULL, nCode, wParam, lParam );
	//
	//log_error( LOG_DEBUG, "LowLevelKeyboardProc( %d, '%s', '%3c' %4u, %u ) threads (cur/radar/l2): %u/%u/%u\n",
	//	nCode, szMes, (char)(phs->vkCode), (unsigned int)(phs->vkCode), (unsigned int)phs->scanCode,
	//	(unsigned int)GetCurrentThreadId(), (unsigned int)g_radardll_thread_id, (unsigned int)g_l2_thread_id );
	//
	if( wParam == WM_KEYUP )
	{
		if( phs->vkCode == VK_PAUSE )
			PostMessage( g_radardll_hwnd, WMMY_WNDSHOWTOGGLE, 0, 0 );
		if( phs->vkCode == VK_NUMLOCK )
		{
			int new_val = 0;
			if( g_radarWnd_drawOnClient == 0 ) new_val = 1;
			g_radarWnd_drawOnClient = new_val;
		}
	}
	return CallNextHookEx( NULL, nCode, wParam, lParam );
}

void InitDirectDraw( HWND hWnd )
{
	// Window info
	if( hWnd != NULL )
	{
		TCHAR cn[256] = {0};
		GetClassName( hWnd, cn, 255 );
		log_error( LOG_DEBUG, "Got class name: [%S]\n", cn );
	}
	// Create DirectDraw object
	HRESULT hRes = DirectDrawCreate( NULL, &lpdd, NULL );
	if( hRes != S_OK )
	{
		log_error( LOG_ERROR, "DirectDraw creating failed! HRESULT = %d (%08X), last error %u\n",
			hRes, hRes, (unsigned int)GetLastError() );
		return;
	}
	log_error( LOG_DEBUG, "DirectDraw create OK\n" );
	// get directdraw7
	hRes = lpdd->QueryInterface( IID_IDirectDraw7, (void **)&lpdd7 );
	lpdd->Release();
	lpdd = NULL;
	if( hRes != S_OK )
	{
		log_error( LOG_ERROR, "IDirectDraw::QueryInterface() failed! err 0x%08x\n", hRes );
		return;
	}
	log_error( LOG_DEBUG, "DirectDraw7 create OK\n" );
	// try to set cooperative level (dest HWND)
	hRes = lpdd7->SetCooperativeLevel( hWnd, DDSCL_NORMAL /* | DDSCL_MULTITHREADED */ );
	if( hRes != S_OK ) log_error( LOG_ERROR, "IDirectDraw7::SetCooperativeLevel() failed! 0x%08X\n", hRes );
	else log_error( LOG_DEBUG, "IDirectDraw7::SetCooperativeLevel() OK\n" );
	//
	/*HDC hdcL2 = GetDC( hWnd );
	hRes = lpdd7->GetSurfaceFromDC( hdcL2, &lpdds );
	if( hRes != S_OK )
	{
		log_error( LOG_ERROR, "IDirectDraw7::GetSurfaceFromDC() failed! 0x%08x\n", hRes );
		switch( hRes )
		{
		case DDERR_GENERIC: log_error( LOG_ERROR, "DDERR_GENERIC\n" ); break;
		case DDERR_INVALIDPARAMS: log_error( LOG_ERROR, "DDERR_INVALIDPARAMS\n" ); break;
		case DDERR_OUTOFMEMORY: log_error( LOG_ERROR, "DDERR_OUTOFMEMORY\n" ); break;
		case DDERR_NOTFOUND: log_error( LOG_ERROR, "DDERR_NOTFOUND\n" ); break;
		default: log_error( LOG_ERROR, "xz\n" ); break;
		}
	}
	else log_error( LOG_DEBUG, "IDirectDraw7::GetSurfaceFromDC() OK! 0x%08x\n", lpdds );
	ReleaseDC( hWnd, hdcL2 );*/
	//
	/*DDSURFACEDESC2 sd2;
	sd2.dwSize = sizeof(sd2);
	sd2.ddsCaps.dwCaps;
	lpdd7->CreateSurface( &sd2, &lpdds7, NULL );*/
}

void CleanupDirectDraw()
{
	if( lpdds7 )
	{
		log_error( LOG_DEBUG, "Releasing Surface...\n" );
		lpdds7->Release();
		lpdds7 = NULL;
	}
	if( lpdd7 )
	{
		log_error( LOG_DEBUG, "Releasing IDirectDraw7...\n" );
		lpdd7->Release();
		lpdd7 = NULL;
	}
}


// old hook installer code
/*
		// find Lineage II Window
		HWND hWndL2;
		int maxTries = 50;
		DWORD l2_pid = 0, l2_tid = 0;
		while( maxTries > 0 )
		{
			maxTries--;
			// try to find by name
			hWndL2 = FindWindow( NULL, TEXT("Lineage II") );
			if( hWndL2 ) log_error( LOG_DEBUG, "Found L2 Window by name 0x%p\n", hWndL2 );
			else
			{
				// or else try to find by class
				hWndL2 = FindWindow( TEXT("l2UnrealWWindowsViewportWindow"), NULL );
				if( hWndL2 ) log_error( LOG_DEBUG, "Found L2 Window by class 0x%p\n", hWndL2 );
				else log_error( LOG_WARNING, "FindWindow() cannot find L2 Window! (triesLeft: %d)\n", maxTries );
			}
			Sleep( 1000 );
			if( hWndL2 != NULL )
			{
				g_hWndL2 = hWndL2;
				l2_tid = GetWindowThreadProcessId( hWndL2, &l2_pid );
				g_l2_thread_id = l2_tid;
				log_error( LOG_DEBUG, "L2 Window Thread: %d, process ID: %d\n", l2_tid, l2_pid );
				break;
			}
		}
		// install keyboard hook only in ingame mode
		//hook = SetWindowsHookEx( WH_KEYBOARD_LL, LowLevelKeyboardProc, g_radardll_hinst, 0 );
		hook = SetWindowsHookEx( WH_KEYBOARD, KeyboardProc, g_radardll_hinst, g_l2_thread_id );
		if( hook == NULL )
		{
			DWORD le = GetLastError();
			ErrorLogger_LogLastError( "SetWindowsHookEx() failed", le );
		}
		else
			log_error( LOG_DEBUG, "SetWindowsHookEx() OK\n" );
*/
