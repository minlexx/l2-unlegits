#pragma once
#include "l2c_utils.h"

class GameConfig
{
public:
	GameConfig();
	~GameConfig();
public:
	void clear();
	bool load();
	bool save();

public:
	// MySQL settings
	wchar_t *mysql_host;
	wchar_t *mysql_user;
	wchar_t *mysql_pass;
	wchar_t *mysql_db;
	int      mysql_max_connections;
	// Server auth settings
	wchar_t *server_hexid;
	int      server_id;
	bool     accept_alternate_server_id;
	bool     list_as_test_server;
	bool     server_list_clock;
	bool     server_list_brackets;
	// Network settings
	wchar_t *login_server_address;
	int      login_server_auth_port;
	int      login_protocol_version;
	wchar_t *game_server_bind_address;
	int      game_server_bind_port;
	wchar_t *report_internal_hostname;
	wchar_t *report_external_hostname;
	int      min_game_protocol_version;
	int      max_game_protocol_version;
	int      max_players;
	// developer settings
	bool     EverybodyHasAdminRights;
	bool     Debug;
	bool     Assert;

protected:
	L2C_ConfigFile *m_cfg_mysql;
	L2C_ConfigFile *m_cfg_hexid;
	L2C_ConfigFile *m_cfg_network;
	L2C_ConfigFile *m_cfg_dev;
	L2C_ConfigFile *m_cfg_rates;

protected:
	bool load_mysql();
	bool load_hexid();
	bool load_network();
	bool load_developer();
	bool load_rates();
};
