#include "stdafx.h"
#include "../Logger.h"
#include "../L2Client.h"

void L2Client::ph_SkillList( class L2Client *pcls, L2GamePacket *p )
{
	pcls->skills.parse_SkillList( p, pcls->account.getL2Version() );
	pcls->postUpdateUI( UPDATE_SKILLS );
	// send RequestSkillCoolTime
	L2GamePacket *pack = new L2GamePacket;
	pack->setPacketType( 0xA6 ); // RequestSkillCoolTIme
	pcls->sendPacket( pack, true );
	delete pack; pack = NULL;
}
