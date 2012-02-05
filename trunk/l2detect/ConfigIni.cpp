#include "stdafx.h"
#include "ConfigIni.h"

CConfig::CConfig()
{
	_initNull();
}

CConfig::~CConfig()
{
	_initNull();
}

void CConfig::_initNull()
{
	isInGameMode = false;
	L2_version = 0;
	L2_client_version = 0;
	//
	szCfgFileName[0] = FakeListenLoginIP[0] = FakeListenGameIP[0];
	FakeListenLoginPort = FakeListenGamePort = 0;
	//   Forward connection to
	RealLoginServerIP[0] = 0;
	RealLoginServerPort = 0;
	//   Catch game server traffic
	//PlayGameServerNo = 0;
	ForceGameServerIP[0] = 0;
	ForceGameServerPort = 0;
	IngameGameServerPort = 7777;
	//   Logging setup
	LogGamePackets = 0;
	LogGameFileNamePrefix[0] = 0;
	WarnUnknownPacketsToStdout = DumpUnknownToStdout = WarnMessageLevel = 0;
	// hacks
	EnableModifyGameTraffic = 0; // enable or disable game protocol-level hacks
	OverrideGameProtocolVersion = 0;
	ReplyL2JGameGuardQuery = 0;
	GraciaEpilogueProtocol_148_hacks = 0;
	TeonPvP_hacks = 0;
	// L2Walker fixes
	L2Walker_DropRequestGMList = 0;
	L2Walker_FixMoveBackwardToLocation = 1;
	L2Walker_FixChangeWaitType2 = 1;
	L2Walker_InjectStatusUpdate = 0;
	// Invis GM
	InvisGMTrackEnable = 1;
	InvisGMSpeed = 400; // //gmspeed 4
}

bool CConfig::ReadConfig( const wchar_t *szConfigFileName )
{
	if( !szConfigFileName ) return false;
	char tempbuf[512] = {0};
	WideCharToMultiByte( CP_ACP, 0, szConfigFileName, -1, tempbuf, 511, NULL, NULL );
	return this->ReadConfig( tempbuf );
}

bool CConfig::ReadConfig( const char *szConfigFileName )
{
	if( !szConfigFileName ) return false;
	strcpy( this->szCfgFileName, szConfigFileName );
	FILE *f = fopen( szConfigFileName, "rt" );
	if( !f ) return false;

	char    line[512];
	size_t  ll; // line length
	char   *token = NULL;

	while( !feof(f) )
	{
		memset( line, 0, sizeof(line) );
		fgets( line, sizeof(line)-1, f );
		if( line[0] == 0 ) /* line empty */ continue;
		if( (line[0] == '#') || (line[0] == ';') ) /* pass comments */ continue;
		/* remove NL & BR from end of line */
		ll = strlen( line );
		if( (line[ll-1] == '\r') || (line[ll-1] == '\n') ) line[ll-1] = 0;
		if( (line[ll-2] == '\r') || (line[ll-2] == '\n') ) line[ll-2] = 0;
		/* find '=' */
		token = strchr( line, '=' );
		if( token )
		{
			token++; // token points to next char after '='
			while( (*token) && ((*token) == ' ') ) token++; // pass spaces
		}
		else
			continue; /* no '=' symbol in line */
		/* is there any string in token */
		if( token[0] == 0 ) continue; /* token is an empty string.. */
		/* find match */
		if( strstr( line, "Lineage2Version" ) == line )
		{
			sscanf( token, "%d", &(this->L2_version) );
		}
		if( strstr( line, "Lineage2ClientVersion" ) == line )
		{
			sscanf( token, "%d", &(this->L2_client_version) );
		}
		if( strstr( line, "FakeListenLoginIP" ) == line )
		{
			strcpy( this->FakeListenLoginIP, token );
		}
		if( strstr( line, "FakeListenLoginPort" ) == line )
		{
			sscanf( token, "%d", &(this->FakeListenLoginPort) );
		}
		if( strstr( line, "FakeListenGameIP" ) == line )
		{
			strcpy( this->FakeListenGameIP, token );
		}
		if( strstr( line, "FakeListenGamePort" ) == line )
		{
			sscanf( token, "%d", &(this->FakeListenGamePort) );
		}
		if( strstr( line, "RealLoginServerIP" ) == line )
		{
			strcpy( this->RealLoginServerIP, token );
		}
		if( strstr( line, "RealLoginServerPort" ) == line )
		{
			sscanf( token, "%d", &(this->RealLoginServerPort) );
		}
		/*if( strstr( line, "PlayGameServerNo" ) == line )
		{
			sscanf( token, "%d", &(this->PlayGameServerNo) );
		}*/
		if( strstr( line, "ForceGameServerIP" ) == line )
		{
			strcpy( this->ForceGameServerIP, token );
		}
		if( strstr( line, "ForceGameServerPort" ) == line )
		{
			sscanf( token, "%d", &(this->ForceGameServerPort) );
		}
		if( strstr( line, "IngameGameServerPort" ) == line )
		{
			sscanf( token, "%d", &(this->IngameGameServerPort) );
		}
		if( strstr( line, "LogGamePackets" ) == line )
		{
			sscanf( token, "%d", &(this->LogGamePackets) );
		}
		if( strstr( line, "LogGameFileNamePrefix" ) == line )
		{
			strcpy( this->LogGameFileNamePrefix, token );
		}
		if( strstr( line, "WarnUnknownPacketsToStdout" ) == line )
		{
			sscanf( token, "%d", &(this->WarnUnknownPacketsToStdout) );
		}
		if( strstr( line, "DumpUnknownToStdout" ) == line )
		{
			sscanf( token, "%d", &(this->DumpUnknownToStdout) );
		}
		if( strstr( line, "WarnMessageLevel" ) == line )
		{
			sscanf( token, "%d", &(this->WarnMessageLevel) );
			if( WarnMessageLevel < 0 ) WarnMessageLevel = 0;
		}
		if( strstr( line, "EnableModifyGameTraffic" ) == line )
		{
			sscanf( token, "%d", &(this->EnableModifyGameTraffic) );
		}
		if( strstr( line, "OverrideGameProtocolVersion" ) == line )
		{
			sscanf( token, "%d", &(this->OverrideGameProtocolVersion) );
		}
		if( strstr( line, "ReplyL2JGameGuardQuery" ) == line )
		{
			sscanf( token, "%d", &(this->ReplyL2JGameGuardQuery) );
		}
		if( strstr( line, "GraciaEpilogueProtocol_148_hacks" ) == line )
		{
			sscanf( token, "%d", &(this->GraciaEpilogueProtocol_148_hacks) );
		}
		if( strstr( line, "TeonPvP_hacks" ) == line )
		{
			sscanf( token, "%d", &(this->TeonPvP_hacks) );
		}
		if( strstr( line, "ThreadProirityRaiseEnable" ) == line )
		{
			sscanf( token, "%d", &(this->ThreadProirityRaiseEnable) );
		}
		if( strstr( line, "L2Walker_DropRequestGMList" ) == line )
		{
			sscanf( token, "%d", &(this->L2Walker_DropRequestGMList) );
		}
		if( strstr( line, "L2Walker_FixMoveBackwardToLocation" ) == line )
		{
			sscanf( token, "%d", &(this->L2Walker_FixMoveBackwardToLocation) );
		}
		if( strstr( line, "L2Walker_FixChangeWaitType2" ) == line )
		{
			sscanf( token, "%d", &(this->L2Walker_FixChangeWaitType2) );
		}
		if( strstr( line, "L2Walker_InjectStatusUpdate" ) == line )
		{
			sscanf( token, "%d", &(this->L2Walker_InjectStatusUpdate) );
		}
		if( strstr( line, "InvisGMTrackEnable" ) == line )
		{
			sscanf( token, "%d", &(this->InvisGMTrackEnable) );
		}
		if( strstr( line, "InvisGMSpeed" ) == line )
		{
			sscanf( token, "%d", &(this->InvisGMSpeed) );
		}
	}

	fclose( f );

	return true;
}

bool CConfig::SaveConfig()
{
	if( szCfgFileName[0] == 0 ) return false;
	FILE *f = fopen( szCfgFileName, "wt" );
	if( !f ) return false;

	// declared in L2PcodeObfuscator.h (include L2Packets.h)
	//#define L2_VERSION_T1   0
	//#define L2_VERSION_T15  1
	//#define L2_VERSION_T2   2
	//#define L2_VERSION_T22  3
	fprintf( f, ""
"# First - configure Lineage2 game protocol version and L2 Client version.\n"
"# Supported versions are:\n"
"#  * Chaotic Throne 1   - The Kamael       = 0\n"
"#  * Chaotic Throne 1.5 - Hellbound        = 1\n"
"#  * Chaotic Throne 2   - Gracia (Part 1)  = 2\n"
"#  * Chaotic Throne 2.2 - Gracia Part 2    = 3\n"
"#  * Chaotic Throne 2.3 - Gracia Final     = 4\n"
"#  * Chaotic Throne 2.4 - Gracia Epilogue  = 5\n"
"#  * 2nd Throne         - Freya            = 6\n"
"#  * 2nd Throne         - High Five        = 7\n"
"# default is 6 (CT2.5 Freya)\n"
"Lineage2Version = %d\n"
"Lineage2ClientVersion = %d\n"
"\n\n",
	L2_version, L2_client_version );

	fprintf( f, ""
"# ===============\n"
"# Network setup  \n"
"# ===============\n"
"\n"
"# =======================\n"
"# Listen ports setup     \n"
"# * Ports on which program will bind listen sockets - local address\n"
"FakeListenLoginIP   = %s\n"
"FakeListenLoginPort = %d\n"
"FakeListenGameIP    = %s\n"
"FakeListenGamePort  = %d\n"
"\n", FakeListenLoginIP, FakeListenLoginPort, FakeListenGameIP, FakeListenGamePort );

	fprintf( f, ""
"# ========================\n"
"# Forward connection to   \n"
"# * Where to redirect login connection - IP/hostname & port\n"
"RealLoginServerIP   = %s\n"
"RealLoginServerPort = %d\n"
"\n", RealLoginServerIP, RealLoginServerPort );

	// this was removed
	/*fprintf( f, ""
"# ===========================\n"
"# Catch game server traffic  \n"
"# * Which Game server's IP and port will be replaced in ServerList packet\n"
"# WARNING: this option is ignored!\n"
"PlayGameServerNo    = %d\n"
"\n", PlayGameServerNo );*/

	fprintf( f, "# You may force radar to redirect connection to specified game server IP:port.\n" );
	fprintf( f, "# To do this, you must set BOTH ForceGameServerIP and ForceGameServerPort.\n" );
	fprintf( f, "# if ForceGameServerIP == \"\" or ForceGameServerPort == 0 then feature is diabled.\n" );
	fprintf( f, ""
"# ===========================\n"
"# Force choose game server by IP:port  \n"
"# * independently of selection in server list window during login\n"
"# * if not set (empty string) - use selected server\n"
"ForceGameServerIP    = %s\n"
"\n", ForceGameServerIP );
	fprintf( f, ""
"# ===========================\n"
"# Force choose game server by IP:port  \n"
"# * independently of selection in server list window during login\n"
"# * if = 0 - use selected server\n"
"ForceGameServerPort    = %d\n"
"\n", ForceGameServerPort );

	fprintf( f, ""
"# ===========================\n"
"# Catch following game traffic port in INGAME mode  \n"
"IngameGameServerPort    = %d\n"
"\n", IngameGameServerPort );


	fprintf( f, ""
"# =========================================================================\n"
"#        Logging Setup                                                     \n"
"# =========================================================================\n"
"# =========================================================================\n"
"# Warn messages level printed to stdout                                    \n"
"#   0 - no messages to screen                                              \n"
"#   1 - errors only ( Recommended :) )                                     \n"
"#   2 - errors, warnings                                                   \n"
"#   3 - errors, warnings, AI                                               \n"
"#   4 - errors, warnings, AI, packet names                                 \n"
"#   5 - errors, warnings, AI, packet names, debug messages                 \n"
"#   6 - errors, warnings, AI, packet names, debug messages, packet dumps   \n"
"# Default: 1;  it cannot be <0, but can be very big number. [0..0xFFFFFFFF]\n"
"WarnMessageLevel = %d\n"
"\n", WarnMessageLevel );

	fprintf( f, ""
"# =========================================================================\n"
"# Warn about unknown packets? 1 - yes, 0 - no; (default: 0, no)            \n"
"WarnUnknownPacketsToStdout = %d\n"
"\n", WarnUnknownPacketsToStdout );

	fprintf( f, ""
"# ==========================================================================\n"
"# Full dump unknown packets to log? 0-no, 1-yes (Default: 0, not display)\n"
"DumpUnknownToStdout   = %d\n"
"\n", DumpUnknownToStdout );

	fprintf( f, ""
"# ====================================\n"
"# Enable or disable logging of Game Server packets 0-disable, 1-enable (Default: 0, disable)\n"
"LogGamePackets        = %d\n"
"\n", LogGamePackets );

	fprintf( f, ""
"# ====================================\n"
"# File name prefix for log file with game server packets log\n"
"# (file name will look like prefix_XXXXXXXX.txt, where XXX-current date/time unix timestamp)\n"
"# Default: prefix_\n"
"LogGameFileNamePrefix = %s\n"
"\n", LogGameFileNamePrefix );

	fprintf( f, ""
"# ==========\n"
"# Hacks!!!! \n"
"# ==========\n"
"            \n"
"# ========================================\n"
"# Enable or disable game packets modifying\n"
"# - If disabled (0), only passive sniffing if possible, NO ANY HACKS will work\n"
"# - If enabled (1), packet-level hacks WILL work. This setting is REQUIRED to be set to ENABLED\n"
"#   if you want any hacks to work\n"
"# - 0-disable, 1-enable; (Default: 0, disable, passive SNIFFING ONLY)\n"
"EnableModifyGameTraffic = %d\n"
"\n", EnableModifyGameTraffic );

	fprintf( f, ""
"# !!!!! Any hacks below will not work, if EnableModifyGameTraffic is set to 0 !!!!!\n"
"\n"
"# =====================================================================\n"
"# * Override game protocol version: change game protocol version number\n"
"# *  in C->S ProtocolVersion packet to given number.\n"
"# * Value: 0 - disable this; any other number greater than 0: override to this number\n"
"# * Default: 0\n"
"OverrideGameProtocolVersion = %d\n"
"# Help on protocol versions (Official server):\n"
"#  - T1   Kamael - 828\n"
"#  - T1.5 Hellbound - 831 ?\n"
"#  - T2   Gracia live - 851\n"
"#  - T2.2 Gracia Part 2 - 12-17\n"
"#  - T2.3 Gracia Final - 83-87\n"
"#  - T2.4 Gracia Epilogue - 146\n"
"\n",
	OverrideGameProtocolVersion );

	fprintf( f, ""
"# If set to 1, L2Detect will send reply to standard L2J GameGuard query\n"
"# by itself, so it will be possible to play with GameGuard disabled\n"
"# on private servers where GameGuard is required.\n"
"# NOTE: L2Detect will NOT reply to unknown queries! You cannot be sure in it on official servers.\n"
"# (Default: disabled)\n"
"ReplyL2JGameGuardQuery = %d\n"
"\n",
	ReplyL2JGameGuardQuery );

	fprintf( f, ""
"# If set to 1, you know that server is using G.Epilogue protocol 148 and client\n"
"# is using G.Epilogue protocol 146; this setting will enable protocol conversion\n"
"# (converting some packets like ItemList, etc) between server and client, allowing\n"
"# clent with protocol 146 to play on server with protocol 148.\n"
"GraciaEpilogueProtocol_148_hacks = %d\n"
"\n",
	GraciaEpilogueProtocol_148_hacks );

	fprintf( f, ""
"# Enables server specific protocol handlers for TeonPvP server.\n"
"TeonPvP_hacks = %d\n"
"\n",
	TeonPvP_hacks );

	fprintf( f, ""
"# Fixes L2Walker wrong or old packets\n"
"L2Walker_DropRequestGMList = %d\n"
"L2Walker_FixMoveBackwardToLocation = %d\n"
"L2Walker_FixChangeWaitType2 = %d\n"
"L2Walker_InjectStatusUpdate = %d\n"
"\n",
	L2Walker_DropRequestGMList,
	L2Walker_FixMoveBackwardToLocation,
	L2Walker_FixChangeWaitType2,
	L2Walker_InjectStatusUpdate );

	fprintf( f, ""
"# Enable/disable game listener thread priority above normal. Try changing this, if you have comp. lags...\n"
"# default: 0, disabled\n"
"ThreadProirityRaiseEnable = %d\n"
"\n",
	ThreadProirityRaiseEnable );


	fprintf( f, ""
"# Enable/disable invisible GM tracking\n"
"# default: 1, enabled\n"
"InvisGMTrackEnable = %d\n"
"\n",
	InvisGMTrackEnable );

	fprintf( f, ""
"# Invisible GM display speed\n"
"# default: 400, as //gmspeed 4\n"
"InvisGMSpeed = %d\n"
"\n",
	InvisGMSpeed );
	

	fclose( f );

	return true;
}

void CConfig::SetDefault()
{
	// declared in L2_versions.h (include L2Packets.h)
	L2_version = 7;
	L2_client_version = 7;

	//   Listen port setup
	strcpy( FakeListenLoginIP, "127.0.0.1" );
	FakeListenLoginPort = 9998;
	strcpy( FakeListenGameIP, "127.0.0.1" );
	FakeListenGamePort = 9999;

	//   Forward connection to
	strcpy( RealLoginServerIP, "0.0.0.0" );
	RealLoginServerPort = 2106;

	//   Catch game server traffic
	//PlayGameServerNo = 0;
	ForceGameServerIP[0] = 0;
	ForceGameServerPort = 0;
	IngameGameServerPort = 7777;

	//   Logging setup
	LogGamePackets = 0;
	LogGameFileNamePrefix[0] = 0;
	WarnUnknownPacketsToStdout = 0;
	DumpUnknownToStdout = 0;
	WarnMessageLevel = 4;

	// hacks
	EnableModifyGameTraffic = 0; // enable or disable game protocol-level hacks
	OverrideGameProtocolVersion = 0;
	ReplyL2JGameGuardQuery = 0;
	GraciaEpilogueProtocol_148_hacks = 0;
	TeonPvP_hacks = 0;

	// L2Walker fixes
	L2Walker_DropRequestGMList = 0;
	L2Walker_FixMoveBackwardToLocation = 1;
	L2Walker_FixChangeWaitType2 = 1;
	L2Walker_InjectStatusUpdate = 0;

	// tweaks
	ThreadProirityRaiseEnable = 1;

	// Invis GM
	InvisGMTrackEnable = 0;
	InvisGMSpeed = 400; // //gmspeed 4
}
