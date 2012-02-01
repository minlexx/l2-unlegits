#ifndef L2PACKETREADER_H_
#define L2PACKETREADER_H_

//#define L2P_NETWORK_DEBUGOUT

#ifdef L2PNET_ENABLE_OLD_RECVSEND
unsigned char *L2PacketReceive(
		unsigned int sock,
		long tv_sec, long tv_usec,
		unsigned int *rcvdLen );

#endif /* L2PNET_ENABLE_OLD_RECVSEND */

/** L2PacketReceive_malloc:\n
 * Reads L2 packet from socket sock\n
 * Reads 1st 2 bytes, treates them as packet len, and then reads len bytes\n
 * places all received data (including 1st 2 bytes) in allocated by malloc() buffer.\n
 * Sets (*rcvdLen) to number of bytes received\n
\n
 * On read timeout (lWaitMiliSecs) returns NULL\n
 * On error returns NULL\n
 * \return pointer to buffer on success. caller should free returned buffer by free(). On error returns NULL
 * \warning caller must free returned buffer by call to free()
 * \param sock - socket
 * \param lWaitMilliSecs - wait timeout
 * \param rcvdLen - (*rcvdLen) will be set to number of bytes successfuly received
*/
unsigned char *L2PacketReceive_malloc( SOCKET sock, long lWaitMilliSecs, unsigned int *rcvdLen );

/** L2PacketReceive_buffer\n
 * Reads L2 packet from socket sock\n
 * Reads 1st 2 bytes, treates them as packet len, and then reads len bytes\n
 * places all received data (including 1st 2 bytes) in buffer recvBuffer.\n
 * Sets (*rcvdLen) to number of bytes received\n
\n
 * On read timeout (lWaitMiliSecs) returns 0\n
 * On error returns -1\n
 * \return 1 on success\n

 * \warning recvBuffer must point to big enough memory block (10 Kb should be enough in all cases)\n
 * \param sock - socket
 * \param lWaitMilliSecs - wait timeout
 * \param rcvdLen - (*rcvdLen) will be set to number of bytes successfuly received
 * \param recvBuffer - buffer in which data will be placed
*/
int L2PacketReceive_buffer( SOCKET sock, long lWaitMilliSecs, unsigned int *rcvdLen, unsigned char *recvBuffer );

#endif /*L2PACKETREADER_H_*/
