#include "pch.h"
#include "Log.h"
#include "LoginConfig.h"

LoginConfig::LoginConfig()
{
	// objects
	cfg_login = cfg_telnet = NULL;
	// network
	ExternalHostname = InternalHostname = LoginserverHostname = LoginHostname = NULL;
	LoginserverPort = LoginPort = IpUpdateTime = 0;
	// security
	LogLoginController = AcceptNewGameServer = ForceGGAuth = EnableFloodProtection = false;
	LoginTryBeforeBan = LoginBlockAfterBan = FastConnectionLimit = NormalConnectionTime = 
		FastConnectionTime = MaxConnectionPerIP = 0;
	// db
	Driver = URL = Login = Password = NULL;
	MaximumDbConnections = MaximumDbIdleTime = 0;
	// misc
	ShowLicence = AutoCreateAccounts = Debug = Assert = Developer = false;
	// telnet
	EnableTelnet = false;
	StatusPort = 0;
	StatusPW = ListOfHosts = NULL;
}

LoginConfig::~LoginConfig()
{
	// cleanup
	if( cfg_login )
	{
		delete cfg_login;
		cfg_login = NULL;
	}
	if( cfg_telnet )
	{
		delete cfg_telnet;
		cfg_telnet = NULL;
	}
}

bool LoginConfig::load()
{
	bool were_errors = false;

	if( cfg_login ) delete cfg_login;
	cfg_login = new L2C_ConfigFile();

	// load main loginserver properties
	if( !cfg_login->load( L".\\config_login\\loginserver.properties" ) )
	{
		LogError( L"Failed to load .\\config_login\\loginserver.properties!" );
		were_errors = true;
	}
	else LogDebug( L".\\config_login\\loginserver.properties: loaded %d vars", cfg_login->getVarsCount() );

	// load telnet properties
	if( cfg_telnet ) delete cfg_telnet;
	cfg_telnet = new L2C_ConfigFile();
	if( !cfg_telnet->load( L".\\config_login\\telnet.properties" ) )
	{
		LogError( L"Failed to load .\\config_login\\telnet.properties!" );
		were_errors = true;
	}
	else LogDebug( L".\\config_login\\telnet.properties: loaded %d vars", cfg_telnet->getVarsCount() );

	// initialize all config
	// network
	ExternalHostname = (wchar_t *)cfg_login->getValueStrW( L"ExternalHostname", L"127.0.0.1" );
	InternalHostname = (wchar_t *)cfg_login->getValueStrW( L"InternalHostname", L"127.0.0.1" );
	LoginserverHostname = (wchar_t *)cfg_login->getValueStrW( L"LoginserverHostname", L"*" );
	LoginserverPort   = cfg_login->getValueInt( L"LoginserverPort", 2106 );
	LoginHostname = (wchar_t *)cfg_login->getValueStrW( L"LoginHostname", L"*" );
	LoginPort   = cfg_login->getValueInt( L"LoginPort", 9014 );
	IpUpdateTime   = cfg_login->getValueInt( L"IpUpdateTime", 15 );
	// security
	LogLoginController = cfg_login->getValueBool( L"LogLoginController", true );
	LoginTryBeforeBan = cfg_login->getValueInt( L"LoginTryBeforeBan", 10 );
	LoginBlockAfterBan = cfg_login->getValueInt( L"LoginBlockAfterBan", 600 );
	AcceptNewGameServer = cfg_login->getValueBool( L"AcceptNewGameServer", false );
	ForceGGAuth = cfg_login->getValueBool( L"ForceGGAuth", true );
	EnableFloodProtection = cfg_login->getValueBool( L"EnableFloodProtection", true );
	FastConnectionLimit = cfg_login->getValueInt( L"FastConnectionLimit", 15 );
	NormalConnectionTime = cfg_login->getValueInt( L"NormalConnectionTime", 700 );
	FastConnectionTime = cfg_login->getValueInt( L"FastConnectionTime", 350 );
	MaxConnectionPerIP = cfg_login->getValueInt( L"MaxConnectionPerIP", 50 );
	// db
	Driver = (wchar_t *)cfg_login->getValueStrW( L"Driver", L"com.mysql.jdbc.Driver" );
	URL = (wchar_t *)cfg_login->getValueStrW( L"URL", L"jdbc:mysql://localhost/l2jdb" );
	Login = (wchar_t *)cfg_login->getValueStrW( L"Login", L"root" );
	Password = (wchar_t *)cfg_login->getValueStrW( L"Password", L"" );
	MaximumDbConnections = cfg_login->getValueInt( L"MaximumDbConnections", 10 );
	MaximumDbIdleTime = cfg_login->getValueInt( L"MaximumDbIdleTime", 0 );
	// misc
	ShowLicence = cfg_login->getValueBool( L"ShowLicence", true );
	AutoCreateAccounts = cfg_login->getValueBool( L"AutoCreateAccounts", false );
	Debug = cfg_login->getValueBool( L"Debug", false );
	Assert = cfg_login->getValueBool( L"Assert", false );
	Developer = cfg_login->getValueBool( L"Developer", false );
	// telnet
	EnableTelnet = cfg_telnet->getValueBool( L"EnableTelnet", false );
	StatusPort   = cfg_telnet->getValueInt( L"StatusPort", 12345 );
	StatusPW     = (wchar_t *)cfg_telnet->getValueStrW( L"StatusPW", NULL );
	EnableTelnet = cfg_telnet->getValueBool( L"EnableTelnet", false );

	return !were_errors;
}
