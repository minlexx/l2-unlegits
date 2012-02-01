#include "stdafx.h"
#include "L2Packet_NetLayer.h"
#include "../base/L2BasePacket.h"
#include "L2PacketSender.h"

// return values:
// > 0 on OK
//   0 on timeout
//  -1 on error

// 1st 2 bytes of rawPacket must be packet size (as in all L2 protocol)
/*int L2PacketSend_1byte( const unsigned char *rawPacket,
		unsigned int sock,
		long tv_sec, long tv_usec,
		unsigned int *sentLen )
{
	if( !sentLen ) return -1;
	(*sentLen) = 0;
	if( !rawPacket ) return -1;
	SOCKET s = (SOCKET)sock;
	if( s == INVALID_SOCKET ) return -1;
	
	int r = 0;
	unsigned int sentb = 0;
	unsigned int plen = 0;
	unsigned char *plenaddr = (unsigned char *)&plen;
	plenaddr[0] = rawPacket[0];
	plenaddr[1] = rawPacket[1];
	
#ifdef L2P_NETWORK_DEBUGOUT
	printf( "L2PacketSend(): Sending %u len packet (0x%02X)\n", plen, plen );
#endif // L2P_NETWORK_DEBUGOUT
	
	int nTimeouts = 0;
	int maxTimeouts = 5;
	while( sentb < plen )
	{
		(*sentLen) = sentb;
		r = sock_tcp_wait_ready_to_send( s, tv_sec, tv_usec );
		if( r < 0 ) return r; // select returned error
		if( r == 0 ) // timeout
		{
			nTimeouts++;
			if( nTimeouts > maxTimeouts )
			{
#ifdef L2P_NETWORK_DEBUGOUT
				printf( "L2PacketSend(): select timeouts too many times (> %d)\n", maxTimeouts );
#endif // L2P_NETWORK_DEBUGOUT
				return 0;
			}
			continue;
		}
		// if we're here, select OK
		nTimeouts = 0; // zero timeouts count
		r = send( s, (const char *)(rawPacket + sentb), 1, 0 );
		if( r > 0 ) sentb += r;
		else
		{
#ifdef L2P_NETWORK_DEBUGOUT
			printf( "L2PacketSend(): send() returned %d!\n", r );
			if( r < 0 ) return r;
#endif // L2P_NETWORK_DEBUGOUT			
		}
		(*sentLen) = sentb;
	}
	
#ifdef L2P_NETWORK_DEBUGOUT
	printf( "L2PacketSend(): All %u (0x%02X) bytes sent\n", plen, plen );
#endif // L2P_NETWORK_DEBUGOUT
	
	return sentb;
}*/





#ifdef L2PNET_ENABLE_OLD_RECVSEND

// 1st 2 bytes of rawPacket must be packet size (as in all L2 protocol)
int L2PacketSend( const unsigned char *rawPacket,
		unsigned int sock,
		long tv_sec, long tv_usec,
		unsigned int *sentLen )
{
	if( !sentLen ) return -1;
	(*sentLen) = 0;
	if( !rawPacket ) return -1;
	SOCKET s = (SOCKET)sock;
	if( s == INVALID_SOCKET ) return -1;
	
	int r = 0;
	unsigned int sentb = 0; // sent bytes
	unsigned int plen = 0;
	unsigned char *plenaddr = (unsigned char *)&plen;
	plenaddr[0] = rawPacket[0];
	plenaddr[1] = rawPacket[1];
	
#ifdef L2P_NETWORK_DEBUGOUT
	printf( "L2PacketSend(): Sending %u len packet (0x%02X)\n", plen, plen );
#endif // L2P_NETWORK_DEBUGOUT
	
	int nTimeouts = 0;
	int maxTimeouts = 5;
	while( sentb < plen )
	{
		(*sentLen) = sentb;
		r = sock_tcp_wait_ready_to_send( s, tv_sec, tv_usec );
		if( r < 0 ) return r; // select returned error
		if( r == 0 ) // timeout
		{
			nTimeouts++;
			if( nTimeouts > maxTimeouts )
			{
#ifdef L2P_NETWORK_DEBUGOUT
				printf( "L2PacketSend(): select timeouts too many times (> %d)\n", maxTimeouts );
#endif // L2P_NETWORK_DEBUGOUT
				return 0;
			}
			continue; // continue loop on select() timeout
		}
		// if we're here, select OK
		nTimeouts = 0; // zero timeouts count
		// we sent [sentb] bytes, left to send: [plen-sentb]
		r = send( s, (const char *)(rawPacket + sentb), (plen-sentb), 0 );
		if( r > 0 ) sentb += r;
		else
		{
#ifdef L2P_NETWORK_DEBUGOUT
			printf( "L2PacketSend(): send() returned %d!\n", r );
			if( r < 0 ) return r;
#endif // L2P_NETWORK_DEBUGOUT			
		}
		(*sentLen) = sentb;
	}
	
#ifdef L2P_NETWORK_DEBUGOUT
	printf( "L2PacketSend(): All %u (0x%02X) bytes sent\n", plen, plen );
#endif // L2P_NETWORK_DEBUGOUT
	
	return sentb;
}

#endif /* L2PNET_ENABLE_OLD_RECVSEND */





int L2PacketSend2( const unsigned char *rawPacket, unsigned int sock, long lWaitMilliSecs, unsigned int *sentLen )
{
	if( !sentLen ) return -1;
	(*sentLen) = 0;
	if( !rawPacket ) return -1;
	if( sock == 0xFFFFFFFF ) return -1;
	
	int r = 0;
	unsigned int sentb = 0; // sent bytes
	unsigned int plen = 0;
	unsigned char *plenaddr = (unsigned char *)&plen;
	plenaddr[0] = rawPacket[0];
	plenaddr[1] = rawPacket[1];

	int rdyR = 0, rdyW = 0;
	
	int nTimeouts = 0;
	int maxTimeouts = 5;
	while( sentb < plen )
	{
		(*sentLen) = sentb;
		//r = sock_tcp_wait_ready_to_send( s, tv_sec, tv_usec );
		r = L2PNet_select( sock, L2PNET_SELECT_WRITE, lWaitMilliSecs, &rdyR, &rdyW );
		if( r < 0 ) return r; // select returned error
		if( r == 0 ) // timeout
		{
			nTimeouts++;
			if( nTimeouts > maxTimeouts )
			{
				return 0;
			}
			continue; // continue loop on select() timeout
		}
		// if we're here, select OK
		nTimeouts = 0; // zero timeouts count
		// we sent [sentb] bytes, left to send: [plen-sentb]
		r = L2PNet_send( sock, (const unsigned char *)(rawPacket + sentb), (plen - sentb) );
		if( r > 0 ) sentb += r;
		else
		{
			if( r < 0 ) return r;
		}
		(*sentLen) = sentb;
	}
	
	return sentb;
}

int L2PacketSend( unsigned int sock, L2BasePacket *pack, long lWaitMilliSecs, unsigned int *sentLen )
{
	return L2PacketSend2( pack->getBytesPtr(), sock, lWaitMilliSecs, sentLen );
}
