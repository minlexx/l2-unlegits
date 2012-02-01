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
// ������� ������ JMP-��� � �������
//
// old_ptr - ����� ������ �������
// new_ptr - ����� ����� �������
// jmp_ptr - ��������� �� ����������, � ������� ��������� ����� ��������
//
void Hook_InterceptCall( DWORD old_ptr, DWORD new_ptr, DWORD *jmp_ptr );

#endif
