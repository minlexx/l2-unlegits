#pragma once
#include "enums.h"
#include "net/ClientPacketHandler.h"
#include "GameClientNetStats.h"
#include "world/model/character/GamePlayer.h"

class GameClient
{
public:
	GameClient( SOCKET s_cl, const char *ip, int port );
	virtual ~GameClient();
	virtual void setUnused();

public:
	const wchar_t            *getAccount() const;
	void                      getPlayKey( unsigned char *buffer ) const;
	void                      getLoginKey( unsigned char *buffer ) const;
	const char               *getIpA() const;
	const wchar_t            *getIpW() const;
	int                       getPort() const;
	const wchar_t            *toString() const;
	ClientState               getState() const;
	bool                      isProtocolOk() const;
	const GameClientNetStats *getNetStats() const;
	unsigned int              getOpcodeObfuscationSeed() const;
	L2PCodeObfuscator        *getOpcodeObfuscator();
	GamePlayer               *getPlayer();

public:
	void                      setAccount( const wchar_t *acc );
	void                      setSessionKeys( const unsigned char *playKey, const unsigned char *loginKey );
	void                      setProtocolOk( bool b ) ;
	void                      setOpcodeObfuscationSeed( unsigned int s );
	void                      setPlayer( GamePlayer *newPlayer, bool changeState = false, ClientState newState = CLIENT_STATE_OFFLINE );

public:
	// begins client processing, starts thread
	bool                 startThread();
	// stops all client activity, disconnects socket, optionally sends packet before client close
	void                 disconnectForceAndWaitThreadToStop( bool server_shutdown, bool reasonDualLogin );
	// signal thread to stop (no wait to stop)
	void                 signalThreadStop( bool setMarkToDeleteSelfAndCleanupClientPool );
	bool                 sendPacket( L2GamePacket *pack, bool deleteAfterSend = false );
	void                 enable_XOR_crypt( bool bEnable, unsigned char *initial_key );
	void                 notifySessionKeysOK();

protected:
	// signals to stop and waits for thread to stop
	bool                 stopThread();
	static DWORD WINAPI  ClientThread( LPVOID lpvParam );

protected:
	// informational stings
	char                 m_ip[16];
	wchar_t              m_wip[16];
	int                  m_port;
	wchar_t              m_account[128];
	wchar_t              m_tostring[128];
	// network-related vars
	SOCKET               m_sock;
	bool                 m_was_force_disconnected;
	bool                 m_flagStop;
	bool                 m_isRunning;
	CriticalSection      m_cs_send;
	ClientState          m_state;
	bool                 m_xor_enabled;
	unsigned char        m_xor_key_send[16];
	unsigned char        m_xor_key_recv[16];
	ClientPacketHandler *m_ph;
	bool                 m_protocolIsOk;
	GameClientNetStats   m_netStats;
	unsigned int         m_opcodeObfuscationSeed;
	L2PCodeObfuscator   *m_obf;
	unsigned char        m_playKey[8];
	unsigned char        m_loginKey[8];
	//
	GamePlayer          *m_player;
};
