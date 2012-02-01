#pragma once

extern HWND g_hWndLog;

HWND LogWnd_Create( HWND hWndParent );

void Log( const wchar_t *_Format, ... );
void Log_np( const wchar_t *_Format, ... );
void LogError( const wchar_t *_Format, ... );
void LogWarning( const wchar_t *_Format, ... );
void LogDebug( const wchar_t *_Format, ... );

void LogFull( bool prependTime, bool appendNL, COLORREF crTextColor, COLORREF crBackColor, const wchar_t *message );

void Log_EnableLogToFile( bool bEnable, FILE *f = NULL );
void Log_FlushFile();

void Log_Win32Error( const wchar_t *comment, DWORD error_code );

