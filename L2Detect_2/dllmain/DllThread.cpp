#include "pch.h"
#include "l2d.h"
#include "hooks/hooks.h"

DWORD WINAPI DllThread( LPVOID lpvParam );

void L2D_ThreadStart()
{
	unsigned int tid = 0;
	HANDLE h = (HANDLE)_beginthreadex( NULL, 0,
		(unsigned int (__stdcall *)(void *)) DllThread, NULL, 0, &tid );
	if( h ) CloseHandle( h );
}

void test_hooks( int maxLoops, int delay )
{
	int i = 0;
	for( i=0; i<maxLoops; i++ )
	{
		Hook_dbg_displayInterceptionState();
		Sleep( delay );
	}
	ErrorLogger_FlushLogFile();
}

DWORD WINAPI DllThread( LPVOID lpvParam )
{
	UNREFERENCED_PARAMETER(lpvParam);
	//
	Hook_connect( false );
	//test_hooks( 100, 500 );
	//
	return 0;
}
