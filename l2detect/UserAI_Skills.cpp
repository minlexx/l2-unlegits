#include "stdafx.h"
#include "Logger.h"
#include "UserAI.h"

/**
writeC(0x6b);
writeD(_dat1);
writeD(_time);
writeD(_time2);*/
void UserAI::UAI_Parse_SetupGauge( class UserAI *cls, void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	p->getPacketType();
	unsigned int color = p->readD();
	unsigned int currentTime = p->readD();
	//unsigned int maxTime = p->readD();
	//log_error( LOG_DEBUG, "SetupGauge: %u, %u, %u\n", color, currentTime, maxTime );
	switch( color )
	{
	case 0: cls->skills.startCasting( currentTime ); break; // cast
	// case 1: // arrow shoot?
	case 2: // under water breathe
		{
			if( currentTime == 0 )
				log_error( LOG_USERAI, "Not sinking!" );
			else
				log_error( LOG_USERAI, "Sinking under water!" );
		} break;
	}
}

void UserAI::UAI_Parse_SkillList( class UserAI *cls, void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	cls->skills.parse_SkillList( p );
}

void UserAI::UAI_Parse_SkillCoolTime( class UserAI *cls, void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	cls->skills.parse_SkillCoolTime( p );
}

void UserAI::UAI_Parse_MagicSkillUse( class UserAI *cls, void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	p->getPacketType();
	unsigned int oid = p->readUInt();
	if( oid == cls->usr.objectID ) // user cast
		cls->skills.parse_MagicSkillUse( p );
}

void UserAI::UAI_Parse_MagicSkillCancelled( class UserAI *cls, void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	p->getPacketType();
	unsigned int oid = p->readUInt();
	if( oid == cls->usr.objectID )
	{
		cls->skills.stopCasting();
		log_error( LOG_USERAI, "Cast break!\n" );
	}
}
