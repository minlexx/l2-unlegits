#include "stdafx.h"
#include "net_hook.h"
#include "Logger.h"

extern unsigned int  connect_orig; // будущий адрес оригинальной функции

DWORD Entry_connect; // перепрыг перехватчика

void Hook_InterceptConnect_Dis()
{
	//сначала получим абсолютный адрес функции для перехвата
	HINSTANCE hws2_32 = GetModuleHandle( TEXT("ws2_32.dll") );
	if( !hws2_32 )
	{
		MessageBox( NULL,
			TEXT("NULL == GetModuleHandle( TEXT(\"ws2_32.dll\") )"), TEXT("Error!"), MB_ICONSTOP );
		return;
	}
	connect_orig = (DWORD)GetProcAddress( hws2_32, "connect" );
	if( connect_orig == 0 )
	{
		MessageBox( NULL, TEXT("Can`t get address of connect_orig"), TEXT("Error!"), MB_ICONSTOP );
		return;
	}

	Hook_InterceptCall( (DWORD)connect_orig, (DWORD)connect_hook_Dis, &Entry_connect );
	if( *(char *)connect_orig != -23 )
	{
		MessageBox( NULL, TEXT("Connect was not intercepted!"),
			TEXT("Hook_InterceptConnect_Dis()"), MB_ICONSTOP );
#ifdef _DEBUG
		DebugBreak();
#endif
	}
}

int __stdcall connect_hook_Dis( unsigned int sock, void *sockaddr, int addrlen )
{
	int ans, retVal;
	ans = IDYES;
	retVal = -1;
	//Здесь вы можете порезвиться от души и выполнить любые, пришедшие вам в голову действия.
	my_sockaddr_in *paddr;
	paddr = (my_sockaddr_in *)sockaddr;
	unsigned int port;
	port = 0;

	if( paddr->sin_family == AF_INET )
	{
		port = 
			((paddr->sin_port & 0x00FF) << 8) |
			((paddr->sin_port & 0xFF00) >> 8);
#ifdef _DEBUG
		TCHAR sip[32];
		sip[0] = 0;
		_stprintf( sip, _T("%d.%d.%d.%d:%d"),
			(int)paddr->sin_addr.S_un.S_un_b.s_b1,
			(int)paddr->sin_addr.S_un.S_un_b.s_b2,
			(int)paddr->sin_addr.S_un.S_un_b.s_b3,
			(int)paddr->sin_addr.S_un.S_un_b.s_b4,
			port
		);
		/*ans = MessageBox( NULL, sip, TEXT("User_connect(): connect target IP?"),
			MB_ICONQUESTION | MB_YESNO );*/
#endif
		log_error( LOG_DEBUG, "connect_hook_Dis(): %d.%d.%d.%d:%d...\n",
			(int)paddr->sin_addr.S_un.S_un_b.s_b1,
			(int)paddr->sin_addr.S_un.S_un_b.s_b2,
			(int)paddr->sin_addr.S_un.S_un_b.s_b3,
			(int)paddr->sin_addr.S_un.S_un_b.s_b4,
			port
		);
	}

	if( ans == IDYES )
	{
		//Вызываем оригинальную функцию через указатель
		//int ret = (CONNECT_FUNC)( sock, sockaddr, addrlen );
		//ret = ( (int (__stdcall*)( unsigned int, void *, int ) )connect_orig)( sock, sockaddr, addrlen );
		
		 __asm
		{
			// save some registers
			//mov esp_save, esp
			//mov ebp_save, ebp
			push esp
			push ebp
			push edi
			// push parameters - right-to left order (__stdcall)
			push addrlen
			push sockaddr
			push sock
			// prolog?
			mov edi,edi
//			push ebp
//			mov ebp,esp

			call Entry_connect

			mov retVal, eax // save returned value

			pop edi // restore registers
			pop ebp
			pop esp
			//mov esp, esp_save
		}
	}

	return retVal; // error C2490: 'return' не допускается в функции с атрибутом 'naked'
}

