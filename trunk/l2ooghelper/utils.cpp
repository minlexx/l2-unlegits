#include "stdafx.h"
#include "utils.h"
#include "DBLayer.h"

// replace $c1, $s1, $c2, $s2, $S1, $S2 to specified parameter

void Utils_replace_in_SM_template_S( wchar_t *sm_template, int iPos, const wchar_t *str )
{
	int i;
	int ll = (int)wcslen( sm_template );
	wchar_t *sm_new = (wchar_t *)malloc( 8192 );
	if( !sm_new ) return;
	sm_new[0] = 0;
	for( i=0; i<(ll-2); i++ )
	{
		if( sm_template[i] == L'$' )
		{
			if( (sm_template[i+1] == L's') || (sm_template[i+1] == L'c') || (sm_template[i+1] == L'S') )
			{
				wchar_t ciPos[2] = {0,0};
				wsprintfW( ciPos, L"%d", iPos );
				if( sm_template[i+2] == ciPos[0] )
				{
					// copy to sm_new chars from sm_template from position 0 to i-1
					int ii;
					for( ii=0; ii<i; ii++ ) sm_new[ii] = sm_template[ii];
					sm_new[ii] = 0; // close string by '\0'
					wcscat( sm_new, str );             // append string str to sm_new
					wcscat( sm_new, sm_template + i + 3 ); // append rest of sm_template to sm_new
					wcscpy( sm_template, sm_new );      // sm_tmeplate = sm_new
					free( sm_new );
					return;
				}
			}
		}
	}
	free( sm_new );
	return;
}

void Utils_process_SystemMessage( void *l2_game_packet, wchar_t *out )
{
	if( !l2_game_packet || !out ) return;
	L2GamePacket *pack = (L2GamePacket *)l2_game_packet;
	//
	pack->getPacketType();
	//
	out[0] = 0;
	size_t sm_template_chars = 8192;
	size_t sm_template_size = sm_template_chars*2;
	wchar_t *sm_template = (wchar_t *)malloc( sm_template_size );
	if( !sm_template ) return;
	//
	memset( sm_template, 0, sm_template_size );
	const char *sm_a = NULL;
	unsigned int i = 0;
	//
	unsigned int sm_ID = pack->readUInt();
	unsigned int paramCount = pack->readUInt();
	unsigned int sm_type = 0;
	//
	sm_a = L2Data_SM_get( sm_ID );
	if( !sm_a )
	{
		lstrcpyW( out, L"<NULL>" );
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
		sm_type = pack->readUInt();
		switch( sm_type )
		{
		case TYPE_TEXT:
			{
				const wchar_t *str = pack->readUnicodeStringPtr();
				Utils_replace_in_SM_template_S( sm_template, i, str );
			} break;
		case TYPE_NUMBER:
			{
				int n = pack->readInt();
				wchar_t str[16];
				wsprintfW( str, L"%d", n );
				Utils_replace_in_SM_template_S( sm_template, i, str );
			} break;
		case TYPE_NPC_NAME:
			{
				unsigned int npcID = pack->readUInt() - 1000000;
				wchar_t npcName[256] = {0};
				wchar_t npcTitle[256] = {0};
				DBLayer_getNpcNameTitle( npcID, npcName, 255, npcTitle, 255 );
				Utils_replace_in_SM_template_S( sm_template, i, npcName );
			} break;
		case TYPE_ITEM_NAME:
			{
				unsigned int itemID = pack->readUInt();
				wchar_t str[256];
				DBLayer_getItemName( itemID, str, 255 );
				Utils_replace_in_SM_template_S( sm_template, i, str );
			} break;
		case TYPE_SKILL_NAME:
			{
				unsigned int skillID = pack->readUInt();
				int skillLvl = pack->readInt();
				wchar_t str[256];
				wchar_t skillName[256] = {0};
				DBLayer_getSkillName( skillID, skillName, 255 );
				wsprintfW( str, L"%s lvl %d", skillName, skillLvl );
				Utils_replace_in_SM_template_S( sm_template, i, str );
			} break;
		case TYPE_ZONE_NAME:
			{
				int x = pack->readInt();
				int y = pack->readInt();
				int z = pack->readInt();
				wchar_t str[32];
				wsprintfW( str, L"(%d,%d,%d)", x, y, z );
				Utils_replace_in_SM_template_S( sm_template, i, str );
			} break;
		}
	}
	wcscpy( out, sm_template );
	free( sm_template );
}

//for (int i = 0; i < _types.size(); i++)
//{
//	int t = _types.get(i).intValue();
//
//	writeD(t);
//
//	switch (t)
//	{
//		case TYPE_TEXT:
//		{
//			writeS( (String)_values.get(i));
//			break;
//		}
//		case TYPE_NUMBER:
//		case TYPE_NPC_NAME:
//		case TYPE_ITEM_NAME:
//		{
//			int t1 = ((Integer)_values.get(i)).intValue();
//			writeD(t1);
//			break;
//		}
//		case TYPE_SKILL_NAME:
//		{
//			int t1 = ((Integer)_values.get(i)).intValue();
//			writeD(t1); // Skill Id
//			writeD(_skillLvL); // Skill lvl
//			break;
//		}
//		case TYPE_ZONE_NAME:
//		{
//			int t1 = ((int[])_values.get(i))[0];
//			int t2 = ((int[])_values.get(i))[1];
//			int t3 = ((int[])_values.get(i))[2];
//			writeD(t1);
//			writeD(t2);
//			writeD(t3);
//			break;
//		}
//	}
//}

//public SystemMessage addNpcName(int id)
//{
//	_types.add(Integer.valueOf(TYPE_NPC_NAME));
//	_values.add(Integer.valueOf(1000000 + id));
//
//	return this;
//}
