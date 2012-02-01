#pragma once

#define BOTIPC_MUTEX_NAME   TEXT("L2Detect_BotIPC")
#define BOTIPC_MAPPING_NAME TEXT("L2Detect_BotIPC_Mapping")

#define BOT_IPC_MAX_BOTS 9

/* Describes one bot client */
struct BOT_INFO
{
	// process info
	unsigned int processId; // Bot program process Id
	// character info
	int          isMaster;            // true, if this bot is master
	int          isSlave;             // slaves follow master commands
	wchar_t      charName[128];       // character name
	unsigned int classId;             // character class
	int          level;               // char level
	unsigned int charOid;             // 0 - unused
	// follow master
	int          followEnable;        // true, if this slave must follow master
	unsigned int oidFollow;           // object Id of master to follow
	// assist master
	int          assistEnable;        // true, if this slave must assist master attacks
	// follow npc chat
	int          followNpcChatEnable; // true, if this slave must follow master npc chat
	unsigned int npcChatOid;          // NPC objectId
	wchar_t      npcChatStr[128];     // chat string to select (not bypass)
};

/* Controls all bots processes */
struct BOT_IPC_DESCRIPTOR
{
	DWORD    dwSize;                  // structure size
	UINT     uBotCount;               // current bot count
	DWORD    dwCreatorPid;            // BotIPC creator processId
	BOT_INFO bot[BOT_IPC_MAX_BOTS];   // array of bots
};

// Initializes one bot description structure
// all other members are initialized to zeroes, false, etc
void BotInfo_Initialize( BOT_INFO *b, const wchar_t *charName, unsigned int charOid, unsigned int classId, int level );


class BotIPC
{
protected:
	BotIPC();
	~BotIPC();
public:
	static BotIPC      *getInstance();
	static void         freeInstance();

public:
	BOT_IPC_DESCRIPTOR *getDescriptor();
	bool                amICreator() const;

	int                 addBotInfo( const BOT_INFO *b );
	bool                getBotInfo( int index, BOT_INFO *b );
	bool                setBotInfo( int index, const BOT_INFO *b );
	bool                delBot( int index );
	void                delAllMyProcessBots();

public:
	bool                Lock();
	void                LockRelease();

public:
	void                dumpToLog();

protected:
	static int     s_refCount;
	static BotIPC *s_instance;
protected:
	HANDLE         hMutex;
	HANDLE         hMapping;
	void          *map;
	bool           iAmCreator;
	bool           isLocked;
};
