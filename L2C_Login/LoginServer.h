#pragma once

#include "LoginConfig.h"
#include "l2c_db.h"
#include "LoginController.h"
#include "GameServerTable.h"

class LoginServer
{
public:
	static const unsigned int LOGIN_PROTOCOL_REV = 0x102;
public:
	LoginServer();
	~LoginServer();

public:
	bool start();
	bool stop();

public:
	MysqlConnection *getDBConnection();
	bool             releaseDBConnection( MysqlConnection *con );

public:
	LoginController *getLoginController();
	const LoginConfig *getConfig() const;
	GameServerTable *getGameServerTable();

public:
	void LogFile( const char *fn, const char *_Format, ... );

protected:
	LoginConfig            *m_config;
	MysqlConnectionManager *m_mysql;
	wchar_t                 m_mysql_server[128];
	wchar_t                 m_mysql_db[128];
	LoginController        *m_loginController;
	GameServerTable        *m_gameServerTable;

protected:
	bool   m_isRunning;
	HANDLE m_hMainThread;
	DWORD  m_dwMainThreadId;
	bool   m_mainThreadShouldStop;
	static unsigned int __stdcall MainThread( LoginServer *pcls );
	bool checkIsBannedIp( unsigned int sock, const char *ip, int port );
	void resourceCleanup();
};
