#include "stdafx.h"

#include "Logger.h"

HANDLE int_mymem_heap = NULL;
int int_mymem_was_init = 0;
unsigned int int_mymem_page_size = 0;

void MyMem_Init()
{
	if( int_mymem_was_init ) return;
	int_mymem_was_init = 1;
	SYSTEM_INFO sysinfo;
	memset( &sysinfo, 0, sizeof(sysinfo) );
	GetSystemInfo( &sysinfo );
	int_mymem_page_size = sysinfo.dwPageSize;
	int_mymem_heap = HeapCreate( HEAP_GENERATE_EXCEPTIONS, 1048576, 0 );
	if( !int_mymem_heap )
		FatalAppExit( 0, TEXT("Memory Manager failed to allocate heap (HeapCreate() failed!)") );
}

void MyMem_Free()
{
	if( !int_mymem_was_init ) return;
	int_mymem_was_init = 0;
	HeapDestroy( int_mymem_heap );
	int_mymem_heap = NULL;
}

// Memory tracking allocation
/*inline void *operator new( size_t size ) // non-debug version
{
	return HeapAlloc( int_mymem_heap, 0, (SIZE_T)size );
}*/

void *operator new( size_t nSize, const char *lpszFileName, int nLine )
{
	log_error( LOG_OK, "[MM]: calling new from %d:\"%s\"... ", nLine, lpszFileName );
	void *ret = HeapAlloc( int_mymem_heap, 0, (SIZE_T)nSize );
	log_error_np( LOG_OK, "0x%p\n", ret );
	ErrorLogger_FlushLogFile();
	return ret;
}

/*void operator delete( void* p )
{
	log_error( LOG_OK, "[MM]: calling delete( 0x%p )... ", p );
	BOOL bRet = FALSE;
	if( p ) bRet = HeapFree( int_mymem_heap, 0, p );
	if( bRet == FALSE ) log_error_np( LOG_OK, "FALSE\n" ); else log_error_np( LOG_OK, "TRUE\n" );
}*/

void operator delete( void* p, size_t size )
{
	log_error( LOG_OK, "[MM]: calling delete( 0x%p, %u )... ", p, (unsigned int)size );
	BOOL bRet = FALSE;
	if( p ) bRet = HeapFree( int_mymem_heap, 0, p );
	if( bRet == FALSE ) log_error_np( LOG_OK, "FALSE\n" ); else log_error_np( LOG_OK, "TRUE\n" );
}

void operator delete( void* p, const char *lpszFileName, int nLine )
{
	log_error( LOG_OK, "[MM]: calling delete( 0x%p ) from %d:\"%s\"... ",
		p, nLine, lpszFileName );
	BOOL bRet = FALSE;
	if( p ) bRet = HeapFree( int_mymem_heap, 0, p );
	if( bRet == FALSE ) log_error_np( LOG_OK, "FALSE\n" ); else log_error_np( LOG_OK, "TRUE\n" );
}
