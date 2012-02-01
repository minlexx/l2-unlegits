#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"
#include "utils.h"

/**
 * ѕакет с подтверждением при саммоне персонажей скилом Summon Friend и подобных.
 * "Nemu wishes to summon you from Imperial Tomb. Do you accept?"
 */
void L2Client::ph_ConfirmDlg( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	//
	size_t sm_template_chars = 8192;
	size_t sm_template_size = sm_template_chars*2;
	wchar_t *sm_template = (wchar_t *)malloc( sm_template_size );
	if( !sm_template )
	{
		log_error( LOG_ERROR, "ph_ConfirmDlg(): malloc failed!\n" );
		return;
	}
	//
	memset( sm_template, 0, sm_template_size );
	const char *sm_a = NULL;
	unsigned int i = 0;
	//
	unsigned int sm_ID = p->readUInt();
	unsigned int paramCount = p->readUInt();
	unsigned int sm_type = 0;
	unsigned int timeLimit = 0;
	unsigned int requestId = 0;
	const wchar_t *text = NULL; // C/—++ не обнул€ет переменные автоматически
	//
	sm_a = L2Data_SM_get( sm_ID );
	if( !sm_a )
	{
		log_error( LOG_ERROR, "ph_ConfirmDlg(): cannot find system message with id = %u\n", sm_ID );
		free( sm_template );
		return;
	}
	MultiByteToWideChar( CP_ACP, 0, sm_a, -1, sm_template, sm_template_chars-1 );
	//
	const unsigned int TYPE_ZONE_NAME = 7;
	const unsigned int TYPE_SKILL_NAME = 4;
	const unsigned int TYPE_ITEM_NAME = 3;
	const unsigned int TYPE_NPC_NAME = 2;
	const unsigned int TYPE_NUMBER = 1;
	const unsigned int TYPE_TEXT = 0;
	//
	for( i = 1;   i < (paramCount + 1);   i++ )
	{
		sm_type = p->readUInt();
		switch( sm_type )
		{
		case TYPE_TEXT:
			{
				const wchar_t *str = p->readUnicodeStringPtr();
				text = str;
				Utils_replace_in_SM_template_S( sm_template, i, str );
			} break;
		case TYPE_NUMBER:
			{
				int n = p->readInt();
				wchar_t str[16];
				wsprintfW( str, L"%d", n );
				Utils_replace_in_SM_template_S( sm_template, i, str );
			} break;
		case TYPE_NPC_NAME:
			{
				unsigned int npcID = p->readUInt() - 1000000;
				wchar_t str[256];
				char npcName[256] = {0};
				L2Data_DB_GetNPCNameTitleByID( npcID, npcName, NULL );
				wsprintfW( str, L"%S", npcName );
				Utils_replace_in_SM_template_S( sm_template, i, str );
			} break;
		case TYPE_ITEM_NAME:
			{
				unsigned int itemID = p->readUInt();
				wchar_t str[256];
				char itemName[256] = {0};
				L2Data_DB_GetItemNamePicByID( itemID, itemName, NULL );
				wsprintfW( str, L"%S", itemName );
				Utils_replace_in_SM_template_S( sm_template, i, str );
			} break;
		case TYPE_SKILL_NAME:
			{
				unsigned int skillID = p->readUInt();
				int skillLvl         = p->readInt();
				wchar_t str[256];
				char skillName[256];
				L2Data_DB_GetSkillNameByID( skillID, skillName );
				wsprintfW( str, L"%S lvl %d", skillName, skillLvl );
				Utils_replace_in_SM_template_S( sm_template, i, str );
			} break;
		case TYPE_ZONE_NAME:
			{
				int x = p->readInt();
				int y = p->readInt();
				int z = p->readInt();
				wchar_t str[32];
				wsprintfW( str, L"(%d,%d,%d)", x, y, z );
				Utils_replace_in_SM_template_S( sm_template, i, str );
			} break;
		}
	}
	timeLimit = p->readUInt();
	requestId = p->readUInt();
	//
	//log_error_np( LOG_OK, "ConfirmDlg: SystemMessage: [%S]\n", sm_template );
	//log_error_np( LOG_OK, "ConfirmDlg: timeout = %u, requestId = %u\n", timeLimit, requestId );
	//free( sm_template );
	pcls->handle_ConfirmDlg( sm_template, sm_ID, requestId, timeLimit, text );
}
