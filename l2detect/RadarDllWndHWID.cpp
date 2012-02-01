#include "stdafx.h"
#include "Logger.h"
#include "RadarDllWndHWID.h"
#include "HWID.h"

DWORD WINAPI RadarDllWnd_HWID_Check_Thread( LPVOID lpParam )
{
	HWND hWnd = (HWND)lpParam;
	//log_error( LOG_DEBUG, "RadarDllWnd_HWID_Check_Thread: started, sleeping...\n" );
	Sleep( 1000 );
	if( !VerifyHWID() )
	{
		log_error( LOG_ERROR, "RadarDllWnd_HWID_Check_Thread: HWID check failed!\n" );
		PostMessage( hWnd, WM_CLOSE, 0, 0 );
	}
	return 0;
}

void RadarDllWnd_Start_HWID_Check( HWND hWnd )
{
	HANDLE hThread = NULL;
	unsigned int tid = 0;
	hThread = (HANDLE)_beginthreadex( NULL, 0,
		(unsigned int (__stdcall *)(void *))RadarDllWnd_HWID_Check_Thread, (void *)hWnd,
		0, &tid );
	if( hThread ) CloseHandle( hThread );
}
