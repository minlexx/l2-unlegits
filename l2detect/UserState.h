#ifndef H_USER_STATE
#define H_USER_STATE

class UserState
{
public:
	UserState();
	~UserState();
	void clear();
	TCHAR *getClassStr();
	TCHAR *getBaseClassStr();
	TCHAR *getRaceStr();
public:
	// vars
	unsigned int objectID;
	wchar_t charName[128];
	wchar_t charTitle[128];
	int level;
	unsigned int sp;
	unsigned long long int exp;
	unsigned int clanID;
	unsigned int allyID;
	unsigned int relation;
	// hp, mp, cp
	int hp; int hp_max;
	int mp; int mp_max;
	int cp; int cp_max;
	// load weight
	int curLoad; int maxLoad;
	// some stats
	int pAtk; int pDef; int mAtk; int mDef;
	int pAtkSpd; int mAtkSpd;
	int evasion; int accuracy; int critical;
	// coords
	int x; int y; int z;
	unsigned int heading;
	int xDst; int yDst; int zDst;
	int run_speed; int walk_speed;
	int isRunning; int isSitting;
	int isDead;
	// race, sex
	int race; int sex;
	unsigned int baseClass;
	unsigned int classID;
	// pvp
	int pvpflag; int karma;
	int pvpkills; int pkkills;
	unsigned int abnormalEffect;
	// target
	unsigned int targetObjectID;
	int targetMaxHP;
	int targetCurHP;
	// vehicles
	unsigned int vehicleID;
	// paperdoll
	unsigned int paperdoll_oid[32]; ///< array of paperdoll objectIDs (only from UserInfo)
	unsigned int paperdoll_iid[32]; ///< array of paperdoll itemIDs
	unsigned int paperdoll_augid[32]; ///< array of paperdoll augmentationIDs
	// timings
	unsigned int lastTick;
public:
	// parsers
	void parse_UserInfo( void *l2_game_packet );
	void parse_StatusUpdate( void *l2_game_packet );
	bool parse_MoveToLocation( void *l2_game_packet );
public:
	void processMoveTick();

protected:
	//utils
	wchar_t unicode_str[128];
};

#endif
