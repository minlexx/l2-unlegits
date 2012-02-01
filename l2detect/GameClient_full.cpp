#include "stdafx.h"
#include "ConfigIni.h"
#include "Logger.h"
#include "GameClient.h"
#include "GameListener.h"
#include "CharArray.h"
#include "ClanList.h"
#include "ProtocolConverter.h"
//
#include "RadarDllWnd.h"

extern class CConfig g_cfg;
extern class GameListener *g_pgame;

bool GameClient::PC_full( SOCKET scl, unsigned char *sip, unsigned short int sport )
{
	sock_client = INVALID_SOCKET;
	sock_server = INVALID_SOCKET;
	this->resetCounters( false );
	log_error( LOG_DEBUG, "GameClient::ProcessClient_full() started\n" );
	// assert
	if( (scl == INVALID_SOCKET) || (!sip) || (sport<1) )
	{
		log_error( LOG_ERROR, "GameClient::ProcessClient_full(): start assert failed\n" );
		return false;
	}
	// vars
	bool retVal = false;
	SOCKET sg = INVALID_SOCKET;
	char ssip[16] = {0};
	sprintf( ssip, "%d.%d.%d.%d", (int)sip[0], (int)sip[1], (int)sip[2], (int)sip[3] );

	// first we should connect to real game server
	// create real GS socket
	sg = L2PNet_TCPsocket_create( true );
	if( sg == INVALID_SOCKET )
	{
		log_error( LOG_ERROR, "GameClient::ProcessClient_full(): real GS socket create\n" );
		return false;
	}

	// connect to real game server
	log_error( LOG_DEBUG, "GameClient::ProcessClient_full(): Connecting to real game server %s:%d ", ssip, (int)sport );
	L2PNet_connect( sg, ssip, sport );
	int nTries = 20;
	int r = 0;
	while( nTries > 0 )
	{
		log_error_np( LOG_DEBUG, "." );
		r = L2PNet_select( sg, L2PNET_SELECT_WRITE, 500, NULL, NULL );
		if( r == 1 ) break;
		nTries--;
	}

	if( r == 1 )
	{
		log_error_np( LOG_DEBUG, " Connected. Tries left: %d\n", nTries );
	}
	else
	{
		log_error_np( LOG_DEBUG, " Failed!\n", nTries );
		log_error( LOG_ERROR, "PC_full(): cannot connect to real game server!\n" );
		// close sockets
		L2PNet_shutdown( scl );
		L2PNet_closesocket( scl );
		L2PNet_shutdown( sg );
		L2PNet_closesocket( sg );
		log_error_np( LOG_DEBUG, " Not connected.\n" );
		return false;
	}

	// allocate receive buffer
	unsigned char *packbuffer = (unsigned char *)malloc( 65536 ); // 64 Kb :)
	if( !packbuffer )
	{
		log_error( LOG_ERROR, "PC_full(): cannot allocate 64 Kb packet receive buffer!\n" );
		// close sockets
		L2PNet_shutdown( scl );
		L2PNet_closesocket( scl );
		L2PNet_shutdown( sg );
		L2PNet_closesocket( sg );
		log_error_np( LOG_DEBUG, " Not connected.\n" );
		return false;
	}

	// open log file
	if( g_cfg.LogGamePackets )
	{
		char filename[256];
		time_t nowTime;
		time( &nowTime );
		sprintf( filename, "%s%u.txt", g_cfg.LogGameFileNamePrefix, (unsigned int)nowTime );
		this->logfile = fopen( filename, "wt" );
		if( !logfile )
			log_error( LOG_WARNING, "GameClient::PC_full(): Cannot open log file [%s] for writing!\n",
				filename );
	}

	// set initial game client state
	this->state = GCST_CONNECTED;
	this->thisWasJustServerPing = false; // initial assign before any packet recieved
	bool bWasModified = false;
	this->sock_client = scl;
	this->sock_server = sg;

	ProtoConv_Init();

	this->postNotify( GCN_STATECHANGE, this->state );
	ai.notifyEvent( UAI_EVENT_START );
	this->counters.startTimer();

	// infinite loop
	int recvTimeout = 120; // seconds
	long recvTimeoutMsec = recvTimeout * 1000;
	int selectTimeout = 1; // seconds
	struct timeval tv;
	fd_set fdReadSet;
	unsigned char *newBytes = NULL;
	unsigned int rcvdLen = 0;
	unsigned int sentLen = 0;
	unsigned int newLen = 0;
	int wsaerr = 0;
	unsigned int scode = 0;

	while( 1 )
	{
		scode = g_pgame->getLastSignal();
		if( scode == FLGS_STOP )
		{
			log_error( LOG_DEBUG, "GameClient::PC_full(): exit signal\n" );
			break;
		}
		
		tv.tv_sec  = selectTimeout;
		tv.tv_usec = 0;

		// check readability on sockets
		FD_ZERO( &fdReadSet );   // zero set
		FD_SET( scl, &fdReadSet ); // add client socket to read set
		FD_SET( sg,  &fdReadSet ); // add real GS socket to read set
		//
		r = L2PNet_select_wrapper_DUMB( 0, &fdReadSet, 0, 0, &tv );
		if( r == 0 ) /* timeout failed  */
		{
			log_error_np( LOG_DEBUGDUMP, "." );
		}
		else if( r == -1 ) /* select error */
		{
			log_error_np( LOG_DEBUGDUMP, "\nselect() ERROR\n" );
			// TODO: what to do on select() error?
			wsaerr = L2PNet_WSAGetLastError();
			log_error( LOG_ERROR, "WinSock select() error %d: ", wsaerr );
			L2PNet_WSAPrintLastError( ErrorLogger_GetLogFile(), wsaerr );
			log_error_np( LOG_DEBUGDUMP, "\n" );
			//log_error( LOG_ERROR, "ProcessClient_full(): socket select() ERROR!\n" );
			//log_error( LOG_ERROR, "ProcessClient_full(): closing processing\n" );
			//goto netError;
		}
		else if( r > 0 )
		{
			log_error_np( LOG_DEBUGDUMP, "\nselect(): number of sockets ready: %d\n", r );
			// client sent something?
			if( L2PNet_FD_ISSET( scl, &fdReadSet ) )
			{
				//log_error( LOG_DEBUGDUMP, "C->S (reading...)\n" );
				// receive packet from client
				r = L2PacketReceive_buffer( scl, recvTimeoutMsec, &rcvdLen, packbuffer );
				//log_error( LOG_DEBUGDUMP, "Received %u bytes from client\n", rcvdLen );
				if( (r == 0) && (rcvdLen == 0) )
				{
					log_error( LOG_WARNING, "Connection closed by client?\n" );
					goto closeSocks;
				}
				if( r <= 0 )
				{
					log_error( LOG_ERROR, "L2PacketReceive() failed!\n" );
					wsaerr = L2PNet_WSAGetLastError();
					log_error( LOG_ERROR, "WinSock error %d: ", wsaerr );
					L2PNet_WSAPrintLastError( ErrorLogger_GetLogFile(), wsaerr );
					log_error_np( LOG_ERROR, "\n" );
					goto netError;
				}

				// process it! :)
				bWasModified = PP_full_fromClient( packbuffer, rcvdLen, &newBytes, &newLen );
				if( newLen != -1 ) // -1 means - Drop packet
				{
					// resend packet to server after processing
					LockSend();
					r = L2PacketSend2( newBytes, sg, recvTimeoutMsec, &sentLen );
					UnlockSend();
					// if packet was not modified: newBytes points to the same memory as p points
					// if packet was modified: newBytes points to new memory
					//    ... in this case we should free new memory separately
					if( newBytes != packbuffer ) free( newBytes ); // :)
					// in both cases newLen contains length to be sent

					//log_error( LOG_DEBUGDUMP, "Sent %u bytes to server\n", sentLen );
					if( sentLen != newLen ) // we must send newLen
					{
						log_error( LOG_ERROR, "PC_full(): After L2PacketSend2() C->S sentLen != newLen!!! (%u != %u, bWasModified = %d)\n",
							sentLen, newLen, (int)bWasModified );
						goto netError;
					}
					// counters
					this->counters.addSentPacket( sentLen );
				}
				//else
				//{
				//	log_error( LOG_DEBUGDUMP, "PC_full(): dropped packet...\n" );
				//}
			}

			// server sent something?
			if( L2PNet_FD_ISSET( sg, &fdReadSet ) )
			{
				//log_error( LOG_DEBUGDUMP, "S->C (reading...)\n" );
				// receive packet from server
				r = L2PacketReceive_buffer( sg, recvTimeoutMsec, &rcvdLen, packbuffer );
				//log_error( LOG_DEBUGDUMP, "Received %u bytes from server\n", rcvdLen );
				if( (r == 0) && (rcvdLen == 0) )
				{
					log_error( LOG_DEBUG, "Connection closed by server?\n" );
					goto closeSocks;
				}
				if( r <= 0 )
				{
					log_error( LOG_ERROR, "L2PacketReceive() failed!\n" );
					wsaerr = L2PNet_WSAGetLastError();
					log_error( LOG_ERROR, "WinSock error %d: ", wsaerr );
					L2PNet_WSAPrintLastError( ErrorLogger_GetLogFile(), wsaerr );
					log_error_np( LOG_ERROR, "\n" );
					goto netError;
				}

				// counters
				this->counters.addRcvdPacket( rcvdLen );

				// process it! :)
				bWasModified = PP_full_fromServer( packbuffer, rcvdLen, &newBytes, &newLen );
				if( newLen != -1 ) // -1 means - Drop packet
				{
					// resend packet to client
					LockSend();
					r = L2PacketSend2( newBytes, scl, recvTimeoutMsec, &sentLen );
					UnlockSend();
					// if packet was not modified: newBytes points to the same memory as p points
					// if packet was modified: newBytes points to new memory
					//    ... in this case we should free new memory separately
					if( newBytes != packbuffer )
					{
						//log_error( LOG_OK, "Packet was modified, freeing newBytes buffer (newLen = %u)\n", newLen );
						free( newBytes ); // :)
					}
					// in both cases newLen contains length to be sent

					//log_error( LOG_DEBUGDUMP, "Sent %u bytes to client\n", sentLen );
					if( sentLen != newLen )
					{
						log_error( LOG_ERROR, "PC_full(): After L2PacketrSend2() S->C sent != rcvd!!! (%u != %u, bWasModified = %d)\n",
							sentLen, newLen, (int)bWasModified );
						goto netError;
					}
				} // newLen != -1
			}
		} // (r>0) // select() OK
	}// while(1)

	retVal = true;
netError:
	retVal = true;
	if( wsaerr > 0 )
	{
		retVal = false;
		log_error( LOG_ERROR, "PC_full(): some error happened or network connection closed...\n" );
	}
closeSocks:
	// save radar window size
	PostMessage( g_radardll_hwnd, WMMY_SAVE_WINDOW_SIZE, 0, 0 );
	// logfile
	if( this->logfile )
	{
		fclose( this->logfile );
		this->logfile = NULL;
	}
	// free packbuffer
	if( packbuffer ) free( packbuffer );
	packbuffer = NULL;
	// stop AI thread
	ai.notifyEvent( UAI_EVENT_STOP );
	// reset internal state
	this->resetConnectedState();
	// sockets
	L2PNet_shutdown( scl );
	L2PNet_closesocket( scl );
	L2PNet_shutdown( sg );
	L2PNet_closesocket( sg );
	// lists
	CharArray_DeleteAll();
	ClanList_DeleteAll();
	// switch state
	this->state = GCST_OFFLINE;
	this->postNotify( GCN_STATECHANGE, this->state );
	// log counters
	GameClient_NetStats netstats;
	counters.calcNowStats( &netstats );
	log_error_np( LOG_OK, "=============================================\n" );
	log_error_np( LOG_OK, "Network usage stats:\n" );
	log_error_np( LOG_OK, "Bytes, packets sent: %I64u, %I64u\n", counters.ullBytesSent, counters.ullPacketsSent );
	log_error_np( LOG_OK, "Bytes, packets rcvd: %I64u, %I64u\n", counters.ullBytesRcvd, counters.ullPacketsRcvd );
	log_error_np( LOG_OK, "Time connected: %0.2f seconds\n", netstats.timePassedSecs );
	log_error_np( LOG_OK, "Avg send speed: %0.2f bytes/sec\n", netstats.avgSendSpeed );
	log_error_np( LOG_OK, "Avg recv speed: %0.2f bytes/sec\n", netstats.avgRecvSpeed );
	log_error_np( LOG_OK, "Avg send L2 packets: %0.2f packets/sec\n", netstats.avgSendPacketsPerSec );
	log_error_np( LOG_OK, "Avg recv L2 packets: %0.2f packets/sec\n", netstats.avgRecvPacketsPerSec );
	log_error_np( LOG_OK, "Avg out packet size: %0.2f bytes\n", netstats.avgSendPacketSize );
	log_error_np( LOG_OK, "Avg in  packet size: %0.2f bytes\n", netstats.avgRecvPacketSize );
	log_error_np( LOG_OK, "=============================================\n" );
	ErrorLogger_FlushLogFile();

	//
	log_error( LOG_DEBUG, "GameClient::PC_full() ending, returning %d\n", (int)retVal );
	return retVal;
}


// if packet was modified, it will be encoded again before resending
bool GameClient::PP_full_reencode_packet( unsigned char *bytes, unsigned int len, unsigned char *key )
{
	if( !this->xor_enabled ) return true; // no need to encrypt now - XOR keys not received yet
	//if( !bytes || (len<3) || !key ) return false; // asserts
	//L2GamePacket p;
	//if( !p.setBytes( bytes, len ) ) return false;
	//if( !p.encodeXOR( key ) ) return false;
	//memcpy( bytes, p.getBytesPtr(), len );
	// speed optimisation - remove constructor(), setBytes() calls and memcpy() call
	bool ret = L2GamePacket::encodeXOR_buffer( bytes, len, key ); // encode directly in buffer
	/*log_error( LOG_DEBUGDUMP, "PP_full_reencode_packet(): packet of len %u re-encode result: %d\n",
		len, (int)ret );*/
	return ret;
}
