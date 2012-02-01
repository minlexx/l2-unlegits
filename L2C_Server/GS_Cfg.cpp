#include "pch.h"
#include "Log.h"
#include "GS_Cfg.h"

GameConfig::GameConfig()
{
	m_cfg_mysql = m_cfg_network = m_cfg_rates = m_cfg_hexid = m_cfg_dev = NULL;
	clear();
}

GameConfig::~GameConfig()
{
	clear();
	if( m_cfg_mysql ) { delete m_cfg_mysql; m_cfg_mysql = NULL; }
	if( m_cfg_network ) { delete m_cfg_network; m_cfg_network = NULL; }
	if( m_cfg_rates ) { delete m_cfg_rates; m_cfg_rates = NULL; }
	if( m_cfg_hexid ) { delete m_cfg_hexid; m_cfg_hexid = NULL; }
	if( m_cfg_dev ) { delete m_cfg_dev; m_cfg_dev = NULL; }
}

void GameConfig::clear()
{
	// clear MySQL settings
	mysql_host = mysql_user = mysql_pass = mysql_db = NULL;
	mysql_max_connections = 0;
	// hexid
	server_hexid = NULL;
	server_id = 0;
	accept_alternate_server_id = list_as_test_server = server_list_clock = 
		server_list_brackets = false;
	// network
	login_server_address = NULL;
	login_server_auth_port = login_protocol_version = 0;
	game_server_bind_address = NULL;
	game_server_bind_port = 0;
	report_internal_hostname = report_external_hostname = NULL;
	min_game_protocol_version = max_game_protocol_version = 0;
	max_players = 0;
	// developer settings
	EverybodyHasAdminRights = Debug = Assert = false;
}

bool GameConfig::load()
{
	bool load_ok = true;
	load_ok &= load_mysql();
	load_ok &= load_hexid();
	load_ok &= load_network();
	load_ok &= load_rates();
	load_ok &= load_developer();
	return load_ok;
}

bool GameConfig::load_mysql()
{
	if( m_cfg_mysql ) delete m_cfg_mysql;
	m_cfg_mysql = new L2C_ConfigFile();
	if( !m_cfg_mysql->load( L"./config_game/mysql.conf" ) )
	{
		LogError( L"Cannot open file: ./config_game/mysql.conf!" );
		return false;
	}
	// init
	mysql_host = (wchar_t *)m_cfg_mysql->getValueStrW( L"mysql_host", L"localhost" );
	mysql_user = (wchar_t *)m_cfg_mysql->getValueStrW( L"mysql_user", L"root" );
	mysql_pass = (wchar_t *)m_cfg_mysql->getValueStrW( L"mysql_pass", L"" );
	mysql_db   = (wchar_t *)m_cfg_mysql->getValueStrW( L"mysql_db",   L"l2jdb" );
	mysql_max_connections = m_cfg_mysql->getValueInt( L"mysql_max_connections", 10 );
	//
	return true;
}

bool GameConfig::load_hexid()
{
	if( m_cfg_hexid ) delete m_cfg_hexid;
	m_cfg_hexid = new L2C_ConfigFile();
	if( !m_cfg_hexid->load( L"./config_game/hexid.txt" ) )
	{
		LogError( L"Cannot open file: ./config_game/hexid.txt!" );
		return false;
	}
	// init
	server_hexid = (wchar_t *)m_cfg_hexid->getValueStrW( L"HexID", L"" );
	server_id    = m_cfg_hexid->getValueInt( L"ServerID", 1 );
	//
	return true;
}

bool GameConfig::load_network()
{
	if( m_cfg_network ) delete m_cfg_network;
	m_cfg_network = new L2C_ConfigFile();
	if( !m_cfg_network->load( L"./config_game/network.conf" ) )
	{
		LogError( L"Cannot open file: ./config_game/network.conf!" );
		return false;
	}
	// init
	login_server_address = (wchar_t *)m_cfg_network->getValueStrW( L"login_server_address", L"127.0.0.1" );
	login_server_auth_port = m_cfg_network->getValueInt( L"login_server_auth_port", 9014 );
	login_protocol_version = m_cfg_network->getValueInt( L"login_protocol_version", 258 );
	game_server_bind_address = (wchar_t *)m_cfg_network->getValueStrW( L"game_server_bind_address", L"0.0.0.0" );
	game_server_bind_port = m_cfg_network->getValueInt( L"game_server_bind_port", 7777 );
	report_internal_hostname = (wchar_t *)m_cfg_network->getValueStrW( L"report_internal_hostname", L"127.0.0.1" );
	report_external_hostname = (wchar_t *)m_cfg_network->getValueStrW( L"report_external_hostname", L"127.0.0.1" );
	min_game_protocol_version = m_cfg_network->getValueInt( L"min_game_protocol_version", 12 );
	max_game_protocol_version = m_cfg_network->getValueInt( L"max_game_protocol_version", 17 );
	max_players = m_cfg_network->getValueInt( L"max_players", 10 );
	// misc net
	accept_alternate_server_id = m_cfg_network->getValueBool( L"accept_alternate_server_id", false );
	list_as_test_server = m_cfg_network->getValueBool( L"list_as_test_server", false );
	server_list_clock = m_cfg_network->getValueBool( L"server_list_clock", false );
	server_list_brackets = m_cfg_network->getValueBool( L"server_list_brackets", false );
	//
	return true;
}

bool GameConfig::load_developer()
{
	if( m_cfg_dev ) delete m_cfg_dev;
	m_cfg_dev = new L2C_ConfigFile();
	if( !m_cfg_dev->load( L"./config_game/developer.conf" ) )
	{
		LogError( L"Cannot open file: ./config_game/developer.conf!" );
		return false;
	}
	// init
	EverybodyHasAdminRights = m_cfg_dev->getValueBool( L"EverybodyHasAdminRights", false );
	Assert = m_cfg_dev->getValueBool( L"Assert", false );
	Debug = m_cfg_dev->getValueBool( L"Debug", false );
	//
	return true;
}

bool GameConfig::load_rates()
{
	return true;
}


bool GameConfig::save()
{
	return true;
}
