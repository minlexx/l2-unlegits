#include "StdAfx.h"
#include "Account.h"
#include "Logger.h"

Account::Account()
{
	account_version = 2;
	clear();
}

Account::~Account()
{
	clear();
}

void Account::clear()
{
	login[0] = pass[0] = 0;
	loginServerIP[0] = 0;
	charSelectName[0] = configFileName[0] = 0;
	loginServerPort = 2106;
	gameserverID = 1;
	gameserverSelectManual = 0;
	charSelectManual = 0;
	strcpy( loginServerIP, "127.0.0.1" );
	protocolVersion = 12;
	serverVersion = 2;
	useDefaultProtocolVersion = 1;
}

Account::Account( const Account& other )
{
	wcscpy( login, other.login );
	wcscpy( pass, other.pass );
	wcscpy( charSelectName, other.charSelectName );
	wcscpy( configFileName, other.configFileName );
	strcpy( loginServerIP, other.loginServerIP );
	loginServerPort = other.loginServerPort;
	gameserverID = other.gameserverID;
	gameserverSelectManual = other.gameserverSelectManual;
	charSelectManual = other.charSelectManual;
	protocolVersion = other.protocolVersion;
	serverVersion = other.serverVersion;
	useDefaultProtocolVersion = other.useDefaultProtocolVersion;
}

Account& Account::operator=( const Account& other )
{
	wcscpy( login, other.login );
	wcscpy( pass, other.pass );
	wcscpy( charSelectName, other.charSelectName );
	wcscpy( configFileName, other.configFileName );
	strcpy( loginServerIP, other.loginServerIP );
	loginServerPort = other.loginServerPort;
	gameserverID = other.gameserverID;
	gameserverSelectManual = other.gameserverSelectManual;
	charSelectManual = other.charSelectManual;
	protocolVersion = other.protocolVersion;
	serverVersion = other.serverVersion;
	useDefaultProtocolVersion = other.useDefaultProtocolVersion;
	return (*this);
}

L2_VERSION Account::getL2Version() const
{
	L2_VERSION ret = L2_VERSION_T1;
	switch( serverVersion )
	{
	case 0: ret = L2_VERSION_T1; break;
	case 1: ret = L2_VERSION_T15; break;
	case 2: ret = L2_VERSION_T2; break;
	case 3: ret = L2_VERSION_T22; break;
	case 4: ret = L2_VERSION_T23; break;
	case 5: ret = L2_VERSION_T24; break;
	default:
		log_error( LOG_WARNING, "Account::getL2Version(): unknown serverVersion %d\n",
			serverVersion );
		break;
	}
	return ret;
}

bool Account::loadFromFile( const wchar_t *fileName )
{
	HANDLE hFile = CreateFileW( fileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE ) return false;
	DWORD dwRead = 0;
	ReadFile( hFile, (void *)this, sizeof(class Account), &dwRead, NULL );
	CloseHandle( hFile );
	return (dwRead == sizeof(class Account) );
}

bool Account::saveToFile( const wchar_t *fileName )
{
	HANDLE hFile = CreateFileW( fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE ) return false;
	DWORD dwWritten = 0;
	WriteFile( hFile, (const void *)this, sizeof(class Account), &dwWritten, NULL );
	CloseHandle( hFile );
	return (dwWritten == sizeof(class Account));
}
