#ifndef FL_ERRORLOGGER_H_
#define FL_ERRORLOGGER_H_

typedef enum eLOG_LEVEL
{
	LOG_OK = 0,
	LOG_ERROR,      // 1
	LOG_WARNING,    // 2
	LOG_USERAI,     // 3
	LOG_PACKETNAME, // 4
	LOG_DEBUG,      // 5
	LOG_DEBUGDUMP,  // 6
	LOGLEVEL_LAST = LOG_DEBUGDUMP
} LOG_LEVEL;

#define LOG_LEVELS 7

void  ErrorLogger_Init();
void  ErrorLogger_Enable( bool bEnable );
void  ErrorLogger_EnableLoggingToConsole( bool bEnable );
void  ErrorLogger_SetLogFile( FILE *f );
FILE *ErrorLogger_GetLogFile();
void  ErrorLogger_FlushLogFile();

void  ErrorLogger_SetAutoPrependErrorType( bool bPrepend );

int log_error( LOG_LEVEL logLevel, const char *_Format, ... );
int log_error_np( LOG_LEVEL logLevel, const char *_Format, ... ); // forces NO prepend even if set to

void  ErrorLogger_FormatLastError( char *msg, size_t nMaxCount, DWORD error_code );
void  ErrorLogger_LogLastError( char *comment, DWORD error_code );

#endif /*FL_ERRORLOGGER_H_*/
