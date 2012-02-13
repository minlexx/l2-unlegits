#include "stdafx.h"
#include "net_hook.h"
#include "Logger.h"
#include "ConfigIni.h"
#include "GameListener.h"

extern CConfig        g_cfg;
extern GameListener  *g_pgame;

/* =================================
8B FF    MOV    EDI, EDI
55       PUSH   EBP
8B EC    MOV    EBP, ESP
..........................
51       PUSH   ECX
56       PUSH   ESI
5D       POP    EBP
=================================

WSAConnect      8B FF 55 8B EC 51
WSARecv         8B FF 55 8B EC 51
WSASend         8B FF 55 8B EC 51
connect         8B FF 55 8B EC 83
send            8B FF 55 8B EC 83
recv            8B FF 55 8B EC 83

accept          8B FF 55 8B EC 6A 00  (6A 00 = PUSH 00) ( really calls WSAAccept(1, 2, 3, 0, 0) )
WSAAccept       8B FF 55 8B EC 51 51  (51 51 = PUSH ECX; PUSH ECX)

listen          8B FF 55 8B EC 51 81

WSASocketA      8B FF 55 8B EC 81 EC  (81 EC = SUB ESP, 278)
WSASocketW      6A 20 68 A0 3D AA 75  (PUSH 20; PUSH rel a03daa75)
socket          8B FF 55 8B EC 51 56  (51 = PUSH ECX; 56 = PUSH ESI) (calls WSASocketW)

VirtualProtectEx (from kernelbase.dll)  8B FF 55 8B EC 56
VirtualProtectEx (kernel32)             8B FF 55 8B EC 5D     // jump follows, relocate,
                  //  rejump/relocate to VirtualProtectEx  inside  kernelbase.dll
===================================== */

#define INSTR_PUSH 0x68
#define INSTR_RET  0xC3

unsigned char old_func_prologue[6] = {0,0,0, 0,0,0}; // область для хранения 6-ти затираемых байт начала функции
jmp_push_ret  jump_code;               // машинные инструкции push addr; ret
unsigned int  connect_orig;            // будущий адрес оригинальной функции

//typedef int ( __stdcall *CONNECT_FUNC)( unsigned int /*socket*/, void * /*sockaddr*/, int /*addrlen*/ );

const unsigned char original_ws2_32_connect_6_bytes[6]    = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x83 };
const unsigned char original_ws2_32_recv_6_bytes[6]       = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x83 };
const unsigned char original_ws2_32_send_6_bytes[6]       = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x83 };
const unsigned char original_ws2_32_WSAConnect_6_bytes[6] = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x51 };
const unsigned char original_ws2_32_WSARecv_6_bytes[6]    = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x51 };
const unsigned char original_ws2_32_WSASend_6_bytes[6]    = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x51 };

const unsigned char original_ws2_32_listen_6_bytes[6]     = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x51 };
const unsigned char original_ws2_32_accept_6_bytes[6]     = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x6A };
const unsigned char original_ws2_32_WSAAccept_6_bytes[6]  = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x51 };
const unsigned char original_ws2_32_socket_6_bytes[6]     = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x51 };
const unsigned char original_ws2_32_WSASocketA_6_bytes[6] = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x81 };
const unsigned char original_ws2_32_WSASocketW_6_bytes[6] = { 0x6A, 0x20, 0x68, 0xA0, 0x3D, 0xAA };

const unsigned char original_vpex_6_bytes[6]              = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x56 };
const unsigned char l2walker_connect_6_bytes[6]           = { 0xE9, 0xB1, 0x3A, 0xB7, 0x90, 0xC3 };

unsigned int   g_hook_flag_allow_write  = PAGE_EXECUTE_READWRITE; // PAGE_EXECUTE_WRITECOPY;
bool           g_hook_restore_read_only = false;
unsigned int   Proxied_VirtualProtectEx = 0;


BOOL __stdcall Proxy_VirtualProtectEx( HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect );


void Hook_InterceptConnect_my()
{
	//сначала получим абсолютный адрес функции для перехвата
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
		log_error( LOG_ERROR, "Hook_InterceptConnect_my(): cannot get adress of ws2_32.dll!connect()!\n" );
		ErrorLogger_FlushLogFile();
		return;
	}

	if( Proxied_VirtualProtectEx )
		log_error( LOG_WARNING, "Hook_InterceptConnect_my(): Using proxied VirtualProtectEx!\n" );
	ErrorLogger_FlushLogFile();

	BOOL ret;
	DWORD old_protect = 0, old_protect_2 = 0;
	unsigned char *pj, *po;

	// struct member alignment must be == 1 !!!!!
	// Зададим машинный код инструкции перехода, который затем впишем в начало полученного адреса:
	jump_code.instr_push = INSTR_PUSH;                    // PUSH
	jump_code.push_arg   = (unsigned int)connect_hook_my; // connect_hook_my
	jump_code.instr_ret  = INSTR_RET;                     // RET

	// Прочитаем и сохраним первые оригинальные 6 байт стандартной API функции
	po = (unsigned char *)&old_func_prologue;
	pj = (unsigned char *)connect_orig;
	if( pj[0] != INSTR_PUSH )
	{
		po[0] = pj[0]; po[1] = pj[1]; po[2] = pj[2];
		po[3] = pj[3]; po[4] = pj[4]; po[5] = pj[5];
	}
	else
		log_error( LOG_WARNING, "Hook_InterceptConnect_my(): current connect() prolog already contains PUSH/RET code, do not overwriting old_prolog!\n" );

	// remove read-only access to memory; remember prev.access rights
	//ret = VirtualProtect( (void *)connect_orig, 6, PAGE_EXECUTE_WRITECOPY, &old_protect );
	//ret = VirtualProtect( (void *)connect_orig, 6, PAGE_EXECUTE_READWRITE, &old_protect );
	DWORD flProtect = g_hook_flag_allow_write;
	ret = 0;
	if( Proxied_VirtualProtectEx )
		ret = Proxy_VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, flProtect, &old_protect );
	else
		ret = VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, flProtect, &old_protect );
	if( (ret != TRUE) )
	{
		DWORD le = GetLastError();
		if( le == ERROR_ACCESS_DENIED )
			log_error( LOG_ERROR, "Hook_InterceptConnect_my(): VirtialProtectEx() failed (allow write): (%d) ERROR_ACCESS_DENIED\n", le );
		else
			ErrorLogger_LogLastError( "Hook_InterceptConnect_my(): VirtialProtectEx() failed (allow write)", le );
		ErrorLogger_FlushLogFile();
	}

	// Запишем команду перехода на нашу функцию поверх этих 6-ти байт
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
	log_error( LOG_DEBUG, "Hook_InterceptConnect_my(): END: work done?...\n" );
#endif
	ErrorLogger_FlushLogFile();
}


void Hook_RestoreConnect_my()
{
	HINSTANCE hws2_32 = GetModuleHandle( TEXT("ws2_32.dll") );
	if( !hws2_32 )
	{
		log_error( LOG_ERROR, "Hook_RestoreConnect_my(): cannot get module handle of ws2_32.dll!\n" );
		ErrorLogger_FlushLogFile();
		return;
	}
	connect_orig = (DWORD)GetProcAddress( hws2_32, "connect" );
	if( connect_orig == 0 )
	{
		log_error( LOG_ERROR, "Hook_RestoreConnect_my(): cannot get adress of ws2_32.dll!connect()!\n" );
		ErrorLogger_FlushLogFile();
		return;
	}

	if( Proxied_VirtualProtectEx )
		log_error( LOG_WARNING, "Hook_RestoreConnect_my(): START: Using proxied VirtualProtectEx!\n" );
	ErrorLogger_FlushLogFile();

	BOOL ret;
	DWORD old_protect = 0, old_protect_2 = 0;
	unsigned char *pc = NULL, *po = NULL;

	// check old prolog
	if( old_func_prologue[0] == 0x8B )
	{
		// make memory page readwrite
		DWORD flProtect = g_hook_flag_allow_write;
		ret = 0;
		if( Proxied_VirtualProtectEx )
			ret = Proxy_VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, flProtect, &old_protect );
		else
			ret = VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, flProtect, &old_protect );
		if( (ret != TRUE) )
		{
			DWORD le = GetLastError();
			if( le == ERROR_ACCESS_DENIED )
				log_error( LOG_ERROR, "Hook_RestoreConnect_my(): VirtialProtectEx() failed (allow write): (%d) ERROR_ACCESS_DENIED\n", le );
			else
				ErrorLogger_LogLastError( "Hook_RestoreConnect_my(): VirtialProtectEx() failed (allow write)", le );
			ErrorLogger_FlushLogFile();
		}
		po = (unsigned char *)&old_func_prologue;
		pc = (unsigned char *)connect_orig;
		pc[0] = po[0]; pc[1] = po[1]; pc[2] = po[2];
		pc[3] = po[3]; pc[4] = po[4]; pc[5] = po[5];
		log_error( LOG_OK, "Hook_RestoreConnect_my(): restored original connect...\n" );
		ErrorLogger_FlushLogFile();
	}
	else
		log_error( LOG_WARNING, "Hook_RestoreConnect_my(): old connect() prolog is invalid :(\n" );

	
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
			ErrorLogger_LogLastError( "Hook_RestoreConnect_my(): VirtualProtectEx() failed (restore prev)", le );
		}
	}

	ErrorLogger_FlushLogFile();
}


bool Hook_check_func_prolog( LPCWSTR dllName, LPCSTR funcName, const unsigned char *orig_bytes )
{
	HINSTANCE hDll = GetModuleHandleW( dllName );
	if( !hDll )
	{
		log_error( LOG_WARNING, "Hook_check_func_prolog(): module [%S] not found!\n", dllName );
		return false;
	}
	unsigned int func_addr = (unsigned int)GetProcAddress( hDll, funcName );
	if( func_addr == 0 )
	{
		log_error( LOG_WARNING, "Hook_check_func_prolog(): module [%S] does not have func [%s]\n", dllName, funcName );
		return false;
	}
	// read prolog
	unsigned char cur[6] = {0,0,0, 0,0,0};
	unsigned char *pc = (unsigned char *)func_addr;
	cur[0] = pc[0];  cur[1] = pc[1];  cur[2] = pc[2];
	cur[3] = pc[3];  cur[4] = pc[4];  cur[5] = pc[5];
	// compare
	if( memcmp( cur, orig_bytes, 6 ) == 0 )
	{
		log_error( LOG_OK, "Hook_check_func_prolog(): %S!%s() prolog OK\n", dllName, funcName );
		ErrorLogger_FlushLogFile();
		return true;
	}
	// not equal
	log_error( LOG_WARNING,
		"Hook_check_func_prolog(): %S!%s() prolog modified, dump of machine codes:\n"
		"  current : %02X %02X %02X %02X %02X %02X\n"
		"  orig    : %02X %02X %02X %02X %02X %02X\n",
		dllName, funcName,
		(int)cur[0], (int)cur[1], (int)cur[2], (int)cur[3], (int)cur[4], (int)cur[5],
		(int)orig_bytes[0], (int)orig_bytes[1], (int)orig_bytes[2],
			(int)orig_bytes[3], (int)orig_bytes[4], (int)orig_bytes[5]
	);
	ErrorLogger_FlushLogFile();
	return false;
}


bool Hook_ValidateInterception_my()
{
	//сначала получим абсолютный адрес функции для перехвата
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
	unsigned char current_prolog[6] = {0,0,0,0,0,0};

	// Прочитаем и сохраним первые 6 байт API функции
	pc = (unsigned char *)&current_prolog;
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
		log_error( LOG_WARNING, "ws2_32.dll!connect() Not intercepted! Dump will follow...\n" );
	}
	else log_error( LOG_OK, "ws2_32.dll!connect() Interception OK!\n" );

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
	return Hook_check_func_prolog( L"ws2_32.dll", "connect", original_ws2_32_connect_6_bytes );
}



// calls ws2_32.dll!connect() with hook
int __stdcall connect_hook_my( unsigned int sock, void *sockaddr, int addrlen )
{
	DWORD old_protect = 0;
	unsigned char *pc, *pj, *po;
	int ret;
	BOOL vp_ret = FALSE;
	DWORD le = 0;

	if( Proxied_VirtualProtectEx )
		log_error( LOG_WARNING, "connect_hook_my(): START: Using proxied VirtualProtectEx!\n" );

#ifdef _DEBUG
	log_error( LOG_DEBUGDUMP, "connect_hook_my(): before restoring old code\n" );
	Hook_ValidateInterception_my();
#endif

	po = (unsigned char *)&old_func_prologue;
	pj = (unsigned char *)&jump_code;
	pc = (unsigned char *)connect_orig;

	//Сначала восстанавливаем 6 первых байт функции. Это не обязательное действие, но нам придется
	// вызвать оригинальную функцию, а для этого следует восстановить ее адрес:
	DWORD flProtect = g_hook_flag_allow_write;
	if( Proxied_VirtualProtectEx )
		vp_ret = Proxy_VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, flProtect, &old_protect );
	else
		vp_ret = VirtualProtectEx( (HANDLE)-1, (void *)connect_orig, 6, flProtect, &old_protect );
	if( !vp_ret )
	{
		le = GetLastError();
		if( le == ERROR_ACCESS_DENIED )
			log_error( LOG_ERROR, "connect_hook_my(): VirtualProtectEx() failed (allow write): (%d) ERROR_ACCESS_DENIED\n", le );
		else
			ErrorLogger_LogLastError( "connect_hook_my(): VirtualProtectEx() failed (allow write)", le );
	}
	pc[0] = po[0]; pc[1] = po[1]; pc[2] = po[2];
	pc[3] = po[3]; pc[4] = po[4]; pc[5] = po[5];
	log_error( LOG_DEBUGDUMP, "connect_hook_my(): after restoring old code\n" );

#ifdef _DEBUG
	log_error( LOG_DEBUGDUMP, "connect_hook_my(): after restoring old code\n" );
	Hook_ValidateInterception_my();
#endif

	ret = -1;
	//Здесь вы можете порезвиться от души и выполнить любые, пришедшие вам в голову действия.
	my_sockaddr_in *paddr = (my_sockaddr_in *)sockaddr;
	unsigned int port = 0;
	if( paddr->sin_family == AF_INET )
	{
		port = ((paddr->sin_port & 0x00FF) << 8) | ((paddr->sin_port & 0xFF00) >> 8);
#ifdef _DEBUG
		log_error( LOG_DEBUG, "\n hooked: connect_hook_my(): Requested connection to: %d.%d.%d.%d:%d... ",
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

	//Вызываем оригинальную функцию через указатель
	ret = ( (int (__stdcall*)( unsigned int, void *, int ) )connect_orig)( sock, sockaddr, addrlen );
	// log
#ifdef _DEBUG
	if( paddr->sin_family == AF_INET )
		log_error_np( LOG_DEBUG, "... original connect() returned %d\n", ret );
	ErrorLogger_FlushLogFile();
#endif

	//Снова заменяем  6 байт функции на команду перехода на нашу функцию
	pc[0] = pj[0]; pc[1] = pj[1]; pc[2] = pj[2];
	pc[3] = pj[3]; pc[4] = pj[4]; pc[5] = pj[5];
	log_error( LOG_DEBUGDUMP, "connect_hook_my(): ENDING: after setting jump again\n" );

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

	if( Proxied_VirtualProtectEx )
		log_error( LOG_WARNING, "connect_nohook_my(): Using proxied VirtualProtectEx!\n" );

	po = (unsigned char *)&old_func_prologue;
	pj = (unsigned char *)&jump_code;
	pc = (unsigned char *)connect_orig;

	// Сначала восстанавливаем 6 первых байт функции. Это не обязательное 
	// действие, но нам придется вызвать оригинальную функцию, а для этого следует восстановить ее адрес:
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

	//Вызываем оригинальную функцию через указатель
	ret = ( (int (__stdcall*)( unsigned int, void *, int ) )connect_orig)( sock, sockaddr, addrlen );

#ifdef _DEBUG
	// log
	if( paddr->sin_family == AF_INET )
		log_error( LOG_DEBUG, "connect_nohook_my(): original connect() returned %d\n", ret );
	ErrorLogger_FlushLogFile();
#endif

	//Снова заменяем  6 байт функции на команду перехода на нашу функцию
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
	bool ret = false;
	// check fo kernelbase.dll
	HMODULE hKernelBaseDLL = GetModuleHandleW( L"kernelbase.dll" );
	unsigned int vpex_addr = 0;
	if( hKernelBaseDLL )
	{
		log_error( LOG_DEBUG, "Hook_CheckVirtualProtect(): kernelbase.dll found, using it\n" );
		ret = Hook_check_func_prolog( L"kernelbase.dll", "VirtualProtectEx", original_vpex_6_bytes );
		vpex_addr = (unsigned)GetProcAddress( hKernelBaseDLL, "VirtualProtectEx" );
		log_error( LOG_WARNING, "Force using my unhooked Proxy_VirtualProtectEx() to kernelbase.dll!\n" );
		Proxied_VirtualProtectEx = vpex_addr + 5;
	}
	else
	{
		log_error( LOG_DEBUG, "Hook_CheckVirtualProtect(): kernelbase.dll not found, using kernel32.dll\n" );
		ret = Hook_check_func_prolog( L"kernel32.dll", "VirtualProtectEx", original_vpex_6_bytes );
		vpex_addr = (unsigned)GetProcAddress( GetModuleHandleW( L"kernel32.dll" ), "VirtualProtectEx" );
	}
	
	if( ret )
	{
		log_error( LOG_OK, "Hook_CheckVirtualProtect(): seems like VirtualProtectEx() is not hooked!\n" );
	}
	else
	{
		log_error( LOG_WARNING, "seems like VirtualProtectEx() is hooked!\n" );
		log_error( LOG_WARNING, "Force using my unhooked Proxy_VirtualProtectEx...\n" );
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
