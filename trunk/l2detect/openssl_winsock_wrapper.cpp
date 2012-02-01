#include "stdafx.h"
#include "net_io/L2Packet_NetLayer.h"

int __stdcall closesocket( SOCKET s )
{
	return L2PNet_closesocket( s );
}

int __stdcall recv( SOCKET s, char *buf, int len, int flags )
{
	return L2PNet_recv( s, (unsigned char *)buf, (unsigned int)len );
}

int __stdcall send( SOCKET s, const char *buf, int len, int flags )
{
	return L2PNet_send( s, (const unsigned char *)buf, (unsigned int)len );
}

int __stdcall shutdown( SOCKET s, int how )
{
	return L2PNet_shutdown( s );
}

int __stdcall WSAGetLastError()
{
	return L2PNet_WSAGetLastError();
}

void __stdcall WSASetLastError( int iError )
{
	L2PNet_WSASetLastError( iError );
}
