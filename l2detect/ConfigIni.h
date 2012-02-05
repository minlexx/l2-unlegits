#ifndef CONFIGINI_H_
#define CONFIGINI_H_

class CConfig
{
public:
	CConfig();
	virtual ~CConfig();

public:
	bool ReadConfig( const char *szConfigFileName );
	bool ReadConfig( const wchar_t *szConfigFilename );
	bool SaveConfig();
	void SetDefault();

protected:
	void _initNull();

public:
	char szCfgFileName[256];
	bool isInGameMode;

	int  L2_version;
	int  L2_client_version;

	//   Listen port setup
	char FakeListenLoginIP[32];
	int  FakeListenLoginPort;
	char FakeListenGameIP[32];
	int  FakeListenGamePort;

	//   Forward connection to
	char RealLoginServerIP[128];
	int  RealLoginServerPort;

	//   Catch game server traffic
	//int  PlayGameServerNo; // removed!
	char ForceGameServerIP[32];
	int  ForceGameServerPort;
	int  IngameGameServerPort;

	//   Logging setup
	int  LogGamePackets;
	char LogGameFileNamePrefix[128];
	int  WarnUnknownPacketsToStdout;
	int  DumpUnknownToStdout;
	int  WarnMessageLevel;

	// hacks
	int  EnableModifyGameTraffic; // enable or disable game protocol-level hacks
	int  OverrideGameProtocolVersion;
	int  ReplyL2JGameGuardQuery;
	int  GraciaEpilogueProtocol_148_hacks;
	int  TeonPvP_hacks;

	// L2Walker fixes
	int  L2Walker_DropRequestGMList;
	int  L2Walker_FixMoveBackwardToLocation;
	int  L2Walker_FixChangeWaitType2;
	int  L2Walker_InjectStatusUpdate;

	// tweaks
	int  ThreadProirityRaiseEnable;

	// invis GM detection
	int  InvisGMTrackEnable;
	int  InvisGMSpeed;
};

#endif /* CONFIGINI_H_ */
