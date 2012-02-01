#ifndef IL2CLIENT
#define IL2CLIENT

// bot settings
#include "BotConfig.h"
// user state
#include "UserInfo_c.h"
#include "UserStorageMaxCount.h"
#include "UserInventory.h"
#include "UserSkills.h"
#include "UserEtcStatus.h"
#include "UserBuffs.h"
#include "UserParty.h"
// world state
#include "WorldObjectTree.h"
#include "CharArray.h"
#include "NpcArray.h"
#include "GIArray.h"
#include "ClanList.h"
// l2client window
#include "MapShowParams.h"

class IL2Client
{
public:
	IL2Client( void *pvL2Client );
	~IL2Client();
protected: // default constructor, copy constructor and operator= are closed
	IL2Client() {}
	IL2Client( const IL2Client& other ) { UNREFERENCED_PARAMETER(other); }
	IL2Client& operator=( const IL2Client& other ) { UNREFERENCED_PARAMETER(other); }

public:
	// get-fucntions
	// botting
	BotConfig           *get_BotConfig();
	bool                 isBotCombatEnabled() const;
	void                 botCombatEnable( bool bEnable = true );
	// state
	int                  is_IN_GAME();
	// user info
	UserInfo            *get_UserInfo();
	UserStorageMaxCount *get_StorageMaxCount();
	UserInventory       *get_Inventory();
	UserSkills          *get_Skills();
	UserEtcStatus       *get_EtcStatus();
	UserBuffs           *get_Buffs();
	UserParty           *get_Party();
	// world state
	WorldObjectTree     *get_WorldObjectTree();
	CharArray           *get_WorldChars();
	NpcArray            *get_WorldNpcs();
	GIArray             *get_WorldItems();
	ClanList            *get_WorldClans();

public:
	// game actions
	// party
	void game_Party_Accept( bool bAccept );
	void game_Party_InvitePlayer( const wchar_t *playerName, unsigned int partyLoot );
	void game_Party_Leave();
	void game_Party_KickPlayer( const wchar_t *playerName );
	void game_Party_ChangeLeader( const wchar_t *playerName );
	// chat
	void game_Say( unsigned int channel, const wchar_t *text );
	void game_SayPm( const wchar_t *text, const wchar_t *to );
	void game_SendBypass( const wchar_t *bypass );
	// move
	void game_MoveBackwardToLocation( int xDst, int yDst, int zDst = 0x7FFFFFFF );
	void game_MoveToDelta( int dx, int dy, int dz = 0 );
	void game_SitStand();
	void game_RunWalk();
	// use skill, item
	void game_SkillUse( unsigned int skillID, unsigned int ctrlPressed = 0, unsigned char shiftPressed = 0 );
	int  game_getSkillLevel( unsigned int skillID ); // 0 if no such skill
	void game_UseItemByObjectId( unsigned int objectID );
	void game_UseItemByItemId( unsigned int itemID );
	unsigned long long int game_getItemCount( unsigned int itemID ); // 0 if no such item in inv
	void game_DestroyItem( unsigned int objectID, long long int count = 1 );
	void game_DropItem( unsigned int objectID, long long int count = 1, int x = 0, int y = 0, int z = 0 );
	void game_CrystallizeItem( unsigned int objectID, long long int count = 1 );
	void game_GiveItemToPet( unsigned int objectID, long long int count = 1 );
	// attack, target
	void game_Action( unsigned int objectID );
	void game_TargetByName( const wchar_t *name );
	void game_TargetCancel();
	void game_Attack( unsigned int objectID = 0 );
	void game_autoSoulshot( unsigned int itemID, bool bEnable = true );
	// buff
	void game_buffCancel( unsigned int skillID );
	// packet hack
	void game_sendPacketHex( const char *szHexStr );

public:
	// for user interfaces that do not directly include L2Client
	int                  get_MapScale();
	void getMapShowParams( st_MapShowParams *st );
public: // talking to NPC support
	void     set_last_NPC_HTML( const wchar_t *html, unsigned int objectID );
	wchar_t *get_last_NPC_HTML( unsigned int *pObjectID );
	void     clear_last_NPC_HTML();
protected:
	void         *m_pcl;
	wchar_t      *m_lastNPCHTML;
	unsigned int  m_lastNPCHTMLoid;
};

#endif
