#include "stdafx.h"
#include "L2Packet_NetLayer.h"
#include <ws2tcpip.h>

// network function pointers table
#define l2pnet_ft_size 11
void *l2pnet_ft[l2pnet_ft_size] = {0, 0,0,0, 0,0,0, 0,0,0, 0};
CRITICAL_SECTION l2pnet_cs;
// winsock2 functions
HINSTANCE l2pnet_hws2_32;
void *ws2_func[l2pnet_ft_size] = {0, 0,0,0, 0,0,0, 0,0,0, 0};
char l2pnet_static_inet_ntoa_buffer[32];

typedef int  (__stdcall *select_func)( int, fd_set *, fd_set *, fd_set *, const struct timeval *);
typedef int  (__stdcall *WSAGetLastError_func)(void);
typedef void (__stdcall *WSASetLastError_func)( int );
typedef int  (__stdcall *WSAStartup_func)( WORD, WSADATA * );
typedef int  (__stdcall *WSACleanup_func)(void);
//typedef void *(__stdcall *gethostbyname_func)( const char * );
typedef int  (__stdcall *getaddrinfo_func)( const char *, const char *, void *, void * ); /* Success returns zero. Failure returns error code */
typedef void (__stdcall *freeaddrinfo_func)( void * );

//select_func select_winsock = NULL;

/** Initializes L2Packets network layer.
 ** By default network functions L2PNet_* will be used,
 ** which use corresponding Winsock2 functions.
 ** returns 1 on success, 0 on error */
int L2PNet_InitDefault()
{
	// global vars init
	int i = 0;
	TCHAR mes[128] = {0};
	l2pnet_static_inet_ntoa_buffer[0] = 0;
	InitializeCriticalSection( &l2pnet_cs );
	//
	for( i=0; i<l2pnet_ft_size; i++ ) l2pnet_ft[i] = NULL;
	l2pnet_hws2_32 = GetModuleHandle( TEXT("ws2_32.dll") );
	if( !l2pnet_hws2_32 )
	{
		l2pnet_hws2_32 = LoadLibrary( TEXT("ws2_32.dll") );
		if( !l2pnet_hws2_32 )
		{
			MessageBox( NULL, TEXT("L2Packets cannot load ws2_32.dll!\nFailing application!"),
				TEXT("L2Packets Error!"), MB_ICONSTOP );
			RaiseException( ERROR_FILE_NOT_FOUND, 0, 0, NULL );
			return 0;
		}
	}
	ws2_func[L2PFUNC_CONNECT]     = (void *)GetProcAddress( l2pnet_hws2_32, "connect" );
	ws2_func[L2PFUNC_SEND]        = (void *)GetProcAddress( l2pnet_hws2_32, "send" );
	ws2_func[L2PFUNC_RECV]        = (void *)GetProcAddress( l2pnet_hws2_32, "recv" );
	ws2_func[L2PFUNC_SHUTDOWN]    = (void *)GetProcAddress( l2pnet_hws2_32, "shutdown" );
	ws2_func[L2PFUNC_CLOSESOCKET] = (void *)GetProcAddress( l2pnet_hws2_32, "closesocket" );
	ws2_func[L2PFUNC_BIND]        = (void *)GetProcAddress( l2pnet_hws2_32, "bind" );
	ws2_func[L2PFUNC_ACCEPT]      = (void *)GetProcAddress( l2pnet_hws2_32, "accept" );
	ws2_func[L2PFUNC_SELECT]      = (void *)GetProcAddress( l2pnet_hws2_32, "select" );
	ws2_func[L2PFUNC_SOCKET]      = (void *)GetProcAddress( l2pnet_hws2_32, "socket" );
	ws2_func[L2PFUNC_LISTEN]      = (void *)GetProcAddress( l2pnet_hws2_32, "listen" );
	for( i=1; i<l2pnet_ft_size; i++ )
	{
		if( ws2_func[i] == NULL )
		{
			_stprintf( mes, _T("ws2_func[%d] == NULL !"), i );
			MessageBox( NULL, mes, TEXT("L2Packets Error!"), MB_ICONSTOP );
		}
	}
	//select_winsock = (select_func)GetProcAddress( l2pnet_hws2_32, "select" );
	WSAStartup_func WSAStartup_ws2 = (WSAStartup_func)GetProcAddress( l2pnet_hws2_32, "WSAStartup" );
	if( !WSAStartup_ws2 )
	{
		MessageBox( NULL, TEXT("L2Packets cannot load ws2_32.dll!WSAStartup( WORD, LPWSADATA )\nFailing application!"),
			TEXT("L2Packets Error!"), MB_ICONSTOP );
		RaiseException( ERROR_FILE_NOT_FOUND, 0, 0, NULL );
	}
	WORD wVersionRequested = MAKEWORD(2,0);
	WSADATA wsa;
	i = WSAStartup_ws2( wVersionRequested, &wsa );
	if( i != 0 )
	{
		MessageBox( NULL, TEXT("ws2_32.dll!WSAStartup() returned error!\nFailing application!"),
			TEXT("L2Packets Error!"), MB_ICONSTOP );
		RaiseException( ERROR_BAD_UNIT, 0, 0, NULL );
	}
	return 1;
}

/** Closes winsock (wrapper around WSACleanup() **/
int L2PNet_Cleanup()
{
	WSACleanup_func WSACleanup_ws2 = (WSACleanup_func)GetProcAddress( l2pnet_hws2_32, "WSACleanup" );
	if( !WSACleanup_ws2 )
	{
		MessageBox( NULL, TEXT("L2Packets cannot load ws2_32.dll!WSACleanup(void)\nFailing application!"),
			TEXT("L2Packets Error!"), MB_ICONSTOP );
		RaiseException( ERROR_FILE_NOT_FOUND, 0, 0, NULL );
		return -1;
	}
	return WSACleanup_ws2();
}

/** Overrides specified function for L2Packets network layer.
 ** New function must have the same prototype as corresponding
 ** L2PNet_* function! */
void L2PNet_setFunction( int funcNo, void *func_addr )
{
	if( (funcNo < 1) || (funcNo >= l2pnet_ft_size) ) return;
	EnterCriticalSection( &l2pnet_cs );
	l2pnet_ft[funcNo] = func_addr; // O_o
	LeaveCriticalSection( &l2pnet_cs );
}

/** Creates TCP socket
 ** returns 0xFFFFFFFF on error, on success - [1..0xFFFFFFFE] :) */
unsigned int L2PNet_TCPsocket_create( bool bUse_NonBlock_IO_Mode )
{
	unsigned int sock = 0xFFFFFFFF;
	if( l2pnet_ft[L2PFUNC_SOCKET] )
	{
		//ret = ( (int (__stdcall*)( unsigned int, void *, int ) )connect_orig)( sock, sockaddr, addrlen );
		sock = ( (unsigned int (*)(bool))(l2pnet_ft[L2PFUNC_SOCKET]) )( bUse_NonBlock_IO_Mode );
	}
	else
	{
		sock = ( (SOCKET (__stdcall*)(int,int,int))(ws2_func[L2PFUNC_SOCKET]))( AF_INET, SOCK_STREAM, IPPROTO_TCP );
		if( (sock != 0xFFFFFFFF) && bUse_NonBlock_IO_Mode )
		{
			void *s_ioctl = (void *)GetProcAddress( l2pnet_hws2_32, "ioctlsocket" );
			if( s_ioctl )
			{
				// If iMode = 0, blocking is enabled; 
				// If iMode != 0, non-blocking mode is enabled.
				unsigned int iMode = 1;
				int res = ((int (__stdcall*)(SOCKET,int,unsigned int *))(s_ioctl))( sock, FIONBIO, &iMode );
				if( res != 0 ) DebugBreak();
			}
			else
			{
				MessageBox( NULL, TEXT("L2Packets cannot find ioctlsocket in ws2_32.dll!"),
					TEXT("L2Packets Error!"), MB_ICONSTOP );
			}
		}
	}
	return sock;
}

/** Waits for socket to be ready for read or write
 ** returns 1 on success, 0 on timeout, -1 on error */
int L2PNet_select( unsigned int sock,            // socket descriptor
				  unsigned int dwSelFlags,       // flags what to wait for: L2PNET_SELECT_READ/WRITE
				  long lWaitMilliSecs,           // wait timeout in milliseconds
				  int *pbReadyForRead,           // will be 1, if ready to read, 0 otherwise (may be NULL)
				  int *pbReadyForWrite )         // will be 1, if ready to write, 0 otherwise (may be NULL)
{
	int ret = -1;
	if( l2pnet_ft[L2PFUNC_SELECT] )
	{
		ret = ( (int (*)(unsigned int, unsigned int, long, int*, int*))(l2pnet_ft[L2PFUNC_SELECT]) )
			( sock, dwSelFlags, lWaitMilliSecs, pbReadyForRead, pbReadyForWrite );
	}
	else
	{
		if( dwSelFlags == 0 ) return -1;
		if( pbReadyForRead ) (*pbReadyForRead) = 0;
		if( pbReadyForWrite ) (*pbReadyForWrite) = 0;
		struct fd_set readFds;
		struct fd_set writeFds;
		struct fd_set *prfds = NULL, *pwfds = NULL;
		FD_ZERO(&readFds);
		FD_ZERO(&writeFds);
		if( dwSelFlags & L2PNET_SELECT_READ )
		{
			FD_SET( sock, &readFds );
			prfds = &readFds;
		}
		if( dwSelFlags & L2PNET_SELECT_WRITE )
		{
			FD_SET( sock, &writeFds );
			pwfds = &writeFds;
		}
		long microsecs = lWaitMilliSecs * 1000; // 1 millisec = 1 000 microsec
		long secs = microsecs / 1000000; // 1 000 000
		microsecs -= secs * 1000000;
		struct timeval tv;
		tv.tv_sec = secs;
		tv.tv_usec = microsecs;
		ret = ( (int (__stdcall*)(int, fd_set *, fd_set *, fd_set *, const struct timeval *))
			(ws2_func[L2PFUNC_SELECT]))( 0, prfds, pwfds, NULL, &tv );
		if( ret == 0 )  return  0; // timeout failed
		if( ret == -1 ) return -1; // select error
		if( L2PNet_FD_ISSET( sock, &readFds )  && pbReadyForRead )  (*pbReadyForRead) = 1; // ready to recv
		if( L2PNet_FD_ISSET( sock, &writeFds ) && pbReadyForWrite ) (*pbReadyForWrite) = 1; // ready to write
	}
	return ret;
}

/** Check state for multiple sockets */
int L2PNet_select_multi( unsigned int *socks_array,    // socket descriptors array
						unsigned int socks_count,      // socket descriptors array item count
						unsigned int dwSelFlags,       // flags what to wait for: L2PNET_SELECT_READ/WRITE
						long lWaitMilliSecs,           // wait timeout in milliseconds
						int *pbReadyForRead,           // array! will be 1, if ready to read, 0 otherwise
						int *pbReadyForWrite )         // array! will be 1, if ready to write, 0 otherwise
{
	if( (!socks_array) || (socks_count==0) || (dwSelFlags == 0) ) return -1;
	if( pbReadyForRead )  memset( pbReadyForRead,  0, sizeof(int)*socks_count );
	if( pbReadyForWrite ) memset( pbReadyForWrite, 0, sizeof(int)*socks_count );
	struct fd_set readFds;
	struct fd_set writeFds;
	struct fd_set *prfds = NULL, *pwfds = NULL;
	FD_ZERO(&readFds);
	FD_ZERO(&writeFds);
	unsigned int i = 0;
	int ret = 0;
	if( dwSelFlags & L2PNET_SELECT_READ )
	{
		for( i=0; i<socks_count; i++ ) FD_SET( socks_array[i], &readFds );
		prfds = &readFds;
	}
	if( dwSelFlags & L2PNET_SELECT_WRITE )
	{
		for( i=0; i<socks_count; i++ ) FD_SET( socks_array[i], &writeFds );
		pwfds = &writeFds;
	}
	long microsecs = lWaitMilliSecs * 1000; // 1 millisec = 1 000 microsec
	long secs = microsecs / 1000000; // 1 second = 1 000 000 microseconds
	microsecs -= secs * 1000000;
	struct timeval tv;
	tv.tv_sec = secs;
	tv.tv_usec = microsecs;
	ret = ( (int (__stdcall*)(int, fd_set *, fd_set *, fd_set *, const struct timeval *))
		(ws2_func[L2PFUNC_SELECT]))( 0, prfds, pwfds, NULL, &tv );
	if( ret == 0 )  return  0; // timeout failed
	if( ret == -1 ) return -1; // select error
	//if( L2PNet_FD_ISSET( sock, &readFds )  && pbReadyForRead )  (*pbReadyForRead) = 1; // ready to recv
	//if( L2PNet_FD_ISSET( sock, &writeFds ) && pbReadyForWrite ) (*pbReadyForWrite) = 1; // ready to write
	for( i=0; i<socks_count; i++ )
	{
		if( L2PNet_FD_ISSET( socks_array[i], &readFds ) && pbReadyForRead ) pbReadyForRead[i] = 1;
		if( L2PNet_FD_ISSET( socks_array[i], &writeFds ) && pbReadyForWrite ) pbReadyForWrite[i] = 1;
	}
	return ret;
}

/** Receives data from socket.
 ** returns number of bytes read, 0 on connection closed, -1 on error */
int L2PNet_recv( unsigned int sock, unsigned char *pucRecvBuffer, unsigned int uBufferSizeBytes )
{
	int ret = -1;
	if( l2pnet_ft[L2PFUNC_RECV] )
	{
		ret = ( (int (*)(unsigned int, unsigned char *, unsigned int))(l2pnet_ft[L2PFUNC_RECV]) )
			( sock, pucRecvBuffer, uBufferSizeBytes );
	}
	else
	{
		ret = ( (int (__stdcall*)(SOCKET, char *, int, int))(ws2_func[L2PFUNC_RECV]))
			( sock, (char *)pucRecvBuffer, (int)uBufferSizeBytes, 0 );
	}
	return ret;
}

/** Writes data to socket.
 ** returns number of bytes sent, -1 on error */
int L2PNet_send( unsigned int sock, const unsigned char *pucSendBuffer, unsigned int uNumberOfBytesToSend )
{
	int ret = -1;
	if( l2pnet_ft[L2PFUNC_SEND] )
	{
		ret = ( (int (*)(unsigned int, const unsigned char *, unsigned int))(l2pnet_ft[L2PFUNC_SEND]) )
			( sock, pucSendBuffer, uNumberOfBytesToSend );
	}
	else
	{
		ret = ( (int (__stdcall*)(SOCKET, const char *, int, int))(ws2_func[L2PFUNC_SEND]))
			( sock, (const char *)pucSendBuffer, (int)uNumberOfBytesToSend, 0 );
	}
	return ret;
}

/** Stops data transfer on socket, preparing to close. */
int L2PNet_shutdown( unsigned int sock )
{
	int ret = -1;
	if( l2pnet_ft[L2PFUNC_SHUTDOWN] )
	{
		ret = ( (int (*)(unsigned))(l2pnet_ft[L2PFUNC_SHUTDOWN]) )( sock );
	}
	else
	{
		ret = ( (int (__stdcall*)(SOCKET,int))(ws2_func[L2PFUNC_SHUTDOWN]))( sock, 0x02 ); // SD_BOTH
	}
	return ret;
}

/** Closes socket and frees its resources from system. */
int L2PNet_closesocket( unsigned int sock )
{
	int ret = -1;
	if( l2pnet_ft[L2PFUNC_CLOSESOCKET] )
	{
		ret = ( (int (*)(unsigned))(l2pnet_ft[L2PFUNC_CLOSESOCKET]) )( sock );
	}
	else
	{
		ret = ( (int (__stdcall*)(SOCKET))(ws2_func[L2PFUNC_CLOSESOCKET]))( sock );
	}
	return ret;
}

/** Connects socket to specified address
 ** returns -1 on error/timeout, 0 on error */
int L2PNet_connect( unsigned int sock, const char *ip, unsigned short port )
{
	if( !ip || (port == 0) ) return -1;
	int ret = -1;
	if( l2pnet_ft[L2PFUNC_CONNECT] )
	{
		ret = ( (int (*)(unsigned int, const char*, unsigned short))(l2pnet_ft[L2PFUNC_CONNECT]) )( sock, ip, port );
	}
	else
	{
		struct sockaddr_in addr;
		memset( &addr, 0, sizeof(addr) );
		addr.sin_family = AF_INET;
		addr.sin_port = L2PNet_htons( port );
		addr.sin_addr.s_addr = L2PNet_inet_addr( ip );
		if( addr.sin_addr.s_addr == INADDR_NONE )
		{
			// try to resolve
			L2PNet_resolveHostname( ip, &addr.sin_addr );
		}
		ret = ( (int (__stdcall*)(SOCKET,const struct sockaddr*,int))(ws2_func[L2PFUNC_CONNECT]))
			( sock, (const struct sockaddr *)&addr, sizeof(addr) );
	}
	return ret;
}

/** Binds socket to specified address
 ** returns 0 on ok, -1 on error */
int L2PNet_bind( unsigned int sock, const char *ip, unsigned short port )
{
	int ret = -1;
	if( l2pnet_ft[L2PFUNC_BIND] )
	{
		ret = ( (int (*)(unsigned int, const char*, unsigned short))(l2pnet_ft[L2PFUNC_BIND]) )( sock, ip, port );
	}
	else
	{
		struct sockaddr_in addr;
		memset( &addr, 0, sizeof(addr) );
		addr.sin_family = AF_INET;
		addr.sin_port = L2PNet_htons( port );
		addr.sin_addr.s_addr = L2PNet_inet_addr( ip );
		if( addr.sin_addr.s_addr == INADDR_NONE )
		{
			// try to resolve
			L2PNet_resolveHostname( ip, &addr.sin_addr );
		}
		ret = ( (int (__stdcall*)(SOCKET,const struct sockaddr *,int))(ws2_func[L2PFUNC_BIND]))
			( sock, (const struct sockaddr *)&addr, sizeof(addr) );
	}
	return ret;
}

/** Starts listening on socket to aceept incoming connections
 ** returns 0 on ok, -1 on error */
int L2PNet_listen( unsigned int sock )
{
	int ret = -1;
	if( l2pnet_ft[L2PFUNC_LISTEN] )
	{
		ret = ( (int (*)(unsigned))(l2pnet_ft[L2PFUNC_LISTEN]) )( sock );
	}
	else
	{
		ret = ( (int (__stdcall*)(SOCKET,int))(ws2_func[L2PFUNC_LISTEN]))( sock, 1 );
	}
	return ret;
}

/** Accepts incoming connection from socket
 ** returns accepted socket on OK, -1 on error */
unsigned int L2PNet_accept( unsigned int sock, char *acceptedIP, unsigned short *acceptedPort )
{
	int ret = -1;
	if( l2pnet_ft[L2PFUNC_ACCEPT] )
	{
		ret = ( (int (*)(unsigned int, char *, unsigned short *))(l2pnet_ft[L2PFUNC_ACCEPT]) )
			( sock, acceptedIP, acceptedPort );
	}
	else
	{
		struct sockaddr_in addr;
		memset( &addr, 0, sizeof(addr) );
		int addrlen = sizeof(addr);
		ret = ( (int (__stdcall*)(SOCKET,struct sockaddr *,int *))(ws2_func[L2PFUNC_ACCEPT]))
			( sock, (struct sockaddr *)&addr, &addrlen );
		if( ret != -1 )
		{
			sprintf( acceptedIP, "%d.%d.%d.%d",
				(int)addr.sin_addr.S_un.S_un_b.s_b1,
				(int)addr.sin_addr.S_un.S_un_b.s_b2,
				(int)addr.sin_addr.S_un.S_un_b.s_b3,
				(int)addr.sin_addr.S_un.S_un_b.s_b4
			);
			(*acceptedPort) = L2PNet_ntohs( addr.sin_port );
		}
	}
	return ret;
}

/** Wrapper around standard select() function **/
/** !!! CANNOT BE OVERRIDED !!! **/
int L2PNet_select_wrapper_DUMB( int nfds,
						  fd_set *readFds,
						  fd_set *writeFds,
						  fd_set *exceptFds,
						  const struct timeval *timeout )
{
	if( !ws2_func[L2PFUNC_SELECT] )
	{
		//RaiseException(
		return -1;
	}
	int ret = ((select_func)(ws2_func[L2PFUNC_SELECT]))( nfds, readFds, writeFds, exceptFds, timeout );
	return ret;
}

/** Wrapper around WSAGetLastError() **/
int L2PNet_WSAGetLastError(void)
{
	WSAGetLastError_func WSAGetLastError_winsock = (WSAGetLastError_func)GetProcAddress
		( l2pnet_hws2_32, "WSAGetLastError" );
	if( !WSAGetLastError_winsock ) return ERROR_CALL_NOT_IMPLEMENTED;
	int ret = WSAGetLastError_winsock();
	return ret;
}

/** Wrapper around WSASetLastError() **/
void L2PNet_WSASetLastError( int iError )
{
	WSASetLastError_func WSASetLastError_winsock = (WSASetLastError_func)GetProcAddress
		( l2pnet_hws2_32, "WSASetLastError" );
	if( !WSASetLastError_winsock ) return;
	WSASetLastError_winsock( iError );
	return;
}

/** Prints Winsock error code as string to specified file **/
// incomplete errors list!!!!
int L2PNet_WSAPrintLastError( FILE *f, int wsaerr )
{
	if( !f ) return -1;
	char s[64] = {'U','N','K','N','O','W','N',0};
	switch( wsaerr )
	{
	case 0:                     strcpy( s, "ERROR_OK" ); break;
	case WSANOTINITIALISED:     strcpy( s, "WSANOTINITIALISED" ); break;
	case WSAEFAULT:             strcpy( s, "WSAEFAULT" ); break;
	case WSAENETDOWN:           strcpy( s, "WSAENETDOWN" ); break;
	case WSAEINVAL:             strcpy( s, "WSAEINVAL" ); break;
	case WSAEINTR:              strcpy( s, "WSAEINTR" ); break;
	case WSAEINPROGRESS:        strcpy( s, "WSAEINPROGRESS" ); break;
	case WSAENOTSOCK:           strcpy( s, "WSAENOTSOCK" ); break;
	case WSAEWOULDBLOCK:        strcpy( s, "WSAEWOULDBLOCK" ); break;
	case WSAECONNRESET:         strcpy( s, "WSAECONNRESET" ); break;
	}
	return fprintf( f, "%d: %s", wsaerr, s );
}



//
// from reactos's ws2_32.dll
//

/* WORD network to host order conversion for little endian machines */
#define WN2H(w) \
	((((w) & 0xFF00) >> 8) | \
	 (((w) & 0x00FF) << 8))

/* WORD host to network byte order conversion for little endian machines */
#define WH2N(w) \
	((((w) & 0xFF00) >> 8) | \
	 (((w) & 0x00FF) << 8))

/* DWORD network to host byte order conversion for little endian machines */
#define DN2H(dw) \
  ((((dw) & 0xFF000000L) >> 24) | \
   (((dw) & 0x00FF0000L) >> 8) | \
	 (((dw) & 0x0000FF00L) << 8) | \
	 (((dw) & 0x000000FFL) << 24))

/* DWORD host to network byte order conversion for little endian machines */
#define DH2N(dw) \
	((((dw) & 0xFF000000L) >> 24) | \
	 (((dw) & 0x00FF0000L) >> 8) | \
	 (((dw) & 0x0000FF00L) << 8) | \
	 (((dw) & 0x000000FFL) << 24))

unsigned long L2PNet_htonl( unsigned long hostlong )
{
    return DH2N(hostlong);
}


unsigned short L2PNet_htons( unsigned short hostshort )
{
    return WH2N(hostshort);
}


unsigned long L2PNet_ntohl( unsigned long netlong )
{
    return DN2H(netlong);
}


unsigned short L2PNet_ntohs( unsigned short netshort )
{
    return WN2H(netshort);
}

/*
 * FUNCTION: Converts a string containing an IPv4 address to an unsigned long
 * ARGUMENTS:
 *     cp = Pointer to string with address to convert
 * RETURNS:
 *     Binary representation of IPv4 address, or INADDR_NONE
 */
unsigned long L2PNet_inet_addr( const char *cp )
{
	unsigned int i;
	char *p;
	unsigned long u = 0;
	p = (char *)cp;
	if( !p ) return INADDR_NONE;
	if( strlen( p ) == 0 ) return INADDR_NONE;
	if( p[0] == 0 ) return INADDR_NONE;
    if( strcmp( p, " " ) == 0 ) return 0;

    for( i = 0; i <= 3; i++ )
    {
        u += ( strtoul( p, &p, 0 ) << (i * 8) );
        if( strlen( p ) == 0 ) return u;
        if( p[0] != '.' ) return INADDR_NONE;
        p++;
    }

    return u;
}


char *L2PNet_inet_ntoa( struct in_addr in )
{
	char b[10];
	char *p = l2pnet_static_inet_ntoa_buffer;
	_itoa( in.S_un.S_addr & 0xFF, b, 10 );
	strcpy( p, b );
	_itoa( (in.S_un.S_addr >> 8) & 0xFF, b, 10 );
	strcat( p, "." );
	strcat( p, b );
	_itoa( (in.S_un.S_addr >> 16) & 0xFF, b, 10 );
	strcat( p, "." );
	strcat( p, b );
	_itoa( (in.S_un.S_addr >> 24) & 0xFF, b, 10 );
	strcat( p, "." );
	strcat( p, b );

    return l2pnet_static_inet_ntoa_buffer;
}

/*typedef struct fd_set {
        u_int fd_count;               // how many are SET
        SOCKET  fd_array[FD_SETSIZE];   // an array of SOCKETs
} fd_set;*/

int L2PNet_FD_ISSET( unsigned int sock, struct fd_set *set )
{
	unsigned int i;
	for ( i = 0; i < set->fd_count; i++ )
	if ( set->fd_array[i] == sock ) return 1;
	return 0;
}

bool L2PNet_resolveHostname( const char *hostname, struct in_addr *pinAddr )
{
	in_addr addr;
	addr.s_addr = L2PNet_inet_addr( hostname );
	if( addr.s_addr == INADDR_NONE )
	{
		getaddrinfo_func getaddrinfo_ws2 = (getaddrinfo_func)GetProcAddress( l2pnet_hws2_32, "getaddrinfo" );
		if( getaddrinfo_ws2 )
		{
			addrinfo addr_hints;
			memset( &addr_hints, 0, sizeof(addr_hints) );
			addr_hints.ai_family = AF_INET;
			addr_hints.ai_socktype = SOCK_STREAM;
			addrinfo *retAddr = NULL;
			int ret = getaddrinfo_ws2( hostname, NULL, &addr_hints, &retAddr );
			if( ret == 0 ) // OK
			{
				if( retAddr )
				{
					pinAddr->s_addr = ((sockaddr_in *)retAddr->ai_addr)->sin_addr.s_addr;
					freeaddrinfo_func freeaddr_ws2 = (freeaddrinfo_func)GetProcAddress( l2pnet_hws2_32, "freeaddrinfo" );
					if( freeaddr_ws2 ) freeaddr_ws2( retAddr );
					return true;
				}
				else
				{
					pinAddr->s_addr = INADDR_NONE;
					return false;
				}
			}
			else
			{
				pinAddr->s_addr = INADDR_NONE;
				return false;
			}
		}
		else
		{
			pinAddr->s_addr = INADDR_NONE;
			return false;
		}
	}
	else // hostname is already IP address
	{
		pinAddr->s_addr = addr.s_addr;
	}
	return true;
}
