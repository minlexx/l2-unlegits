#include "stdafx.h"
#include "Logger.h"
#include "RadarDllWndSize.h"

void RadarDllWnd_RestoreWindowSize( HWND hWnd )
{
	HKEY hKey = NULL;
	if( RegOpenKeyEx( HKEY_CURRENT_USER, TEXT("Software\\L2Detect"), 0,	KEY_READ, &hKey ) == NO_ERROR )
	{
		DWORD dwType = REG_DWORD, cb = 4;
		int x0 = 0, y0 = 0, w = 0, h = 0;
		cb = 4; RegQueryValueEx( hKey, TEXT("x0"), NULL, &dwType, (LPBYTE)&x0, &cb );
		cb = 4; RegQueryValueEx( hKey, TEXT("y0"), NULL, &dwType, (LPBYTE)&y0, &cb );
		cb = 4; RegQueryValueEx( hKey, TEXT("w"), NULL, &dwType, (LPBYTE)&w, &cb );
		cb = 4; RegQueryValueEx( hKey, TEXT("h"), NULL, &dwType, (LPBYTE)&h, &cb );
		if( x0 < 0 ) x0 = 1;
		if( y0 < 0 ) y0 = 1;
		if( x0 > 1200 ) x0 = 1200;
		if( y0 > 1000 ) y0 = 1000;
		if( w < 50 ) w = 50;
		if( h < 100 ) h = 100;
		if( x0>0 && y0>0 && w>0 && h>0 )
			MoveWindow( hWnd, x0, y0, w, h, TRUE );
	}
	else
	{
		DWORD le = GetLastError();
		ErrorLogger_LogLastError( "RadarDllWnd_RestoreWindowSize(): RegOpenKeyEx() failed", le );
	}
}

void RadarDllWnd_SaveWidowSize( HWND hWnd )
{
	RECT r;
	GetWindowRect( hWnd, &r );
	HKEY hKey = NULL;
	DWORD disp = 0;
	if( RegCreateKeyEx( HKEY_CURRENT_USER, TEXT("Software\\L2Detect"), 0, NULL,
		0, KEY_READ | KEY_WRITE, NULL, &hKey, &disp ) == NO_ERROR )
	{
		if( disp == REG_CREATED_NEW_KEY )
			log_error( LOG_DEBUG, "RadarDllWnd_SaveWidowSize(): created key\n" );
		if( disp == REG_OPENED_EXISTING_KEY )
			log_error( LOG_DEBUG, "RadarDllWnd_SaveWidowSize(): opened existing key\n" );
		RegSetValueEx( hKey, TEXT("x0"), 0, REG_DWORD, (LPCBYTE)&r.left, 4 );
		RegSetValueEx( hKey, TEXT("y0"), 0, REG_DWORD, (LPCBYTE)&r.top, 4 );
		DWORD dw = r.right - r.left;
		RegSetValueEx( hKey, TEXT("w"), 0, REG_DWORD, (LPCBYTE)&dw, 4 );
		dw = r.bottom - r.top;
		RegSetValueEx( hKey, TEXT("h"), 0, REG_DWORD, (LPCBYTE)&dw, 4 );
		RegCloseKey( hKey );
	}
	else ErrorLogger_LogLastError( "RadarDllWnd_SaveWidowSize(): RegCreateKeyEx() failed", GetLastError() );
}
