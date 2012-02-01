#include "stdafx.h"
#include "UserAI.h"
#include "Logger.h"
#include "RadarDllWnd.h"
//

UserAI::UserAI()
{
	m_last_npc_html = NULL;
	clear();
}

UserAI::~UserAI()
{
	clear();
}

void UserAI::clear()
{
	usr.clear();
	inv.clear();
	buffs.clear();
	party.clear();
	healItems.clear();
	invisGM.clear();
	cfgQuickTarg.clear();
	targetManager.clear();
	folAsiCfg.clear();
	//
	state = UAI_IDLE;
	//
	hThread = NULL;
	dwThreadID = 0;
	// heal items
	healItems.enableHealHP = healItems.enableHealMP = healItems.enableHealCP = true;
	// offparty bd, sws
	offBDSWS_nextBDInvTime = offBDSWS_nextSWSInvTime = offBDSWS_nextBDDisTime = offBDSWS_nextSWSDisTime = 0;
	offBD_inrange = offSWS_inrange = false;
	// sound alerts
	soundAlert_enabled = true;
	soundAlert_fileNameCritical[0] = soundAlert_fileNameWarning[0] = soundAlert_fileNameInfo[0] = 0;
	soundAlert_alertOnClanWar = 0; // 0 - none, 1 - info, 2 - warning, 3 - critical
	soundAlert_alertOnTargeted = 0;
	soundAlert_alertOnInvisGM = 0;
	soundAlert_noPartyTargetAlert = soundAlert_noClanTargetAlert = 1;
	soundAlert_screenMessage = 0;
	// follow / asi
	m_last_follow_time_msec = 0;
	m_follow_oid = 0;
	m_assist_oid = 0;
	m_last_assist_time_msec = 0;
	// npc html
	m_last_npc_html_npcOid = 0;
	if( m_last_npc_html )
	{
		free( m_last_npc_html );
		m_last_npc_html = NULL;
	}
}

void UserAI::userLogout()
{
	usr.clear();
	inv.clear();
	buffs.clear();
	party.clear();
	healItems.clear();
	invisGM.clear();
	cfgQuickTarg.clear();
	targetManager.clear();
	folAsiCfg.clear();
	m_follow_oid = 0;
	m_assist_oid = 0;
	//
	state = UAI_IDLE;
	//
	CharArray_DeleteAll();
	NpcArray_DeleteAll();
	GIArray::getInstance()->DeleteAll();
	log_error( LOG_USERAI, "UserAI: Logout OK, User logged out (notified)\n" );
	RadarWnd_ForceUpdateCharsList();
	RadarWnd_ForceUpdateNpcsList();
	RadarWnd_UpdateChecksState();
}

void UserAI::init()
{
	clear();
	// load all config
	healItems.LoadFromFile( "L2Detect_healItems.ini" );
	off_BD_SWS.readFromFile( "L2Detect_offp_BDSWS.ini" );
	setOffpartyBDSWS_Enable( off_BD_SWS.allEnabled ); // process...
	cfgQuickTarg.loadFromFile( "L2Detect_quickTarg.ini" );
	targetManager.clear();
	loadSoundAlertsCfgFromFile( "L2Detect_soundAlerts.ini" );
	folAsiCfg.loadFromFile( "L2Detect_folAsi.ini" );
	// force radar window to set all checks from our confg
	RadarWnd_UpdateChecksState();
}

void UserAI::notifyEvent( int event_id )
{
	switch( event_id )
	{
	case UAI_EVENT_START:
		{
			log_error( LOG_USERAI, "UserAI::notifyEvent(): START\n" );
			if( hThread == NULL )
			{
				dwThreadID = 0;
				hThread = (HANDLE)_beginthreadex( NULL, 0,
					(unsigned int (__stdcall *)(void *))UserAIThread, (void *)this, 0,
					(unsigned int *)&dwThreadID );
				if( hThread )
					log_error( LOG_USERAI, "UserAI::notifyEvent(): START OK %u\n", dwThreadID );
				else
					log_error( LOG_ERROR, "UserAI::notifyEvent(): START ERROR %d\n", GetLastError() );
			}
			else
				log_error( LOG_ERROR, "UserAI::notifyEvent(): START: already running\n" );
		} break;
	case UAI_EVENT_STOP:
		{
			log_error( LOG_USERAI, "UserAI::notifyEvent(): STOPPING %u...\n", dwThreadID );
			if( hThread != NULL )
			{
				if( !PostThreadMessage( dwThreadID, WM_CLOSE, 0, 0 ) )
				{
					log_error( LOG_ERROR, "UserAI::notifyEvent(): STOP: PostThreadMessage() error %u\n",
						(unsigned int)GetLastError );
					return;
				}
				Sleep( 100 );
				while( hThread )
				{
					log_error_np( LOG_OK, "." );
					Sleep( 250 );
				}
				log_error( LOG_USERAI, "\nUserAI::notifyEvent(): STOP WAIT OK\n" );
			}
			else
				log_error( LOG_ERROR, "UserAI::notifyEvent(): STOP: not running\n" );
		} break;
	}
}

void UserAI::notifyEventPacket( int event_id, const unsigned char *bytes, unsigned int length )
{
	if( !bytes || (length<1) ) return;
	L2GamePacket *p = new L2GamePacket( bytes, length );
	if( !p ) return;
#ifdef _DEBUG
	BOOL ret = PostThreadMessage( dwThreadID, WM_KEYDOWN, event_id, (LPARAM)p );
	if( !ret )
	{
		DWORD le = GetLastError();
		ErrorLogger_LogLastError( "UserAI::notifyEventPacket(): PostThreadMessage() failed", le );
		log_error( LOG_ERROR, "tid = %u\n", dwThreadID );
	}
#else
	PostThreadMessage( dwThreadID, WM_KEYDOWN, event_id, (LPARAM)p );
#endif
}

void UserAI::notifyUIKeyUp( int vkey )
{
	PostThreadMessage( dwThreadID, WM_KEYUP, (WPARAM)vkey, (LPARAM)0 );
}

DWORD WINAPI UserAI::UserAIThread( LPVOID lpParam )
{
	class UserAI *cls = (class UserAI *)lpParam;
	if( !cls ) return 12345;
	MSG msg;
	// create thread's message queue
	PeekMessage( &msg, NULL, WM_USER, WM_USER, PM_NOREMOVE );
	// now message queue for this thread is created
	log_error( LOG_USERAI, "UserAIThread(): started, created message queue\n" );
	//
	int shouldExit = 0;
	int was_message = 0;
	do
	{
		was_message = 0;
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			was_message = 1;
			//log_error( LOG_OK, "UserAIThread(): GotMessage %u\n", msg.message );
			switch( msg.message )
			{
			//case WM_CREATE: { /*log_error( LOG_DEBUG, "UserAIThread(): WM_CREATE\n" );*/ } break;
			case WM_CLOSE: { /*log_error( LOG_DEBUG, "UserAIThread(): WM_CLOSE\n" );*/ shouldExit = 1; } break;
			case WM_KEYDOWN: UAI_ParsePacket( cls, msg.wParam, msg.lParam ); break;
			case WM_KEYUP: cls->UAI_handle_keyboard( (int)msg.wParam ); break;
			}
		}
		if( shouldExit ) break;
		if( !was_message ) Sleep( 250 );

		// process all L2Character moves in visible area, also process user move
		// process buff ticks, skill reuses
		UAI_ProcessWorld( cls );

		// check HP,MP,CP
		if( cls->healItems.enableHealHP ) UAI_CheckHP( cls );
		if( cls->healItems.enableHealMP ) UAI_CheckMP( cls );
		if( cls->healItems.enableHealCP ) UAI_CheckCP( cls );

		// check if enabled offparty bd, sws invite/dismiss
		if( cls->off_BD_SWS.allEnabled ) UAI_ProcessOffpartyBDSWS( cls );

		// process follow/assist
		if( cls->folAsiCfg.m_follow_enable ) UAI_ProcessFollow( cls );
		if( cls->folAsiCfg.m_assist_enable ) UAI_ProcessAssist( cls );
	}
	while( !shouldExit );
	//
	log_error( LOG_USERAI, "UserAIThread(): ending\n" );
	//
	CloseHandle( cls->hThread );
	cls->hThread = NULL;
	cls->dwThreadID = 0;
	return 0;
}

void UserAI::UAI_ParsePacket( class UserAI *cls, WPARAM wParam, LPARAM lParam )
{
	if( lParam == 0 ) return;
	L2GamePacket *p = (L2GamePacket *)lParam;
	int event_id = (int)wParam;
	//
	try
	{
	//
	switch( event_id )
	{
	case UAI_PEVENT_USERINFO:                cls->usr.parse_UserInfo( p ); break;
	case UAI_PEVENT_STATUSUPDATE:            cls->UAI_Parse_StatusUpdate( cls, p ); break;
	case UAI_PEVENT_ITEMLIST:                cls->inv.parse_ItemList( p ); break;
	case UAI_PEVENT_INVENTORYUPDATE:         cls->inv.parse_InventoryUpdate( p ); break;
	case UAI_PEVENT_MOVETOLOCATION:          cls->UAI_Parse_MoveToLocation( cls, p ); break;
	case UAI_PEVENT_STOPMOVE:                cls->UAI_Parse_StopMove( cls, p ); break;
	case UAI_PEVENT_TELEPORTTOLOCATION:      cls->UAI_Parse_TeleportToLocation( cls, p ); break;
	case UAI_PEVENT_PLEDGERECEIVEWARLIST:    cls->warList.parse_PledgeReceiveWarList( p ); break;
	case UAI_PEVENT_NPCINFO:                 cls->UAI_Parse_NpcInfo( cls, p ); break;
	case UAI_PEVENT_DELETEOBJECT:            cls->UAI_Parse_DeleteObject( cls, p ); break;
	case UAI_PEVENT_CHARINFO:                cls->UAI_Parse_CharInfo( cls, p ); break;
	case UAI_PEVENT_TARGETSELECTED:          cls->UAI_Parse_TargetSelected( cls, p ); break;
	case UAI_PEVENT_TARGETUNSELECTED:        cls->UAI_Parse_TargetUnselected( cls, p ); break;
	case UAI_PEVENT_RELATIONCHANGED:         cls->UAI_Parse_RelationChanged( cls, p ); break;
	case UAI_PEVENT_MYTARGETSELECTED:        cls->UAI_Parse_MyTargetSelected( cls, p ); break;
	// party
	case UAI_PEVENT_PARTYADD:                cls->UAI_Parse_PartySmallWindowAdd( cls, p ); break;
	case UAI_PEVENT_PARTYALL:                cls->UAI_Parse_PartySmallWindowAll( cls, p ); break;
	case UAI_PEVENT_PARTYDEL:                cls->UAI_Parse_PartySmallWindowDelete( cls, p ); break;
	case UAI_PEVENT_PARTYDELALL:             cls->UAI_Parse_PartySmallWindowDeleteAll( cls, p ); break;
	case UAI_PEVENT_PARTYUPDATE:             cls->UAI_Parse_PartySmallWindowUpdate( cls, p ); break;
	case UAI_PEVENT_PARTYSPELLED:            cls->UAI_Parse_PartySpelled( cls, p ); break;
	// die/revive
	case UAI_PEVENT_DIE:                     cls->UAI_Parse_Die( cls, p ); break;
	case UAI_PEVENT_REVIVE:                  cls->UAI_Parse_Revive( cls, p ); break;
	// buffs
	case UAI_PEVENT_ABNORMALSTATUSUPDATE:    cls->UAI_Parse_AbnormalStatusUpdate( cls, p ); break;
	case UAI_PEVENT_SHORTBUFFSTATUSUPDATE:   cls->UAI_Parse_ShortBuffStatusUpdate( cls, p ); break;
	// skills
	case UAI_PEVENT_SETUPGAUGE:              cls->UAI_Parse_SetupGauge( cls, p ); break;
	case UAI_PEVENT_SKILLLIST:               cls->UAI_Parse_SkillList( cls, p ); break;
	case UAI_PEVENT_SKILLCOOLTIME:           cls->UAI_Parse_SkillCoolTime( cls, p ); break;
	case UAI_PEVENT_MAGICSKILLUSE:           cls->UAI_Parse_MagicSkillUse( cls, p ); break;
	case UAI_PEVENT_MAGICSKILLCANCELLED:     cls->UAI_Parse_MagicSkillCancelled( cls, p ); break;
	// items
	case UAI_PEVENT_DROPITEM:                cls->UAI_Parse_DropItem( cls, p ); break;
	case UAI_PEVENT_SPAWNITEM:               cls->UAI_Parse_SpawnItem( cls, p ); break;
	}
	//
	} // try
	catch( L2P_Exception& e )
	{
		log_error( LOG_ERROR, "L2P_Exception in UAI_ParsePacket (event_id = %d):\n%s\n",
			event_id, e.what() );
	}
	//
	delete p;
}

void UserAI::setHealItemsTable( const HealItemsTable *other )
{
	if( !other ) return;
	// possible decision - reload table from saved file
	//this->healItems.LoadFromFile( "L2Detect_healItems.ini" );
	// other way - full memcpy :)
	memcpy( (void *)(&(this->healItems)), (const void *)other, sizeof(class HealItemsTable) );
}

void UserAI::setOffpartyBDSWS_Config( const UserAI_OffpartyBDSWS *other )
{
	if( !other ) return;
	memcpy( (void *)(&(this->off_BD_SWS)), (const void *)other, sizeof(class UserAI_OffpartyBDSWS) );
}

void UserAI::setOffpartyBDSWS_Enable( int bEnable )
{
	if( (bEnable != 0) /*&& (this->off_BD_SWS.allEnabled == 0)*/ )
	{
		this->off_BD_SWS.allEnabled = 1;
		this->offBDSWS_nextBDInvTime = GetTickCount() + 500; // ~ current time
		this->offBDSWS_nextSWSInvTime = this->offBDSWS_nextBDInvTime;
		// next dismiss time = next invite time + dismiss timeout
		this->offBDSWS_nextBDDisTime = this->offBDSWS_nextBDInvTime + (this->off_BD_SWS.dismiss_bd_secs * 1000);
		this->offBDSWS_nextSWSDisTime = this->offBDSWS_nextSWSInvTime + (this->off_BD_SWS.dismiss_sws_secs * 1000);
		log_error( LOG_USERAI, "Enabled offparty BD/SWS invite\n" );
		return;
	}
	if( (bEnable == 0) /*&& (this->off_BD_SWS.allEnabled == 1)*/ )
	{
		this->off_BD_SWS.allEnabled = 0;
		log_error( LOG_USERAI, "Disabled offparty BD/SWS invite\n" );
	}
}
