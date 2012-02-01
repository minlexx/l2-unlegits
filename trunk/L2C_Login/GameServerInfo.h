#pragma once

class GameServerInfo
{
public:
	enum ServerStatus
	{
		STATUS_AUTO = 0x00,
		STATUS_GOOD = 0x01,
		STATUS_NORMAL = 0x02,
		STATUS_FULL = 0x03,
		STATUS_DOWN = 0x04,
		STATUS_GM_ONLY = 0x05
	};

public:
	GameServerInfo( int id, const char *hexid );
	~GameServerInfo();

public:
	int           getId() const;
	const char   *getHexId() const;
	bool          isAuthed() const;
	ServerStatus  getStatus() const;
	const char   *getInternalIp() const;
	const char   *getExternalIp() const;
	const char   *getExternalHost() const;
	int           getPort() const;
	bool          isPvP() const;
	bool          isTestServer() const;
	bool          isShowingClock() const;
	bool          isShowingBrackets() const;
	int           getMaxPlayers() const;

public:
	void          setAuthed( bool a );
	void          setGameServerInfo( const char *ip, const wchar_t *internalHost, const wchar_t *externalHost, int port, int maxPlayers );
	void          setStatus( GameServerInfo::ServerStatus st );
	static void   getStatusStr( GameServerInfo::ServerStatus st, std::wstring& ws );
	void          setShowingClock( bool b );
	void          setShowingBrackets( bool b );
	void          setTestServer( bool b );
	void          setMaxPlayers( int m );
	void          setGameServerThread( void *pv_GS_ConnectionInfo );
	void          setDown();

public:
	void          kickPlayer( const char *account );
	bool          stopThread();
	int           getPlayerCount() const;
	bool          hasAccountOnGS( const char *account ) const;
	void          addAccountOnGS( const char *account );
	void          removeAccountFromGS( const char *account );

protected:
	int                   m_Id;
	char                  m_hexId[64];
	bool                  m_isAuthed;
	ServerStatus          m_status;
	char                  m_internalIp[16];
	char                  m_externalIp[16];
	char                  m_externalHost[128];
	int                   m_port;
	bool                  m_isPvp;
	bool                  m_isTestServer;
	bool                  m_isShowingClock;
	bool                  m_isShowingBrackets;
	int                   m_maxPlayers;
	std::set<std::string> m_accountsOnGS;
	void                 *m_pthread;
};
