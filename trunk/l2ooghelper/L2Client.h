#ifndef H_L2CLIENT
#define H_L2CLIENT

#include "Account.h"
#include "BotConfig.h"
#include "MessageBoxTimeout.h"
#include "MapShowParams.h"
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
#include "BuySellList.h"
// L2Client interface
#include "IL2Client.h"
// scripting
#include "ScriptEngine.h"
// non-modal persistent dialogs
#include "TradeP2PDlg.h"
#include "NpcHtmlMessageDlg.h"
// bot interprocess communications
#include "bot_interaction/BotIPC.h"
// AI
#include "L2ClientAI.h"

#define THREAD_SIGNAL_LOGOUT            1
#define THREAD_SIGNAL_DISCONNECT        2
#define THREAD_SIGNAL_FORCE_DISCONNECT  3

#define WMMY_UI_UPDATE                 (WM_USER+123) // wParam - update code, lParam - ext param
#define WMMY_UI_MESSAGEBOXTIMEOUTREPLY (WM_USER+124) // wParam: 0-no,1-yes; lParam - question type
#define WMMY_UI_BUYSELLDLGREPLY        (WM_USER+126) // wParam: type (0-buylist, 1-selllist)
                                                     // lParam: resulting TradeItemsList to buy/sell (may be NULL!) must be freed
#define WMMY_UI_PRIVATESTOREDLGREPLY   (WM_USER+127) // wParam: type (0-buy, 1-sell, 2-craft, 3-commoncraft)
                                                     // lParam: resulting TradeItemsList to buy/sell (may be NULL!) must be freed


class L2Client
{
public:
	typedef enum eSTATE
	{
		STATE_OFFLINE = 0,
		STATE_CONNECTING_LOGIN,
		STATE_CONNECTED_LOGIN,
		STATE_AUTHED_LOGIN,
		STATE_CONNECTING_GAME,
		STATE_CONNECTED_GAME,
		STATE_AUTHED_GAME,
		STATE_IN_GAME
	} STATE;
	static const int NUM_CHAT_TABS = 10;
	typedef enum eCHAT_TAB_ID
	{
		CHAT_SYS = 0,
		CHAT_DMG,
		CHAT_ALL,
		CHAT_TRADE,
		CHAT_SHOUT,
		CHAT_PARTY,
		CHAT_CLAN,
		CHAT_ALLY,
		CHAT_HERO,
		CHAT_WHISPER,
		CHAT_LAST = CHAT_WHISPER
	} CHAT_TAB_ID;
	static const int NUM_INFO_TABS = 8;
	typedef enum eINFO_TAB_ID
	{
		INFO_USER = 0,
		INFO_INV,
		INFO_SKILLS,
		INFO_BUFFS,
		INFO_PARTY,
		INFO_MAP,
		INFO_OTHER,
		INFO_STATS
	} INFO_TAB_ID;
	typedef enum eUIUPDATECODE
	{
		UPDATE_USER = 1,
		UPDATE_INV,
		UPDATE_SKILLS,
		UPDATE_SKILL_COOLTIME,
		UPDATE_BUFFS,
		UPDATE_BUFF_DURATION,
		UPDATE_MAP_PLAYERS,
		UPDATE_MAP_MOBS,
		UPDATE_MAP_NPCS,
		UPDATE_MAP_ITEMS,
		UPDATE_PARTY,
		UPDATE_PARTY_BUFFSDURATION,
		UPDATE_USER_TARGET,
		UPDATE_USER_CASTING
	} UIUPDATECODE;
public:
	L2Client();
	~L2Client();
public:
	void clear();
	void clearUserAndWorldState();
public:
	void       setUnused()      { bUsed = false; }
	void       setUsed()        { bUsed = true; }
	bool       isUsed() const   { if( bUsed ) return true; return false; }
	bool       isUnused() const { if( bUsed ) return false; return true; }
public:
	void       setAccount( const class Account *acc ) { if( acc ) account = (*acc); }
	Account    getAccount() const { return account; }
	BotConfig  getConfig() { return botcfg; }
	void       setBotConfig( const BotConfig &other ) { botcfg = other; }
public:
	HWND createWindow( HWND hWndParent );
	HWND getWindow() const { return hWnd; }
	void showWindow();
	void hideWindow();
	void updateWindow();
	void setTabId( int tabID ) { tab_id = tabID; }
	int  getTabId() const { return tab_id; }
	void closeTab();
	bool isThreadRunning() const { return (hThread != NULL); }
	void postUpdateUI( UIUPDATECODE updateCode, LPARAM lParam = 0 );
	int  getScaleSliderPos();
	void postUpdateMap();
	void worldTick();
	void getMapShowParams( st_MapShowParams *p );
	void loadDefaultBotConfig();
	void ensureRemoveSelfFromBotIPC();
public:
	void bottingEnable( bool bEnable = true );
	void bottingCombatEnable( bool bEnable = true );
	L2_VERSION getL2Version() const;
public:
	void     addChatToTab( CHAT_TAB_ID chatTabId, wchar_t *message, wchar_t *from = NULL );
	void     addChatToTabFormat( CHAT_TAB_ID chatTabId, wchar_t *_Format, ... );
	void     addChatAnnouncement( wchar_t *message );
	// do not directly call these
	void     addChatStringToTab_internal( CHAT_TAB_ID chatTabId, CHAT_TAB_ID origTabId, wchar_t *message, bool scroll = true );
	static bool     addChatStringToTab_internal_shouldAlsoAddToAll( CHAT_TAB_ID chatTabId );
	static COLORREF addChatStringToTab_internal_getChatTabColor( CHAT_TAB_ID chatTabId );
protected:
	void onButtonSystem();
	void onButtonConfig();
	void onButtonScript();
	void onButtonActionTarget();
	void onButtonAttackTarget();
	void onButtonCancelTarget();
	void onButtonSitStand();
	void onButtonRunWalk();
	void onButtonSay();
	void onButtonActions();
	bool handleSayCommand( LPWSTR text );
	void onButtonDebug();
	void OnUiUpdateUser( HWND hDlg );
	void OnUiUpdateInv( HWND hDlg );
	void OnUiUpdateSkills( HWND hDlg );
	void OnUiUpdateBuffs( HWND hDlg );
	void OnUiUpdateMapTab( HWND hDlg, int tabNo );
	void OnUiUpdateParty( HWND hDlg );
	void OnUiUpdateTarget( HWND hDlg );
	void OnUiUpdateCasting( HWND hDlg );
	void OnUiUpdateSkillCoolTimes();
	void OnUiUpdateBuffsDuration();
	void OnUiUpdatePartyBuffsDuration();
public:
	void setState( L2Client::STATE newState );
	L2Client::STATE getState() const { return state; }
public:
	IL2Client *getInterface();
	friend class IL2Client;

protected:
	static INT_PTR CALLBACK L2ClientDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	static void OnInitDialog( HWND hDlg, LPARAM lParam );
	static void OnDestroy( HWND hDlg );
	static void OnCommand( HWND hDlg, WPARAM wParam, LPARAM lParam );
	static void OnNotify( HWND hDlg, WPARAM wParam, LPARAM lParam );
	static void OnPaint( HWND hDlg, WPARAM wParam, LPARAM lParam );
	static void OnMyUiUpdate( HWND hDlg, WPARAM wParam, LPARAM lParam );
	static void OnMyMessageBoxTimeoutReply( HWND hDlg, WPARAM wParam, LPARAM lParam );
	//static void OnMyNpcHtmlMessageReply( HWND hDlg, WPARAM wParam, LPARAM lParam );
	static void OnMyBuySellDlgReply( HWND hDlg, WPARAM wParam, LPARAM lParam );
	static void OnMyPrivateStoreManageDlgReply( HWND hDlg, WPARAM wParam, LPARAM lParam );

public:
	void handle_AskJoinParty( const wchar_t *requester, unsigned int itemDistribution );
	//void handle_AskJoinPledge( const wchar_t *requester );
	//void handle_AksJoinAlly( const wchar_t *requester );
	void handle_BuyList( TradeItemsList *list );
	void handle_SellList( TradeItemsList *list );
	void handle_PrivateStoreManageListSell( TradeItemsList *list1, TradeItemsList *list2 );
	void handle_PrivateStoreManageListBuy( TradeItemsList *list1, TradeItemsList *list2 );
	void handle_RecipeShopManageList( TradeItemsList *list1, TradeItemsList *list2 );
	void handle_PrivateStoreBuyList( TradeItemsList *list );
	void handle_PrivateStoreSellList( TradeItemsList *list );
	void handle_ConfirmDlg( wchar_t *question, unsigned int sm_ID, unsigned int requestId, unsigned int timeLimit, const wchar_t *text );
	void game_MoveToDelta( int dx, int dy, int dz = 0 );

	void send_Appearing();
	void send_RequestGMList();
	void send_RequestAnswerJoinParty( unsigned int accepted );
	void send_RequestJoinParty( const wchar_t *target, unsigned int itemDistribution = 0 );
	void send_RequestWithdrawalParty();
	void send_RequestOustPartyMember( const wchar_t *playerName );
	void send_RequestChangePartyLeader( const wchar_t *playerName );
	void send_Say2( unsigned int chat_type, const wchar_t *text, const wchar_t *to );
	void send_MoveBackwardToLocation( int xDst, int yDst, int zDst = 0x7FFFFFFF );
	void send_RequestMagicSkillUse( unsigned int skillID, unsigned int ctrlPressed = 0, unsigned char shiftPressed = 0 );
	void send_Action( unsigned int objectID, int x = 0, int y = 0, int z = 0, unsigned char useShift = 0 );
	void send_RequestTargetCanceld();
	void send_AttackRequest( unsigned int objectID = 0, int x = 0, int y = 0, int z = 0, unsigned char useShift = 0 );
	void send_RequestUserCommand( unsigned int commandID );
	void send_RequestAutoSoulshot( unsigned int itemID, bool enable );
	void send_RequestActionUse( unsigned int actionID, unsigned int ctrlPressed = 0, unsigned char shiftPressed = 0 );
	void send_UseItem( unsigned int objectID );
	void send_RequestDestroyItem( unsigned int objectID, long long int count = 1 );
	void send_RequestDropItem( unsigned int objectID, long long int count = 1, int x = 0, int y = 0, int z = 0 );
	void send_RequestCrystallizeItem( unsigned int objectID, long long int count = 1 );
	void send_RequestGiveItemToPet( unsigned int objectID, long long int count = 1 );
	void send_RequestDispel( unsigned int skillID );
	void send_RequestBypassToServer( const wchar_t *bypassStr );
	void send_RequestLinkHtml( const wchar_t *link );
	void send_ValidatePosition();
	void send_RequestRestartPoint( unsigned int pointType = 0 );
	void send_SendBypassBuildCmd( const wchar_t *cmd );
	void send_RequestBuyItem( TradeItemsList *plist );
	void send_RequestSellItem( TradeItemsList *plist );
	void send_RequestEvaluate();
	void send_RequestPrivateStoreQuitSell();
	void send_RequestPrivateStoreQuitBuy();
	void send_RequestRecipeShopManageQuit();
	void send_SetPrivateStoreMsgBuy( const wchar_t *message );
	void send_SetPrivateStoreMsgSell( const wchar_t *message );
	void send_SetPrivateStoreWholeMsg( const wchar_t *message );
	void send_RequestRecipeShopMessageSet( const wchar_t *message );
	void send_SetPrivateStoreListSell( TradeItemsList *list );
	void send_SetPrivateStoreListBuy( TradeItemsList *list );
	void send_RequestRecipeShopListSet( TradeItemsList *list );
	void send_RequestPrivateStoreBuy( TradeItemsList *list );
	void send_RequestPrivateStoreSell( TradeItemsList *list );
	void send_TradeRequest( unsigned int targetObjectId = 0 );
	void send_AnswerTradeRequest( bool accept = true );
	void send_AddTradeItem( unsigned int tradeID, unsigned int objectID, long long int count );
	void send_TradeDone( int bConfirm );
	void send_DlgAnswer( unsigned int sm_ID, int answer, unsigned int requestId );
	void send_AuthLogin();
	void send_Logout();
	void send_RequestRestart();
	void send_CharacterSelect( int iCharSlot );
	void send_RequestGotoLobby();
	void send_NewCharacter();
	void send_CharacterDelete( int iCharSlot );
	void send_CharacterRestore( int iCharSlot );
	void send_CharacterCreate( const wchar_t *name, const L2Game_NewCharacterTemplate *tmpl,
		int hairStyle, int hairColor, int face, int is_female );
	void send_GameGuardReply( unsigned int r1, unsigned int r2, unsigned int r3, unsigned int r4 );
	// hack function, do not use!
	// input: string consisting of sequence of hex chars, representing packet, starting from packet type
	// packet length will be prefixed automatically!
	// example: 1F39A400109ACB00003D2BFFFFA9F3FFFF00 - 1F Action packet, without packet len
	void send_hackPacketHex( const char *szHexStr );

protected:
	void startClient();
	void logoutClient();
	void disconnectClient( bool force = false );
	static void threadNetCleanup( class L2Client *pcls );
	static DWORD WINAPI L2ClientThread( LPVOID lpParam );
	static bool L2ClientThread_Login( class L2Client *pcls );
	static bool L2ClientThread_Game( class L2Client *pcls );
	static void L2ClientThread_Game_BotIPC( class L2Client *pcls );

protected: // packet handlers
	static void ph_AbnormalStatusUpdate( class L2Client *pcls, L2GamePacket *p );
	static void ph_AskJoinParty( class L2Client *pcls, L2GamePacket *p );
	static void ph_AuthLoginFail( class L2Client *pcls, L2GamePacket *p );
	static void ph_BuyList( class L2Client *pcls, L2GamePacket *p );
	static void ph_ChangeMoveType( class L2Client *pcls, L2GamePacket *p );
	static void ph_ChangeWaitType( class L2Client *pcls, L2GamePacket *p );
	static void ph_CharCreateFail( class L2Client *pcls, L2GamePacket *p );
	static void ph_CharCreateSuccess( class L2Client *pcls, L2GamePacket *p );
	static void ph_CharDeleteFail( class L2Client *pcls, L2GamePacket *p );
	static void ph_CharDeleteSuccess( class L2Client *pcls, L2GamePacket *p );
	static void ph_CharInfo( class L2Client *pcls, L2GamePacket *p );
	static void ph_CharSelected( class L2Client *pcls, L2GamePacket *p );
	static void ph_CharSelectionInfo( class L2Client *pcls, L2GamePacket *p );
	static void ph_CreatureSay( class L2Client *pcls, L2GamePacket *p );
	static void ph_ConfirmDlg( class L2Client *pcls, L2GamePacket *p );
	static void ph_DeleteObject( class L2Client *pcls, L2GamePacket *p );
	static void ph_Die( class L2Client *pcls, L2GamePacket *p );
	static void ph_DropItem( class L2Client *pcls, L2GamePacket *p );
	static void ph_EtcStatusUpdate( class L2Client *pcls, L2GamePacket *p );
	static void ph_ExNpcQuestHtmlMessage( class L2Client *pcls, L2GamePacket *p );
	static void ph_ExSetPrivateStoreWholeMsg( class L2Client *pcls, L2GamePacket *p );
	static void ph_ExStorageMaxCount( class L2Client *pcls, L2GamePacket *p );
	static void ph_GameGuardQuery( class L2Client *pcls, L2GamePacket *p );
	static void ph_GetItem( class L2Client *pcls, L2GamePacket *p );
	static void ph_ItemList( class L2Client *pcls, L2GamePacket *p );
	static void ph_InventoryUpdate( class L2Client *pcls, L2GamePacket *p );
	static void ph_JoinParty( class L2Client *pcls, L2GamePacket *p );
	static void ph_KeyPacket( class L2Client *pcls, L2GamePacket *p );
	static void ph_MagicSkillCanceld( class L2Client *pcls, L2GamePacket *p );
	static void ph_MagicSkillUse( class L2Client *pcls, L2GamePacket *p );
	static void ph_MoveToLocation( class L2Client *pcls, L2GamePacket *p );
	static void ph_MoveToPawn( class L2Client *pcls, L2GamePacket *p );
	static void ph_NewCharacterSuccess( class L2Client *pcls, L2GamePacket *p );
	static void ph_NpcHtmlMessage( class L2Client *pcls, L2GamePacket *p );
	static void ph_NpcInfo( class L2Client *pcls, L2GamePacket *p );
	static void ph_PartyMemberPosition( class L2Client *pcls, L2GamePacket *p );
	static void ph_PartySmallWindowAdd( class L2Client *pcls, L2GamePacket *p );
	static void ph_PartySmallWindowAll( class L2Client *pcls, L2GamePacket *p );
	static void ph_PartySmallWindowDelete( class L2Client *pcls, L2GamePacket *p );
	static void ph_PartySmallWindowDeleteAll( class L2Client *pcls, L2GamePacket *p );
	static void ph_PartySmallWindowUpdate( class L2Client *pcls, L2GamePacket *p );
	static void ph_PartySpelled( class L2Client *pcls, L2GamePacket *p );
	static void ph_PledgeInfo( class L2Client *pcls, L2GamePacket *p );
	static void ph_PrivateStoreListBuy( class L2Client *pcls, L2GamePacket *p );
	static void ph_PrivateStoreListSell( class L2Client *pcls, L2GamePacket *p );
	static void ph_PrivateStoreManageListBuy( class L2Client *pcls, L2GamePacket *p );
	static void ph_PrivateStoreManageListSell( class L2Client *pcls, L2GamePacket *p );
	static void ph_PrivateStoreMsgBuy( class L2Client *pcls, L2GamePacket *p );
	static void ph_PrivateStoreMsgSell( class L2Client *pcls, L2GamePacket *p );
	static void ph_RecipeShopManageList( class L2Client *pcls, L2GamePacket *p );
	static void ph_RecipeShopMsg( class L2Client *pcls, L2GamePacket *p );
	static void ph_Revive( class L2Client *pcls, L2GamePacket *p );
	static void ph_SellList( class L2Client *pcls, L2GamePacket *p );
	static void ph_SendTradeRequest( class L2Client *pcls, L2GamePacket *p );
	static void ph_SetupGauge( class L2Client *pcls, L2GamePacket *p );
	static void ph_ShortBuffStatusUpdate( class L2Client *pcls, L2GamePacket *p );
	static void ph_SkillCoolTime( class L2Client *pcls, L2GamePacket *p );
	static void ph_SkillList( class L2Client *pcls, L2GamePacket *p );
	static void ph_SpawnItem( class L2Client *pcls, L2GamePacket *p );
	static void ph_SSQInfo( class L2Client *pcls, L2GamePacket *p );
	static void ph_StatusUpdate( class L2Client *pcls, L2GamePacket *p );
	static void ph_StopMove( class L2Client *pcls, L2GamePacket *p );
	static void ph_SystemMessage( class L2Client *pcls, L2GamePacket *p );
	static void ph_TargetSelected( class L2Client *pcls, L2GamePacket *p );
	static void ph_TargetUnselected( class L2Client *pcls, L2GamePacket *p );
	static void ph_TradeDone( class L2Client *pcls, L2GamePacket *p );
	static void ph_TradeOtherAdd( class L2Client *pcls, L2GamePacket *p );
	static void ph_TradeOwnAdd( class L2Client *pcls, L2GamePacket *p );
	static void ph_TradeStart( class L2Client *pcls, L2GamePacket *p );
	static void ph_TeleportToLocation( class L2Client *pcls, L2GamePacket *p );
	static void ph_UserInfo( class L2Client *pcls, L2GamePacket *p );
	static void ph_ValidateLocation( class L2Client *pcls, L2GamePacket *p );

protected: // utils
	static void log_packet( L2Client *pcls, bool fromServer, const unsigned char *p, unsigned int plen,
		const char *hintStr = NULL, LOG_LEVEL logLevel = LOG_DEBUGDUMP );
	bool pack_OpcodeDeObfuscate( unsigned char *bytes, unsigned int len );
	bool pack_OpcodeObfuscate( unsigned char *bytes, unsigned int len );
	bool pack_OpcodeDeObfuscate( L2GamePacket *p );
	bool pack_OpcodeObfuscate( L2GamePacket *p );
	// returns number of bytes sent
	int sendPacket( L2GamePacket *pack, bool obfuscateAndXOREncode );

protected:
	bool      bUsed; // true if this account slot is in use
	Account   account; // account info - login/pass, server ip:port, proto ver, ...
	BotConfig botcfg;
	int       tab_id; // tab number in main window :)

	// child windows
	HWND hWnd;
	HWND hWndTabChat;
	HWND hWndTabInfo;
	HWND hWndProgressHP;
	HWND hWndProgressMP;
	HWND hWndProgressCP;
	HWND hWndProgressWeight;
	HWND hWndProgressTargetHP;
	HWND hWndProgressExp;
	HWND hWndMap;
	HWND hWndSliderMapScale;
	HWND hWndProgressCasting;
	// map check buttons
	st_MapShowParams mapShowParams;
	// chat tab windows
	int curTabChat;
	HWND hWndTabChat_item[NUM_CHAT_TABS];
	// info tab windows
	int curTabInfo;
	HWND hWndTabInfo_item[NUM_INFO_TABS];
	// non-modal dialogs
	TradeP2PDlg *tradep2pdlg;
	NpcHtmlMessageDlg *npcHtmlDlg;

protected:
	L2Client::STATE    state;
	SOCKET             sock;
	HANDLE             hThread;
	DWORD              dwThreadId;
	unsigned char      login_sessionKey1[8];
	unsigned char      login_sessionKey2[8];
	int                login_selectedGsId;
	char               login_selectedGsIP[32];
	int                login_selectedGsPort;
	long               game_recvTimeoutMsec;
	long               game_sendTimeoutMsec;
	bool               game_logoutRequestSent;
	bool               game_XOR_enabled;
	unsigned char      game_key_send[16];
	unsigned char      game_key_recv[16];
	unsigned int       game_opcodeObfuscatorSeed;
	L2PCodeObfuscator *game_pCodeObfuscator;
	int                threadSignal; // 0 - none, 1 - logout, 2 - disconnect
	CRITICAL_SECTION   cs_sendPacket;
protected:
	//bool bEnableAutoFun;
	//bool bEnableAutoCombat;
protected: // user info and state (inventory, skills, buffs, party, ...)
	UserInfo            usr;
	UserStorageMaxCount storageMaxCount;
	UserInventory       inv;
	UserSkills          skills;
	UserEtcStatus       etcStatus;
	UserBuffs           buffs;
	UserParty           party;
protected: // world info and state (chars, npcs, mobs, items, clans)
	WorldObjectTree     world_tree;
	CharArray           world_chars;
	NpcArray            world_npcs;
	GIArray             world_ground_items;
	ClanList            world_clans;
protected: // scripting
	ScriptEngine        scripter;     // user script VM
	ScriptEngine        sys_scripter; // internal script VM
	char                sys_main_ai_script_filename[256]; // internal script filename
	int                 bot_enabled;
	int                 bot_combat_enabled;
protected:
	IL2Client          *pInterface;
protected:
	int                 botipc_index;
protected:
	L2ClientAI         *m_ai;
};

#endif
