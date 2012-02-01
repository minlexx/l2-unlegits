#ifndef FAKELOGIN_GAMECLIENT_H_
#define FAKELOGIN_GAMECLIENT_H_

#include "UserAI.h"
#include "GameClientCounters.h"

// client state
#define GCST_OFFLINE    0 // not connected to GS :)
#define GCST_CONNECTED  1 // just connected to GS
#define GCST_AUTHED     2 // passed GS auth, sees chars selection list
#define GCST_IN_GAME    3 // selectedc har and entered game

// window notification wParam
#define GCN_STATECHANGE 1 // lParam will hold new state

class GameClient
{
public:
	GameClient();
	virtual ~GameClient();
	
public: // sniffer functions
	bool PC_sniff( SOCKET scl, unsigned char *sip, unsigned short int sport );
	void PP_sniff_fromServer( unsigned char *bytes, unsigned int len );
	void PP_sniff_fromClient( unsigned char *bytes, unsigned int len );
	
public:
	// algorythm of processing with full packet modifying control
	bool PC_full( SOCKET scl, unsigned char *sip, unsigned short int sport );

	// Parameters:
	//
	// returns true if packet was modified
	// if packet was not modified:
	//     sets (*newLen) to len;
	//     sets (*newBytes) to bytes
	//      - caller should ignore newLen and newBytes content in this case
	//      - caller should send original packet
	// if packet was modified:
	//     sets (*newLen) to new length of packet
	//     sets (*newBytes) to point to new bytes buffer
	//      - newBytes are properly encoded if needed,
	//      - caller just should send newBytes and free them after sending
	// if packet should be dropped:
	//     sets (*newLen) to -1
	//      - caller should ignore newLen and newBytes content
	//      - caller should not send any packet :)
	//
	bool PP_full_fromServer( unsigned char *bytes, unsigned int len,
		unsigned char **newBytes, unsigned int *newLen );

	// Parameters:
	//
	// returns true if packet was modified
	// if packet was not modified:
	//     sets (*newLen) to len;
	//     sets (*newBytes) to bytes
	//      - caller should ignore newLen and newBytes content in this case
	//      - caller should send original packet
	// if packet was modified:
	//     sets (*newLen) to new length of packet
	//     sets (*newBytes) to point to new bytes buffer
	//      - newBytes are properly encoded if needed,
	//      - caller just should send newBytes and free them after sending
	// if packet should be dropped:
	//     sets (*newLen) to -1
	//      - caller should ignore newLen and newBytes content
	//      - caller should not send any packet :)
	//
	bool PP_full_fromClient( unsigned char *bytes, unsigned int len,
		unsigned char **newBytes, unsigned int *newLen );

	// if packet was modified, it will be encoded again before resending
	bool PP_full_reencode_packet( unsigned char *bytes, unsigned int len, unsigned char *key );

public: // counters
	void resetCounters( bool addFakePacket );
	void getCounters( GameClientCounters& out ) const;
	void setCountersStartingExp( unsigned long long int exp );
	void startCountersTimer();
public:
	int getState() const { return this->state; }
	bool InjectPacketToServer( unsigned char *bytes, unsigned int len, bool doObfuscate );
	bool InjectPacketToClient( unsigned char *bytes, unsigned int len );
	//void setHealItemsTable( HealItemsTable *other );
	void setNotifyWindow( HWND notifyHwnd, UINT notifyMsg );

protected:
	void LockSend();
	void UnlockSend();
	void logPacket( const unsigned char *bytes, unsigned int len, bool bFromServer );
	void logPacketRaw( const unsigned char *bytes, unsigned int len, bool bFromServer );
	void resetConnectedState();
	void postNotify( WPARAM wParam, LPARAM lParam );

public: // char state, inventory...
	UserAI ai;

	bool wasJustServerPing() { return thisWasJustServerPing; }
	
protected:

	// protocol state vars
	bool            xor_enabled;
	unsigned char   key_client_sc[32]; // key S->C (to re-encrypt FOR client side)
	unsigned char   key_client_cs[32]; // key C->S (to decrypt FROM client side)
	unsigned char   key_server_sc[32]; // key S->C (to decrypt FROM server side)
	unsigned char   key_server_cs[32]; // key C->S (to re-encrypt FOR server side)
	unsigned int    gameProtoVer;
	unsigned int    opcodeObfuscator;
	void           *clsObfuscator;
	bool            thisWasJustServerPing; // remains true, when client just sends server ping

	// system vars
	CRITICAL_SECTION cs_send;
	int              state;
	FILE            *logfile;
	FILE            *logfileRaw;

	// counters
	GameClientCounters counters;

	// sockets
	SOCKET sock_client;
	SOCKET sock_server;

	// notify window
	HWND hWndNotify;
	UINT uMsgNotify;
};

#endif /*FAKELOGIN_GAMECLIENT_H_*/
