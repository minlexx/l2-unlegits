#include "stdafx.h"
#include "UserAI.h"
#include "Logger.h"
#include "PacketInjector.h"
#include "GameClient.h"

extern class GameClient *g_game_client;

void UserAI::UAI_handle_keyboard( int vkey )
{
	switch( vkey )
	{
	//case 'Q': UAIKEY_targetNearestTarget(); break;
	//case 'W': UAIKEY_displayConfig(); break;
	case VK_OEM_3: UAIKEY_displayConfig(); break; // '~'
	}
}

void UserAI::UAIKEY_targetNearestTarget()
{
	int i = 0, n = 0, cnt;
	double min_dist = 999999.99;
	unsigned int min_oidTarget = 0;
	wchar_t targetName[64]; targetName[0] = 0;
	CharArray_Lock();
	cnt = CharArray_GetCount();
	// полный перебор всего списка чаров
	for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
	{
		if( chars_array[i] )
		{
			L2Player *pl = chars_array[i];
			bool isDead = (pl->isAlikeDead != 0);
			bool isInParty = ((party.getCount() > 0) ? (party.isInParty( pl->objectID, NULL )) : (false));
			bool isInClan = ((usr.clanID == 0) ? (false) : (usr.clanID == pl->clanID));
			bool isInAlly = ((usr.allyID == 0) ? (false) : (usr.allyID == pl->allyID));
			// не таргетить мертвых, сопартийцев и союзников по клану и алли
			if( !isDead && !isInParty && !isInClan && !isInAlly )
			{
				if(	(pl->classID == 16) || // BP
					(pl->classID == 30) || // EE
					(pl->classID == 97) || // Cardi
					(pl->classID == 105) || // Evas Saint
					(pl->classID == 93) || // Adventurer
					(pl->classID == 101) || // Wind Rider
					(pl->classID == 108)  // Ghost Hunter
				)
				{
					double dx = (double)(pl->x - usr.x);
					double dy = (double)(pl->y - usr.y);
					double dz = abs( (double)(pl->z - usr.z) ); // abs = модуль dz
					double dist = sqrt( dx*dx + dy*dy + dz*dz );
					//log_error_np( LOG_USERAI, "[TARG] Player [%S] is healer... dist = %0.1f...\n", pl->charName, dist );
					if( (dist < 1500.0) && (dist < min_dist) && (dz <= 1000.0) )
					{
						min_dist = dist;
						min_oidTarget = pl->objectID;
						wcscpy( targetName, pl->charName );
					}
				}
			}
			n++;
		}
		if( n >= cnt ) break;
	}
	CharArray_Unlock();
	//
	if( min_oidTarget != 0 )
	{
		log_error_np( LOG_USERAI, "[TARG] decided target [%u] [%S]\n", min_oidTarget, targetName );
		// first cancel current target
		//PGen_RequestTargetCanceld( false ); // just request to cancel target
		// then request target slected player
		if( usr.targetObjectID != min_oidTarget )
			PGen_Action( min_oidTarget, usr.x, usr.y, usr.z, 0 );
		// заявить об этом в пати чат! (Только если в пати)
		/*if( party.getCount() > 0 )
		{
			wchar_t text[512];
			swprintf( text, 511, L"Assist *** %s ***", targetName );
			PGen_Say2C( 3, text ); // 3 - channelID party #
		}*/
	}
	else // пусть будет камент в логе...
		log_error_np( LOG_USERAI, "[TARG] not found anyone to target!\n" );
}

void UserAI::UAIKEY_displayConfig()
{
	wchar_t *text = (wchar_t *)malloc( 8192 );
	if( !text ) return;
	//
	GameClientCounters gcnt;
	GameClient_NetStats gcns;
	g_game_client->getCounters( gcnt );
	gcnt.calcNowStats( &gcns );
	unsigned long long int nowExp = g_game_client->ai.usr.exp;
	double expAdded               = (double)(nowExp - gcnt.ullStartingExp);
	double expPerHour             = expAdded / (gcns.timePassedSecs * 3600.0);
	//
	// determine locktarget status
	wchar_t lockTargetStr[256] = {0};
	if( this->lockTargetMgr.isEnabled() )
		swprintf( lockTargetStr, 255, L"ON [ <a action=\"bypass -h L2Detect_LT_off\">OFF</a> ]" );
	else
		swprintf( lockTargetStr, 255, L"OFF [ <a action=\"bypass -h L2Detect_LT_on\">ON</a> ]" );
	//
	swprintf( text, 8191,
		L"<html><title>L2Detect config</title>"
		L"<body>"
		L"Lock target: %s<br1>"
		L"===== Stats: =====<br1>"
		L"Time: %0.1f secs<br1>"
		L"Exp added: %0.1f<br1>"
		L"Exp/hour: %0.2f<br1>"
		L"Avg recv spd: %0.1f byte/sec (%0.1f pkt/sec)<br1>"
		L"Avg send spd: %0.1f byte/sec (%0.1f pkt/sec)<br1>"
		L"Avg pkt size: IN: %0.1f; OUT: %0.1f<br1>"
		L"[ <a action=\"bypass -h L2Detect_resetStats\">Reset stats</a> ]<br>"
		L"</body></html>",
		lockTargetStr,
		gcns.timePassedSecs, expAdded, expPerHour,
		gcns.avgRecvSpeed, gcns.avgRecvPacketsPerSec,
		gcns.avgSendSpeed, gcns.avgSendPacketsPerSec,
		gcns.avgRecvPacketSize, gcns.avgSendPacketSize );
	// check str length
	size_t slen = wcslen( text );
	if( slen >= 8192 )
	{
		log_error( LOG_WARNING, "UserAI::UAIKEY_displayConfig(): HTML text may be too long (%u)\n", slen );
		ErrorLogger_FlushLogFile();
	}
	//
	PGenC_NpcHtmlMessage( 5, text );
	if( text ) free( text );
	text = NULL;
}

// if packet should be dropped:
//     set (*newLen) to -1
bool UserAI::UAI_process_RequestBypassToServer( unsigned char *bytes, unsigned int len, unsigned char **newBytes, unsigned int *newLen )
{
	if( !bytes || (len<3) || !newBytes || !newLen ) return false;
	L2GamePacket p;
	p.setBytes( bytes, len );
	unsigned char opcode = p.getPacketType();
	if( (opcode != 0x23) && (opcode != 0x22) ) return false; // must be RequestBypassToServer or RequestLinkHtml
	wchar_t *cmd = p.readUnicodeString();
	if( !cmd ) return false;
	// parse command // ex.: bypass [L2Detect_resetStats]
	if( opcode == 0x22 )
		log_error( LOG_USERAI, "RequestLinkHtml: [%S]\n", cmd );
	if( opcode == 0x23 )
		log_error( LOG_USERAI, "RequestBypassToServer: [%S]\n", cmd );
	if( !wcsstr( cmd, L"L2Detect_" ) )
	{
		// this is not radar setup bypass, send it to server unchanged
		free( cmd );
		return false;
	}
	(*newLen) = (unsigned int)-1; // mark packet to drop, this is radar config bypass
	log_error_np( LOG_OK, "UserAI BYPASS [%S]\n", cmd );
	if( wcscmp( cmd, L"L2Detect_resetStats" ) == 0 )
	{
		g_game_client->resetCounters( true );
		g_game_client->setCountersStartingExp( this->usr.exp );
		g_game_client->startCountersTimer();
	}
	else if( wcscmp( cmd, L"L2Detect_LT_off" ) == 0 )
	{
		g_game_client->ai.lockTargetMgr.setEnable( false );
	}
	else if( wcscmp( cmd, L"L2Detect_LT_on" ) == 0 )
	{
		g_game_client->ai.lockTargetMgr.setEnable( true );
	}
	//
	free( cmd ); // cleanup
	return true;
}
