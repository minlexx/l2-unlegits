#include "stdafx.h"
#include "net_hook.h"
#include "Logger.h"
#include "ConfigIni.h"

#define INSTR_INT3 0xCC

void Hook_GetTcpTable()
{
	DWORD oldProtect = 0;
	HANDLE hCurProc = GetCurrentProcess();
	HMODULE hIP = GetModuleHandleW( L"iphlpapi.dll" );
	if( !hIP )
	{
		log_error( LOG_ERROR, "Hook_GetTcpTable(): iphlpapi.dll not loaded\n" );
		return;
	}
	unsigned char *addr = (unsigned char *)GetProcAddress( hIP, "GetTcpTable" );
	if( addr )
	{
		log_error( LOG_DEBUG, "Hook_GetTcpTable(): INT3 on iphlpapi.dll!GetTcpTable() (addr = 0x%08X)\n", (unsigned)addr );
		Call_VirtualProtectEx( hCurProc, addr, 1, PAGE_READWRITE, &oldProtect );
		addr[0] = INSTR_INT3;
	}
	addr = (unsigned char *)GetProcAddress( hIP, "GetTcpTable2" );
	if( addr )
	{
		log_error( LOG_DEBUG, "Hook_GetTcpTable(): INT3 on iphlpapi.dll!GetTcpTable2() (addr = 0x%08X)\n", (unsigned)addr );
		Call_VirtualProtectEx( hCurProc, addr, 1, PAGE_READWRITE, &oldProtect );
		addr[0] = INSTR_INT3;
	}
}
