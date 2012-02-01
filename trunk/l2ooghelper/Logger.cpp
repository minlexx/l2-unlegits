#include "stdafx.h"
#include "Logger.h"

// global ErrorLogger vars
bool              g_error_logger_enabled;
bool              g_error_logger_enabled_console;
FILE             *g_error_logger_file;
bool              g_error_logger_auto_prepend;
CRITICAL_SECTION  g_error_logger_cs;
HANDLE            g_error_logger_stdout;
// strings
char              g_error_logger_strtype_unknown[16];
char              g_error_logger_strtype[LOG_LEVELS][16];
//
LOG_LEVEL         g_errorLogger_WarnMessageLevel;

void ErrorLogger_InitStrings()
{
	strcpy( g_error_logger_strtype_unknown,         "[??] " );
	strcpy( g_error_logger_strtype[LOG_OK],         "[++] " );
	strcpy( g_error_logger_strtype[LOG_ERROR],      "[--] " );
	strcpy( g_error_logger_strtype[LOG_WARNING],    "[WARN] " );
	strcpy( g_error_logger_strtype[LOG_USERAI],     "[AI] " );
	strcpy( g_error_logger_strtype[LOG_PACKETNAME], "[PACK] " );
	strcpy( g_error_logger_strtype[LOG_DEBUG],      "[DBG] " );
	strcpy( g_error_logger_strtype[LOG_DEBUGDUMP],  "[DUMP] " );
}

void ErrorLogger_Init()
{
	g_error_logger_enabled         = false;
	g_error_logger_enabled_console = false;
	g_error_logger_file            = stdout;
	g_error_logger_auto_prepend    = false;
	g_error_logger_stdout          = INVALID_HANDLE_VALUE;
	g_errorLogger_WarnMessageLevel = LOG_DEBUG;
	InitializeCriticalSection( &g_error_logger_cs );
	ErrorLogger_InitStrings();
}

void  ErrorLogger_Enable( bool bEnable )
{
	g_error_logger_enabled = bEnable;
}

void  ErrorLogger_SetWarnMessageLevel( LOG_LEVEL level )
{
	g_errorLogger_WarnMessageLevel = level;
}

void  ErrorLogger_EnableLoggingToConsole( bool bEnable )
{
	EnterCriticalSection( &g_error_logger_cs );
	if( bEnable )
	{
		if( g_error_logger_enabled_console == false ) AllocConsole();
		g_error_logger_stdout = GetStdHandle( STD_OUTPUT_HANDLE );
	}
	else
	{
		g_error_logger_stdout = INVALID_HANDLE_VALUE;
		if( g_error_logger_enabled_console == true ) FreeConsole();
	}
	g_error_logger_enabled_console = bEnable;
	LeaveCriticalSection( &g_error_logger_cs );
}

void ErrorLogger_SetLogFile( FILE *f )
{
	EnterCriticalSection( &g_error_logger_cs );
	if( !f ) g_error_logger_file = stdout; else g_error_logger_file = f;
	LeaveCriticalSection( &g_error_logger_cs );
}

FILE *ErrorLogger_GetLogFile()
{
	return g_error_logger_file;
}

void  ErrorLogger_FlushLogFile()
{
	EnterCriticalSection( &g_error_logger_cs );
	if( g_error_logger_file ) fflush( g_error_logger_file );
	LeaveCriticalSection( &g_error_logger_cs );
}

void ErrorLogger_SetAutoPrependErrorType( bool bPrepend )
{
	g_error_logger_auto_prepend = bPrepend;
}

int log_error( LOG_LEVEL logLevel, const char *_Format, ... )
{
	if( !g_error_logger_enabled ) return 0;
	int ret = 0;
	if( (logLevel < 0) || (!_Format) ) return 0;
	if( logLevel > g_errorLogger_WarnMessageLevel ) return 0;
	EnterCriticalSection( &g_error_logger_cs );
	va_list l;
	va_start( l, _Format );
	if( g_error_logger_auto_prepend )
	{
		char *szPrepend = g_error_logger_strtype_unknown;
		if( (logLevel >= LOG_OK) && (logLevel <= LOGLEVEL_LAST) )
			szPrepend = g_error_logger_strtype[logLevel];
		fprintf( g_error_logger_file, "%s", szPrepend );
		// also write to console, if enabled
		if( g_error_logger_enabled_console )
		{
			DWORD numWritten = 0;
			WriteConsoleA( g_error_logger_stdout, szPrepend, (DWORD)strlen(szPrepend), &numWritten, NULL );
		}
	}
	ret = vfprintf( g_error_logger_file, _Format, l );
	// also write to console, if enabled
	if( g_error_logger_enabled_console )
	{
		char *cbuffer = (char *)malloc( 20480 ); // 20 Kb >_<
		if( cbuffer )
		{
			va_list l2;
			va_start( l2, _Format );
			vsprintf( cbuffer, _Format, l2 );
			DWORD numWritten = 0;
			WriteConsoleA( g_error_logger_stdout, cbuffer, (DWORD)strlen(cbuffer), &numWritten, NULL );
			free( cbuffer );
		}
	}
	LeaveCriticalSection( &g_error_logger_cs );
	return ret;
}

// forces NO prepend even if set to
int log_error_np( LOG_LEVEL logLevel, const char *_Format, ... )
{
	if( !g_error_logger_enabled ) return 0;
	int ret = 0;
	if( (logLevel < 0) || (!_Format) ) return 0;
	if( logLevel > g_errorLogger_WarnMessageLevel ) return 0;
	EnterCriticalSection( &g_error_logger_cs );
	va_list l;
	va_start( l, _Format );
	// no prepend, just vfprintf
	ret = vfprintf( g_error_logger_file, _Format, l );
	// also write to console, if enabled?
	if( g_error_logger_enabled_console )
	{
		char *cbuffer = (char *)malloc( 20480 );
		if( cbuffer )
		{
			va_list l2;
			va_start( l2, _Format );
			vsprintf( cbuffer, _Format, l2 );
			DWORD numWritten = 0;
			WriteConsoleA( g_error_logger_stdout, cbuffer, (DWORD)strlen(cbuffer), &numWritten, NULL );
			free( cbuffer );
		}
	}
	LeaveCriticalSection( &g_error_logger_cs );
	return ret;
}

//int log_console( LOG_LEVEL logLevel, const char *_Format, ... )
//{
//	if( !g_error_logger_enabled_console ) return 0;
//}
//
//int log_console_np( LOG_LEVEL logLevel, const char *_Format, ... )
//{
//	if( !g_error_logger_enabled_console ) return 0;
//}

void  ErrorLogger_FormatLastError( char *msg, size_t nMaxCount, DWORD error_code )
{
	FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error_code, 0, msg, nMaxCount, NULL );
}

void  ErrorLogger_LogLastError( char *comment, DWORD error_code )
{
	char errbuf[512];
	errbuf[0] = 0;
	ErrorLogger_FormatLastError( errbuf, 511, error_code );
	errbuf[511] = 0;
	size_t ll = strlen( errbuf );
	if( ll > 0 )
	{
		if( errbuf[ll-1] == '\n' || errbuf[ll-1] == '\r' ) errbuf[ll-1] = 0;
		if( ll > 1 )
		{
			if( errbuf[ll-2] == '\n' || errbuf[ll-2] == '\r' ) errbuf[ll-2] = 0;
		}
	}
	log_error( LOG_ERROR, "%s: error code: %u (%s)\n", comment, (unsigned int)error_code, errbuf );
}
