#pragma once
#include "l2c_utils.h"
#include "enums.h"
#include "LS_Connection.h"
#include "GS_Cfg.h"
#include "net/ClientPool.h"
#include "utils/IdFactory.h"

class GameServer
{
protected:
	GameServer(); // cannot be created - only by getInstance()
	~GameServer(); // canot be deleted - only by freeInstnce()
public:
	static GameServer *getInstance();
	static void freeInstance();
protected:
	static GameServer *_self;
	static int _referenceCount;

public:
	bool start();
	bool stop();
	bool isRunning() const;
public:
	GameConfig      *getConfig() const;
	MysqlConnection *getDBConnection();
	bool             releaseDBConnection( MysqlConnection *con );
	void             LogFile( const char *fn, const char *_Format, ... );
	const wchar_t   *getServerName() const;
	int              getServerId() const;
	ServerStatus     getServerStatus();
	int              getMaxPlayers() const;
	int              getCurrentOnline();
	ClientPool      *getClientPool() const;
	IdFactory       *getIdFactory();

public:
	bool             kickPlayerByAccount( const wchar_t *accountName, bool reasonDualLogin );
	bool             logoutAccountFromLoginServer( const wchar_t *accountName );
	void             addWaitingClientAndSendPlayerAuthRequest( const wchar_t *accountName, unsigned char *loginKey, unsigned char *playKey );
	void             notifyLoginPlayerAuth( const wchar_t *account, int ok );

protected:
	CriticalSection         m_gsLock;
	GameConfig             *m_cfg;
	MysqlConnectionManager *m_mysql;
	LoginConnection        *m_loginConnection;
	ServerStatus            m_serverStatus;
	ClientPool             *m_clientPool;
	IdFactory              *m_idf;

protected:
	bool m_isRunning;
	bool m_flagStop;
	static DWORD WINAPI GS_Thread( LPVOID lpvParam );
	static bool checkFreePort( const wchar_t *bind_addr, int bind_port );
};
