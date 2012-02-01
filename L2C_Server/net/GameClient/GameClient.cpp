#include "pch.h"
#include "l2c_utils.h"
#include "GameClient.h"

GameClient::GameClient( SOCKET s_cl, const char *ip, int port )
{
	setUnused();
	if( ip )
	{
		strncpy( m_ip, ip, 16 ); m_ip[16] = 0;
		l2c_ansi_to_unicode( m_ip, m_wip, 16 );
	}
	m_port = port;
	m_sock = s_cl;
	m_was_force_disconnected = false;
	m_flagStop = m_isRunning = false;
	swprintf( m_tostring, 128, L"Client [IP: %s:%d]", m_wip, m_port );
	m_ph = new ClientPacketHandler( this );
}

GameClient::~GameClient()
{
	if( m_ph )
	{
		delete m_ph;
		m_ph = NULL;
	}
	if( m_obf )
	{
		delete m_obf;
		m_obf = NULL;
	}
	if( m_player )
	{
		delete m_player; // really? some notification?
		m_player = NULL;
	}
	setUnused();
}

void GameClient::setUnused()
{
	// zero our members
	m_account[0] = 0;
	memset( m_playKey, 0, sizeof(m_playKey) );
	memset( m_loginKey, 0, sizeof(m_loginKey) );
	m_ip[0] = 0;
	m_wip[0] = 0;
	m_port = 0;
	m_sock = INVALID_SOCKET;
	m_tostring[0] = 0;
	m_state = CLIENT_STATE_OFFLINE;
	m_xor_enabled = false;
	memset( m_xor_key_recv, 0, sizeof(m_xor_key_recv) );
	memset( m_xor_key_send, 0, sizeof(m_xor_key_send) );
	m_protocolIsOk = false;
	m_netStats.clear();
	m_opcodeObfuscationSeed = 0;
	m_obf = NULL;
	m_player = NULL;
}

const wchar_t  *GameClient::getAccount() const { return (const wchar_t *)m_account; }
void            GameClient::getPlayKey( unsigned char *buffer ) const { memcpy( buffer, m_playKey, 8 ); }
void            GameClient::getLoginKey( unsigned char *buffer ) const { memcpy( buffer, m_loginKey, 8 ); }
const char     *GameClient::getIpA() const { return (const char *)m_ip; }
const wchar_t  *GameClient::getIpW() const { return (const wchar_t *)m_wip; }
int             GameClient::getPort() const { return m_port; }
const wchar_t  *GameClient::toString() const { return (const wchar_t *)m_tostring; }
ClientState     GameClient::getState() const { return m_state; }
bool            GameClient::isProtocolOk() const { return m_protocolIsOk; }
const GameClientNetStats *GameClient::getNetStats() const { return &m_netStats; }
unsigned int    GameClient::getOpcodeObfuscationSeed() const { return m_opcodeObfuscationSeed; }
L2PCodeObfuscator *GameClient::getOpcodeObfuscator() { return m_obf; }
GamePlayer     *GameClient::getPlayer() { return m_player; }

void GameClient::setAccount( const wchar_t *acc )
{
	if( acc )
	{
		wcsncpy( m_account, acc, sizeof(m_account)/sizeof(wchar_t) );
		m_account[sizeof(m_account)/sizeof(wchar_t) - 1] = 0;
	}
}

void GameClient::setSessionKeys( const unsigned char *playKey, const unsigned char *loginKey )
{
	memcpy( m_playKey, playKey, 8 );
	memcpy( m_loginKey, loginKey, 8 );
}

void GameClient::setProtocolOk( bool b ) { m_protocolIsOk = b; }

void GameClient::setOpcodeObfuscationSeed( unsigned int s )
{
	m_opcodeObfuscationSeed = s;
	if( m_obf )
	{
		delete m_obf;
		m_obf = NULL;
	}
	if( s != 0 )
	{
		m_obf = new L2PCodeObfuscator();
		m_obf->init_tables( m_opcodeObfuscationSeed );
	}
}

void GameClient::setPlayer( GamePlayer *newPlayer, bool changeState, ClientState newState )
{
	if( m_player ) delete m_player;
	m_player = NULL;
	if( newPlayer ) m_player = newPlayer;
	if( changeState ) m_state = newState;
}
