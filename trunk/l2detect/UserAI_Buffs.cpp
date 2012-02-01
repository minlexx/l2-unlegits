#include "stdafx.h"
#include "UserAI.h"

void UserAI::UAI_Parse_AbnormalStatusUpdate( class UserAI *cls, void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	cls->buffs.parse_AbnormalStatusUpdate( p );
}

void UserAI::UAI_Parse_ShortBuffStatusUpdate( class UserAI *cls, void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	cls->buffs.parse_ShortBuffStatusUpdate( p );
}
