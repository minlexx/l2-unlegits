#include "stdafx.h"
#include "net_hook.h"
#include "Logger.h"
#include "ConfigIni.h"
#include "GameListener.h"

extern CConfig        g_cfg;
extern GameListener  *g_pgame;

unsigned char old_func_prologue[6] = {0,0,0, 0,0,0}; // ������� ��� �������� 6-�� ���������� ���� ������ �������
jmp_push_ret  jump_code;               // �������� ���������� push addr; ret
unsigned int  connect_orig;            // ������� ����� ������������ �������

//typedef int ( __stdcall *CONNECT_FUNC)( unsigned int /*socket*/, void * /*sockaddr*/, int /*addrlen*/ );

const unsigned char original_ws2_32_connect_6_bytes[6] = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x83 };
const unsigned char original_vpex_6_bytes[6]           = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x56 };
const unsigned char l2walker_connect_6_bytes[6]        = { 0xE9, 0xB1, 0x3A, 0xB7, 0x90, 0xC3 };

unsigned int   g_hook_flag_allow_write  = PAGE_EXECUTE_READWRITE; // PAGE_EXECUTE_WRITECOPY;
bool           g_hook_restore_read_only = false;
unsigned int   Proxied_VirtualProtectEx = 0;

BOOL __stdcall Proxy_VirtualProtectEx( HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect );

void Hook_InterceptConnect_my()
{
	//������� ������� ���������� ����� ������� ��� ���������
	HINSTANCE hws2_32 = GetModuleHandle( TEXT("ws2_32.dll") );
	if( !hws2_32 )
	{
		log_error( LOG_ERROR, "Hook_InterceptConnect_my(): cannot get module handle of ws2_32.dll!\n" );
		ErrorLogger_FlushLogFile();
		return;
	}
	connect_orig = (DWORD)GetProcAddress( hws2_32, "connect" );
	if( connect_orig == 0 )
	{
		log_error( LOG_ERROR, "Hook_InterceptConnect_my(): cannot get adress of original connect()!\n" );
		ErrorLogger_FlushLogFile();
		return;
	}

	if( Proxied_VirtualProtectEx ) log_error( LOG_WARNING, "Using proxied VirtualProtectEx!\n" );

	BOOL ret;
	DWORD old_protect = 0, old_protect_2 = 0;
	unsigned char *pj, *po;

	// struct member alignment must be == 1 !!!!!
	// ������� �������� ��� ���������� ��������, ������� ����� ������ � ������ ����������� ������:
	jump_code.instr_push = 0x68;
	jump_code.push_arg   = (unsigned int)connect_hook_my;
	jump_code.instr_ret  = 0xC3;

	// ��������� � �������� ������ ������������ 6 ���� ����������� API �������
	po = (unsigned char *)&old_func_prologue;
	pj = (unsigned char *)connect_orig;
	po[0] = pj[0]; po[1] = pj[1]; po[2] = pj[2];
	po[3] = pj[3]; po[4] = pj[4]; po[5] = pj[5];

	// remove read-only access to memory; remember prev.access rights
	//ret = VirtualProtect( (void *)connect_orig, 6, PAGE_EXECUTE_WRITECOPY, &old_protect );
	//ret = VirtualProtect( (void *)connect_orig, 6, PAGE_EXECUTE_READWRITE, &old_protect );
	DWORD flProtect = g_hook_flag_allow_write;
	if( Proxied_VirtualProtectEx )
		ret = Proxy_VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, flProtect, &old_protect );
	else
		ret = VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, flProtect, &old_protect );
	if( (ret != TRUE) )
	{
		DWORD le = GetLastError();
		ErrorLogger_LogLastError( "Hook_InterceptConnect_my(): VirtialProtectEx() failed (allow write)", le );
		ErrorLogger_FlushLogFile();
	}

	// ������� ������� �������� �� ���� ������� ������ ���� 6-�� ����
	pj = (unsigned char *)&jump_code;
	po = (unsigned char *)connect_orig;
	po[0] = pj[0]; po[1] = pj[1]; po[2] = pj[2];
	po[3] = pj[3]; po[4] = pj[4]; po[5] = pj[5];

	// restore previous access to memory
	if( g_hook_restore_read_only )
	{
		if( Proxied_VirtualProtectEx )
			ret = Proxy_VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, old_protect, &old_protect_2 );
		else
			ret = VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, old_protect, &old_protect_2 );
		if( (ret != TRUE) )
		{
			DWORD le = GetLastError();
			ErrorLogger_LogLastError( "Hook_InterceptConnect_my(): VirtualProtectEx() failed (restore prev)", le );
		}
	}
#ifdef _DEBUG
	log_error( LOG_DEBUG, "Hook_InterceptConnect_my(): work done?...\n" );
#endif
	ErrorLogger_FlushLogFile();
}


bool Hook_ValidateInterception_my()
{
	//������� ������� ���������� ����� ������� ��� ���������
	HINSTANCE hws2_32 = GetModuleHandle( TEXT("ws2_32.dll") );
	if( !hws2_32 )
	{
		log_error( LOG_ERROR, "Hook_ValidateInterception_my(): cannot get module handle of ws2_32.dll!\n" );
		ErrorLogger_FlushLogFile();
		return false;
	}
	unsigned int connect_ws2 = (unsigned int)GetProcAddress( hws2_32, "connect" );
	if( connect_ws2 == 0 )
	{
		log_error( LOG_ERROR, "Hook_ValidateInterception_my(): cannot get adress of original connect()!\n" );
		ErrorLogger_FlushLogFile();
		return false;
	}

	unsigned char *pj, *po, *pc;
	unsigned char current_connect_prolog[6] = {0,0,0,0,0,0};

	// ��������� � �������� ������ 6 ���� API �������
	pc = (unsigned char *)&current_connect_prolog;
	pj = (unsigned char *)connect_ws2;
	pc[0] = pj[0]; pc[1] = pj[1]; pc[2] = pj[2];
	pc[3] = pj[3]; pc[4] = pj[4]; pc[5] = pj[5];

	pj = (unsigned char *)&jump_code;
	po = (unsigned char *)&old_func_prologue;

	// test
	bool inok = true;
	int i = 0;
	for( i=0; i<6; i++ )
	{
		if( pc[i] != pj[i] ) inok = false;
	}

	LOG_LEVEL logLevel = LOG_DEBUG;
	if( !inok )
	{
		logLevel = LOG_WARNING;
		log_error( LOG_WARNING, "Not intercepted! Dump will follow...\n" );
	}
	else log_error( LOG_OK, "Interception OK!\n" );

	log_error( logLevel,
		"dump of machine codes:\n"
		"current (ws2) : %02X %02X %02X %02X %02X %02X\n"
		"jump code     : %02X %02X %02X %02X %02X %02X\n"
		"old           : %02X %02X %02X %02X %02X %02X\n",
		(int)pc[0], (int)pc[1], (int)pc[2], (int)pc[3], (int)pc[4], (int)pc[5],
		(int)pj[0], (int)pj[1], (int)pj[2], (int)pj[3], (int)pj[4], (int)pj[5],
		(int)po[0], (int)po[1], (int)po[2], (int)po[3], (int)po[4], (int)po[5]
	);
	ErrorLogger_FlushLogFile();

	return inok;
}

bool Hook_IsWinsockConnectOrig()
{
	//������� ������� ���������� ����� ������� ��� ���������
	HINSTANCE hws2_32 = GetModuleHandle( TEXT("ws2_32.dll") );
	if( !hws2_32 )
	{
		log_error( LOG_ERROR, "Hool_IsWinsockConnectOrig(): cannot get module handle of ws2_32.dll!\n" );
		ErrorLogger_FlushLogFile();
		return false;
	}
	unsigned int connect_ws2 = (unsigned int)GetProcAddress( hws2_32, "connect" );
	if( connect_ws2 == 0 )
	{
		log_error( LOG_ERROR, "Hool_IsWinsockConnectOrig(): cannot get adress of original connect()!\n" );
		ErrorLogger_FlushLogFile();
		return false;
	}

	unsigned char *pj, *po, *pc;
	unsigned char current_connect_prolog[6] = {0,0,0,0,0,0};

	// ��������� � �������� ������ 6 ���� API �������
	pc = (unsigned char *)&current_connect_prolog;
	pj = (unsigned char *)connect_ws2;
	pc[0] = pj[0]; pc[1] = pj[1]; pc[2] = pj[2];
	pc[3] = pj[3]; pc[4] = pj[4]; pc[5] = pj[5];

	// does current prolog equal to original?
	pj = (unsigned char *)&original_ws2_32_connect_6_bytes;
	po = (unsigned char *)&current_connect_prolog;
	// test it
	bool inok = true;
	int i = 0;
	for( i=0; i<6; i++ )
	{
		if( pc[i] != pj[i] ) inok = false;
	}

	LOG_LEVEL logLevel = LOG_DEBUGDUMP;
	if( !inok )
	{
		logLevel = LOG_WARNING;
		log_error( LOG_WARNING, "ws2_32 connect() is not original! Dump will follow...\n" );
	}
	else log_error( LOG_OK, "ws2_32 connect() is original!\n" );

	pc = current_connect_prolog;
	po = (unsigned char *)&original_ws2_32_connect_6_bytes;
	log_error( logLevel,
		"dump of machine codes:\n"
		"current (ws2) : %02X %02X %02X %02X %02X %02X\n"
		"original code : %02X %02X %02X %02X %02X %02X\n",
		(int)pc[0], (int)pc[1], (int)pc[2], (int)pc[3], (int)pc[4], (int)pc[5],
		(int)po[0], (int)po[1], (int)po[2], (int)po[3], (int)po[4], (int)po[5]
	);
	ErrorLogger_FlushLogFile();

	return inok;
}



// calls ws2_32.dll!connect() with hook
int __stdcall connect_hook_my( unsigned int sock, void *sockaddr, int addrlen )
{
	DWORD old_protect = 0;
	unsigned char *pc, *pj, *po;
	int ret;
	BOOL vp_ret = FALSE;
	DWORD le = 0;

	if( Proxied_VirtualProtectEx ) log_error( LOG_WARNING, "connect_hook_my(): Using proxied VirtualProtectEx!\n" );

/*#ifdef _DEBUG
	log_error( LOG_DEBUGDUMP, "connect_hook_my(): before restoring old code\n" );
	Hook_ValidateInterception_my();
#endif*/

	po = (unsigned char *)&old_func_prologue;
	pj = (unsigned char *)&jump_code;
	pc = (unsigned char *)connect_orig;

	//������� ��������������� 6 ������ ���� �������. ��� �� ������������ ��������, �� ��� ��������
	// ������� ������������ �������, � ��� ����� ������� ������������ �� �����:
	DWORD flProtect = g_hook_flag_allow_write;
	if( Proxied_VirtualProtectEx )
		vp_ret = Proxy_VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, flProtect, &old_protect );
	else
		vp_ret = VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, flProtect, &old_protect );
	if( !vp_ret )
	{
		le = GetLastError();
		ErrorLogger_LogLastError( "connect_hook_my(): VirtualProtectEx() failed (allow write)", le );
	}
	pc[0] = po[0]; pc[1] = po[1]; pc[2] = po[2];
	pc[3] = po[3]; pc[4] = po[4]; pc[5] = po[5];
	log_error( LOG_DEBUGDUMP, "connect_hook_my(): after restoring old code\n" );

/*#ifdef _DEBUG
	log_error( LOG_DEBUGDUMP, "connect_hook_my(): after restoring old code\n" );
	//Hook_ValidateInterception_my();
#endif*/

	ret = -1;
	//����� �� ������ ����������� �� ���� � ��������� �����, ��������� ��� � ������ ��������.
	my_sockaddr_in *paddr = (my_sockaddr_in *)sockaddr;
	unsigned int port = 0;
	if( paddr->sin_family == AF_INET )
	{
		port = ((paddr->sin_port & 0x00FF) << 8) | ((paddr->sin_port & 0xFF00) >> 8);
#ifdef _DEBUG
		log_error( LOG_DEBUG, "\n hooked: connect_hook_my(): %d.%d.%d.%d:%d... ",
			(int)paddr->sin_addr.S_un.S_un_b.s_b1,
			(int)paddr->sin_addr.S_un.S_un_b.s_b2,
			(int)paddr->sin_addr.S_un.S_un_b.s_b3,
			(int)paddr->sin_addr.S_un.S_un_b.s_b4,
			port );
		ErrorLogger_FlushLogFile();
#endif

		// =============== should we change ip & port? =========
		//if( port == 7776 || port == 7777 || port == 7778 )
		//if( port == g_cfg.IngameGameServerPort )
		if( port == 7776 || port == 7777 || port == 7778 || (port == ((unsigned int)g_cfg.IngameGameServerPort)) )
		{
			log_error( LOG_DEBUG, "connect_hook_my(): intercepting INGAME game server traffic on port %u\n", port );
			// save real game server ip:port
			PlayServerInfo psinfo;
			psinfo.port = (unsigned short)port;
			memcpy( (void *)(&psinfo.ip), (const void *)&(paddr->sin_addr.s_addr), 4 );
			// set play server info to game listener
			g_pgame->setPlayServerInfo( &psinfo );
			// redirect connection lo FakeListenGameIP : FakeListenGamePort
			paddr->sin_addr.s_addr = L2PNet_inet_addr( g_cfg.FakeListenGameIP );
			paddr->sin_port = L2PNet_htons( (unsigned short)g_cfg.FakeListenGamePort );
		}
		else log_error( LOG_DEBUG, "connect_hook_my(): not a game server traffic port: %u\n", port );
	}

	//�������� ������������ ������� ����� ���������
	ret = ( (int (__stdcall*)( unsigned int, void *, int ) )connect_orig)( sock, sockaddr, addrlen );
	// log
#ifdef _DEBUG
	if( paddr->sin_family == AF_INET )
		log_error_np( LOG_DEBUG, "original connect() returned %d\n", ret );
	ErrorLogger_FlushLogFile();
#endif

	//����� ��������  6 ���� ������� �� ������� �������� �� ���� �������
	pc[0] = pj[0]; pc[1] = pj[1]; pc[2] = pj[2];
	pc[3] = pj[3]; pc[4] = pj[4]; pc[5] = pj[5];
	log_error( LOG_DEBUGDUMP, "connect_hook_my(): after setting jump again\n" );

	// restore prev. protect
	if( g_hook_restore_read_only )
	{
		DWORD old_protect_2 = 0;
		if( Proxied_VirtualProtectEx )
			vp_ret = Proxy_VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, old_protect, &old_protect_2 );
		else
			vp_ret = VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, old_protect, &old_protect_2 );
		if( !vp_ret )
		{
			le = GetLastError();
			ErrorLogger_LogLastError( "connect_hook_my(): VirtualProtectEx() failed (restore access)", le );
		}
	}

/*#ifdef _DEBUG
	log_error( LOG_DEBUGDUMP, "connect_hook_my(): after setting jump again\n" );
	Hook_ValidateInterception_my();
#endif*/
	
	return ret;
}


// calls original ws2_32.dll!connect() without hooking
int __stdcall connect_nohook_my( unsigned int sock, void *sockaddr, int addrlen )
{
	DWORD old_protect = 0;
	DWORD old_protect_2 = 0;
	unsigned char *pc, *pj, *po;
	int ret = 0;
	BOOL vp_ret = FALSE;
	DWORD le = 0; // last error

	if( Proxied_VirtualProtectEx ) log_error( LOG_WARNING, "connect_nohook_my(): Using proxied VirtualProtectEx!\n" );

	po = (unsigned char *)&old_func_prologue;
	pj = (unsigned char *)&jump_code;
	pc = (unsigned char *)connect_orig;

	// ������� ��������������� 6 ������ ���� �������. ��� �� ������������ 
	// ��������, �� ��� �������� ������� ������������ �������, � ��� ����� ������� ������������ �� �����:
	DWORD flProtect = g_hook_flag_allow_write;
	if( Proxied_VirtualProtectEx )
		vp_ret = Proxy_VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, flProtect, &old_protect );
	else
		vp_ret = VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, flProtect, &old_protect );
	if( !vp_ret )
	{
		le = GetLastError();
		ErrorLogger_LogLastError( "connect_nohook_my(): VirtualProtectEx() failed (allow write)", le );
	}
	pc[0] = po[0]; pc[1] = po[1]; pc[2] = po[2];
	pc[3] = po[3]; pc[4] = po[4]; pc[5] = po[5];

	ret = -1;
	// log
#ifdef _DEBUG
	my_sockaddr_in *paddr = (my_sockaddr_in *)sockaddr;
	unsigned int port = 0;
	if( paddr->sin_family == AF_INET )
	{
		port = ((paddr->sin_port & 0x00FF) << 8) | ((paddr->sin_port & 0xFF00) >> 8);
		log_error( LOG_DEBUG, "connect_nohook_my(): %d.%d.%d.%d:%d... \n",
			(int)paddr->sin_addr.S_un.S_un_b.s_b1,
			(int)paddr->sin_addr.S_un.S_un_b.s_b2,
			(int)paddr->sin_addr.S_un.S_un_b.s_b3,
			(int)paddr->sin_addr.S_un.S_un_b.s_b4,
			port );
		ErrorLogger_FlushLogFile();
	}
#endif

	//�������� ������������ ������� ����� ���������
	ret = ( (int (__stdcall*)( unsigned int, void *, int ) )connect_orig)( sock, sockaddr, addrlen );

#ifdef _DEBUG
	// log
	if( paddr->sin_family == AF_INET )
		log_error( LOG_DEBUG, "connect_nohook_my(): original connect() returned %d\n", ret );
	ErrorLogger_FlushLogFile();
#endif

	//����� ��������  6 ���� ������� �� ������� �������� �� ���� �������
	pc[0] = pj[0]; pc[1] = pj[1]; pc[2] = pj[2];
	pc[3] = pj[3]; pc[4] = pj[4]; pc[5] = pj[5];

	// restore old access
	if( g_hook_restore_read_only )
	{
		if( Proxied_VirtualProtectEx )
			vp_ret = Proxy_VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, old_protect, &old_protect_2 );
		else
			vp_ret = VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, old_protect, &old_protect_2 );
		if( !vp_ret )
		{
			le = GetLastError();
			ErrorLogger_LogLastError( "connect_nohook_my(): VirtualProtectEx() failed (restore)", le );
		}
	}
	
	return ret;
}


/** Connects socket to specified address (no hook)
 ** returns -1 on error/timeout, 0 on error */
int connect_l2packets_my( unsigned int sock, const char *ip, unsigned short port )
{
	struct my_sockaddr_in addr;
	memset( &addr, 0, sizeof(my_sockaddr_in) );
	addr.sin_family      = AF_INET;
	addr.sin_port        = L2PNet_htons( port );
	addr.sin_addr.s_addr = L2PNet_inet_addr( ip );
	if( addr.sin_addr.s_addr == INADDR_NONE )
	{
		// try to resolve hostname
		L2PNet_resolveHostname( ip, (in_addr *)&addr.sin_addr );
	}

	log_error( LOG_DEBUG, "connect_l2packets_my(): sock_%u, %s:%u... ", sock, ip, (unsigned int)port );
	int ret = connect_nohook_my( sock, (void *)&addr, (int)sizeof(addr) );
	log_error_np( LOG_DEBUG, "connect_l2packets_my(): returning %d\n", ret );

	return ret;
}




/**
[WARN] seems like VirtualProtectEx is hooked!
Dump of machine codes:
current  : E9 FA 78 0A C9 56
original : 8B FF 55 8B EC 56
**/
bool Hook_CheckVirtualProtect()
{
	HINSTANCE hk32 = GetModuleHandle( TEXT("kernel32.dll") );
	if( !hk32 )
	{
		log_error( LOG_ERROR, "Hook_CheckVirtualProtect(): cannot get handle of kernel32.dll!\n" );
		return false;
	}
	unsigned int vpex_addr = (unsigned int)GetProcAddress( hk32, "VirtualProtectEx" );
	if( !vpex_addr )
	{
		log_error( LOG_ERROR, "Hook_CheckVirtualProtect(): cannot find VirtualProtectEx() in kernel32.dll!\n" );
		return false;
	}
	// read prolog
	unsigned char vp_prolog[6] = {0,0,0, 0,0,0};
	unsigned char *vpc = (unsigned char *)vpex_addr;
	vp_prolog[0] = vpc[0];  vp_prolog[1] = vpc[1];  vp_prolog[2] = vpc[2];
	vp_prolog[3] = vpc[3];  vp_prolog[4] = vpc[4];  vp_prolog[5] = vpc[5];
	bool ret = true;
	if( memcmp( vp_prolog, original_vpex_6_bytes, 6 ) == 0 )
	{
		log_error( LOG_OK, "seems like VirtualProtectEx is not hooked!\n" );
	}
	else
	{
		log_error( LOG_WARNING, "seems like VirtualProtectEx is hooked!\n" );
		log_error_np( LOG_WARNING, "Dump of machine codes:\n"
			"current  : %02X %02X %02X %02X %02X %02X\n"
			"original : %02X %02X %02X %02X %02X %02X\n",
			vp_prolog[0], vp_prolog[1], vp_prolog[2], vp_prolog[3], vp_prolog[4], vp_prolog[5],
			original_vpex_6_bytes[0], original_vpex_6_bytes[1], original_vpex_6_bytes[2],
			original_vpex_6_bytes[3], original_vpex_6_bytes[4], original_vpex_6_bytes[5] );
		Proxied_VirtualProtectEx = vpex_addr + 5;
	}
	//
	return ret;
}

__declspec(naked) BOOL __stdcall
	Proxy_VirtualProtectEx( HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect )
{
	UNREFERENCED_PARAMETER(hProcess);
	UNREFERENCED_PARAMETER(lpAddress);
	UNREFERENCED_PARAMETER(dwSize);
	UNREFERENCED_PARAMETER(flNewProtect);
	UNREFERENCED_PARAMETER(lpflOldProtect);
	// "jump" over GameGuard's interceptor hook
	__asm mov edi, edi
	__asm push ebp
	__asm mov ebp, esp
	__asm jmp Proxied_VirtualProtectEx
}
