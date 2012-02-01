#include "stdafx.h"
#include "BotIPC.h"
#include "Logger.h"


void BotInfo_Initialize( BOT_INFO *b, const wchar_t *charName, unsigned int charOid, unsigned int classId, int level )
{
	memset( b, 0, sizeof(BOT_INFO) );
	wcsncpy( b->charName, charName, 127 ); b->charName[127] = 0;
	b->classId = classId;
	b->level = level;
	b->charOid = charOid;
	b->isMaster = b->isSlave = false;
	b->assistEnable = b->followEnable = false;
	b->processId = GetCurrentProcessId();
}

LONG BotIPC_ExceptionHandler( const char *funcName, LPEXCEPTION_POINTERS ep )
{
	log_error( LOG_ERROR, "BotIPC: ExceptionHandler invoked from function [%s]: exc: 0x%08X at 0x%p\n",
		funcName,
		ep->ExceptionRecord->ExceptionCode,
		ep->ExceptionRecord->ExceptionAddress );
	return EXCEPTION_EXECUTE_HANDLER;
}



int BotIPC::s_refCount = 0;
BotIPC *BotIPC::s_instance = NULL;

BotIPC *BotIPC::getInstance()
{
	if( !BotIPC::s_instance )
	{
		BotIPC::s_instance = new BotIPC();
		BotIPC::s_refCount++;
		log_error( LOG_DEBUG, "BotIPC: instance created\n" );
	}
	return BotIPC::s_instance;
}


void BotIPC::freeInstance()
{
	if( BotIPC::s_instance )
	{
		BotIPC::s_refCount--;
		if( BotIPC::s_refCount == 0 )
		{
			delete BotIPC::s_instance;
			BotIPC::s_instance = NULL;
			log_error( LOG_DEBUG, "BotIPC: instance deleted\n" );
		}
	}
}


BotIPC::BotIPC()
{
#ifdef _DEBUG
	log_error( LOG_DEBUG, "BotIPC: one bot info size : %u\n", sizeof(BOT_INFO) );
	log_error( LOG_DEBUG, "BotIPC: shared mem size   : %u\n", sizeof(BOT_IPC_DESCRIPTOR) );
#endif
	iAmCreator = false;
	isLocked   = false;
	hMutex = hMapping = NULL;
	map        = NULL;
	// try to open mutex
	hMutex = OpenMutex( SYNCHRONIZE | MUTEX_MODIFY_STATE, FALSE, BOTIPC_MUTEX_NAME );
	if( !hMutex )
	{
		DWORD le = GetLastError();
		if( le != ERROR_FILE_NOT_FOUND )
		{
			log_error( LOG_ERROR, "BotIPC::BotIPC(): OpenMutex() failed: %u\n", le );
			ErrorLogger_LogLastError( "OpenMutex()", le );
			return;
		}
		log_error( LOG_DEBUG, "BotIPC::BotIPC(): mutex not exists, creating...\n" );
		hMutex = CreateMutex( NULL, FALSE, BOTIPC_MUTEX_NAME );
		if( !hMutex )
		{
			ErrorLogger_LogLastError( "CreateMutex() failed!", GetLastError() );
			return;
		}
	}
	// try to open file mapping
	hMapping = OpenFileMapping( FILE_MAP_READ | FILE_MAP_WRITE, FALSE, BOTIPC_MAPPING_NAME );
	if( !hMapping )
	{
		DWORD le = GetLastError();
		if( le != ERROR_FILE_NOT_FOUND )
		{
			log_error( LOG_ERROR, "BotIPC::BotIPC(): OpenFileMapping() failed: %u\n", le );
			ErrorLogger_LogLastError( "OpenFileMapping()", le );
			return;
		}
		log_error( LOG_DEBUG, "BotIPC::BotIPC(): file mapping not exists, creating...\n" );
		hMapping = CreateFileMapping( INVALID_HANDLE_VALUE, // page file
			NULL, PAGE_READWRITE | SEC_COMMIT, 0,
			sizeof(BOT_IPC_DESCRIPTOR), BOTIPC_MAPPING_NAME );
		if( !hMapping )
		{
			log_error( LOG_ERROR, "BotIPC::BotIPC(): CreateFileMapping() failed!\n", le );
			ErrorLogger_LogLastError( "v()", GetLastError() );
			return;
		}
		iAmCreator = true;
	}
	map = MapViewOfFile( hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(BOT_IPC_DESCRIPTOR) );
	if( !map )
	{
		log_error( LOG_ERROR, "BotIPC::BotIPC(): MapViewOfFile() failed!" );
		ErrorLogger_LogLastError( "MapViewOfFile()", GetLastError() );
	}
	//
	BOT_IPC_DESCRIPTOR *desc = (BOT_IPC_DESCRIPTOR *)map;
	if( iAmCreator ) // we created shmem, init it
	{
		Lock(); // lock reads/writes
		desc->dwSize = sizeof(BOT_IPC_DESCRIPTOR);
		desc->dwCreatorPid = GetCurrentProcessId();
		desc->uBotCount = 0; // no BOT_INFO structures while
		int i = 0;
		while( i<BOT_IPC_MAX_BOTS ) desc->bot[i++].charOid = 0;
		LockRelease(); // unlock
		log_error( LOG_OK, "BotIPC: created shared mem!\n" );
	}
	else
	{
		log_error( LOG_DEBUG, "BotIPC: opened existing shared mem, creator PID: %u; number of bots: %u\n",
			desc->dwCreatorPid, desc->uBotCount );
	}
	ErrorLogger_FlushLogFile();
}


BotIPC::~BotIPC()
{
	if( map )
	{
		if( !UnmapViewOfFile( map ) ) ErrorLogger_LogLastError( "UnmapViewOfFile()", GetLastError() );
		map = NULL;
		log_error( LOG_DEBUG, "BotIPC::~BotIPC(): shared memory view unmapped\n" );
	}
	if( hMutex )
	{
		LockRelease();
		CloseHandle( hMutex );
		hMutex = NULL;
		log_error( LOG_DEBUG, "BotIPC::~BotIPC(): mutex closed\n" );
	}
	if( hMapping )
	{
		CloseHandle( hMapping );
		hMapping = NULL;
		log_error( LOG_DEBUG, "BotIPC::~BotIPC(): mapping closed\n" );
	}
	iAmCreator = false;
	isLocked = false;
}


BOT_IPC_DESCRIPTOR *BotIPC::getDescriptor()
{
	if( !map )
	{
		log_error( LOG_WARNING, "BotIPC::getDescriptor(): failed, because shared mem is not mapped!\n" );
		return NULL;
	}
	return (BOT_IPC_DESCRIPTOR *)map;
}


bool BotIPC::amICreator() const
{
	return iAmCreator;
}



int BotIPC::addBotInfo( const BOT_INFO *b )
{
	if( !b ) return false;
	BOT_IPC_DESCRIPTOR *d = getDescriptor();
	if( !d ) return -1;
	if( !Lock() ) return -1;
	int added_idx = -1;
	__try
	{
		if( d->uBotCount < BOT_IPC_MAX_BOTS )
		{
			// find unused bot info slot
			int i;
			for( i=0; i<BOT_IPC_MAX_BOTS; i++ ) if( d->bot[i].charOid == 0 ) { added_idx = i; break; }
			if( added_idx == -1 )
			{
				log_error( LOG_ERROR, "BotIPC::addBotInfo(): ERROR: cannot find free slot to add!\n" );
			}
			else
			{
				memcpy( (void *)&(d->bot[added_idx]), (const void *)b, sizeof(BOT_INFO) );
				d->uBotCount++; // new one bot added
			}
		}
		else
		{
			log_error( LOG_ERROR, "BotIPC::addBotInfo(): failed beacuse mex bot count (%d) reached!\n", BOT_IPC_MAX_BOTS );
		}
	}
	__except( BotIPC_ExceptionHandler( "addBotInfo()", GetExceptionInformation() ) ) {}
	LockRelease();
	return added_idx;
}


bool BotIPC::getBotInfo( int index, BOT_INFO *b )
{
	if( !b ) return false;
	if( index<0 || index>=BOT_IPC_MAX_BOTS )
	{
		log_error( LOG_ERROR, "BotIPC::getBotInfo( index = %d ): invalid index\n", index );
		return false;
	}
	BOT_IPC_DESCRIPTOR *d = getDescriptor();
	if( !d )
	{
		log_error( LOG_ERROR, "BotIPC::getBotInfo( index = %d ): shared mem not mapped!\n", index );
		return false;
	}
	if( !Lock() ) return false;
	__try
	{
		BOT_INFO *src = &(d->bot[index]);
		memcpy( (void *)b, (const void *)src, sizeof(BOT_INFO) );
	}
	__except( BotIPC_ExceptionHandler( "getBotInfo()", GetExceptionInformation() ) ) {}
	LockRelease();
	return true;
}


bool BotIPC::setBotInfo( int index, const BOT_INFO *b )
{
	if( !b ) return false;
	if( index<0 || index>=BOT_IPC_MAX_BOTS )
	{
		log_error( LOG_ERROR, "BotIPC::setBotInfo(): invalid index %d\n", index );
		return false;
	}
	BOT_IPC_DESCRIPTOR *d = getDescriptor();
	if( !d )
	{
		log_error( LOG_ERROR, "BotIPC::setBotInfo(): shared mem not mapped!\n" );
		return false;
	}
	if( !Lock() ) return false;
	__try
	{
		BOT_INFO *dst = &(d->bot[index]);
		memcpy( (void *)dst, (const void *)b, sizeof(BOT_INFO) );
	}
	__except( BotIPC_ExceptionHandler( "setBotInfo()", GetExceptionInformation() ) ) {}
	LockRelease();
	return true;
}


bool BotIPC::delBot( int index )
{
	if( index<0 || index>=BOT_IPC_MAX_BOTS )
	{
		log_error( LOG_ERROR, "BotIPC::delBot( int index = %d ): invalid index\n", index );
		return false;
	}
	BOT_IPC_DESCRIPTOR *d = getDescriptor();
	if( !d )
	{
		log_error( LOG_ERROR, "BotIPC::delBot(): shared mem not mapped!\n" );
		return false;
	}
	if( !Lock() ) return false;
	bool ret = false;
	__try
	{
		if( d->bot[index].charOid > 0 )
		{
			d->bot[index].charOid = 0;
			ret = true;
		}
		else
		{
			ret = true;
			log_error( LOG_WARNING, "BotIPC::delBot( %d ): charOid is already 0!\n", index );
		}
	}
	__except( BotIPC_ExceptionHandler( "delBot()", GetExceptionInformation() ) ) {}
	LockRelease();
	return ret;
}


void BotIPC::delAllMyProcessBots()
{
	log_error( LOG_DEBUG, "BotIPC::delAllMyProcessBots(): start\n" );
	BOT_IPC_DESCRIPTOR *d = getDescriptor();
	if( !d )
	{
		log_error( LOG_ERROR, "BotIPC::delAllMyProcessBots(): shared mem not mapped!\n" );
		return;
	}
	if( !Lock() )
	{
		log_error( LOG_ERROR, "BotIPC::delAllMyProcessBots(): Lock failed!\n" );
		ErrorLogger_FlushLogFile();
		return;
	}
	int index = 0;
	unsigned int myPid = GetCurrentProcessId();
	int nFreed = 0;
	__try
	{
		for( index=0; index<BOT_IPC_MAX_BOTS; index++ )
		{
			if( d->bot[index].processId == myPid )
			{
				d->bot[index].charOid = 0;
				nFreed++;
				log_error( LOG_DEBUG, "BotIPC::delAllMyProcessBots(): deleted bot index %d\n", index );
			}
		}
	}
	__except( BotIPC_ExceptionHandler( "delAllMyProcessBots()", GetExceptionInformation() ) ) {}
	LockRelease();
	log_error( LOG_DEBUG, "BotIPC::delAllMyProcessBots(): deleted %d my bots\n", nFreed );
}

void BotIPC::dumpToLog()
{
	BOT_IPC_DESCRIPTOR *d = getDescriptor();
	if( !d ) return;
	if( !Lock() ) return;
	__try
	{
		int idx;
		log_error( LOG_DEBUG, "===== BotIPC::dumpToLog() =====\n" );
		for( idx = 0; idx < BOT_IPC_MAX_BOTS; idx++ )
		{
			if( d->bot[idx].charOid > 0 )
			{
				log_error( LOG_DEBUG, "Bot %d: char: %S lvl %d (class %d %s) [isSlave: %d] [isMaster: %d]\n",
					idx, d->bot[idx].charName, d->bot[idx].level, d->bot[idx].classId,
					L2Data_getClass( d->bot[idx].classId ),
					(int)d->bot[idx].isSlave, (int)d->bot[idx].isMaster );
			}
		}
		log_error( LOG_DEBUG, "===== BotIPC::dumpToLog() END =====\n" );
	}
	__except( BotIPC_ExceptionHandler( "dumpToLog()", GetExceptionInformation() ) ) {}
	LockRelease();
}





bool BotIPC::Lock()
{
	if( !hMutex ) return false;
	if( isLocked ) return true;
	DWORD wait_res = WaitForSingleObject( hMutex, 1000 );
	if( wait_res == WAIT_OBJECT_0 )
	{
		isLocked = true;
		return true;
	}
	// error handle
	DWORD le = GetLastError();
	log_error( LOG_ERROR, "BotIPC lock failed!!! possible fatal error, may need restarting all radars/bots!\n" );
	switch( wait_res )
	{
	case WAIT_TIMEOUT: log_error( LOG_ERROR, "BotIPC lock: wait_res: WAIT_TIMEOUT!\n" ); break;
	case WAIT_ABANDONED:
		{
			log_error( LOG_ERROR, "BotIPC lock: wait res: WAIT_TIMEOUT!\n" );
			// but in this case we own mutex
			log_error( LOG_WARNING, "Mutex wait abandoned, but now we own mutex... all OK\n" );
			isLocked = true;
			return true;
		} break;
	default: log_error( LOG_ERROR, "BotIPC lock: wait res: unk %u!\n", wait_res ); break;
	}
	ErrorLogger_LogLastError( "BotIPC lock: last error was: ", le );
	return false;
}

void BotIPC::LockRelease()
{
	if( !hMutex ) return;
	if( !isLocked ) return;
	if( !ReleaseMutex( hMutex ) )
	{
		log_error( LOG_WARNING, "BotIPC::LockRelease(): ReleaseMutex failed (not owner?)\n" );
		ErrorLogger_LogLastError( "ReleaseMutex()", GetLastError() );
	}
	isLocked = false;
}

