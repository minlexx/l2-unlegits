#include "stdafx.h"
#include "Resource.h"
//#include "mymem.h"
#include "utils.h"
#include "Logger.h"
// lists
#include "CharArray.h"
#include "ClanList.h"
#include "NpcArray.h"
#include "GIArray.h"
#include "WorldObjectTree.h"
// listeners
#include "LoginListener.h"
#include "GameListener.h"
#include "GameClient.h"
// windows
#include "RadarDllWnd.h"
#include "ConfigIni.h"
// net hook
#include "net_hook.h"
// scripting
#include "ScriptEngine.h"


// global app instance handle
HINSTANCE g_hinst;

// global app configuration
CConfig g_cfg;
FILE *g_errorLogFile = NULL; // log file

// global listeners
LoginListener *g_plogin;
GameListener *g_pgame;
GameClient *g_game_client;

int int_dll_was_init = 0;

/** Initializes DLL. returns 0 on success **/
int InitDLL();
/** Uninitializes DLL **/
int UninitDLL();
/** */
void DllThreadStart();

extern void CleanupDirectDraw();

BOOL WINAPI DllMain( HINSTANCE hInstDLL, ULONG reason, LPVOID lpvReserved )
{
	lpvReserved = NULL;
	if( reason == DLL_PROCESS_ATTACH )
	{
		//MessageBox( 0, TEXT("DLL_PROCESS_ATTACH"), NULL, 0 );
		if( int_dll_was_init ) return TRUE;
		InterlockedIncrement( (LONG *)&int_dll_was_init );
		g_hinst = hInstDLL;
		DisableThreadLibraryCalls( hInstDLL );
		//
		DllThreadStart();
	}
	else if( reason == DLL_PROCESS_DETACH )
	{
		//MessageBox( 0, TEXT("DLL_PROCESS_DETTACH"), NULL, 0 );
		if( int_dll_was_init == 0 ) return TRUE;
		InterlockedDecrement( (LONG *)&int_dll_was_init );
		UninitDLL();
	}
	return TRUE;
}

#ifdef _DEBUG
void test_GetClassInfoEx()
{
	WNDCLASSEXW wc;
	if( GetClassInfoExW( NULL, WC_TABCONTROLW, &wc ) )
	{
		MessageBox( NULL, TEXT("GetClassInfoExW() OK"), TEXT("Test"), 0 );
	}
	else
	{
		TCHAR tstr[128];
		wsprintf( tstr, TEXT("GetClassInfoExW() failed! Error %u"), (unsigned int)GetLastError() );
		MessageBox( NULL, tstr, TEXT("Test"), 0 );
	}
}
#endif /* _DEBUG */

DWORD WINAPI DllThread(LPVOID lpParam)
{
	UNREFERENCED_PARAMETER(lpParam);
#ifdef _DEBUG
	//test_GetClassInfoEx();
#endif /* _DEBUG */
	if( InitDLL() )
	{
		return 12345;
	}
#ifdef _DEBUG
	//test_GetClassInfoEx();
#endif /* _DEBUG */

	if( !Hook_ValidateAlign() )
	{
		MessageBox( NULL, TEXT("Structure alignment error!"), TEXT("Alignment error!"), MB_ICONSTOP );
		RaiseException( EXCEPTION_DATATYPE_MISALIGNMENT, 0, 0, NULL );
		return 123456;
	}

	// install hook over ws2_32.dll!connect()
	// SEH only under msvc
#ifdef _MSC_VER
	__try
	{
#endif
		if( !Hook_IsWinsockConnectOrig() )
		{
			log_error( LOG_WARNING, "Seems like ws2_32.dll!connect() is already hooked by someone...\n" );
			log_error( LOG_WARNING, "Some firewall/protection sofware?\n" );
			log_error( LOG_WARNING, "I'll try to set up my hook, but program may not function correctly later.\n" );
		}
		if( !Hook_CheckVirtualProtect() )
		{
			log_error( LOG_WARNING, "Seems like kernel32.dll!VirtualProtectEx() is hooked, so...\n" );
			log_error( LOG_WARNING, "Maybe we're running in GameGuard protected program?\n" );
			log_error( LOG_WARNING, "Be careful!\n" );
		}
		Hook_InterceptConnect_my();
		if( !Hook_ValidateInterception_my() )
		{
			log_error( LOG_ERROR, "Seems like my try to intercept ws2_32.dll!connect() failed.\n" );
			log_error( LOG_ERROR, "All network connections will not be intercepted!\n" );
		}
		//Hook_InterceptConnect_Dis(); // this unused..
#ifdef _MSC_VER
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		ErrorLogger_LogLastError( "Exception occured during call to Hook_InterceptConnect_my()", GetLastError() );
		log_error( LOG_ERROR, "We will try to continue execution...\n" );
		ErrorLogger_FlushLogFile();
	}
#endif

	L2PNet_setFunction( L2PFUNC_CONNECT, (void *)connect_l2packets_my );

	/*int i;
	i = 6;
	while( i >= 0 )
	{
		Hook_ValidateInterception_my();
		Sleep( 10000 );
		i--;
	}*/

	//UninitDLL();
	return 0;
}


void DllThreadStart()
{
	HANDLE hThread = NULL;
	DWORD dwTID = 0;
	hThread = (HANDLE)_beginthreadex( NULL, 0,
		(unsigned int (__stdcall *)(void *))DllThread, NULL, 0,
		(unsigned int *)&dwTID );
	if( hThread != NULL ) CloseHandle( hThread );
}

#ifdef L2D_INIT_SSL_LIBRARY
bool InitSSL()
{
	SSL_load_error_strings();
	SSL_library_init();
	void *rndbuf = malloc(1024);
	if( rndbuf )
	{
		RAND_seed( rndbuf, 1024 );
		free( rndbuf );
		return true;
	}
	return false;
}
#endif



//#define L2D_INIT_COMMON_CONTROLS
#ifdef L2D_INIT_COMMON_CONTROLS
#pragma comment( lib, "comctl32" )
#endif

void AreWeInIngameMode()
{
	TCHAR tszEvtName[256];
	wsprintf( tszEvtName, TEXT("L2Detect Outgame Event_%d"), (unsigned)GetCurrentProcessId() );
	HANDLE hEvtDbg = OpenEvent( EVENT_MODIFY_STATE, FALSE, tszEvtName );
	if( hEvtDbg == NULL ) // ingame
	{
		g_cfg.isInGameMode = true;
		CloseHandle( hEvtDbg );
		log_error( LOG_OK, "Running in ingame mode...\n" );
	}
	else
	{
		g_cfg.isInGameMode = false;
		log_error( LOG_OK, "Running in outgame mode...\n" );
	}
}

/** Initializes DLL. returns 0 on success **/
int InitDLL()
{
	// init common controls?
	//MyInitCommonControls();
#ifdef L2D_INIT_COMMON_CONTROLS
	//MyInitCommonControlsEx();
#endif

	// read global app config
	g_cfg.SetDefault();
	if( !g_cfg.ReadConfig( _T("L2Detect.ini") ) ) g_cfg.SetDefault();
	// determine INGAME or OUTGAME mode. NOW
	AreWeInIngameMode();

	// setup L2Packets Net Layer
	L2PNet_InitDefault();
	L2PacketNames_Init();

	// setup Logger
	ErrorLogger_Init();
	ErrorLogger_SetLogFile( NULL );
#ifdef _DEBUG
	ErrorLogger_SetAutoPrependErrorType( true );
	g_errorLogFile = fopen( "L2Detect.log", "wt" );
	ErrorLogger_SetLogFile( g_errorLogFile );
	ErrorLogger_Enable( true );
	//ErrorLogger_EnableLoggingToConsole( true );
	log_error( LOG_DEBUG, "InitDLL() called! int_dll_was_init = %d\n", int_dll_was_init );
	ErrorLogger_FlushLogFile();
#endif

	// Init COM
	HRESULT comRes = CoInitialize( NULL );
	if( comRes != S_OK )
	{
		log_error( LOG_ERROR, "CoInitialize() failed: HRESULT = %d (GetLastError() = %u)\n",
			comRes, (unsigned int)GetLastError() );
	}

	// init MyMem after logger
	//MyMem_Init();

	// init OpenSSL
#ifdef L2D_INIT_SSL_LIBRARY
	InitSSL();
#endif

	// init L2 Data
	char outErrMsg[512] = {0};
	L2Data_init();
	L2Data_DB_Init( outErrMsg, "L2Detect_items.sqlite3db", "L2Detect_mobs.sqlite3db", "L2Detect_skills.sqlite3db" );
	if( outErrMsg[0] ) MessageBoxA( NULL, outErrMsg, "L2Data_DB_Init() error!", MB_ICONSTOP );
	L2Data_SM_init();

	// setup binTrees
	CharArray_Init();
	ClanList_Init();
	NpcArray_Init();
	GIArray::getInstance();
	WorldObjectTree_Init();

	// create listeners
	// create LoginListener only in outgame mode
	g_plogin = NULL;
	if( g_cfg.isInGameMode == false )
	{
		g_plogin = new LoginListener();
		if( !g_plogin->start() )
		{
			log_error( LOG_ERROR, "Login server listener creation failed!\n" );
			MessageBox( NULL, TEXT("Login server listener creation failed!"),
				TEXT("Init error!"), MB_ICONSTOP );
			return 1;
		}
	} // create LoginListener only in outgame mode

	g_pgame = new GameListener();
	if( !g_pgame->start() )
	{
		log_error( LOG_ERROR, "Game server listener creation failed!\n" );
		MessageBox( NULL, TEXT("Game server listener creation failed!"),
			TEXT("Init error!"), MB_ICONSTOP );
		return 1;
	}

	// create global GameClient object (important!!!!!)
	g_game_client = new GameClient();

	// instantiate ScriptEngine
	ScriptEngine::getInstance()->init();

#ifdef _DEBUG
	ErrorLogger_FlushLogFile();
#endif

	RadarDllWindowStart( g_hinst );

	return 0; // ALL OK
}


/** Uninitializes DLL **/
int UninitDLL()
{
	log_error( LOG_DEBUG, "UninitDLL() called! int_dll_was_init = %d\n", int_dll_was_init );
	// delete ScriptEngine
	ScriptEngine::freeInstance();
	// stop listeners
	log_error( LOG_DEBUG, "Signaling listeners to exit...\n" );
	if( g_plogin ) g_plogin->signalStop(); // LoginListener may not be started in ingame mode at all
	g_pgame->signalStop();
	// wait for stop
	if( g_plogin ) // LoginListener may not be started in ingame mode at all
	{
		log_error( LOG_DEBUG, "Waiting for login listener to stop (10 secs)...\n" );
		if( !g_plogin->waitStopped( 10000 ) )
		{
			log_error( LOG_WARNING, "Login listener wait timeout expired... :( \n" );
			g_plogin->terminateThread();
		}
		else log_error( LOG_DEBUG, "Login listener wait OK\n" );
		delete g_plogin;
		g_plogin = NULL;
	}

	log_error( LOG_DEBUG, "Waiting for game listener to stop (10 secs)...\n" );
	if( !g_pgame->waitStopped( 10000 ) )
	{
		log_error( LOG_WARNING, "Game listener wait timeout expired... :( \n" );
		g_pgame->terminateThread();
	}
	else log_error( LOG_DEBUG, "Game listener wait OK\n" );
	delete g_pgame;
	g_pgame = NULL;

	delete g_game_client;

#ifdef _DEBUG
	ErrorLogger_FlushLogFile();
#endif


	//
	// Cleanup
	//

	// close database
	L2Data_DB_Free();
	L2Data_SM_free();

	// save global app config
	g_cfg.SaveConfig();
	// clean binTrees
	CharArray_Free();
	ClanList_Free();
	NpcArray_Free();
	GIArray::freeInstance();
	WorldObjectTree_Free();

	// close MyMem before logger
	//MyMem_Free();

	if( g_cfg.isInGameMode ) CleanupDirectDraw();

	// Uninitialize COM
	CoUninitialize();

	// close log
#ifdef _DEBUG
	log_error( LOG_OK, "Closing log. Last message.. Bye!\n" );
	ErrorLogger_FlushLogFile();
	ErrorLogger_SetLogFile( NULL );
	if( g_errorLogFile ) fclose( g_errorLogFile );
	g_errorLogFile = NULL;
#endif
	// shutdown winsock
	//WSACleanup();
	//L2PNet_Cleanup();

	return 0;
}
