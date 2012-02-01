#include "stdafx.h"
#include "Logger.h"

#ifdef USE_SEH
#ifdef _MSC_VER // only under MS Visual C++

#ifdef _DEBUG

const int SEH_MAX_NAMES = 10000;

class SEH_NAME
{
public:
	char *name;
	unsigned int addr;
};

SEH_NAME *SEH_names[SEH_MAX_NAMES];
int SEH_loaded_names_count = 0;

bool SEH_loadMap()
{
	if( SEH_loaded_names_count > 0 ) return true; // already loaded
	log_error( LOG_DEBUG, "SEH_loadMap(): loading L2OOGHelper_d.map...\n" );
	FILE *f = fopen( "L2OOGHelper_d.map", "rt" );
	if( !f )
	{
		log_error( LOG_WARNING, "SEH_loadMap(): cannot load L2OOGHelper_d.map!\n" );
		return false;
	}
	//
	char line[1024];
	bool step1_ok = false;
	// pass first several lines
	while( !feof(f) )
	{
		line[0] = 0;
		fgets( line, sizeof(line), f );
		if( line[0] == 0 ) continue;
		if( strstr( line, "Address" ) && strstr( line, "Publics by Value" ) &&
			strstr( line, "Rva+Base" ) && strstr( line, "Lib:Object" ) )
		{
			step1_ok = true;
			break;
		}
	}
	if( !step1_ok )
	{
		log_error( LOG_WARNING, "SEH_loadMap(): Not a valid map file?\n" );
		fclose( f );
		return false;
	}
	//
	// init seh names array
	int i;
	for( i=0; i<SEH_MAX_NAMES; i++ ) SEH_names[i] = NULL;
	int nSEHNamesLoaded = 0;
	//
	while( !feof(f) )
	{
		line[0] = 0;
		fgets( line, sizeof(line), f );
		if( line[0] == 0 ) continue;
		if( strchr( line, ':' ) && (strlen( line ) > 21) )
		{
			char delimeter[] = " \r\n\t";
			char *token1 = strtok( line, delimeter );
			char *token2 = strtok( NULL, delimeter );
			char *token3 = strtok( NULL, delimeter );
			if( token1 && token2 && token3 )
			{
				unsigned int addr = 0;
				sscanf( token3, "%x", &addr );
				if( addr > 0 )
				{
					SEH_NAME *sename = new SEH_NAME();
					if( sename )
					{
						sename->addr = addr;
						sename->name = _strdup( token2 );
						SEH_names[nSEHNamesLoaded] = sename;
						//log_error( LOG_OK, "%d: %s (0x%x)\n",
						//	nSEHNamesLoaded, sename->name, sename->addr );
						nSEHNamesLoaded++;
					}
				}
			}
		}
		if( nSEHNamesLoaded >= SEH_MAX_NAMES )
		{
			log_error( LOG_WARNING, "SEH_loadMap(): loaded map names up to limit %d!\n",
				SEH_MAX_NAMES );
			break;
		}
	}
	//
	fclose( f );
	SEH_loaded_names_count = nSEHNamesLoaded;
	log_error( LOG_OK, "SEH_loadMap(): loaded L2OOGHelper_d.map (%d names).\n", nSEHNamesLoaded );
	return true;
}

void SEH_getFuncNameByAddr( unsigned int addr, char *outFN )
{
	outFN[0] = 0;
	if( !SEH_loadMap() ) return;
	int i;
	char *prev_name = NULL;
	unsigned int prev_addr = 0;
	for( i=0; i<SEH_loaded_names_count; i++ )
	{
		if( SEH_names[i] )
		{
			if( SEH_names[i]->addr > addr )
			{
				if( prev_name ) strcpy( outFN, prev_name );
				else strcpy( outFN, "<internal error>" );
				return;
			}
			prev_addr = SEH_names[i]->addr;
			prev_name = SEH_names[i]->name;
		}
	}
	strcpy( outFN, "<not found>" );
}

#endif

LONG WINAPI GlobalUnhandledExceptionFilter( LPEXCEPTION_POINTERS e )
{
	log_error( LOG_ERROR, "Unhandled exception!\n" );
	log_error( LOG_ERROR, "Exception code: 0x%08X at address 0x%08X\n",
		e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress );
	if( e->ExceptionRecord->ExceptionCode == 0xE06D7363 ) log_error( LOG_ERROR, "Microsoft C++ Exception?\n" );
#ifdef _DEBUG
	char fn[256] = {0};
	SEH_getFuncNameByAddr( e->ContextRecord->Eip, fn );
	log_error( LOG_ERROR, "In function: [%s]\n", fn );
#endif
	ErrorLogger_FlushLogFile();
	char errbuf[1024];
	sprintf( errbuf,
		"Unhandled exception!\n"
		"Exception code: 0x%08X at address 0x%08X\n"
		"Exception: EIP = %08X\n",
		e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress,
		e->ContextRecord->Eip );
	MessageBoxA( NULL, errbuf, "L2OOGHelper unhandled exception handler", MB_ICONSTOP );
	return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI ScriptEngineThreadExceptonHandler( LPEXCEPTION_POINTERS e )
{
	log_error( LOG_ERROR, "Exception in ScriptEngine thread... possible small memory leak\n" );
	log_error( LOG_ERROR, "Exception code: 0x%08X at address 0x%08X\n",
		e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress );
	if( e->ExceptionRecord->ExceptionCode == 0xE06D7363 ) log_error( LOG_ERROR, "Microsoft C++ Exception?\n" );
#ifdef _DEBUG
	char fn[256] = {0};
	SEH_getFuncNameByAddr( e->ContextRecord->Eip, fn );
	log_error( LOG_ERROR, "In function: [%s]\n", fn );
#endif
	ErrorLogger_FlushLogFile();
	char errbuf[1024];
	sprintf( errbuf,
		"Exception in ScriptEngine thread... possible small memory leak\n"
		"Exception code: 0x%08X at address 0x%08X\n"
		"Exception: EIP = %08X\n",
		e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress,
		e->ContextRecord->Eip );
	MessageBoxA( NULL, errbuf, "ScriptEngine exception handler", MB_ICONSTOP );
	return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI L2ClientThreadExceptonHandler( LPEXCEPTION_POINTERS e )
{
	log_error( LOG_ERROR, "Exception in L2Client thread...\n" );
	log_error( LOG_ERROR, "Exception code: 0x%08X at address 0x%08X\n",
		e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress );
	if( e->ExceptionRecord->ExceptionCode == 0xE06D7363 ) log_error( LOG_ERROR, "Microsoft C++ Exception?\n" );
#ifdef _DEBUG
	char fn[256] = {0};
	SEH_getFuncNameByAddr( e->ContextRecord->Eip, fn );
	log_error( LOG_ERROR, "In function: [%s]\n", fn );
#endif
	ErrorLogger_FlushLogFile();
	char errbuf[1024];
	sprintf( errbuf,
		"Exception in L2Client thread... \n"
		"Exception code: 0x%08X at address 0x%08X\n"
		"Exception: EIP = %08X\n",
		e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress,
		e->ContextRecord->Eip );
	MessageBoxA( NULL, errbuf, "L2ClientThread exception handler", MB_ICONSTOP );
	return EXCEPTION_EXECUTE_HANDLER;
}

#endif
#endif
