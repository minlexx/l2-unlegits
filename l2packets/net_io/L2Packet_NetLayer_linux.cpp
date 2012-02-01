#include "stdafx.h"
#include "L2Packet_NetLayer.h"

// keeps result of inet_ntoa
char l2pnet_static_inet_ntoa_buffer[32];

int L2PNet_InitDefault()
{
	l2pnet_static_inet_ntoa_buffer[0] = 0;
	return 1;
}

/** Closes winsock (wrapper around WSACleanup() **/
int L2PNet_Cleanup()
{
	// does nothing
	return 1;
}

/** Creates TCP socket
 ** returns 0xFFFFFFFF on error, on success - [1..0xFFFFFFFE] :) */
unsigned int L2PNet_TCPsocket_create( bool bUse_NonBlock_IO_Mode )
{
	int sock = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( sock == -1 ) return 0xFFFFFFFF;
	if( bUse_NonBlock_IO_Mode )
	{
		int fl = fcntl( sock, F_GETFL, 0 );
		fl |= O_NONBLOCK;
		fcntl( sock, F_SETFL, fl );
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
	// L2PNet_select_dumb
	if( dwSelFlags == 0 ) return -1;
	if( pbReadyForRead ) (*pbReadyForRead) = 0;
	if( pbReadyForWrite ) (*pbReadyForWrite) = 0;
	fd_set readFds;
	fd_set writeFds;
	fd_set *prfds = NULL, *pwfds = NULL;
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
	int ret = select( sock+1, prfds, pwfds, NULL, &tv );
	if( ret == 0 )  return  0; // timeout failed
	if( ret == -1 ) return -1; // select error
	if( L2PNet_FD_ISSET( sock, &readFds )  && pbReadyForRead )  (*pbReadyForRead) = 1; // ready to recv
	if( L2PNet_FD_ISSET( sock, &writeFds ) && pbReadyForWrite ) (*pbReadyForWrite) = 1; // ready to write
	//
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
	fd_set readFds;
	fd_set writeFds;
	fd_set *prfds = NULL, *pwfds = NULL;
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
	// calc nfds - get max socket fd
	int nfds = socks_array[0];
	for( i=0; i<socks_count; i++ ) if( nfds > (int)(socks_array[i]) ) nfds = (int)(socks_array[i]);
	// init timeval
	long microsecs = lWaitMilliSecs * 1000; // 1 millisec = 1 000 microsec
	long secs = microsecs / 1000000; // 1 second = 1 000 000 microseconds
	microsecs -= secs * 1000000;
	struct timeval tv;
	tv.tv_sec = secs;
	tv.tv_usec = microsecs;
	ret = select( nfds+1, prfds, pwfds, NULL, &tv );
	if( ret == 0 )  return  0; // timeout failed
	if( ret == -1 ) return -1; // select error
	// results
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
	ret = recv( sock, pucRecvBuffer, (int)uBufferSizeBytes, 0 );
	return ret;
}

/** Writes data to socket.
 ** returns number of bytes sent, -1 on error */
int L2PNet_send( unsigned int sock, const unsigned char *pucSendBuffer, unsigned int uNumberOfBytesToSend )
{
	int ret = -1;
	ret = send( sock, pucSendBuffer, (int)uNumberOfBytesToSend, 0 );
	return ret;
}

/** Stops data transfer on socket, preparing to close. */
int L2PNet_shutdown( unsigned int sock )
{
	int ret = -1;
	ret = shutdown( sock, 0 );
	return ret;
}

/** Closes socket and frees its resources from system. */
int L2PNet_closesocket( unsigned int sock )
{
	int ret = -1;
	ret = close( sock );
	return ret;
}

/** Connects socket to specified address
 ** returns -1 on error/timeout, 0 on error */
int L2PNet_connect( unsigned int sock, const char *ip, unsigned short port )
{
	if( !ip || (port == 0) ) return -1;
	int ret = -1;
	sockaddr_in addr;
	memset( &addr, 0, sizeof(addr) );
	addr.sin_family = AF_INET;
	addr.sin_port = L2PNet_htons( port );
	addr.sin_addr.s_addr = L2PNet_inet_addr( ip );
	if( addr.sin_addr.s_addr == INADDR_NONE )
	{
		// try to resolve
		L2PNet_resolveHostname( ip, &addr.sin_addr );
	}
	ret = connect( sock, (const sockaddr *)&addr, sizeof(addr) );
	return ret;
}

/** Binds socket to specified address
 ** returns 0 on ok, -1 on error */
int L2PNet_bind( unsigned int sock, const char *ip, unsigned short port )
{
	sockaddr_in addr;
	memset( &addr, 0, sizeof(addr) );
	addr.sin_family = AF_INET;
	addr.sin_port = L2PNet_htons( port );
	addr.sin_addr.s_addr = L2PNet_inet_addr( ip );
	if( addr.sin_addr.s_addr == INADDR_NONE )
	{
		// try to resolve
		L2PNet_resolveHostname( ip, &addr.sin_addr );
	}
	int ret = bind( sock, (const sockaddr *)&addr, sizeof(addr) );
	return ret;
}

/** Starts listening on socket to aceept incoming connections
 ** returns 0 on ok, -1 on error */
int L2PNet_listen( unsigned int sock )
{
	int ret = -1;
	ret = listen( sock, 1 );
	return ret;
}

/** Accepts incoming connection from socket
 ** returns accepted socket on OK, -1 on error */
unsigned int L2PNet_accept( unsigned int sock, char *acceptedIP, unsigned short *acceptedPort )
{
	sockaddr_in addr;
	memset( &addr, 0, sizeof(addr) );
	socklen_t addrlen = sizeof(addr);
	int ret = accept( sock, (sockaddr *)&addr, &addrlen );
	if( ret != -1 )
	{
		sprintf( acceptedIP, "%d.%d.%d.%d",
			(int)( (addr.sin_addr.s_addr & 0x000000FF) ),
			(int)( (addr.sin_addr.s_addr & 0x0000FF00) >> 8),
			(int)( (addr.sin_addr.s_addr & 0x00FF0000) >> 16),
			(int)( (addr.sin_addr.s_addr & 0xFF000000) >> 24)
		);
		(*acceptedPort) = L2PNet_ntohs( addr.sin_port );
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
	timeval *tv_noconst = const_cast<timeval *>(timeout);
	int ret = select( nfds, readFds, writeFds, exceptFds, tv_noconst );
	return ret;
}

/** Wrapper around WSAGetLastError() **/
int L2PNet_WSAGetLastError(void)
{
	int ret = errno;
	return ret;
}

/** Wrapper around WSASetLastError() **/
void L2PNet_WSASetLastError( int iError )
{
	iError = 0;
	// does nothing
	return;
}

/** Prints Winsock error code as string to specified file **/
// incomplete errors list!!!!
int L2PNet_WSAPrintLastError( FILE *f, int wsaerr )
{
	if( !f ) return -1;
	return fprintf( f, "%d: %s", wsaerr, strerror( wsaerr ) );
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
	//itoa( in.s_addr & 0xFF, b, 10 );
	sprintf( b, "%d", (int)(in.s_addr & 0xFF) );
	strcpy( p, b );
	//itoa( (in.s_addr >> 8) & 0xFF, b, 10 );
	sprintf( b, "%d", (int)((in.s_addr >> 8) & 0xFF) );
	strcat( p, "." );
	strcat( p, b );
	//itoa( (in.s_addr >> 16) & 0xFF, b, 10 );
	sprintf( b, "%d", (int)((in.s_addr >> 16) & 0xFF) );
	strcat( p, "." );
	strcat( p, b );
	//itoa( (in.s_addr >> 24) & 0xFF, b, 10 );
	sprintf( b, "%d", (int)((in.s_addr >> 24) & 0xFF) );
	strcat( p, "." );
	strcat( p, b );

    return l2pnet_static_inet_ntoa_buffer;
}

// :(((
int L2PNet_FD_ISSET( unsigned int sock, fd_set *set )
{
	int ret = FD_ISSET( sock, set );
	return ret;
}


bool L2PNet_resolveHostname( const char *hostname, struct in_addr *pinAddr )
{
	in_addr addr;
	addr.s_addr = L2PNet_inet_addr( hostname );
	if( addr.s_addr == INADDR_NONE )
	{
		addrinfo addr_hints;
		memset( &addr_hints, 0, sizeof(addr_hints) );
		addr_hints.ai_family = AF_INET;
		addr_hints.ai_socktype = SOCK_STREAM;
		addrinfo *retAddr = NULL;
		int ret = getaddrinfo( hostname, NULL, &addr_hints, &retAddr );
		if( ret == 0 ) // OK
		{
			if( retAddr )
			{
				pinAddr->s_addr = ((sockaddr_in *)retAddr->ai_addr)->sin_addr.s_addr;
				freeaddrinfo( retAddr );
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
	else // hostname is already IP address
	{
		pinAddr->s_addr = addr.s_addr;
	}
	return true;
}
