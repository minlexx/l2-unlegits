#pragma once
#include "enums.h"

class LoginConnection
{
public:
	LoginConnection();
	~LoginConnection();

public:
	void start( const wchar_t *ls_addr, int ls_port, int ls_protover );
	void stop();

public:
	int getRegisteredServerId() const;
	const wchar_t *getRegisteredServerName() const;

protected:
	CriticalSection m_lock;
	wchar_t         m_ls_addr[128];
	int             m_ls_port;
	int             m_ls_protover;
	int             m_registeredServerId;
	wchar_t         m_registeredServerName[256];
protected:
	static DWORD WINAPI LS_ConnThread( LPVOID lpvParam );
	bool            m_isRunning;
	bool            m_isConnected;
	bool            m_flagStop;
	SOCKET          m_sock;
	unsigned char   m_bfKey[64];
	unsigned int    m_bfKeyLen;
	RSA            *m_rsaKey;

protected:
	// senders
	void send_BlowfishKey();                              // 0x00
	void send_GameServerAuthRequest();                    // 0x01
public: // these are public senders
	void send_PlayerInGame( const wchar_t *accountName ); // 0x02
	void send_PlayerLogOut( const wchar_t *accountName ); // 0x03
	void send_ChangeAccessLevel( const wchar_t *accountName, int new_access_level ); // 0x04
	void send_PlayerAuthRequest( const wchar_t *accountName, unsigned char *loginKey, unsigned char *playKey ); // 0x05
	void send_ServerStatus();                             // 0x06

protected:
	// handlers
	void ph_InitLS( L2LoginPacket *pack );              // 0x00
	void ph_LoginServerFail( L2LoginPacket *pack );     // 0x01
	void ph_LoginServerOK( L2LoginPacket *pack );       // 0x02
	void ph_PlayerAuthResponse( L2LoginPacket *pack );  // 0x03
	void ph_KickPlayer( L2LoginPacket *pack );          // 0x04
	// internal
	void sendLSPacket( L2LoginPacket *pack );
};
