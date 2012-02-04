#include "pch.h"
#include "d3d9_proxy.h"

// global variables
#pragma data_seg (".d3d9_shared")
HINSTANCE           gl_hOriginalDll;
HINSTANCE           gl_hThisInstance;
HINSTANCE           gl_hL2Detect;
#pragma data_seg ()

FILE *g_fLog = NULL;


BOOL WINAPI DllMain( HINSTANCE hInstDLL, DWORD reason, LPVOID lpvReserved )
{
	lpvReserved = NULL;
	switch( reason )
	{
	case DLL_PROCESS_ATTACH:
		{
			gl_hThisInstance = hInstDLL;
			DisableThreadLibraryCalls( hInstDLL );
			D3D9Proxy_Initialize();
		} break;
	case DLL_PROCESS_DETACH:
		{
			D3D9Proxy_Uninitialize();
		} break;
	}
	return TRUE;
}


void D3D9Proxy_Initialize()
{
	// init Logger
	ErrorLogger_Init();
	ErrorLogger_Enable( true );
#ifdef _DEBUG
	ErrorLogger_EnableLoggingToConsole( true );
#endif
	g_fLog = fopen( "D3D9Proxy.log", "wt" );
	ErrorLogger_SetLogFile( g_fLog );
	ErrorLogger_SetAutoPrependErrorType( true );
#ifdef _DEBUG
	ErrorLogger_SetWarnMessageLevel( LOG_DEBUGDUMP );
#else
	ErrorLogger_SetWarnMessageLevel( LOG_WARNING );
#endif
	log_error( LOG_OK, "D3D9Proxy_Initialize() called.\n" );
	
	// Initialisation
	gl_hOriginalDll        = NULL;
	gl_hThisInstance       = NULL;
	gl_hL2Detect           = NULL;
}


void D3D9Proxy_Uninitialize()
{
	log_error( LOG_OK, "D3D9Proxy_Uninitialize() called.\n" );
	
	// Release the system's d3d9.dll
	if( gl_hOriginalDll )
	{
		::FreeLibrary( gl_hOriginalDll );
		gl_hOriginalDll = NULL;
		log_error( LOG_DEBUG, "gl_hOriginalDll (d3d9) unloaded.\n" );
	}

	// Release L2Detect DLL
	if( gl_hL2Detect )
	{
		::FreeLibrary( gl_hL2Detect );
		gl_hL2Detect = NULL;
		log_error( LOG_DEBUG, "gl_hL2Detect (L2Detect) unloaded.\n" );
	}

	// close logger
	log_error( LOG_OK, "Closing log. Last message! Bye!\n=======================\n" );
	ErrorLogger_FlushLogFile();
	ErrorLogger_Enable( false );
#ifdef _DEBUG
	ErrorLogger_EnableLoggingToConsole( false );
#endif
	if( g_fLog )
	{
		ErrorLogger_SetLogFile( NULL );
		fflush( g_fLog );
		fclose( g_fLog );
		g_fLog = NULL;
	}
}


bool D3D9Proxy_LoadL2Detect()
{
	if( gl_hL2Detect != NULL )
		return true; // already loaded

	gl_hL2Detect = LoadLibraryW( L"L2Detect_d.dll" );
	if( gl_hL2Detect == NULL ) // first try failed
	{
		gl_hL2Detect = LoadLibraryW( L"L2Detect.dll" );
		if( gl_hL2Detect == NULL )
		{
			log_error( LOG_ERROR, "Failed to load radar DLL!  Tried to find L2Detect_d / L2Detect dlls.\n" );
			return false;
		}
		else
			log_error( LOG_OK, "Loaded L2Detect.dll at 0x%08X\n", gl_hL2Detect );
	}
	else
		log_error( LOG_OK, "Loaded L2Detect_d.dll (debug version) at 0x%08X\n", gl_hL2Detect );
	return true;
}


void LoadOriginalDll()
{
	wchar_t buffer[MAX_PATH];
	buffer[0] = 0;
	// Getting path to system dir and to d3d8.dll
	::GetSystemDirectory( buffer, MAX_PATH );
	// Append dll name
	wcscat( buffer, L"\\d3d9.dll" );
	// try to load the system's d3d9.dll, if pointer empty
	if( !gl_hOriginalDll ) gl_hOriginalDll = ::LoadLibrary( buffer );
	// Debug
	if( !gl_hOriginalDll )
	{
		log_error( LOG_ERROR, "LoadOriginalDll(): Original d3d9.dll not loaded!\n");
		ErrorLogger_FlushLogFile();
		::ExitProcess(0);
	}
}


// Exported function (faking d3d9.dll's one-and-only export)
IDirect3D9* WINAPI Direct3DCreate9( UINT SDKVersion )
{
	if( !gl_hOriginalDll ) LoadOriginalDll(); // looking for the "right d3d9.dll"

	// Hooking IDirect3D Object from Original Library
	typedef IDirect3D9 *(WINAPI* D3D9_Type)(UINT SDKVersion);
	D3D9_Type D3DCreate9_fn = (D3D9_Type)GetProcAddress( gl_hOriginalDll, "Direct3DCreate9" );

	// Debug
	if( !D3DCreate9_fn ) 
	{
		log_error( LOG_ERROR, "Direct3DCreate9(): Pointer to original Direct3DCreate9 function not received!\n" );
		ErrorLogger_FlushLogFile();
		::ExitProcess(0); // exit the hard way
	}

	if( D3D9Proxy_LoadL2Detect() )
		ErrorLogger_EnableLoggingToConsole( false );

	// Request pointer from Original Dll. 
	IDirect3D9 *pIDirect3D9_orig = D3DCreate9_fn( SDKVersion );

	// Return pointer to hooking Object instead of "real one"
	return pIDirect3D9_orig;
}
