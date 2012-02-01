#include "stdafx.h"
#include "L2Npc.h"
#include "../l2data/L2Data.h"
#include "../game/L2GamePacket.h"
#include "../os/os_abstraction.h"

L2Npc::L2Npc()
{
	setUnused();
}

L2Npc::~L2Npc()
{
	setUnused();
}

L2Npc::L2Npc( const L2Npc& other ): L2Character()
{
	setUnused();
	operator=( other );
}

const L2Npc& L2Npc::operator=( const L2Npc& other )
{
	if( this == &other ) return (*this);
	L2Character::operator=( other );
	templateID = other.templateID;
	isAttackable = other.isAttackable;
	iid_left_hand = other.iid_left_hand;
	iid_right_hand = other.iid_right_hand;
	iid_chest = other.iid_chest;
	return (*this);
}

void L2Npc::setUnused()
{
	L2Character::setUnused();
	templateID = 0;
	isAttackable = 0;
	// weapon
	iid_left_hand = iid_right_hand = iid_chest = 0;
}

void L2Npc::setNpcNameTitleByTemplate()
{
	char db_name[256], db_title[256];
	db_name[0] = db_title[0] = 0;
	if( L2Data_DB_GetNPCNameTitleByID( templateID, db_name, db_title ) )
	{
		setName( db_name );
		setTitle( db_title );
	}
}

bool L2Npc::parse_NpcInfo( void *l2_game_packet, L2_VERSION l2_version )
{
	if( !l2_game_packet ) return false;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	// parse
	unsigned char ptype = p->getPacketType();
	if( ptype != 0x0C ) return false; // 0x0C NpcInfo
	//
	objectID     = p->readUInt();
	templateID   = p->readUInt() - 1000000; // ? :) L2J adds 1000000 to this field
	isAttackable = p->readD(); // _isAttackable
	x            = p->readD();
	y            = p->readD();
	z            = p->readD();
	heading      = p->readUInt();
	p->readD(); // 0x00

	mAtkSpd      = p->readD();
	pAtkSpd      = p->readD();
	runSpeed     = p->readD();
	walkSpeed    = p->readD();
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
		runSpeed = (int)( ((double)runSpeed) * moveSpeedMultiplier );
		walkSpeed = (int)( ((double)walkSpeed) * moveSpeedMultiplier );
	}
	if( attackSpeedMultiplier > 0 ) pAtkSpd = (int)( ((double)pAtkSpd) * attackSpeedMultiplier );

	collisionRadius = p->readF();
	collisionHeight = p->readF();

	iid_right_hand = p->readUInt();
	iid_chest      = p->readUInt();
	iid_left_hand  = p->readUInt();

	p->readC(); // has title
	isRunning   = p->readC();
	isInCombat  = p->readC();
	isAlikeDead = p->readC();
	p->readC(); // is summoned

	setName( p->readUnicodeStringPtr() );
	setTitle( p->readUnicodeStringPtr() );

	p->readD(); // title color
	p->readD(); // 0x00
	pvpFlag = p->readD(); // pvp flag

	abnormalEffect = p->readUInt();
	clanID         = p->readUInt(); // Gracia Final
	clanCrestID    = p->readUInt(); // Gracia Final

	isFlying       = p->readC(); // Gracia Final
	p->readC();                          // 0x00

	p->readF(); // collisionRadius (again?)
	p->readF(); // collisionHeight (again?)

	p->readD(); // 0x00
	p->readD(); // isFlying? (again?) Gracia Final
	p->readD(); // 0x00
	p->readD(); // pet form and skills (?)

	// Gracia Final has some more to read
	if( l2_version >= L2_VERSION_T23 )
	{
		p->readC(); // 0x01
		p->readC(); // 0x01
		p->readD(); // 0x00000000
	}

	// set last time when npc coordinates were known exactly
	lastMoveTickTime = OS_GetTickCount();

	return true;
}
