#include "stdafx.h"
#include "UserAI.h"
#include "Logger.h"
#include "ConfigIni.h"

extern CConfig g_cfg;

void UserAI::UAI_Parse_NpcInfo( class UserAI *cls, void *l2_game_packet )
{
	if( !l2_game_packet ) return;
	cls;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	L2Npc npcInfo;

	// parse
	p->getPacketType();
	npcInfo.objectID     = p->readUInt();

	// invis GM left inivs mode?
	if( cls->invisGM.isInvisGM( npcInfo.objectID ) )
	{
		cls->invisGM.deletePossibleInvisGM( npcInfo.objectID );
		log_error_np( LOG_WARNING, "[INVISGM] possible invis GM left invis mode [%u]\n", npcInfo.objectID );
	}

	npcInfo.templateID   = p->readUInt() - 1000000; // ? :) L2J adds 1000000 to this field
	npcInfo.isAttackable = p->readD(); // _isAttackable
	npcInfo.x            = p->readD();
	npcInfo.y            = p->readD();
	npcInfo.z            = p->readD();
	npcInfo.heading      = p->readUInt();
	p->readD(); // 0x00

	npcInfo.mAtkSpd      = p->readD();
	npcInfo.pAtkSpd      = p->readD();
	npcInfo.runSpeed     = p->readD();
	npcInfo.walkSpeed    = p->readD();
	p->readD(); // swim run speed
	p->readD(); // swim walk speed
	p->readD(); // fl run speed (?)
	p->readD(); // fl walk speed (?)
	p->readD(); // fly run speed (?) same as fl
	p->readD(); // fly walk speed (?)

	double moveSpeedMultiplier   = p->readF();
	double attackSpeedMultiplier = p->readF();
	if( moveSpeedMultiplier > 0 )
	{
		npcInfo.runSpeed = (int)( ((double)npcInfo.runSpeed) * moveSpeedMultiplier );
		npcInfo.walkSpeed = (int)( ((double)npcInfo.walkSpeed) * moveSpeedMultiplier );
	}
	if( attackSpeedMultiplier > 0 ) npcInfo.pAtkSpd = (int)( ((double)npcInfo.pAtkSpd) * attackSpeedMultiplier );

	npcInfo.collisionRadius = p->readF();
	npcInfo.collisionHeight = p->readF();

	npcInfo.iid_right_hand = p->readUInt();
	npcInfo.iid_chest      = p->readUInt();
	npcInfo.iid_left_hand  = p->readUInt();

	p->readC(); // has title
	npcInfo.isRunning   = p->readC();
	npcInfo.isInCombat  = p->readC();
	npcInfo.isAlikeDead = p->readC();
	p->readC(); // is summoned

	npcInfo.setName( p->readUnicodeStringPtr() );
	npcInfo.setTitle( p->readUnicodeStringPtr() );

	p->readD(); // title color (0 = client default)
	npcInfo.pvpFlag = p->readD(); // pvp flag
	npcInfo.karma   = p->readD(); // karma

	npcInfo.abnormalEffect = p->readUInt();

	if( g_cfg.L2_version >= (int)L2_VERSION_T23 )
	{
		npcInfo.clanID         = p->readUInt(); // Gracia Final
		npcInfo.clanCrestID    = p->readUInt(); // Gracia Final
		npcInfo.allyID         = p->readUInt(); // Gracia Final
		npcInfo.allyCrestID    = p->readUInt(); // Gracia Final

		npcInfo.isFlying       = p->readC(); // Gracia Final
		p->readC();                          // title color (again??)

		p->readF(); // collisionRadius (again?)
		p->readF(); // collisionHeight (again?)

		p->readD(); // enchantEffect?
		p->readD(); // isFlying? (again?) Gracia Final
		p->readD(); // 0x00
		p->readD(); // pet form and skills (?)
	
		p->readC(); // 0x01
		p->readC(); // 0x01
		p->readD(); // specialEffect
	}

	NpcArray_AddNpcInfo( &npcInfo );
	// p is deleted by calling function
}

