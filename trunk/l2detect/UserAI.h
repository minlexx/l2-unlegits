#ifndef H_USER_AI
#define H_USER_AI
#include "UserState.h"
#include "UserInventory.h"
#include "UserBuffs.h"
#include "UserParty.h"
#include "UserSkills.h"
#include "HealItemsTable.h"
#include "ClanWarList.h"
#include "CharArray.h"
#include "ClanList.h"
#include "NpcArray.h"
#include "GIArray.h"
#include "WorldObjectTree.h"
#include "UserAI_OffpartyBDSWS.h"
#include "UserAI_InvisGM.h"
#include "UserAI_QuickTargetSetup.h"
#include "UserAI_QuickTargetMgr.h"
#include "UserAI_FolAsiCfg.h"
#include "UserAI_LockTargetMgr.h"

// user AI intentions
#define UAI_IDLE              0x00000000
#define UAI_WANT_HEAL_HP      0x00000001
#define UAI_WANT_HEAL_MP      0x00000002
#define UAI_WANT_HEAL_CP      0x00000004

// user AI notification           notifyEvent( int event_id )
#define UAI_EVENT_START       1
#define UAI_EVENT_STOP        2
// user AI notification packets   notifyEventPacket( int event_id, const unsigned char *bytes, unsigned int length )
#define UAI_PEVENT_USERINFO             100 // done
#define UAI_PEVENT_CHARSELECTED         101
#define UAI_PEVENT_STATUSUPDATE         102 // done
#define UAI_PEVENT_ITEMLIST             103 // done
#define UAI_PEVENT_INVENTORYUPDATE      104 // done
#define UAI_PEVENT_MOVETOLOCATION       105 // done
#define UAI_PEVENT_STOPMOVE             106 // done
#define UAI_PEVENT_TELEPORTTOLOCATION   107 // done

#define UAI_PEVENT_ABNORMALSTATUSUPDATE  150 // done
#define UAI_PEVENT_SHORTBUFFSTATUSUPDATE 151 // done

#define UAI_PEVENT_PLEDGEINFO           200 //-- will not be done, no need
#define UAI_PEVENT_PLEDGERECEIVEWARLIST 201 // done

#define UAI_PEVENT_NPCINFO              300 // done
#define UAI_PEVENT_CHARINFO             301 // done
#define UAI_PEVENT_DIE                  302 // done
#define UAI_PEVENT_REVIVE               303 // done

#define UAI_PEVENT_TARGETSELECTED       350 // done
#define UAI_PEVENT_TARGETUNSELECTED     351 // done
#define UAI_PEVENT_RELATIONCHANGED      352 // done
#define UAI_PEVENT_MYTARGETSELECTED     353 // done

#define UAI_PEVENT_PARTYADD             400 // done
#define UAI_PEVENT_PARTYALL             401 // done
#define UAI_PEVENT_PARTYDEL             402 // done
#define UAI_PEVENT_PARTYDELALL          403 // done
#define UAI_PEVENT_PARTYUPDATE          404 // done
#define UAI_PEVENT_PARTYSPELLED         405 // done

#define UAI_PEVENT_SPAWNITEM            500 // done
#define UAI_PEVENT_DROPITEM             501 // done
#define UAI_PEVENT_DELETEOBJECT         999 // done
// skills
#define UAI_PEVENT_SETUPGAUGE           600 // done
#define UAI_PEVENT_SKILLLIST            601 // done
#define UAI_PEVENT_SKILLCOOLTIME        602 // done
#define UAI_PEVENT_MAGICSKILLUSE        603 // done
#define UAI_PEVENT_MAGICSKILLCANCELLED  604 // done

class UserAI
{
	friend void DebugDlg_OnBnClickedPrintAddrTid( HWND hDlg );
public:
	UserAI();
	~UserAI();
	void init();
	void clear();
	void userLogout();
	void notifyEvent( int event_id );
	void notifyEventPacket( int event_id, const unsigned char *bytes, unsigned int length );
	void notifyUIKeyUp( int vkey );

public: // healing
	void setEnableHealHP( int e ) { this->healItems.enableHealHP = e; }
	void setEnableHealMP( int e ) { this->healItems.enableHealMP = e; }
	void setEnableHealCP( int e ) { this->healItems.enableHealCP = e; }
	void setHealItemsTable( const HealItemsTable *other );
	bool isHealingEnabledAny() const { return (this->healItems.enableHealHP > 0) || (this->healItems.enableHealMP > 0) || (this->healItems.enableHealCP > 0); }

public: // offparty bd, sws
	void setOffpartyBDSWS_Config( const UserAI_OffpartyBDSWS *other );
	void setOffpartyBDSWS_Enable( int bEnable );

public:
	void followDisable();
	void assistDisable();

public: // custom packet processors
	bool UAI_process_RequestBypassToServer( unsigned char *bytes, unsigned int len, unsigned char **newBytes, unsigned int *newLen );
	bool UAI_process_NpcHtml( unsigned char *bytes, unsigned int len );
	wchar_t *get_last_NPC_HTML( unsigned int *npc_oid );
	void     clear_last_NPC_HTML();
protected:
	wchar_t      *m_last_npc_html;
	unsigned int  m_last_npc_html_npcOid;
public:
	void UAI_process_ChangeMoveType( unsigned char *bytes, unsigned int len );
	void UAI_process_ChangeWaitType( unsigned char *bytes, unsigned int len );

public:
	UserState                 usr;
	UserInventory             inv;
	UserBuffs                 buffs;
	UserParty                 party;
	UserSkills                skills;
	ClanWarList               warList;
	UserAI_OffpartyBDSWS      off_BD_SWS;
	UserAI_InivisGMManager    invisGM;
	UserAI_QuickTargetSetup   cfgQuickTarg;
	UserAI_QuickTargetManager targetManager;
	UserAI_FolAsiCfg          folAsiCfg;
	UserAI_LockTargetMgr      lockTargetMgr;

protected:
	HealItemsTable healItems;
	unsigned int state;
	// offparty
	bool  offBD_inrange;
	bool  offSWS_inrange;
	DWORD offBDSWS_nextBDInvTime;
	DWORD offBDSWS_nextSWSInvTime;
	DWORD offBDSWS_nextBDDisTime;
	DWORD offBDSWS_nextSWSDisTime;
	// follow/assist internal vars
	unsigned int m_last_follow_time_msec;
	unsigned int m_follow_oid;
	unsigned int m_assist_oid;
	unsigned int m_last_assist_time_msec;

public: // sound alerts cfg
	// funcs
	void loadSoundAlertsCfgFromFile( const char *filename = NULL );
	void saveSoundAlertsCfgToFile( const char *filename = NULL );
	void playSoundAlert( int alert_type );
	// vars
	char soundAlert_fileNameCritical[256];
	char soundAlert_fileNameWarning[256];
	char soundAlert_fileNameInfo[256];
	int  soundAlert_alertOnClanWar; // 0 - none, 1 - info, 2 - warning, 3 - critical
	int  soundAlert_alertOnTargeted; // 0 - none, 1 - info, 2 - warning, 3 - critical
	int  soundAlert_alertOnInvisGM; // 0 - none, 1 - info, 2 - warning, 3 - critical
	bool soundAlert_enabled;
	int  soundAlert_noPartyTargetAlert;
	int  soundAlert_noClanTargetAlert;
	int  soundAlert_screenMessage;

protected:
	HANDLE hThread;
	DWORD dwThreadID;
	//
	static DWORD WINAPI UserAIThread( LPVOID lpParam );
	//
	static void UAI_ParsePacket( class UserAI *cls, WPARAM wParam, LPARAM lParam );
	static void UAI_Parse_NpcInfo( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_DeleteObject( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_CharInfo( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_MoveToLocation( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_StopMove( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_TeleportToLocation( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_TargetSelected( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_TargetUnselected( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_MyTargetSelected( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_RelationChanged( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_PartySmallWindowAdd( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_PartySmallWindowAll( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_PartySmallWindowDelete( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_PartySmallWindowDeleteAll( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_PartySmallWindowUpdate( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_PartySpelled( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_Die( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_Revive( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_AbnormalStatusUpdate( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_ShortBuffStatusUpdate( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_SetupGauge( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_SkillList( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_SkillCoolTime( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_StatusUpdate( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_MagicSkillUse( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_MagicSkillCancelled( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_DropItem( class UserAI *cls, void *l2_game_packet );
	static void UAI_Parse_SpawnItem( class UserAI *cls, void *l2_game_packet );
	//
	static void UAI_ProcessWorld( class UserAI *cls );
	static void UAI_ProcessMoves( class UserAI *cls );
	static void UAI_CheckHP( class UserAI *cls );
	static void UAI_CheckMP( class UserAI *cls );
	static void UAI_CheckCP( class UserAI *cls );
	static void UAI_ProcessOffpartyBDSWS( class UserAI *cls );
	static void UAI_ProcessFollow( class UserAI *cls );
	static void UAI_ProcessAssist( class UserAI *cls );
	static void UAI_Check_Distance_And_Follow( class UserAI *cls, L2Player *followTarget );
	static void UAI_Check_Target_By_Assist( class UserAI *cls, L2Player *assistBy );
	//
	void UAI_handle_keyboard( int vkey );
	void UAIKEY_targetNearestTarget();
	void UAIKEY_displayConfig();
};

#endif
