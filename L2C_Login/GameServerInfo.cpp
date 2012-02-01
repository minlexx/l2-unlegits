#include "pch.h"
#include "GameServerInfo.h"
#include "GameServerTable.h"
#include "GS_ConnectionInfo.h"
#include "l2c_utils.h"

GameServerInfo::GameServerInfo( int id, const char *hexid )
{
	m_Id = id;
	if( hexid ) strcpy( m_hexId, hexid );
	m_isAuthed = false;
	m_status = STATUS_DOWN;
	m_internalIp[0] = 0;
	m_externalIp[0] = 0;
	m_externalHost[0] = 0;
	m_port = 0;
	m_isPvp = false;
	m_isTestServer = false;
	m_isShowingClock = false;
	m_isShowingBrackets = false;
	m_maxPlayers = 0;
	m_pthread = NULL;
	//
	m_accountsOnGS.clear();
}

GameServerInfo::~GameServerInfo()
{
	//
	m_Id = 0;
	m_hexId[0] = 0;
	m_isAuthed = false;
	m_status = STATUS_DOWN;
	m_internalIp[0] = 0;
	m_externalIp[0] = 0;
	m_externalHost[0] = 0;
	m_port = 0;
	m_isPvp = false;
	m_isTestServer = false;
	m_isShowingClock = false;
	m_isShowingBrackets = false;
	m_maxPlayers = 0;
	//
	stopThread();
	m_accountsOnGS.clear();
}

int GameServerInfo::getId() const
{
	return m_Id;
}

const char *GameServerInfo::getHexId() const
{
	return m_hexId;
}

bool GameServerInfo::isAuthed() const
{
	return m_isAuthed;
}

GameServerInfo::ServerStatus GameServerInfo::getStatus() const
{
	return m_status;
}

const char *GameServerInfo::getInternalIp() const
{
	return m_internalIp;
}

const char *GameServerInfo::getExternalIp() const
{
	return m_externalIp;
}

const char *GameServerInfo::getExternalHost() const
{
	return m_externalHost;
}

int GameServerInfo::getPort() const
{
	return m_port;
}

bool GameServerInfo::isPvP() const
{
	return m_isPvp;
}

bool GameServerInfo::isTestServer() const
{
	return m_isTestServer;
}

bool GameServerInfo::isShowingClock() const
{
	return m_isShowingClock;
}

bool GameServerInfo::isShowingBrackets() const
{
	return m_isShowingBrackets;
}

int GameServerInfo::getMaxPlayers() const
{
	return m_maxPlayers;
}

void GameServerInfo::setAuthed( bool a )
{
	m_isAuthed = a;
}

void GameServerInfo::setGameServerInfo( const char *ip, const wchar_t *internalHost, const wchar_t *externalHost, int port, int maxPlayers )
{
	// TODO: better address conversion
	l2c_unicode_to_ansi( internalHost, m_internalIp, sizeof(m_internalIp) );
	l2c_unicode_to_ansi( externalHost, m_externalIp, sizeof(m_externalIp) );
	if( m_internalIp[0] == '*' ) strcpy( m_internalIp, ip );
	if( m_externalIp[0] == '*' ) strcpy( m_externalIp, ip );
	m_port = port;
	m_maxPlayers = maxPlayers;
}

void GameServerInfo::setStatus( GameServerInfo::ServerStatus st )
{
	m_status = st;
}

void GameServerInfo::getStatusStr( GameServerInfo::ServerStatus st, std::wstring& ws )
{
	ws = L"STATUS_UNKNOWN";
	switch( st )
	{
	case STATUS_AUTO: ws = L"STATUS_AUTO"; break;
	case STATUS_GOOD: ws = L"STATUS_GOOD"; break;
	case STATUS_NORMAL: ws = L"STATUS_NORMAL"; break;
	case STATUS_FULL: ws = L"STATUS_FULL"; break;
	case STATUS_DOWN: ws = L"STATUS_DOWN"; break;
	case STATUS_GM_ONLY: ws = L"STATUS_GM_ONLY"; break;
	}
}

void GameServerInfo::setShowingClock( bool b )
{
	m_isShowingClock = b;
}

void GameServerInfo::setShowingBrackets( bool b )
{
	m_isShowingBrackets = b;
}

void GameServerInfo::setTestServer( bool b )
{
	m_isTestServer = b;
}

void GameServerInfo::setMaxPlayers( int m )
{
	m_maxPlayers = m;
}

void GameServerInfo::setGameServerThread( void *pv_GS_ConnectionInfo )
{
	m_pthread = pv_GS_ConnectionInfo;
}

void GameServerInfo::setDown()
{
	m_isAuthed = false;
	m_status = STATUS_DOWN;
	m_port = 0;
	m_pthread = NULL;
	// not change IPs and hosts?
}

////////////////////////////////////////////////////////////////

bool GameServerInfo::stopThread()
{
	if( m_pthread )
	{
		GS_ConnectionInfo *pinfo = (GS_ConnectionInfo *)m_pthread;
		//
		pinfo->flagBreak = true;
		Sleep( 100 ); // O_o xDDD
		//
		m_pthread = NULL;
	}
	return true;
}

void GameServerInfo::kickPlayer( const char *account )
{
	if( !account ) return;
	if( m_pthread )
	{
		GS_ConnectionInfo *pinfo = (GS_ConnectionInfo *)m_pthread;
		wchar_t waccount[256];
		memset( waccount, 0, sizeof(waccount) );
		l2c_ansi_to_unicode( account, waccount, 256 );
		GameServerTable::send_KickPlayer( pinfo, waccount );
	}
}


int GameServerInfo::getPlayerCount() const
{
	return m_accountsOnGS.size();
}

bool GameServerInfo::hasAccountOnGS( const char *account ) const
{
	if( !account ) return false;
	std::string a = account;
	std::set<std::string>::const_iterator iter = m_accountsOnGS.find( a );
	if( iter == m_accountsOnGS.end() ) return false;
	return true;
}

void GameServerInfo::addAccountOnGS( const char *account )
{
	if( !account ) return;
	std::string a = account;
	m_accountsOnGS.insert( a );
}

void GameServerInfo::removeAccountFromGS( const char *account )
{
	if( !account ) return;
	std::string a = account;
	std::set<std::string>::iterator iter = m_accountsOnGS.find( a );
	if( iter == m_accountsOnGS.end() ) return;
	m_accountsOnGS.erase( iter );
}

