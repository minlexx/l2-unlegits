#pragma once

#include "GameServerInfo.h"

class LoginConfig;
class LoginServer;
struct GS_ConnectionInfo;

class GameServerTable
{
public:
	static const int MAX_RSA_KEYS = 10;
	static const int RSA_NUM_BITS = 512; // 1024 for L2 login clients, 512 for game servers
	static const int RSA_PUBLIC_EXPONENT = 65537;

	enum LoginFailReason
	{
		REASON_IP_BANNED = 1,
		REASON_IP_RESERVED = 2,
		REASON_WRONG_HEXID = 3,
		REASON_ID_RESERVED = 4,
		REASON_NO_FREE_ID = 5,
		NOT_AUTHED = 6,
		REASON_ALREADY_LOGGED_IN = 7
	};

public:
	GameServerTable( LoginServer *pLogin );
	~GameServerTable();
public:
	bool init();
	void cleanup();
	RSA *getRandomRSAKey();

public:
	bool            getServerNameById( int id, std::wstring& name );
	GameServerInfo *getRegisteredGameServerById( int id );
	bool            hasRegisteredGameServerOnId( int id );
	bool            registerServer( int id, GameServerInfo *gsinfo );
	bool            registerServerInDb( GameServerInfo *gsinfo );
	bool            registerServerInDb( int id, const char *hexid, const char *externalHost );
	bool            registerWithFirstAvaliableId( GameServerInfo *gsinfo );

public:
	const std::map<int, GameServerInfo *>& getRegisteredGameServers() const;
	bool            isAccountInAnyGameServer( const char *account );
	GameServerInfo *getGameServerThreadForAccount( const char *account );
	bool            isLoginPossible( int serverId, int accesslevel, const char *account, int lastServer );

public:
	void beginProcessGameServerClient( const char *ip, unsigned int sock );

protected:
	void loadServerNames();
	void loadRSAKeys();
	void loadRegisteredGameServers();

	const LoginConfig *getConfig() const;

protected:
	LoginServer                      *m_loginServer;
	std::map<int, std::wstring>       m_serverNames;
	std::map<int, GameServerInfo *>   m_gameServerTable;
	RSA                              *m_rsa_keys[MAX_RSA_KEYS];
	CRITICAL_SECTION                  m_lock;

protected:
	void Lock();
	void Unlock();
	static DWORD WINAPI GSConnThread( LPVOID lpParam );
	// GS packets handlers
	static void ph_ReceiveBlowfishKey( GS_ConnectionInfo *pinfo, L2LoginPacket *pack );
	static void ph_ReceiveGameServerAuth( GS_ConnectionInfo *pinfo, L2LoginPacket *pack );
	static void ph_ReceivePlayerInGame( GS_ConnectionInfo *pinfo, L2LoginPacket *pack );
	static void ph_ReceivePlayerLogOut( GS_ConnectionInfo *pinfo, L2LoginPacket *pack );
	static void ph_ReceiveChangeAccessLevel( GS_ConnectionInfo *pinfo, L2LoginPacket *pack );
	static void ph_ReceivePlayerAuthRequest( GS_ConnectionInfo *pinfo, L2LoginPacket *pack );
	static void ph_ReceiveServerStatus( GS_ConnectionInfo *pinfo, L2LoginPacket *pack );
	// for internal use
	static void attachGameServerInfo( GS_ConnectionInfo *pinfo, GameServerInfo *gsi, const wchar_t *internalHost, const wchar_t *externalHost, int port, int maxPlayers );
	static bool sendGSPacket( GS_ConnectionInfo *pinfo, L2LoginPacket *pack );

public:
	static void forceClose( GS_ConnectionInfo *pinfo, GameServerTable::LoginFailReason reason );
	static void send_LoginServerFail( GS_ConnectionInfo *pinfo, GameServerTable::LoginFailReason reason );
	static void send_KickPlayer( GS_ConnectionInfo *pinfo, const wchar_t *account );
	static void send_PlayerAuthResponse( GS_ConnectionInfo *pinfo, const wchar_t *account, bool ok );
};
