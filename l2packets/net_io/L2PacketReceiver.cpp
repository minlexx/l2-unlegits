#include "stdafx.h"
#include "L2Packet_NetLayer.h"
#include "L2PacketReceiver.h"

//unsigned char l2p_packetReceive_dummyBuffer[2] = {0,0};

#ifdef L2PNET_ENABLE_OLD_RECVSEND

unsigned char *L2PacketReceive(
		unsigned int sock,
		long tv_sec, long tv_usec,
		unsigned int *rcvdLen )
{
	if( !rcvdLen ) return NULL;
	(*rcvdLen) = 0;
	SOCKET s = (SOCKET)sock;
	if( s == INVALID_SOCKET ) return NULL;
	unsigned int plen = 0;
	int r = 0, rr = 0;
	unsigned char *packbuf = NULL;
	unsigned char *bufptr = NULL;
	//unsigned int buffree = 0;
	
	// first try to receive packet len
	r = sock_tcp_wait_ready_to_recv( s, tv_sec, tv_usec );
	if( r <= 0 ) return NULL;
	r = recv( s, (char *)(&plen), 1, 0 );
	if( r == 0 ) return l2p_packetReceive_dummyBuffer;
	if( r != 1 ) return NULL;
	(*rcvdLen) = 1;
	
	r = sock_tcp_wait_ready_to_recv( s, tv_sec, tv_usec );
	if( r <= 0 ) return NULL;
	r = recv( s, (char *)(&plen) + 1, 1, 0 );
	if( r == 0 ) return l2p_packetReceive_dummyBuffer;
	if( r != 1 ) return NULL;
	(*rcvdLen) = 2;
	
#ifdef L2P_NETWORK_DEBUGOUT
	printf( "L2PacketReceive(): Received plen byte 2; plen is: %04X (%d)\n",
		plen, plen );
#endif // L2P_NETWORK_DEBUGOUT
	
	if( plen == 0 ) return NULL;
	
	packbuf = (unsigned char *)malloc( plen + 2 );
	if( !packbuf ) return NULL;
	bufptr = packbuf + 2;
	memcpy( packbuf, &plen, 2 );
	
	unsigned int nBytesRcvd = 2; // packet len already received
	unsigned int nTimeouts = 0;
	unsigned int maxTimeouts = 5;
	while( nBytesRcvd < plen )
	{
		r = sock_tcp_wait_ready_to_recv( s, tv_sec, tv_usec );
		if( r > 0 ) // select() OK
		{
			rr = recv( s, (char *)bufptr, (plen-nBytesRcvd), 0 );
			if( rr > 0 ) // recv() OK, received rr bytes
			{
				bufptr += rr;
				nBytesRcvd += rr;
				nTimeouts = 0;
			}
			else if( rr == 0 ) // recv() returns 0 when connection closed by remote host?
			{
#ifdef L2P_NETWORK_DEBUGOUT
				printf( "L2PacketReceive(): recv() returned 0; received %u bytes\n",
					nBytesRcvd );
#endif // L2P_NETWORK_DEBUGOUT
				nTimeouts++;
				if( nTimeouts > maxTimeouts ) break;
			}
			else if( rr == -1 ) // recv() error?
			{
#ifdef L2P_NETWORK_DEBUGOUT
				printf( "L2PacketReceive(): recv returned -1; total received %u bytes\n",
					nBytesRcvd );
#endif // L2P_NETWORK_DEBUGOUT
				nTimeouts++;
				if( nTimeouts > maxTimeouts ) break;
			}
		}
		else if ( r == 0 ) // select() timeout
		{
			nTimeouts++;
			if( nTimeouts > maxTimeouts )
			{
#ifdef L2P_NETWORK_DEBUGOUT
				printf( "L2PacketReceive(): select too many timeouts, > maximum of %d\n",
					maxTimeouts );
#endif // L2P_NETWORK_DEBUGOUT
			}
		}
		else if( r == -1 )
		{
#ifdef L2P_NETWORK_DEBUGOUT
			printf( "L2PacketReceive(): select ERROR!\n" );
#endif // L2P_NETWORK_DEBUGOUT
			nTimeouts++;
			if( nTimeouts > maxTimeouts )
			{
#ifdef L2P_NETWORK_DEBUGOUT
				printf( "L2PacketReceive(): select too many timeouts, > maximum of %d\n",
					maxTimeouts );
#endif // L2P_NETWORK_DEBUGOUT
			}
		}
	} // while
	
	if( nBytesRcvd == plen )
	{
#ifdef L2P_NETWORK_DEBUGOUT
		printf( "L2PacketReceive(): Received full packet len %u\n", plen );
#endif // L2P_NETWORK_DEBUGOUT
		(*rcvdLen) = plen;
		return packbuf;
	}
	else if( nBytesRcvd < plen )
	{
#ifdef L2P_NETWORK_DEBUGOUT
		printf( "L2PacketReceive(): ERROR: packet not received full (%u < %u)\n",
			nBytesRcvd, plen );
#endif // L2P_NETWORK_DEBUGOUT
		free( packbuf );
		return NULL;
	}
	
#ifdef L2P_NETWORK_DEBUGOUT
	printf( "L2PacketReceive(): ERROR: Unknown error!\n" );
#endif // L2P_NETWORK_DEBUGOUT
	
	free( packbuf );
	return NULL;
}

#endif /* L2PNET_ENABLE_OLD_RECVSEND */


/**
 L2PacketReceive_malloc
 * Reads L2 packet from socket sock
 * Reads 1st 2 bytes, treates them as packet len, and then reads len bytes
 * places all received data (including 1st 2 bytes) in allocated by malloc() buffer.
 * Sets (*rcvdLen) to number of bytes received

 * On read timeout (lWaitMiliSecs) returns NULL
 * On error returns NULL
 * returns pointer to buffer on success. caller should free returned buffer by free()
*/
unsigned char *L2PacketReceive_malloc( SOCKET sock, long lWaitMilliSecs, unsigned int *rcvdLen )
{
	if( !rcvdLen ) return NULL;
	(*rcvdLen) = 0;
	if( sock == 0xFFFFFFFF ) return NULL;
	unsigned int plen = 0;
	int r = 0, rr = 0;
	unsigned char *packbuf = NULL;
	unsigned char *bufptr = NULL;

	int rdyR = 0, rdyS = 0;
	
	// first try to receive packet len
	r = L2PNet_select( sock, L2PNET_SELECT_READ, lWaitMilliSecs, &rdyR, &rdyS );
	if( r <= 0 ) return NULL;
	r = L2PNet_recv( sock, (unsigned char *)(&plen), 1 );
	if( r != 1 ) return NULL;
	(*rcvdLen) = 1;
	
	r = L2PNet_select( sock, L2PNET_SELECT_READ, lWaitMilliSecs, &rdyR, &rdyS );
	if( r <= 0 ) return NULL;
	r = L2PNet_recv( sock, (unsigned char *)(&plen) + 1, 1 );
	if( r != 1 ) return NULL;
	(*rcvdLen) = 2;
	
	if( plen == 0 ) return NULL;
	
	packbuf = (unsigned char *)malloc( plen + 2 ); // possible speed loss <--------
	if( !packbuf ) return NULL;
	packbuf[0] = ((unsigned char *)&plen)[0];
	packbuf[1] = ((unsigned char *)&plen)[1];
	bufptr = packbuf + 2;
	
	unsigned int nBytesRcvd = 2; // packet len already received
	unsigned int nTimeouts = 0;
	unsigned int maxTimeouts = 5;
	while( nBytesRcvd < plen )
	{
		(*rcvdLen) = nBytesRcvd;
		r = L2PNet_select( sock, L2PNET_SELECT_READ, lWaitMilliSecs, &rdyR, &rdyS );
		if( r > 0 ) // select() OK
		{
			rr = L2PNet_recv( sock, (unsigned char *)bufptr, (plen - nBytesRcvd) );
			if( rr > 0 ) // recv() OK, received rr bytes
			{
				bufptr += rr;
				nBytesRcvd += rr;
				nTimeouts = 0;
			}
			else if( rr <= 0 ) // recv() returns 0 when connection closed by remote host? or returns -1 on error
			{
				nTimeouts++;
				if( nTimeouts > maxTimeouts ) break;
			}
		}
		else if ( r <= 0 ) // select() timeout or error
		{
			nTimeouts++;
			if( nTimeouts > maxTimeouts ) break;
		}
	} // while
	
	if( nBytesRcvd == plen ) // successful return
	{
		(*rcvdLen) = plen;
		return packbuf;
	}
	else if( nBytesRcvd < plen ) // received less than should, error
	{
		free( packbuf );
		return NULL;
	}
	
	// other error
	free( packbuf );
	return NULL;
}


/**
 L2PacketReceive_buffer
 * Reads L2 packet from socket sock
 * Reads 1st 2 bytes, treates them as packet len, and then reads len bytes
 * places all received data (including 1st 2 bytes) in buffer recvBuffer.
 * Sets (*rcvdLen) to number of bytes received

 * On read timeout (lWaitMiliSecs) returns 0
 * On error returns -1
 * returns 1 on success

 * recvBuffer must point to big enough memory block (10 Kb min)
*/
int L2PacketReceive_buffer( SOCKET sock, long lWaitMilliSecs, unsigned int *rcvdLen, unsigned char *recvBuffer )
{
	if( !rcvdLen || !recvBuffer ) return 0; // assert
	(*rcvdLen) = 0;                       // zero bytes received
	recvBuffer[0] = recvBuffer[1] = 0;    // zero 1st 2 bytes in packet
	if( sock == 0xFFFFFFFF ) return 0;    // assert
	unsigned int plen = 0;                // receiving packet len
	int r = 0, rr = 0;                    // net functions results return codes
	unsigned char *bufptr = NULL;         // current receive buffer pointer
	int rdyR = 0, rdyS = 0;               // ready to recv, ready to send
	
	// first try to receive packet len. receive 1st byte
	r = L2PNet_select( sock, L2PNET_SELECT_READ, lWaitMilliSecs, &rdyR, &rdyS );
	if( r <= 0 ) return r;
	r = L2PNet_recv( sock, (unsigned char *)(&plen), 1 );
	if( r <= 0 ) return r;
	(*rcvdLen) = 1;
	// receive 2nd byte
	r = L2PNet_select( sock, L2PNET_SELECT_READ, lWaitMilliSecs, &rdyR, &rdyS );
	if( r <= 0 ) return r;
	r = L2PNet_recv( sock, (unsigned char *)(&plen) + 1, 1 );
	if( r <= 0 ) return r;
	(*rcvdLen) = 2;
	
	if( plen == 0 ) return 0; // protocol error - packet length 0 cannot be! minimum is 2
	
	// set packet len in packet
	recvBuffer[0] = ((unsigned char *)&plen)[0];
	recvBuffer[1] = ((unsigned char *)&plen)[1];
	bufptr = recvBuffer + 2; // update current bufer pos to pass 1st 2 bytes
	
	unsigned int nBytesRcvd = 2; // packet len already received
	unsigned int nTimeouts = 0;  // number of timeouts on receive process
	unsigned int maxTimeouts = 5; // max.number of timeouts

	// receive loop
	while( nBytesRcvd < plen )
	{
		(*rcvdLen) = nBytesRcvd;
		r = L2PNet_select( sock, L2PNET_SELECT_READ, lWaitMilliSecs, &rdyR, &rdyS );
		if( r > 0 ) // select() OK
		{
			rr = L2PNet_recv( sock, (unsigned char *)bufptr, (plen - nBytesRcvd) );
			if( rr > 0 ) // recv() OK, received rr bytes
			{
				bufptr += rr;
				nBytesRcvd += rr;
				nTimeouts = 0;
			}
			else if( rr <= 0 ) // recv() returns 0 when connection closed by remote host? or returns -1 on error
			{
				nTimeouts++;
				if( nTimeouts > maxTimeouts ) break;
			}
		}
		else if ( r <= 0 ) // select() timeout or error
		{
			nTimeouts++;
			if( nTimeouts > maxTimeouts ) break;
		}
	} // while
	
	if( nBytesRcvd == plen ) // successful return
	{
		(*rcvdLen) = plen;
		return 1;
	}
	else if( nBytesRcvd < plen ) // received less than should, error
	{
		return -1;
	}
	
	// other error
	return -1;
}
