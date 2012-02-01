#pragma once

class LoginServer;

class LoginClient
{
public:
	enum State { NONE, CONNECTED, AUTHED_GG, AUTHED_LOGIN };
public:
	LoginClient( LoginServer *pLoginServer, unsigned int sock, const char *clientIp, int clientPort );
	~LoginClient();
private:
	LoginClient();
	LoginClient( const LoginClient& other );

public:
	void disconnect( bool force );
	void beginProcessing();

public:
	const char *getIpAddress() const;
	const char *getAccount() const;
	const wchar_t *toString() const;

	int  getAccessLevel() const;
	void setAccessLevel( int a );
	int  getLastServer() const;
	void setLastServer( int s );
	void setAccount( const char *account );
	bool isUsingInternalIp() const;
	bool hasJoinedGS( int *gs_id );

	void getSessionKeys( unsigned char *sessionKey1, unsigned char *sessionKey2 );

protected:
	LoginServer  *m_loginServer;
	unsigned int  m_sock;
	char          m_client_ip[16];
	int           m_client_port;
	bool          m_running;
	wchar_t       m_clientDesc[128];
	State         m_state;
	bool          m_flagBreak;
	char          m_account[64];
	int           m_accessLevel;
	int           m_lastServer;
	bool          m_isUsingInternalIp;
	bool          m_joinedGS;
	int           m_joinedGSID;

protected: // protocol state vars
	unsigned char  p_sessionId[4];
	unsigned char  p_rsa_pubkey_mod[128];
	unsigned char  p_bf_dyn_key[16];
	RSA           *p_rsaKey;
	unsigned char  p_sessionKey1[8];
	unsigned char  p_sessionKey2[8];

protected:
	static unsigned int __stdcall LoginClientThread( LoginClient *pcls );
	void invalidPacket( L2LoginPacket *pack );
	void ph_RequestGGAuth( L2LoginPacket *pack );
	void ph_RequestAuthLogin( L2LoginPacket *pack );
	void ph_RequestServerList( L2LoginPacket *pack );
	void ph_RequestServerLogin( L2LoginPacket *pack );
	void encodeAndSendPacket( L2LoginPacket *pack );
};
