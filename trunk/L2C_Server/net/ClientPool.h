#pragma once
#include "l2c_utils.h"
#include "net/GameClient/GameClient.h"

class ClientPool
{
public:
	ClientPool();
	~ClientPool();

public:
	int getCurrentOnline();

public:
	bool addClient( SOCKET s_cl, const char *ip, int port );

public:
	void disconnectAllPlayers();
	bool disconnectPlayerByAccount( const wchar_t *accountName, bool reasonDualLogin );
	bool notifyClientThreadEnded( GameClient *ptr );
	void notifyClientAuthResponse( const wchar_t *accountName, bool ok );

public:
	void dumpClientsToLog();

protected:
	CriticalSection         m_lock;
	std::vector<GameClient *> *m_clients;
};
