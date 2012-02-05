#include "stdafx.h"
#include "ConfigIni.h"
#include "Logger.h"
#include "GameClient.h"
#include "GameListener.h"
#include "CharArray.h"
#include "ClanList.h"

extern class CConfig g_cfg;
extern class GameListener *g_pgame;

bool GameClient::PC_sniff( SOCKET scl, unsigned char *sip, unsigned short int sport )
{
	sock_client = INVALID_SOCKET;
	sock_server = INVALID_SOCKET;
	this->resetCounters( false );
	log_error( LOG_DEBUG, "GameClient::ProcessClient_onlySniff() started\n" );
	// assert
	if( (scl == INVALID_SOCKET) || (!sip) || (sport<1) )
	{
		log_error( LOG_ERROR, "GameClient::ProcessClient_onlySniff(): start assert failed\n" );
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
		log_error( LOG_ERROR, "GameClient::ProcessClient_onlySniff(): real GS socket create\n" );
		return false;
	}
	
	// connect to real game server
	log_error( LOG_DEBUG, "GameClient::ProcessClient_onlySniff(): Connecting to real game server %s:%d ", ssip, (int)sport );
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

	log_error_np( LOG_DEBUG, "\n" );
	if( r == 1 )
	{
		log_error( LOG_DEBUG, "GameClient::ProcessClient_onlySniff(): Connected. Tries left: %d\n", nTries );
	}
	else
	{
		// close sockets
		L2PNet_shutdown( scl );
		L2PNet_closesocket( scl );
		L2PNet_shutdown( sg );
		L2PNet_closesocket( sg );
		log_error( LOG_DEBUG, "GameClient::ProcessClient_onlySniff(): Not connected.\n" );
		return false;
	}

	// open log file
	if( g_cfg.LogGamePackets )
	{
		char filename[256];
		char filename_raw[256];
		time_t nowTime;
		time( &nowTime );
		sprintf( filename, "%s%u.txt",
			g_cfg.LogGameFileNamePrefix,
			(unsigned int)nowTime );
		sprintf( filename_raw, "%s%u_raw.txt",
			g_cfg.LogGameFileNamePrefix,
			(unsigned int)nowTime );
		this->logfile = fopen( filename, "wt" );
		if( !logfile )
			log_error( LOG_WARNING, "GameClient::ProcessClient_onlySniff(): Cannot open log file [%s] for writing!\n",
				filename );
		// open raw log data for not decrypted packets, if problems with decryption
		this->logfileRaw = fopen( filename_raw, "wt" );
	}

	// set initial game client state
	this->state = GCST_CONNECTED;
	this->sock_client = scl;
	this->sock_server = sg;
	this->postNotify( GCN_STATECHANGE, this->state );
	this->counters.startTimer();
	ai.notifyEvent( UAI_EVENT_START );

	// infinite loop
	int recvTimeout = 20; // seconds
	long recvTimeoutMsec = recvTimeout * 1000;
	int selectTimeout = 1; // seconds
	struct timeval tv;
	fd_set fdReadSet;
	unsigned char *p = NULL;
	unsigned int rcvdLen = 0;
	unsigned int sentLen = 0;
	unsigned int scode = 0;

	while( 1 )
	{
		// first check are we signaled to exit?
		scode = g_pgame->getLastSignal();
		if( scode == FLGS_STOP )
		{
			log_error( LOG_DEBUG, "GameClient::ProcessClient_onlySniff(): exit signal\n" );
			break;
		}

		tv.tv_sec  = selectTimeout;
		tv.tv_usec = 0;

		// check readability on sockets
		FD_ZERO( &fdReadSet );   // zero set
		FD_SET( scl, &fdReadSet ); // add client socket to read set
		FD_SET( sg,  &fdReadSet ); // add real GS socket to read set
		//r = select( 0, &fdReadSet, 0, 0, &tv );
		r = L2PNet_select_wrapper_DUMB( 0, &fdReadSet, 0, 0, &tv );
		if( r == 0 ) /* timeout failed  */
		{
			log_error_np( LOG_DEBUGDUMP, "." );
		}
		else if( r == -1 ) /* select error */
		{
			log_error_np( LOG_DEBUGDUMP, "\nselect() ERROR\n" );
			// TODO: what to do on select() error?
			//int wsaerr = WSAGetLastError();
			int wsaerr = L2PNet_WSAGetLastError();
			log_error( LOG_ERROR, "WinSock error: " );
			L2PNet_WSAPrintLastError( ErrorLogger_GetLogFile(), wsaerr );
			log_error_np( LOG_DEBUGDUMP, "\n" );
			//log_error( LOG_ERROR, "ProcessClient_onlySniff(): socket select() ERROR!\n" );
			//log_error( LOG_ERROR, "ProcessClient_onlySniff(): closing processing\n" );
			//goto netError;
		}
		else if( r > 0 )
		{
			log_error_np( LOG_DEBUGDUMP, "\nselect(): number of sockets ready: %d\n", r );
			if( L2PNet_FD_ISSET( scl, &fdReadSet ) ) // client sent something?
			{
				log_error( LOG_DEBUGDUMP, "C->S (reading...)\n" );
				// first receive packet from client
				p = L2PacketReceive_malloc( scl, recvTimeoutMsec, &rcvdLen );
				log_error( LOG_DEBUGDUMP, "Received %u bytes from client\n", rcvdLen );
				if( p && (rcvdLen == 0) )
				{
					log_error( LOG_WARNING, "Connection closed by client?\n" );
					goto closeSocks;
				}
				if( !p )
				{
					log_error( LOG_ERROR, "L2PacketReceive() failed!\n" );
					goto netError;
				}

				// counters
				this->counters.addSentPacket( rcvdLen );

				// ... then resend packet to server
				r = L2PacketSend2( p, sg, recvTimeout*1000, &sentLen );
				log_error( LOG_DEBUGDUMP, "Sent %u bytes to server\n", sentLen );
				if( rcvdLen != sentLen )
				{
					log_error( LOG_ERROR, "sent != rcvd!!! (%u != %u)\n", sentLen, rcvdLen );
					goto netError;
				}

				// log raw bytes, before decode
				logPacketRaw( p, rcvdLen, false );
				// ... and only then process this packet
				PP_sniff_fromClient( p, rcvdLen );
				logPacket( p, rcvdLen, false );
				free( p );
				p = NULL;
			}

			if( L2PNet_FD_ISSET( sg, &fdReadSet ) ) // server sent something?
			{
				log_error( LOG_DEBUGDUMP, "S->C (reading...)\n" );
				// first receive packet from server
				p = L2PacketReceive_malloc( sg, recvTimeoutMsec, &rcvdLen );
				log_error( LOG_DEBUGDUMP, "Received %u bytes from server\n", rcvdLen );
				if( p && (rcvdLen == 0) )
				{
					log_error( LOG_WARNING, "Connection closed by server?\n" );
					goto closeSocks;
				}
				if( !p )
				{
					log_error( LOG_ERROR, "L2PacketReceive() failed!\n" );
					goto netError;
				}

				// counters
				this->counters.addRcvdPacket( rcvdLen );

				// .. then resend packet to client
				r = L2PacketSend2( p, scl, recvTimeout*1000, &sentLen );
				log_error( LOG_DEBUGDUMP, "Sent %u bytes to client\n", sentLen );
				if( rcvdLen != sentLen )
				{
					log_error( LOG_ERROR, "sent != rcvd!!! (%u != %u)\n", sentLen, rcvdLen );
					goto netError;
				}

				// log raw bytes, before decode
				logPacketRaw( p, rcvdLen, true );
				// ... and only then process it
				PP_sniff_fromServer( p, rcvdLen );
				logPacket( p, rcvdLen, true );
				free( p );
				p = NULL;
			}
		} // (r>0) // select() OK
	}// while(1)

	retVal = true;
netError:
	log_error( LOG_ERROR, "PC_sniff(): some error happened or network connection closed...\n" );
	if( p ) free( p );
	p = NULL;
	retVal = false;
closeSocks: // all cleanup
	// logfile
	if( this->logfile )
	{
		fclose( this->logfile );
		this->logfile = NULL;
	}
	if( this->logfileRaw )
	{
		fclose( this->logfileRaw );
		this->logfileRaw = NULL;
	}
	// stop AI thread
	ai.notifyEvent( UAI_EVENT_STOP );
	this->resetConnectedState();
	// sockets
	L2PNet_shutdown( scl );
	L2PNet_closesocket( scl );
	L2PNet_shutdown( sg );
	L2PNet_closesocket( sg );
	// lists
	CharArray_DeleteAll();
	ClanList_DeleteAll();
	// state
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
	log_error( LOG_DEBUG, "GameClient::PC_sniff() ending, returning %d\n", (int)retVal );
	return retVal;
}

