#include "pch.h"
#include "hooks.h"

unsigned int hook_addr_ws2_connect = 0;
unsigned int hook_addr_ws2_send = 0;
unsigned int hook_addr_ws2_recv = 0;
unsigned int hook_addr_ws2_WSAconnect = 0;
unsigned int hook_addr_ws2_WSAsend = 0;
unsigned int hook_addr_ws2_WSArecv = 0;

const unsigned char hook_original_connect_6_bytes[6]    = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x83 };
const unsigned char hook_original_send_6_bytes[6]       = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x83 };
const unsigned char hook_original_recv_6_bytes[6]       = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x83 };
const unsigned char hook_original_WSAconnect_6_bytes[6] = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x51 };
const unsigned char hook_original_WSArecv_6_bytes[6]    = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x51 };
const unsigned char hook_original_WSAsend_6_bytes[6]    = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x51 };
const unsigned char hook_original_vpex_6_bytes[6]       = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x56 };

unsigned char hook_my_connect_prolog[5] = {0,0,0,0,0};

#pragma pack( push, 1 )
struct hook_jmp_addr
{
	unsigned char instr_jmp;
	unsigned int  jmp_addr;
};
#pragma pack( pop )

int __stdcall connect_hook( int sock, void *psockaddr, int addrlen );
int __stdcall call_real_connect( int s, void *a, int l );

void Hook_connect( bool write_hook )
{
	BOOL bRet = FALSE;
	DWORD oldProtect = 0;
	HINSTANCE hws2 = LoadLibraryW( L"ws2_32.dll" );
	if( !hws2 )
	{
		ErrorLogger_LogLastError( "Hook_connect(): cannot load ws2_32.dll!", GetLastError() );
		return;
	}

	// find ws_32.dll!connect() address
	hook_addr_ws2_connect = (unsigned int)GetProcAddress( hws2, "connect" );
	if( hook_addr_ws2_connect == 0 )
	{
		ErrorLogger_LogLastError( "Hook_connect(): cannot find address of connect()!", GetLastError() );
		return;
	}

	hook_addr_ws2_send = (unsigned int)GetProcAddress( hws2, "send" );
	hook_addr_ws2_recv = (unsigned int)GetProcAddress( hws2, "recv" );
	hook_addr_ws2_WSAconnect = (unsigned int)GetProcAddress( hws2, "WSAConnect" );
	hook_addr_ws2_WSAsend = (unsigned int)GetProcAddress( hws2, "WSASend" );
	hook_addr_ws2_WSArecv = (unsigned int)GetProcAddress( hws2, "WSARecv" );

	log_error( LOG_DEBUG, "hooks: ws2_32.dll loaded at 0x%08X, connect() found at 0x%08X\n", hws2, hook_addr_ws2_connect );

	if( write_hook )
	{
		bRet = VirtualProtectEx( (HANDLE)-1, (LPVOID)hook_addr_ws2_connect, 5, PAGE_EXECUTE_READWRITE, &oldProtect );
		if( !bRet )
		{
			ErrorLogger_LogLastError( "Hook_connect(): VirtualProtectEx() failed!", GetLastError() );
			return;
		}
	}

	hook_jmp_addr st;
	st.instr_jmp = 0xE9;
	st.jmp_addr = (unsigned int)connect_hook - (unsigned int)hook_addr_ws2_connect - 5;
	// save my prolog bytes
	memcpy( (void *)&hook_my_connect_prolog, (const void *)&st, sizeof(hook_jmp_addr) );

	if( write_hook )
	{
		unsigned char *src = (unsigned char *)&st;
		unsigned char *dst = (unsigned char *)hook_addr_ws2_connect;
		// write hook code
		dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; dst[3] = src[3]; dst[4] = src[4];
		log_error( LOG_DEBUG, "Hook_connect(): connect hook written\n" );
	}

	ErrorLogger_FlushLogFile();
}

int __stdcall connect_hook( int sock, void *psockaddr, int addrlen )
{
	sockaddr_in *psaddr = (sockaddr_in *)psockaddr;
	char ip[16] = {0};
	strcpy( ip, inet_ntoa( psaddr->sin_addr ) );
	int port = psaddr->sin_port;
	int host_port = ntohs( psaddr->sin_port );
	log_error( LOG_DEBUG, "connect_hook( %d, 0x%p (%s:%d), %d );\n", sock, psaddr, ip, host_port, addrlen );
	ErrorLogger_FlushLogFile();
	// check if we should change connect address
	if( host_port == 7777 )
	{
		log_error( LOG_DEBUG, "Intercepting GS connection...\n" );
		int real_ip = psaddr->sin_addr.s_addr;
		//GameListener *gl = GameListener::getInstance();
		//gl->setRealPlayServerInfo( real_ip, port );
		//psaddr->sin_port = htons( (unsigned short)GameListener::LISTEN_PORT );
		psaddr->sin_addr.s_addr = inet_addr( "127.0.0.1" );
	}
	//
	int result = call_real_connect( sock, psockaddr, addrlen );
	log_error( LOG_DEBUG, "connect_hook(): orig. connect returned %d\n", result );
	ErrorLogger_FlushLogFile();
	return result;
}

__declspec(naked) int __stdcall call_real_connect( int s, void *a, int l )
{
	UNREFERENCED_PARAMETER( s );
	UNREFERENCED_PARAMETER( a );
	UNREFERENCED_PARAMETER( l );
	__asm
	{
		mov     edi, edi
		push    ebp
		mov     ebp, esp
		mov     eax, [hook_addr_ws2_connect]
		add     eax, 5
		//jmp     eax
		//jmp     [addr_ws2_connect]
		call    eax
		mov     esp, ebp
		pop     ebp
		ret
	}
}

/////////////////////////////////////////////////////////////////////////////
//
// Функция ставит JMP-хук в функцию
//
// old_ptr - адрес старой функции
// new_ptr - адрес новой функции
// jmp_ptr - указатель на переменную, в которую запишется адрес перехода
//
/*void Hook_InterceptCall( DWORD old_ptr, DWORD new_ptr, DWORD *jmp_ptr )
{
	hook_jmp_addr jump;
	jump.instr_jmp = 0xE9;
	jump.jmp_addr  = new_ptr - old_ptr - 5;

	DWORD oldProtect, prot;

	GetCurrentProcess();

	// Обычно страницы в этой области недоступны для записи
	// поэтому принудительно разрешаем запись
	prot = PAGE_EXECUTE_WRITECOPY; // PAGE_READWRITE;
	BOOL vp_res = VirtualProtectEx( (HANDLE)-1, (LPVOID)old_ptr, 5, prot, &oldProtect );
	if( !vp_res )
	{
		DWORD le = GetLastError();
		ErrorLogger_LogLastError( "Hook_InterceptCall(): VirtualProtectEx() failed", le );
		ErrorLogger_FlushLogFile();
		return;
	}

	// Пишем новый адрес
	//WriteProcessMemory(hprocess, (LPVOID)old_ptr, (void*)(&jump), 5, &written);
	unsigned char *po = (unsigned char *)old_ptr;
	unsigned char *pj = (unsigned char *)&jump;
	po[0] = pj[0];
	po[1] = pj[1];
	po[2] = pj[2];
	po[3] = pj[3];
	po[4] = pj[4];

	//восстанавливаем первоначальную защиту области по записи
	prot = oldProtect;
	VirtualProtectEx( (HANDLE)-1, (LPVOID)old_ptr, 5, prot, &oldProtect );

	(*jmp_ptr) = old_ptr + 5;
}*/

int hook_compare_bytes( unsigned int addr, const unsigned char *bytes, size_t size )
{
	unsigned char *pcur = (unsigned char *)addr;
	return memcmp( pcur, bytes, size );
}

void hook_log_bytes_uint( unsigned int addr, size_t sz )
{
	unsigned char *b = (unsigned char *)addr;
	size_t s = 0;
	while( s < sz )
	{
		log_error_np( LOG_OK, "%02X", (unsigned int)(b[s]) );
		s++;
		if( s < sz ) log_error_np( LOG_OK, " " );
	}
}

bool Hook_validateIntercept_connect()
{
	int r = hook_compare_bytes( hook_addr_ws2_connect, hook_my_connect_prolog, 5 );
	if( r == 0 )
	{
		log_error( LOG_OK, "Hooks: ws2_32!connect is hooked by us!\n" );
		ErrorLogger_FlushLogFile();
		return true;
	}
	else
	{
		log_error( LOG_WARNING, "Hooks: our ws2_32!connect hook is not installed!\n" );
		r = hook_compare_bytes( hook_addr_ws2_connect, hook_original_connect_6_bytes, 6 );
		if( r == 0 ) log_error( LOG_WARNING, "Hooks: ws_32!connect is original\n" );
		else
		{
			log_error( LOG_WARNING, "Hooks: ws_32!connect hook unknown!\n" );
			log_error( LOG_WARNING, "Hooks: bytes: " );
			hook_log_bytes_uint( hook_addr_ws2_connect, 6 );
			log_error_np( LOG_WARNING, "\n" );
		}
	}
	ErrorLogger_FlushLogFile();
	return false;
}

void Hook_dbg_displayInterceptionState()
{
	int r = 0;
	log_error( LOG_DEBUG, "Hook_dbg_displayInterceptionState(): 0 if original, 1 if hooked\n" );
	// connect
	r = hook_compare_bytes( hook_addr_ws2_connect, hook_original_connect_6_bytes, 6 );
	log_error( LOG_DEBUG, "connect: %d (", r ); hook_log_bytes_uint( hook_addr_ws2_connect, 6 ); log_error_np( LOG_DEBUG, ")\n" );
	// send
	r = hook_compare_bytes( hook_addr_ws2_send, hook_original_send_6_bytes, 6 );
	log_error( LOG_DEBUG, "send: %d (", r ); hook_log_bytes_uint( hook_addr_ws2_send, 6 ); log_error_np( LOG_DEBUG, ")\n" );
	// recv
	r = hook_compare_bytes( hook_addr_ws2_recv, hook_original_recv_6_bytes, 6 );
	log_error( LOG_DEBUG, "recv: %d (", r ); hook_log_bytes_uint( hook_addr_ws2_recv, 6 ); log_error_np( LOG_DEBUG, ")\n" );
	// WSConnect
	r = hook_compare_bytes( hook_addr_ws2_WSAconnect, hook_original_WSAconnect_6_bytes, 6 );
	log_error( LOG_DEBUG, "WSAConnect: %d (", r ); hook_log_bytes_uint( hook_addr_ws2_WSAconnect, 6 ); log_error_np( LOG_DEBUG, ")\n" );
	// WSASend
	r = hook_compare_bytes( hook_addr_ws2_WSAsend, hook_original_WSAsend_6_bytes, 6 );
	log_error( LOG_DEBUG, "WSASend: %d (", r ); hook_log_bytes_uint( hook_addr_ws2_WSAsend, 6 ); log_error_np( LOG_DEBUG, ")\n" );
	// WSARecv
	r = hook_compare_bytes( hook_addr_ws2_WSArecv, hook_original_WSArecv_6_bytes, 6 );
	log_error( LOG_DEBUG, "WSARecv: %d (", r ); hook_log_bytes_uint( hook_addr_ws2_WSArecv, 6 ); log_error_np( LOG_DEBUG, ")\n" );
	//
	log_error_np( LOG_DEBUG, "===================\n" );
}
