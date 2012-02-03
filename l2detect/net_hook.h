#ifndef H_NET_HOOK
#define H_NET_HOOK

#include "net_hook_defs.h"

/** Validates istructure alignment. returns true on OK **/
bool Hook_ValidateAlign();

// my method
void Hook_InterceptConnect_my();
bool Hook_ValidateInterception_my();
bool Hook_IsWinsockConnectOrig();
bool Hook_CheckVirtualProtect();

// checking hooks
extern const unsigned char original_ws2_32_connect_6_bytes[6];
extern const unsigned char original_ws2_32_recv_6_bytes[6];
extern const unsigned char original_ws2_32_send_6_bytes[6];
extern const unsigned char original_ws2_32_WSAConnect_6_bytes[6];
extern const unsigned char original_ws2_32_WSARecv_6_bytes[6];
extern const unsigned char original_ws2_32_WSASend_6_bytes[6];
extern const unsigned char original_vpex_6_bytes[6];
extern const unsigned char l2walker_connect_6_bytes[6];
// ^^ orig_bytes
bool Hook_check_func_prolog( LPCWSTR dllName, LPCSTR funcName, const unsigned char *orig_bytes );

int __stdcall connect_hook_my( unsigned int sock, void *sockaddr, int addrlen );
int __stdcall connect_nohook_my( unsigned int sock, void *sockaddr, int addrlen );

// method of DistortNeo
void          Hook_InterceptConnect_Dis();
int __stdcall connect_hook_Dis( unsigned int sock, void *sockaddr, int addrlen );

/** Connects socket to specified address
 ** returns -1 on error/timeout, 0 on error */
int connect_l2packets_my( unsigned int sock, const char *ip, unsigned short port );


/////////////////////////////////////////////////////////////////////////////
//
// Функция ставит JMP-хук в функцию
//
// old_ptr - адрес старой функции
// new_ptr - адрес новой функции
// jmp_ptr - указатель на переменную, в которую запишется адрес перехода
//
void Hook_InterceptCall( DWORD old_ptr, DWORD new_ptr, DWORD *jmp_ptr );

#endif
