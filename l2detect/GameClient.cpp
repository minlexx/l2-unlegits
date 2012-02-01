#include "stdafx.h"
#include "ConfigIni.h"
#include "Logger.h"
#include "GameClient.h"
#include "GameListener.h"

extern class GameListener *g_pgame;      // main.cpp
extern class CConfig g_cfg;               // main.cpp

GameClient::GameClient()
{
	memset( key_client_sc, 0, sizeof(key_client_sc) );
	memset( key_client_cs, 0, sizeof(key_client_cs) );
	memset( key_server_sc, 0, sizeof(key_server_sc) );
	memset( key_server_cs, 0, sizeof(key_server_cs) );
	state = 0;
	gameProtoVer = 0;
	xor_enabled = false;
	logfile = NULL;
	logfileRaw = NULL;
	this->resetCounters( false );
	this->opcodeObfuscator = 0;
	sock_client = INVALID_SOCKET;
	sock_server = INVALID_SOCKET;
	clsObfuscator = 0;
	thisWasJustServerPing = false; // constructor
	hWndNotify = NULL;
	uMsgNotify = 0;
	InitializeCriticalSection( &cs_send );
	ai.clear();
}

GameClient::~GameClient()
{
	state = 0;
	gameProtoVer = 0;
	xor_enabled = false;
	if( logfile )
	{
		fflush( logfile );
		fclose( logfile );
		logfile = NULL;
	}
	this->resetCounters( false );
	this->opcodeObfuscator = 0;
	thisWasJustServerPing = false; // destructor
	if( sock_client != INVALID_SOCKET )
	{
		//closesocket( sock_client );
		L2PNet_closesocket( sock_client );
		sock_client = INVALID_SOCKET;
	}
	if( sock_server != INVALID_SOCKET )
	{
		//closesocket( sock_server );
		L2PNet_closesocket( sock_server );
		sock_server = INVALID_SOCKET;
	}
	// delete obfuscator, if exists
	if( this->clsObfuscator )
	{
		class L2PCodeObfuscator * lpco = (class L2PCodeObfuscator *)clsObfuscator;
		lpco->clear();
		delete lpco;
		this->clsObfuscator = NULL;
	}
	ai.clear();
	hWndNotify = NULL;
	uMsgNotify = 0;
}

void GameClient::logPacket(
		const unsigned char *bytes,
		unsigned int len,
		bool bFromServer )
{
	if( !this->logfile || !bytes || (len<1) ) return;
	//
	//if( bFromServer ) return; //:)
	//
	if( bFromServer ) fprintf( logfile, "Server: " );
	             else fprintf( logfile, "Client: " );
	if( len < 3 )
	{
		fprintf( logfile, "Warning: packet length = %u ( < 3!)\n", len );
		return;
	}
	char pname[256] = {0};
	L2_VERSION l2_version = (L2_VERSION)g_cfg.L2_version;
	if( !bFromServer ) l2_version = (L2_VERSION)g_cfg.L2_client_version;
	L2Packets_GetL2PacketName( l2_version, bFromServer, bytes[2], bytes[3], bytes[5], pname, 255 );
	fprintf( logfile, "Len %4u [%s] | %02X %02X / %02X / ", len, pname,
		bytes[0], bytes[1], bytes[2] );
	int i;
	for( i=3; i<(int)len; i++ )
	{
		fprintf( logfile, "%02X ", bytes[i] );
		if( ((i-2) % 4) == 0 ) fprintf( logfile, " " );
	}
	fprintf( logfile, "\n" );
	fflush( logfile );
}

void GameClient::logPacketRaw(
		const unsigned char *bytes,
		unsigned int len,
		bool bFromServer )
{
	if( !this->logfileRaw || !bytes || (len<1) ) return;
	//
	//if( bFromServer ) return; //:)
	//
	if( bFromServer ) fprintf( logfileRaw, "Server: " );
	             else fprintf( logfileRaw, "Client: " );
	if( len < 3 )
	{
		fprintf( logfileRaw, "Warning: packet length = %u ( < 3!)\n", len );
		return;
	}
	fprintf( logfileRaw, "Len %4u | %02X %02X / %02X / ", len,
		bytes[0], bytes[1], bytes[2] );
	int i;
	for( i=3; i<(int)len; i++ )
	{
		fprintf( logfileRaw, "%02X ", bytes[i] );
		if( ((i-2) % 4) == 0 ) fprintf( logfileRaw, " " );
	}
	fprintf( logfileRaw, "\n" );
	fflush( logfileRaw );
}

void GameClient::resetConnectedState()
{
	// reset state
	state = 0;
	// reset other session vars: game proto ver, XOR encrypting
	gameProtoVer = 0;
	xor_enabled = false;
	// reset XOR encryting keys
	memset( key_client_sc, 0, sizeof(key_client_sc) );
	memset( key_client_cs, 0, sizeof(key_client_cs) );
	memset( key_server_sc, 0, sizeof(key_server_sc) );
	memset( key_server_cs, 0, sizeof(key_server_cs) );
	// sockets
	sock_client = INVALID_SOCKET;
	sock_server = INVALID_SOCKET;
	// obfuscator
	opcodeObfuscator = 0;
	if( this->clsObfuscator )
	{
		class L2PCodeObfuscator * lpco = (class L2PCodeObfuscator *)clsObfuscator;
		lpco->clear();
		delete lpco;
		log_error( LOG_DEBUG, "GameClient::resetConnectedState(): deleted obfuscator class\n" );
		this->clsObfuscator = NULL;
	}
	//thisWasJustServerPing = false; // NO!!! don't reset it here!
	//hWndNotify = NULL;
	//uMsgNotify = 0;
}

void GameClient::resetCounters( bool addFakePacket )
{
	counters.resetCounters();
	if( addFakePacket )
	{
		counters.addRcvdPacket( 10 );
		counters.addSentPacket( 10 );
	}
}

void GameClient::getCounters( GameClientCounters& out ) const
{
	out = counters;
}

void GameClient::setCountersStartingExp( unsigned long long int exp )
{
	if( counters.ullStartingExp == 0 )
		counters.ullStartingExp = exp;
}

void GameClient::startCountersTimer()
{
	counters.startTimer();
}

void GameClient::setNotifyWindow( HWND notifyHwnd, UINT notifyMsg )
{
	this->hWndNotify = notifyHwnd;
	this->uMsgNotify = notifyMsg;
}

void GameClient::postNotify( WPARAM wParam, LPARAM lParam )
{
	if( hWndNotify == NULL || (uMsgNotify < WM_USER) ) return;
	PostMessage( hWndNotify, uMsgNotify, wParam, lParam );
}

void GameClient::LockSend()
{
	EnterCriticalSection( &cs_send );
}

void GameClient::UnlockSend()
{
	LeaveCriticalSection( &cs_send );
}

