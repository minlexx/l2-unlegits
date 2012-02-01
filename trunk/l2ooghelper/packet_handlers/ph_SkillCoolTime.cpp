#include "stdafx.h"
#include "../Logger.h"
#include "../L2Client.h"

void L2Client::ph_SkillCoolTime( class L2Client *pcls, L2GamePacket *p )
{
	pcls->skills.parse_SkillCoolTime( p );
	pcls->postUpdateUI( UPDATE_SKILLS );
}
