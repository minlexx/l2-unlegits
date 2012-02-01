#include "pch.h"
#include "Log.h"
#include "Debugging.h"

#define MAX_SYM_NAME_LEN 256

Debug *Debug::s_instance = NULL;

Debug *Debug::getInstance()
{
	if( !s_instance )
		s_instance = new Debug();
	return s_instance;
}

void Debug::freeInstance()
{
	if( s_instance )
	{
		delete s_instance;
		s_instance = NULL;
	}
}

/*BOOL CALLBACK Debug_EnumerateModulesProc64( PCSTR ModuleName, DWORD64 BaseOfDll, PVOID UserContext )
{
	//LogDebug( L" EnumerateModulesProc [%S] 0x%08X", ModuleName, (DWORD)BaseOfDll );
	wchar_t w[1024];
	wsprintfW( w, L" EnumerateModulesProc [%S] 0x%08X", ModuleName, (DWORD)BaseOfDll );
	MessageBox( NULL, w, L"Enum mod", 0 );
	return TRUE;
}*/

Debug::Debug()
{
	InitializeCriticalSectionAndSpinCount( &m_lock, 10 );
	EnterCriticalSection( &m_lock );
	m_initOK = false;
	m_symHandle = GetCurrentProcess();
	// set symbol handler options
	SymSetOptions( SYMOPT_UNDNAME | /*SYMOPT_DEFERRED_LOADS |*/ SYMOPT_LOAD_LINES );
	// create symbol search path
	WCHAR windir[256] = {0};
	WCHAR szSearchPath[256] = {0};
	GetWindowsDirectoryW( windir, 255 );
	wsprintfW( szSearchPath, L".;.\\Symbols;%s\\Symbols;%s\\Symbols\\dll", windir, windir );
	//MessageBoxW( NULL, szSearchPath, NULL, 0 );
	// initialize symbol handler
	BOOL ok = SymInitializeW( m_symHandle, szSearchPath/*NULL*/, TRUE );
	if( !ok )
	{
		DWORD le = GetLastError();
		wchar_t msg[512];
		wsprintfW( msg, L"SymInitialize() failed: %u", le );
		MessageBox( NULL, msg, L"Error", MB_ICONSTOP );
	}
	else
	{
		m_initOK = true;
		//
		// load PDBs for all modules possible
		//SymEnumerateModules64( m_symHandle, Debug_EnumerateModulesProc64, NULL );
		//
	}
	LeaveCriticalSection( &m_lock );
}

Debug::~Debug()
{
	EnterCriticalSection( &m_lock );
	if( m_initOK )
	{
		SymCleanup( m_symHandle );
		m_initOK = false;
	}
	LeaveCriticalSection( &m_lock );
	DeleteCriticalSection( &m_lock );
}

void Debug::createStackTrace( DebugStackTrace& traceObject )
{
	traceObject.clear();
	EnterCriticalSection( &m_lock );
	//
	// fill stackframe struct
	STACKFRAME64 stackFrame;
	memset( &stackFrame, 0, sizeof(stackFrame) );
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Mode = AddrModeFlat;
	stackFrame.AddrStack.Mode = AddrModeFlat;
	unsigned int i;
	__asm mov i, esp
	stackFrame.AddrStack.Offset = i;
	__asm mov i, ebp
	stackFrame.AddrFrame.Offset = i;
	//stackFrame.AddrPC.Offset = (DWORD64)(void *)(Debug::createStackTrace);
	stackFrame.AddrPC.Offset = (DWORD64)(void *)&stackFrame;
	BOOL bRet = TRUE;
	//
	bRet = StackWalk64( IMAGE_FILE_MACHINE_I386, this->m_symHandle, GetCurrentThread(), &stackFrame,
		NULL, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL );
	while( bRet )
	{
		DebugOneStackFrameInfo frameInfo;
		DWORD64 addr_offset = 0; // address offset
		SYMBOL_INFO *psymInfo = (SYMBOL_INFO *)malloc( sizeof(SYMBOL_INFO) + MAX_SYM_NAME_LEN + 1 );
		memset( psymInfo, 0, sizeof(SYMBOL_INFO) + MAX_SYM_NAME_LEN + 1 );
		psymInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
		psymInfo->MaxNameLen = MAX_SYM_NAME_LEN;
		if( stackFrame.AddrReturn.Offset > 0 )
		{
			bRet = SymFromAddr( this->m_symHandle, stackFrame.AddrReturn.Offset, &addr_offset, psymInfo );
			if( bRet )
			{
				// get module info
				char *moduleName = NULL;
				IMAGEHLP_MODULE64 moduleInfo;
				memset( &moduleInfo, 0, sizeof(moduleInfo) );
				moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64); // 584?
				if( !SymGetModuleInfo64( this->m_symHandle, psymInfo->Address, &moduleInfo ) )
				{
					Log_Win32Error( L"SymGetModuleInfo64() failed", GetLastError() );
					LogError( L"For addr 0x%08X name %S", (DWORD)psymInfo->Address, psymInfo->Name );
				}
				else moduleName = moduleInfo.ModuleName;
				//
				// get file name and line number
				IMAGEHLP_LINE64 lineinfo;
				memset( &lineinfo, 0, sizeof(lineinfo) );
				lineinfo.SizeOfStruct = sizeof(lineinfo);
				DWORD disp = 0;
				if( SymGetLineFromAddr64( GetCurrentProcess(), psymInfo->Address, &disp, &lineinfo ) )
				{
					frameInfo.set( psymInfo->Name, psymInfo->Address, addr_offset,
						lineinfo.FileName, lineinfo.LineNumber,
						moduleName );
				}
				else
				{
					DWORD le = GetLastError();
					if( le != ERROR_INVALID_ADDRESS ) // Attempt to access invalid address.
					{
						Log_Win32Error( L"dbghelp!SymGetLineFromAddr64() failed", le );
						LogError( L"For name: [%S]",  psymInfo->Name );
					}
					frameInfo.set( psymInfo->Name, psymInfo->Address, addr_offset, NULL, 0, moduleName );
				}
			}
			else
			{
				DWORD le = GetLastError();
				Log_Win32Error( L"dbghelp!SymFromAddr() failed", le );
				LogError( L"For addr: 0x%08X", (DWORD)stackFrame.AddrReturn.Offset );
			}
			// add frame info
			traceObject.addStackFrame( frameInfo );
		}
		free( psymInfo );
		//
		bRet = StackWalk64( IMAGE_FILE_MACHINE_I386, GetCurrentProcess(), GetCurrentThread(), &stackFrame, NULL,
			NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL );
	}
	DWORD le = GetLastError();
	if( le != ERROR_INVALID_ADDRESS  &&  le != NO_ERROR )
		Log_Win32Error( L"StackWalk64() ended", le );
	//
	// unlock
	LeaveCriticalSection( &m_lock );
}








DebugOneStackFrameInfo::~DebugOneStackFrameInfo()
{
	if( m_name ) free( m_name );
	m_name = NULL;
	if( m_fileName ) free( m_fileName );
	m_fileName = NULL;
	if( m_moduleName ) free( m_moduleName );
	m_moduleName = NULL;
	m_addr = 0;
	m_addr_offset = 0;
	m_line = 0;
}

DebugOneStackFrameInfo::DebugOneStackFrameInfo( const DebugOneStackFrameInfo& other )
{
	m_name = NULL;
	m_fileName = NULL;
	m_addr = 0;
	m_addr_offset = 0;
	m_line = 0;
	m_moduleName = NULL;
	this->operator=( other );
}

const DebugOneStackFrameInfo& DebugOneStackFrameInfo::operator=( const DebugOneStackFrameInfo& other )
{
	if( this == &other ) return (*this);
	set( other.m_name, other.m_addr, other.m_addr_offset, other.m_fileName, other.m_line, other.m_moduleName );
	return (*this);
}

// compares only address & addr offset
bool DebugOneStackFrameInfo::operator==( const DebugOneStackFrameInfo& other )
{
	if( this->m_addr != other.m_addr ) return false; // different addr
	if( this->m_addr_offset != other.m_addr_offset ) return false; // different offset
	return true; // addresses equal
}

void DebugOneStackFrameInfo::set(
		const char         *name,
		unsigned long long  addr,
		unsigned long long  addr_offset,
		const char         *fileName,
		unsigned int        line,
		const char         *moduleName )
{
	m_addr        = addr;
	m_addr_offset = addr_offset;
	m_line        = line;
	if( m_name ) free( m_name );
	m_name = NULL;
	if( name ) m_name = _strdup( name );
	if( m_fileName ) free( m_fileName );
	m_fileName = NULL;
	if( fileName ) m_fileName = _strdup( fileName );
	if( m_moduleName ) free( m_moduleName );
	m_moduleName = NULL;
	if( moduleName ) m_moduleName = _strdup( moduleName );
}








DebugStackTrace::DebugStackTrace()
{
	m_list.clear();
}

DebugStackTrace::~DebugStackTrace()
{
	m_list.clear();
}

void DebugStackTrace::clear()
{
	m_list.clear();
}

void DebugStackTrace::addStackFrame( DebugOneStackFrameInfo& ref )
{
	m_list.push_back( ref );
}

void DebugStackTrace::logStackTrace()
{
	std::list<DebugOneStackFrameInfo>::const_iterator iter = m_list.begin();
	int nFrame = 0;
	while( iter != m_list.end() )
	{
		DebugOneStackFrameInfo frm = (*iter);
		if( frm.getModuleName() == NULL )
		{
			if( frm.getFileName() )
			{
				Log( L"    %2d: %S()   (0x%08X + %I64u)   [%S line %u]", nFrame,
					frm.getName(), (unsigned int)frm.getAddr(), frm.getAddrOffset(),
					frm.getFileName(), frm.getLine() );
			}
			else
			{
				Log( L"    %2d: %S()   (0x%08X + %I64u)", nFrame,
					frm.getName(), (unsigned int)frm.getAddr(), frm.getAddrOffset() );
			}
		}
		else
		{
			if( frm.getFileName() )
			{
				Log( L"    %2d: %S!%S()   (0x%08X + %I64u)   [%S line %u]", nFrame,
					frm.getModuleName(), frm.getName(), (unsigned int)frm.getAddr(), frm.getAddrOffset(),
					frm.getFileName(), frm.getLine() );
			}
			else
			{
				Log( L"    %2d: %S!%S()   (0x%08X + %I64u)", nFrame,
					frm.getModuleName(), frm.getName(), (unsigned int)frm.getAddr(), frm.getAddrOffset() );
			}
		}
		nFrame++;
		iter++;
	}
}
