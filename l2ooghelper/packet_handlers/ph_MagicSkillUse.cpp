#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/** Server: Len   49 [MagicSkillUse]
31 00
48
6C 08 00 10   // objectId
AC 16 00 10   // targetObjectId
79 05 00 00   // skillID          1401 - Major HEal
0A 00 00 00   // skillLevel       level 10
57 07 00 00   // hitTime          cast time 1879 ms
CF 08 00 00   // reuseDelay       2255 reuse delay
5B AD 00 00   // x
E5 A4 00 00   // y
5D F2 FF FF   // z
00 00 00 00   // ??
00 00 00 00   // ??
00 00         // ??  **/
void L2Client::ph_MagicSkillUse( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	unsigned int objectID = p->readUInt();
	/*unsigned int targetObjectID =*/ p->readUInt();
	unsigned int skillID = p->readUInt();
	unsigned int skillLvl = p->readUInt();
	if( objectID == pcls->usr.objectID )
	{
		pcls->skills.parse_MagicSkillUse( p );
		//pcls->postUpdateUI( UPDATE_SKILL_COOLTIME ); // updated automatically at world tick every 250 ms :)
		//pcls->postUpdateUI( UPDATE_USER_CASTING );
		char askillName[256] = {0};
		wchar_t wskillName[256] = {0};
		L2Data_DB_GetSkillNameByID( skillID, askillName );
		MultiByteToWideChar( CP_ACP, 0, askillName, -1, wskillName, 255 );
		wchar_t wmes[512] = {0};
		swprintf( wmes, 511, L"You use [%s] lvl %u", wskillName, skillLvl );
		pcls->addChatToTab( CHAT_DMG, wmes );
	}
}
