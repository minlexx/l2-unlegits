#pragma once

#include "l2c_configFile.h"

class LoginConfig
{
public:
	LoginConfig();
	~LoginConfig();
	bool load();
protected:
	L2C_ConfigFile *cfg_login;
	L2C_ConfigFile *cfg_telnet;
public:
	// network
	wchar_t *ExternalHostname;
	wchar_t *InternalHostname;
	wchar_t *LoginserverHostname;
	int      LoginserverPort;
	wchar_t *LoginHostname;
	int      LoginPort;
	int      IpUpdateTime;
	// security
	bool     LogLoginController;
	int      LoginTryBeforeBan;
	int      LoginBlockAfterBan;
	bool     AcceptNewGameServer;
	bool     ForceGGAuth;
	bool     EnableFloodProtection;
	int      FastConnectionLimit;
	int      NormalConnectionTime;
	int      FastConnectionTime;
	int      MaxConnectionPerIP;
	// db
	wchar_t *Driver;
	wchar_t *URL;
	wchar_t *Login;
	wchar_t *Password;
	int      MaximumDbConnections;
	int      MaximumDbIdleTime;
	// misc
	bool     ShowLicence;
	bool     AutoCreateAccounts;
	bool     Debug;
	bool     Assert;
	bool     Developer;
	// telnet
	bool     EnableTelnet;
	int      StatusPort;
	wchar_t *StatusPW;
	wchar_t *ListOfHosts;
};

