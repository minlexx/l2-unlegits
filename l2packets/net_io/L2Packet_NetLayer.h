#ifndef H_L2P_NET_LAYER
#define H_L2P_NET_LAYER

// define SOCKET type for Linux
#ifdef L2PACKETS_LINUX
typedef unsigned int SOCKET;
#endif

/** Initializes L2Packets network layer.\n
 * Win32:\n
 * By default network functions L2PNet_* will be used,
 * which use corresponding Winsock2 functions.
 * Linux:\n
 * does nothing. Under Linux, most of functions here do nothing, they are just wrapper around glibc
 * \return 1 on success, 0 on error
 */
int L2PNet_InitDefault();

/** Closes winsock (Win32: wrapper around WSACleanup()) (Linux: does nothing) **/
int L2PNet_Cleanup();

#ifdef L2PACKETS_WINDOWS
#define L2PFUNC_CONNECT       1
#define L2PFUNC_SEND          2
#define L2PFUNC_RECV          3
#define L2PFUNC_SHUTDOWN      4
#define L2PFUNC_CLOSESOCKET   5
#define L2PFUNC_BIND          6
#define L2PFUNC_ACCEPT        7
#define L2PFUNC_SELECT        8
#define L2PFUNC_SOCKET        9
#define L2PFUNC_LISTEN        10
/** Overrides specified function for L2Packets network layer.\n
 * New function must have the same prototype as corresponding
 * L2PNet_* function!
 * only under Windows!
 * \param funcNo - function number to replace
 * \param func_addr - pointer to new function
 */
void L2PNet_setFunction( int funcNo, void *func_addr );
#endif

/** Creates TCP socket
 * \param bUse_NonBlock_IO_Mode - if true, socket will be non-blocking
 * \return 0xFFFFFFFF on error, on success - socket [1..0xFFFFFFFE] :) */
unsigned int L2PNet_TCPsocket_create( bool bUse_NonBlock_IO_Mode );

/** flag for L2PNet_select() to test for read ready */
#define L2PNET_SELECT_READ  0x000000001
/** flag for L2PNet_select() to test for write ready */
#define L2PNET_SELECT_WRITE 0x000000002

/** Waits for socket to be ready for read or write
 * \param sock - socket
 * \param dwSelFlags - select flags (L2PNET_SELECT_READ|L2PNET_SELECT_WRITE)
 * \param lWaitMilliSecs - wait timeout in milliseconds
 * \param pbReadyForRead - (*pbReadyForRead) will be set to 1, if sock is ready to read. may be NULL
 * \param pbReadyForWrite - (*pbReadyForWrite) will be set to 1, if sock is ready to write. may be NULL
 * \returns 1 on success, 0 on timeout, -1 on error */
int L2PNet_select( unsigned int sock,            // socket descriptor
				  unsigned int dwSelFlags,       // flags what to wait for: L2PNET_SELECT_READ/WRITE
				  long lWaitMilliSecs,           // wait timeout in milliseconds
				  int *pbReadyForRead,           // will be 1, if ready to read, 0 otherwise (may be NULL)
				  int *pbReadyForWrite );        // will be 1, if ready to write, 0 otherwise (mya be NULL)

/** Check state for multiple sockets\n
* \warning NOT DONE, DO NOT USE
*/
int L2PNet_select_multi( unsigned int *socks_array,    // socket descriptors array
						unsigned int socks_count,      // socket descriptors array item count
						unsigned int dwSelFlags,       // flags what to wait for: L2PNET_SELECT_READ/WRITE
						long lWaitMilliSecs,           // wait timeout in milliseconds
						int *pbReadyForRead,           // array! will be 1, if ready to read, 0 otherwise
						int *pbReadyForWrite );        // array! will be 1, if ready to write, 0 otherwise

/** Receives data from socket.
 * \param sock - socket
 * \param pucRecvBuffer - pointer to receive buffer
 * \param uBufferSizeBytes - bufer size in bytes
 * \return number of bytes read, 0 on connection closed, -1 on error */
int L2PNet_recv( unsigned int sock, unsigned char *pucRecvBuffer, unsigned int uBufferSizeBytes );

/** Writes data to socket.
 * \param sock - socket
 * \param pucSendBuffer - pointer to buffer to send
 * \param uNumberOfBytesToSend - number of bytes to send from buffer
 * \return number of bytes sent, -1 on error */
int L2PNet_send( unsigned int sock, const unsigned char *pucSendBuffer, unsigned int uNumberOfBytesToSend );

/** Stops data transfer on socket, preparing to close.
* \param sock - socket
* \return dont care
*/
int L2PNet_shutdown( unsigned int sock );

/** Closes socket and frees its resources from system.
* \param sock - socket
* \return dont care
*/
int L2PNet_closesocket( unsigned int sock );

/** Connects socket to specified address
* \param sock - socket
* \param ip - string ip (ex. "127.0.0.1")
* \param port - port to connect to
* \return -1 on error/timeout, 0 on error */
int L2PNet_connect( unsigned int sock, const char *ip, unsigned short port );

/** Binds socket to specified address
 * \param sock - socket
 * \param ip - IP to bind socket to
 * \param port - port to bind socket to
 * \returns 0 on ok, -1 on error */
int L2PNet_bind( unsigned int sock, const char *ip, unsigned short port );

/** Starts listening on socket to aceept incoming connections
 * \param sock - socket
 * \returns 0 on ok, -1 on error */
int L2PNet_listen( unsigned int sock );

/** Accepts incoming connection from socket
 * \param sock - socket
 * \param acceptedIP - will contain IP of accepted client
 * \param acceptedPort - (*acceptedPort) will be set to port of accepted client
 * \return accepted socket on OK, -1 on error */
unsigned int L2PNet_accept( unsigned int sock, char *acceptedIP, unsigned short *acceptedPort );

/** Wrapper around standard WinSock select() function.
* \param nfds - ignored on Win32
* \param readFds - fd_set of sockets to test to readability
* \param writeFds - fd_set of sockets to test to writeability
* \param exceptFds - fd_set of sockets to test for errors
* \param timeout - timeval struct, wait timeout
* \warning !!! CANNOT BE OVERRIDED !!! by L2PNet_setFunction() (only for Win32)
*/
int L2PNet_select_wrapper_DUMB( int nfds,
						  fd_set *readFds,
						  fd_set *writeFds,
						  fd_set *exceptFds,
						  const struct timeval *timeout );

/** Win32: Wrapper around WSAGetLastError()\n
* Linux: returns same as errno
* \return last error code
*/
int L2PNet_WSAGetLastError(void);

/** Win32: Wrapper around WSASetLastError()
* Linux: does nothing
*/
void L2PNet_WSASetLastError( int iError );

/** Prints Winsock error code as string to specified file.\n
* Under linux uses strerror()
* \warning Win32: incomplete errors list!!!!
*/
int L2PNet_WSAPrintLastError( FILE *f, int wsaerr );



// socket functions duplications

/** host-to-network-long */
unsigned long L2PNet_htonl( unsigned long hostlong );
/** host-to-network-short */
unsigned short L2PNet_htons( unsigned short hostshort );
/** net-to-host-long */
unsigned long L2PNet_ntohl( unsigned long netlong );
/** net-to-host-short */
unsigned short L2PNet_ntohs( unsigned short netshort );


/** Converts a string containing an IPv4 address to an unsigned long
* \param cp - Pointer to string with address to convert
* \return Binary representation of IPv4 address, or INADDR_NONE
*/
unsigned long L2PNet_inet_addr( const char *cp );

/* Replacer for inet_ntoa(). Converts IP address form binary form to string.
* \param in struct in_addr ip address to convert to string
* \warning This function is not thread-safe now -_-
*/
char *L2PNet_inet_ntoa( struct in_addr in );

/** Cross-platform FD_ISSET() macro replacement */
int L2PNet_FD_ISSET( unsigned int sock, fd_set *set );

/** Resolves hostname, places result in in_addr struct.\n
* If string in hostname is ip, works like inet_addr() :)
* \param hostname input string with hostname
* \param pinAddr output struct. pinAddr->s_addr will be resulting IP address
*/
bool L2PNet_resolveHostname( const char *hostname, struct in_addr *pinAddr );

#endif /* H_L2P_NET_LAYER */
