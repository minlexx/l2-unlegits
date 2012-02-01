#include "pch.h"
#include "Log.h"

HWND g_hWndLog = NULL;
extern HINSTANCE g_hInst;

CRITICAL_SECTION g_cs_log;
FILE *g_log_file = NULL;

HWND LogWnd_Create( HWND hWndParent )
{
	InitializeCriticalSectionAndSpinCount( &g_cs_log, 10 );
	RECT rc;
	GetClientRect( hWndParent, &rc );
	HWND hWndLog = CreateWindowEx( WS_EX_CLIENTEDGE, RICHEDIT_CLASS, TEXT(""),
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
			// richedit styles
			ES_LEFT | ES_NOHIDESEL | ES_READONLY | ES_MULTILINE,
		10, 10, rc.right-rc.left-20, 230, hWndParent, NULL, g_hInst, NULL );
	SendMessage( hWndLog, WM_SETTEXT, 0, (LPARAM)TEXT("") );
	SendMessage( hWndLog, EM_SETTEXTMODE, TM_RICHTEXT, 0 );
	SendMessage( hWndLog, EM_SETBKGNDCOLOR, FALSE, RGB(255,255,255) );
	SendMessage( hWndLog, EM_AUTOURLDETECT, TRUE, 0 );
	// setup default char format for control
	CHARFORMAT2 cf2;
	memset( &cf2, 0, sizeof(cf2) );
	cf2.cbSize = sizeof(cf2);
	cf2.dwMask = CFM_BACKCOLOR | CFM_COLOR | CFM_LINK | CFM_FACE | CFM_SIZE | CFM_WEIGHT;
	cf2.crBackColor = RGB(255,255,255);
	cf2.crTextColor = RGB(0,0,0);
	cf2.dwEffects = 0; //CFE_LINK | CFE_AUTOCOLOR | CFE_AUTOBACKCOLOR;
	_tcscpy( cf2.szFaceName, _T("Tahoma") );
	cf2.yHeight = 170;
	cf2.wWeight = FW_NORMAL;
	SendMessage( hWndLog, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf2 );
	SendMessage( hWndLog, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf2 );
	g_hWndLog = hWndLog;
	return hWndLog;
}

void Log( const wchar_t *_Format, ... )
{
	va_list _ArgList;
	va_start( _ArgList, _Format );
	// calc string len
	int llen = _vscwprintf( _Format, _ArgList );
	if( llen < 1 ) return;
	// alloc string
	wchar_t *message = (wchar_t *)malloc( llen*2 + 2 );
	if( !message ) return;
	// sprintf
	va_start( _ArgList, _Format );
	_vsnwprintf( message, llen, _Format, _ArgList );
	message[llen] = 0;
	//
	LogFull( true, true, 0, RGB(255,255,255), message );
	// free string
	free( message );
	message = NULL;
}

void Log_np( const wchar_t *_Format, ... )
{
	va_list _ArgList;
	va_start( _ArgList, _Format );
	// calc string len
	int llen = _vscwprintf( _Format, _ArgList );
	if( llen < 1 ) return;
	// alloc string
	wchar_t *message = (wchar_t *)malloc( llen*2 + 2 );
	if( !message ) return;
	// sprintf
	va_start( _ArgList, _Format );
	_vsnwprintf( message, llen, _Format, _ArgList );
	message[llen] = 0;
	//
	LogFull( false, false, 0, RGB(255,255,255), message );
	// free string
	free( message );
	message = NULL;
}

void LogError( const wchar_t *_Format, ... )
{
	va_list _ArgList;
	va_start( _ArgList, _Format );
	// calc string len
	int llen = _vscwprintf( _Format, _ArgList );
	if( llen < 1 ) return;
	// alloc string
	wchar_t *message = (wchar_t *)malloc( llen*2 + 2 );
	if( !message ) return;
	// sprintf
	va_start( _ArgList, _Format );
	_vsnwprintf( message, llen, _Format, _ArgList );
	message[llen] = 0;
	//
	LogFull( true, true, RGB(255,0,0), RGB(255,255,255), message );
	// free string
	free( message );
	message = NULL;
}

void LogWarning( const wchar_t *_Format, ... )
{
	va_list _ArgList;
	va_start( _ArgList, _Format );
	// calc string len
	int llen = _vscwprintf( _Format, _ArgList );
	if( llen < 1 ) return;
	// alloc string
	wchar_t *message = (wchar_t *)malloc( llen*2 + 2 );
	if( !message ) return;
	// sprintf
	va_start( _ArgList, _Format );
	_vsnwprintf( message, llen, _Format, _ArgList );
	message[llen] = 0;
	//
	LogFull( true, true, RGB(196,64,0), RGB(255,255,255), message );
	// free string
	free( message );
	message = NULL;
}

void LogDebug( const wchar_t *_Format, ... )
{
//#ifdef _DEBUG
	va_list _ArgList;
	va_start( _ArgList, _Format );
	// calc string len
	int llen = _vscwprintf( _Format, _ArgList );
	if( llen < 1 ) return;
	// alloc string
	wchar_t *message = (wchar_t *)malloc( llen*2 + 2 );
	if( !message ) return;
	// sprintf
	va_start( _ArgList, _Format );
	_vsnwprintf( message, llen, _Format, _ArgList );
	message[llen] = 0;
	//
	LogFull( true, true, RGB(128,128,128), RGB(255,255,255), message );
	// free string
	free( message );
	message = NULL;
//#else
//	_Format = NULL; // unreferenced parameter
//#endif
}

void LogFull( bool prependTime, bool appendNL, COLORREF crTextColor, COLORREF crBackColor, const wchar_t *message )
{
	//EnterCriticalSection( &g_cs_log ); // TODO: LogFull EnterCriticalSection
	//
	static wchar_t nl[2] = { L'\r', L'\0' }; // newline line-break
	// move selection after last char
	CHARRANGE cr;
	cr.cpMin = -1;
	cr.cpMax = -1;
	SendMessage( g_hWndLog, EM_EXSETSEL, 0, (LPARAM)&cr );
	// prepend time?
	if( prependTime )
	{
		wchar_t tstr[128];
		SYSTEMTIME st;
		GetLocalTime( &st );
		_snwprintf( tstr, 127, L"%02d:%02d:%02d.%03d ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );
		// log to file?
		if( g_log_file )
			fwprintf( g_log_file, L"%02d:%02d:%02d.%03d: ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );
		// out to window
		CHARFORMAT2 tcf2;
		memset( &tcf2, 0, sizeof(tcf2) );
		tcf2.cbSize = sizeof(tcf2);
		tcf2.dwMask = CFM_BACKCOLOR | CFM_COLOR | CFM_ITALIC | CFM_BOLD;
		tcf2.crBackColor = RGB(255,255,255);
		tcf2.crTextColor = RGB(100,100,100);
		tcf2.dwEffects = 0;
		SendMessage( g_hWndLog, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&tcf2 );
		SendMessage( g_hWndLog, EM_REPLACESEL, FALSE, (LPARAM)tstr );
	}
	// log to file?
	if( g_log_file ) fwprintf( g_log_file, L"%s", message );
	// out to window
	// set color
	CHARFORMAT2 cf2;
	memset( &cf2, 0, sizeof(cf2) );
	cf2.cbSize = sizeof(cf2);
	cf2.dwMask = CFM_BACKCOLOR | CFM_COLOR | CFM_ITALIC | CFM_BOLD;
	cf2.crBackColor = crBackColor;
	cf2.crTextColor = crTextColor;
	cf2.dwEffects = CFE_BOLD;
	SendMessage( g_hWndLog, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf2 );
	// add string
	SendMessage( g_hWndLog, EM_REPLACESEL, FALSE, (LPARAM)message );
	// add line-break
	if( appendNL )
	{
		SendMessage( g_hWndLog, EM_REPLACESEL, FALSE, (LPARAM)nl );
		if( g_log_file ) fwprintf( g_log_file, L"\n" );
	}
	//
	//LeaveCriticalSection( &g_cs_log );
}

void Log_EnableLogToFile( bool bEnable, FILE *f /*= NULL*/ )
{
	if( bEnable )
	{
		if( g_log_file ) fclose( g_log_file );
		g_log_file = f;
	}
	else
	{
		if( g_log_file ) fclose( g_log_file );
		g_log_file = NULL;
	}
}

void Log_FlushFile()
{
	if( g_log_file ) fflush( g_log_file );
}
