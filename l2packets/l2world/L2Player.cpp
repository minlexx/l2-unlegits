#include "stdafx.h"
#include "L2Player.h"
#include "../l2data/L2Data.h"
#include "../game/L2GamePacket.h"
#include "../os/os_abstraction.h"

L2Player::L2Player()
{
	setUnused();
}

L2Player::~L2Player()
{
	setUnused();
}

L2Player::L2Player( const L2Player& other ): L2Character()
{
	setUnused();
	this->operator=( other );
}

const L2Player& L2Player::operator=( const L2Player& other )
{
	if( this == &other ) return (*this); // self-assign
	L2Character::operator=( other );
	//
	int i;
	//
	classID = other.classID;
	baseClassID = other.baseClassID;
	race = other.race;
	sex = other.sex;
	hairStyle = other.hairStyle;
	hairColor = other.hairColor;
	face = other.face;
	experience = other.experience;
	skill_points = other.skill_points;
	curLoad = other.curLoad;
	maxLoad = other.maxLoad;
	pkKills = other.pkKills;
	pvpKills = other.pvpKills;
	enchantEffect = other.enchantEffect;
	relation = other.relation;
	autoAttackable = other.autoAttackable;
	for( i=0; i<32; i++ )
	{
		paperdoll_oid[i] = other.paperdoll_oid[i];
		paperdoll_iid[i] = other.paperdoll_iid[i];
		paperdoll_augid[i] = other.paperdoll_augid[i];
	}
	isFakeDeath = other.isFakeDeath;
	isFishing = other.isFishing;
	fishX = other.fishX;
	fishY = other.fishY;
	fishZ = other.fishZ;
	privateStoreType = other.privateStoreType;
	wcsncpy( privateStoreMsgBuy, other.privateStoreMsgBuy, 64 );
	privateStoreMsgBuy[63] = 0;
	wcsncpy( privateStoreMsgSell, other.privateStoreMsgSell, 64 );
	privateStoreMsgSell[63] = 0;
	wcsncpy( privateStoreMsgRecipe, other.privateStoreMsgRecipe, 64 );
	privateStoreMsgRecipe[63] = 0;
	mountType = other.mountType;
	mountNpcId = other.mountNpcId;
	recomLeft = other.recomLeft;
	recomHave = other.recomHave;
	isNoble = other.isNoble;
	isHero = other.isHero;
	nameColor = other.nameColor;
	titleColor = other.titleColor;
	pledgeClass = other.pledgeClass;
	pledgeType = other.pledgeType;
	cursedWeaponId = other.cursedWeaponId;
	clanRepScore = other.clanRepScore;
	transformId = other.transformId;
	agathionId = other.agathionId;
	vehicleObjectId = other.vehicleObjectId;
	hasDwarvenCraft = other.hasDwarvenCraft;
	numCubics = other.numCubics;
	for( i=0; i<4; i++ ) cubicId[i] = other.cubicId[i];
	clanPrivs = other.clanPrivs;
	inventoryLimit = other.inventoryLimit;
	elements = other.elements;
	fame = other.fame;
	vitalityLevel = other.vitalityLevel;
	//
	return (*this);
}

void L2Player::setUnused()
{
	L2Character::setUnused();
	classID = baseClassID = 0;
	clanID = clanCrestID = clanCrestLargeID = allyID = allyCrestID = 0;
	race = sex = 0;
	hairStyle = hairColor = face = 0;
	experience = 0;
	skill_points = 0;
	curLoad = maxLoad = 0;
	pkKills = pvpKills = 0;
	relation = RELATION_NONE;
	autoAttackable = 0;
	int i;
	for( i=0; i<32; i++ )
	{
		paperdoll_oid[i] = 0;
		paperdoll_iid[i] = 0;
		paperdoll_augid[i] = 0;
	}
	isFishing = 0;
	fishX = fishY = fishZ = 0;
	privateStoreType = 0;
	privateStoreMsgBuy[0] = privateStoreMsgSell[0] = privateStoreMsgRecipe[0] = 0;
	mountType = mountNpcId = 0;
	recomLeft = recomHave = 0;
	isNoble = isHero = 0;
	nameColor = titleColor = 0;
	pledgeClass = pledgeType = 0;
	cursedWeaponId = transformId = agathionId = 0;
	clanRepScore = 0;
	vehicleObjectId = 0;
	hasDwarvenCraft = 0;
	numCubics = 0;
	for( i=0; i<4; i++ ) cubicId[i] = 0;
	clanPrivs = 0;
	inventoryLimit = 0;
	elements.clear();
}

void L2Player::getRaceStr( wchar_t *out ) const
{
	if( !out ) return;
	out[0] = 0;
	const char *ansi = L2Data_getRace( this->race );
#ifdef L2PACKETS_WINDOWS
	_snwprintf( out, 32, L"%S", ansi );
#endif
#ifdef L2PACKETS_LINUX
	swprintf( out, 32, L"%ls", ansi );
#endif
	out[31] = 0;
}

void L2Player::getSexStr( wchar_t *out ) const
{
	if( !out ) return;
	out[0] = 0;
	const char *ansi = L2Data_getSex( this->sex );
#ifdef L2PACKETS_WINDOWS
	_snwprintf( out, 32, L"%S", ansi );
#endif
#ifdef L2PACKETS_LINUX
	swprintf( out, 32, L"%ls", ansi );
#endif
	out[31] = 0;
}

void L2Player::getClassStr( wchar_t *out ) const
{
	if( !out ) return;
	out[0] = 0;
	const char *ansi = L2Data_getClass( this->classID );
#ifdef L2PACKETS_WINDOWS
	_snwprintf( out, 32, L"%S", ansi );
#endif
#ifdef L2PACKETS_LINUX
	swprintf( out, 32, L"%ls", ansi );
#endif
	out[31] = 0;
}

void L2Player::getBaseClassStr( wchar_t *out ) const
{
	if( !out ) return;
	out[0] = 0;
	const char *ansi = L2Data_getClass( this->baseClassID );
#ifdef L2PACKETS_WINDOWS
	_snwprintf( out, 32, L"%S", ansi );
#endif
#ifdef L2PACKETS_LINUX
	swprintf( out, 32, L"%ls", ansi );
#endif
}

bool L2Player::parse_CharInfo( void *l2_game_packet, L2_VERSION l2_version )
{
	if( !l2_game_packet ) return false;
	int i;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;

	// parse
	unsigned char ptype = p->getPacketType();    // 0x31
	if( ptype != 0x31 ) return false;

	x = p->readInt();                       // x
	y = p->readInt();                       // y
	z = p->readInt();                       // z
	p->readUInt();                          // 0x00
	objectID = p->readUInt();               // objectID

	setName( p->readUnicodeStringPtr() );   // name
	race        = p->readUInt();            // race
	sex         = p->readInt();             // sex (0-male)
	baseClassID = p->readUInt();            // base class

	// paperdoll item IDs
	paperdoll_iid[L2_PAPERDOLL_UNDER]     = p->readUInt(); // PAPERDOLL_UNDER
	paperdoll_iid[L2_PAPERDOLL_HEAD]      = p->readUInt(); // PAPERDOLL_HEAD
	paperdoll_iid[L2_PAPERDOLL_RHAND]     = p->readUInt(); // PAPERDOLL_RHAND
	paperdoll_iid[L2_PAPERDOLL_LHAND]     = p->readUInt(); // PAPERDOLL_LHAND
	paperdoll_iid[L2_PAPERDOLL_GLOVES]    = p->readUInt(); // PAPERDOLL_GLOVES
	paperdoll_iid[L2_PAPERDOLL_CHEST]     = p->readUInt(); // PAPERDOLL_CHEST
	paperdoll_iid[L2_PAPERDOLL_LEGS]      = p->readUInt(); // PAPERDOLL_LEGS
	paperdoll_iid[L2_PAPERDOLL_FEET]      = p->readUInt(); // PAPERDOLL_FEET
	paperdoll_iid[L2_PAPERDOLL_BACK]      = p->readUInt(); // PAPERDOLL_BACK
	paperdoll_iid[L2_PAPERDOLL_LRHAND]    = p->readUInt(); // PAPERDOLL_LRHAND
	paperdoll_iid[L2_PAPERDOLL_HAIR]      = p->readUInt(); // PAPERDOLL_HAIR
	paperdoll_iid[L2_PAPERDOLL_HAIR2]     = p->readUInt(); // PAPERDOLL_HAIR2
	// kamael
	paperdoll_iid[L2_PAPERDOLL_RBRACELET] = p->readUInt(); // PAPERDOLL_RBRACELET
	paperdoll_iid[L2_PAPERDOLL_LBRACELET] = p->readUInt(); // PAPERDOLL_LBRACELET
	paperdoll_iid[L2_PAPERDOLL_DECO1]     = p->readUInt(); // PAPERDOLL_DECO1
	paperdoll_iid[L2_PAPERDOLL_DECO2]     = p->readUInt(); // PAPERDOLL_DECO2
	paperdoll_iid[L2_PAPERDOLL_DECO3]     = p->readUInt(); // PAPERDOLL_DECO3
	paperdoll_iid[L2_PAPERDOLL_DECO4]     = p->readUInt(); // PAPERDOLL_DECO4
	paperdoll_iid[L2_PAPERDOLL_DECO5]     = p->readUInt(); // PAPERDOLL_DECO5
	paperdoll_iid[L2_PAPERDOLL_DECO6]     = p->readUInt(); // PAPERDOLL_DECO6
	//
	if( l2_version >= L2_VERSION_T23 )
		paperdoll_iid[L2_PAPERDOLL_BELT] = p->readUInt(); // PAPERDOLL_BELT

	// paperdoll augmentation IDs
	paperdoll_augid[L2_PAPERDOLL_UNDER]     = p->readUInt(); // PAPERDOLL_UNDER
	paperdoll_augid[L2_PAPERDOLL_HEAD]      = p->readUInt(); // PAPERDOLL_HEAD
	paperdoll_augid[L2_PAPERDOLL_RHAND]     = p->readUInt(); // PAPERDOLL_RHAND
	paperdoll_augid[L2_PAPERDOLL_LHAND]     = p->readUInt(); // PAPERDOLL_LHAND
	paperdoll_augid[L2_PAPERDOLL_GLOVES]    = p->readUInt(); // PAPERDOLL_GLOVES
	paperdoll_augid[L2_PAPERDOLL_CHEST]     = p->readUInt(); // PAPERDOLL_CHEST
	paperdoll_augid[L2_PAPERDOLL_LEGS]      = p->readUInt(); // PAPERDOLL_LEGS
	paperdoll_augid[L2_PAPERDOLL_FEET]      = p->readUInt(); // PAPERDOLL_FEET
	paperdoll_augid[L2_PAPERDOLL_BACK]      = p->readUInt(); // PAPERDOLL_BACK
	paperdoll_augid[L2_PAPERDOLL_LRHAND]    = p->readUInt(); // PAPERDOLL_LRHAND
	paperdoll_augid[L2_PAPERDOLL_HAIR]      = p->readUInt(); // PAPERDOLL_HAIR
	paperdoll_augid[L2_PAPERDOLL_HAIR2]     = p->readUInt(); // PAPERDOLL_HAIR2
	// kamael
	paperdoll_augid[L2_PAPERDOLL_RBRACELET] = p->readUInt(); // PAPERDOLL_RBRACELET
	paperdoll_augid[L2_PAPERDOLL_LBRACELET] = p->readUInt(); // PAPERDOLL_LBRACELET
	paperdoll_augid[L2_PAPERDOLL_DECO1]     = p->readUInt(); // PAPERDOLL_DECO1
	paperdoll_augid[L2_PAPERDOLL_DECO2]     = p->readUInt(); // PAPERDOLL_DECO2
	paperdoll_augid[L2_PAPERDOLL_DECO3]     = p->readUInt(); // PAPERDOLL_DECO3
	paperdoll_augid[L2_PAPERDOLL_DECO4]     = p->readUInt(); // PAPERDOLL_DECO4
	paperdoll_augid[L2_PAPERDOLL_DECO5]     = p->readUInt(); // PAPERDOLL_DECO5
	paperdoll_augid[L2_PAPERDOLL_DECO6]     = p->readUInt(); // PAPERDOLL_DECO6
	//
	if( l2_version >= L2_VERSION_T23 )
		paperdoll_augid[L2_PAPERDOLL_BELT] = p->readUInt(); // PAPERDOLL_BELT

	// Gracia Final 2.3
	if( l2_version == L2_VERSION_T23 )
	{
		p->readD();
		p->readD();
	}

	pvpFlag = p->readUInt();   // pvp flag
	karma   = p->readInt();    // karma

	mAtkSpd = p->readInt();  // cast speed
	pAtkSpd = p->readInt();  // atk speed

	pvpFlag = p->readUInt();   // pvp flag
	karma   = p->readInt();    // karma

	runSpeed  = p->readInt();  // run spd
	walkSpeed = p->readInt();  // walk spd
	p->readInt();                  // _swimRunSpd  (?)
	p->readInt();                  // _swimWalkSpd (?)
	p->readInt();                  // _flRunSpd   (?)
	p->readInt();                  // _flWalkSpd  (?)
	p->readInt();                  // _flyRunSpd  (?)
	p->readInt();                  // _flyWalkSpd (?)
	double moveMult = p->readDouble();  // move spd mult
	double atkMult  = p->readDouble();  // atk spd mult

	// apply multipliers
	if( moveMult > 0 )
	{
		runSpeed  = (int)( ((double)(runSpeed)) * moveMult );
		walkSpeed = (int)( ((double)(walkSpeed)) * moveMult );
	}
	if( atkMult > 0 ) pAtkSpd = (int)( ((double)(pAtkSpd)) * atkMult );

	collisionRadius = p->readF();      // collisionRadius
	collisionHeight = p->readF();      // collisionHeight

	hairStyle = p->readInt();
	hairColor = p->readInt();
	face      = p->readInt();

	setTitle( p->readUnicodeStringPtr() ); // title

	clanID      = p->readUInt();
	clanCrestID = p->readInt();
	allyID      = p->readInt();
	allyCrestID = p->readInt();
	p->readInt();                        // relation(?), which is always 0

	isSitting   = (int)(!p->readUChar()); // standing = 1  sitting = 0
	isRunning   = (int)p->readUChar();    // running = 1   walking = 0
	isInCombat  = (int)p->readUChar();
	isAlikeDead = (int)p->readUChar();

	p->readUChar();                         // invisible = 1  visible =0

	this->mountType        = p->readUChar(); // 1-on Strider, 2-on Wyvern, 3-on Great Wolf, 0-no mount
	this->privateStoreType = p->readUChar(); // 1 - sellshop

	// cubics
	numCubics = (int)p->readUShort();   // cubics count
	if( numCubics > 4 ) numCubics = 4;
	for( i=0; i<numCubics; i++ )
		cubicId[i] = (unsigned int)p->readUShort();   // cubic ID

	p->readUChar();                      // find party members
	abnormalEffect = p->readUInt(); // abnormalEffect

	recomLeft = (int)p->readUChar();
	recomHave = (int)p->readUShort(); // Blue value for name (0 = white, 255 = pure blue)
	mountNpcId = p->readUInt() - 1000000; // mountNpcId + 1000000

	classID = p->readUInt(); // class ID
	p->readUInt();           // 0x00
	enchantEffect = (unsigned int)p->readUChar();

	p->readUChar(); // team circle around feet 1 = Blue, 2 = red

	clanCrestLargeID = p->readUInt();
	isNoble = p->readUChar();
	isHero  = p->readUChar(); // hero aura

	isFishing = p->readUChar(); // 0x01: Fishing Mode (Cant be undone by setting back to 0)
	fishX = p->readInt();
	fishY = p->readInt();
	fishZ = p->readInt();

	nameColor    = p->readInt();
	heading      = p->readUInt();

	pledgeClass = p->readUInt(); // PledgeClass
	pledgeType  = p->readUInt(); // PledgeType

	titleColor = p->readUInt(); // TitleColor

	cursedWeaponId = p->readUInt(); // CursedWeaponEquippedId

	clanRepScore = p->readInt(); // clan ReputationScore

	// T1
	transformId = p->readUInt(); // TransformationId
	agathionId  = p->readUInt(); // AgathionId

	// T2
	//writeD(0x01);
	
	// T2.3
    //writeD(0x00);
    //writeD(0x00);
    //writeD(0x00);
    //writeD(0x00);

	// set last time when char coordinates were known exactly
	lastMoveTickTime = OS_GetTickCount();

	return true;
}

bool L2Player::parse_UserInfo( void *l2_game_packet, L2_VERSION l2_version )
{
	if( !l2_game_packet ) return false;

	unsigned int tempUINT;
	int i;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;

	unsigned char ptype = p->getPacketType();
	if( ptype != 0x32 ) return false;

	x = p->readInt();
	y = p->readInt();
	z = p->readInt();

	if( l2_version <= L2_VERSION_T22 )      heading = p->readUInt();
	else if( l2_version >= L2_VERSION_T23 ) vehicleObjectId = p->readUInt();

	objectID = p->readUInt();

	setName( p->readUnicodeStringPtr() );
	race = p->readInt();
	sex  = p->readInt();

	baseClassID = p->readUInt();

	level      = p->readInt();
	experience = p->readUInt64();

	s_STR = p->readInt(); // writeD(_activeChar.getSTR());
	s_DEX = p->readInt(); // writeD(_activeChar.getDEX());
	s_CON = p->readInt(); // writeD(_activeChar.getCON());
	s_INT = p->readInt(); // writeD(_activeChar.getINT());
	s_WIT = p->readInt(); // writeD(_activeChar.getWIT());
	s_MEN = p->readInt(); // writeD(_activeChar.getMEN());

	maxHp        = (double)p->readInt(); // writeD(_activeChar.getMaxHp());
	curHp        = (double)p->readInt(); // writeD((int) _activeChar.getCurrentHp());
	maxMp        = (double)p->readInt(); // writeD(_activeChar.getMaxMp());
	curMp        = (double)p->readInt(); //writeD((int) _activeChar.getCurrentMp());
	skill_points = p->readUInt(); // writeD(_activeChar.getSp());
	curLoad      = p->readUInt(); // writeD(_activeChar.getCurrentLoad());
	maxLoad      = p->readUInt(); // writeD(_activeChar.getMaxLoad());

	//writeD(_activeChar.getActiveWeaponItem() != null ? 0x40 : 0x20); // 0x20 no weapon, 0x40 weapon equipped
	bool isWeaponEquipped = false; // why?... cannot we check weapon paperdoll objectId/itemId?
	tempUINT = p->readUInt();
	if( tempUINT == 0x40 ) isWeaponEquipped = true;

	// paperdoll and its augments
	// paperdoll objectIDs (25 items)
	paperdoll_oid[L2_PAPERDOLL_UNDER]     = p->readUInt(); // Inventory.PAPERDOLL_UNDER));
	paperdoll_oid[L2_PAPERDOLL_REAR]      = p->readUInt(); // writeD(....PAPERDOLL_REAR));
	paperdoll_oid[L2_PAPERDOLL_LEAR]      = p->readUInt(); // writeD(....PAPERDOLL_LEAR));
	paperdoll_oid[L2_PAPERDOLL_NECK]      = p->readUInt(); // writeD(....PAPERDOLL_NECK));
	paperdoll_oid[L2_PAPERDOLL_RFINGER]   = p->readUInt(); // writeD(....PAPERDOLL_RFINGER));
	paperdoll_oid[L2_PAPERDOLL_LFINGER]   = p->readUInt(); // writeD(....PAPERDOLL_LFINGER));
	paperdoll_oid[L2_PAPERDOLL_HEAD]      = p->readUInt(); // writeD(....PAPERDOLL_HEAD));
	paperdoll_oid[L2_PAPERDOLL_RHAND]     = p->readUInt(); // writeD(....PAPERDOLL_RHAND));
	paperdoll_oid[L2_PAPERDOLL_LHAND]     = p->readUInt(); // writeD(....PAPERDOLL_LHAND));
	paperdoll_oid[L2_PAPERDOLL_GLOVES]    = p->readUInt(); // writeD(....PAPERDOLL_GLOVES));
	paperdoll_oid[L2_PAPERDOLL_CHEST]     = p->readUInt(); // writeD(....PAPERDOLL_CHEST));
	paperdoll_oid[L2_PAPERDOLL_LEGS]      = p->readUInt(); // writeD(....PAPERDOLL_LEGS));
	paperdoll_oid[L2_PAPERDOLL_FEET]      = p->readUInt(); // writeD(....PAPERDOLL_FEET));
	paperdoll_oid[L2_PAPERDOLL_BACK]      = p->readUInt(); // writeD(....PAPERDOLL_BACK));
	paperdoll_oid[L2_PAPERDOLL_LRHAND]    = p->readUInt(); // writeD(....PAPERDOLL_LRHAND));
	paperdoll_oid[L2_PAPERDOLL_HAIR]      = p->readUInt(); // writeD(....PAPERDOLL_HAIR));
	paperdoll_oid[L2_PAPERDOLL_HAIR2]     = p->readUInt(); // writeD(....PAPERDOLL_HAIR2));
	// T1 new D's
	paperdoll_oid[L2_PAPERDOLL_RBRACELET] = p->readUInt(); // writeD(....PAPERDOLL_RBRACELET));
	paperdoll_oid[L2_PAPERDOLL_LBRACELET] = p->readUInt(); // writeD(....PAPERDOLL_LBRACELET));
	paperdoll_oid[L2_PAPERDOLL_DECO1]     = p->readUInt(); // writeD(....PAPERDOLL_DECO1));
	paperdoll_oid[L2_PAPERDOLL_DECO2]     = p->readUInt(); // writeD(....PAPERDOLL_DECO2));
	paperdoll_oid[L2_PAPERDOLL_DECO3]     = p->readUInt(); // writeD(....PAPERDOLL_DECO3));
	paperdoll_oid[L2_PAPERDOLL_DECO4]     = p->readUInt(); // writeD(....PAPERDOLL_DECO4));
	paperdoll_oid[L2_PAPERDOLL_DECO5]     = p->readUInt(); // writeD(....PAPERDOLL_DECO5));
	paperdoll_oid[L2_PAPERDOLL_DECO6]     = p->readUInt(); // writeD(....PAPERDOLL_DECO6));
	if( l2_version >= L2_VERSION_T23 )
		paperdoll_oid[L2_PAPERDOLL_BELT]  = p->readUInt(); // PAPERDOLL_BELT

	// paperdoll itemIDs (25 items)
	paperdoll_iid[L2_PAPERDOLL_UNDER]     = p->readUInt(); // Inventory.PAPERDOLL_UNDER));
	paperdoll_iid[L2_PAPERDOLL_REAR]      = p->readUInt(); // writeD(....PAPERDOLL_REAR));
	paperdoll_iid[L2_PAPERDOLL_LEAR]      = p->readUInt(); // writeD(....PAPERDOLL_LEAR));
	paperdoll_iid[L2_PAPERDOLL_NECK]      = p->readUInt(); // writeD(....PAPERDOLL_NECK));
	paperdoll_iid[L2_PAPERDOLL_RFINGER]   = p->readUInt(); // writeD(....PAPERDOLL_RFINGER));
	paperdoll_iid[L2_PAPERDOLL_LFINGER]   = p->readUInt(); // writeD(....PAPERDOLL_LFINGER));
	paperdoll_iid[L2_PAPERDOLL_HEAD]      = p->readUInt(); // writeD(....PAPERDOLL_HEAD));
	paperdoll_iid[L2_PAPERDOLL_RHAND]     = p->readUInt(); // writeD(....PAPERDOLL_RHAND));
	paperdoll_iid[L2_PAPERDOLL_LHAND]     = p->readUInt(); // writeD(....PAPERDOLL_LHAND));
	paperdoll_iid[L2_PAPERDOLL_GLOVES]    = p->readUInt(); // writeD(....PAPERDOLL_GLOVES));
	paperdoll_iid[L2_PAPERDOLL_CHEST]     = p->readUInt(); // writeD(....PAPERDOLL_CHEST));
	paperdoll_iid[L2_PAPERDOLL_LEGS]      = p->readUInt(); // writeD(....PAPERDOLL_LEGS));
	paperdoll_iid[L2_PAPERDOLL_FEET]      = p->readUInt(); // writeD(....PAPERDOLL_FEET));
	paperdoll_iid[L2_PAPERDOLL_BACK]      = p->readUInt(); // writeD(....PAPERDOLL_BACK));
	paperdoll_iid[L2_PAPERDOLL_LRHAND]    = p->readUInt(); // writeD(....PAPERDOLL_LRHAND));
	paperdoll_iid[L2_PAPERDOLL_HAIR]      = p->readUInt(); // writeD(....PAPERDOLL_HAIR));
	paperdoll_iid[L2_PAPERDOLL_HAIR2]     = p->readUInt(); // writeD(....PAPERDOLL_HAIR2));
	// T1 new D's
	paperdoll_iid[L2_PAPERDOLL_RBRACELET] = p->readUInt(); // writeD(....PAPERDOLL_RBRACELET));
	paperdoll_iid[L2_PAPERDOLL_LBRACELET] = p->readUInt(); // writeD(....PAPERDOLL_LBRACELET));
	paperdoll_iid[L2_PAPERDOLL_DECO1]     = p->readUInt(); // writeD(....PAPERDOLL_DECO1));
	paperdoll_iid[L2_PAPERDOLL_DECO2]     = p->readUInt(); // writeD(....PAPERDOLL_DECO2));
	paperdoll_iid[L2_PAPERDOLL_DECO3]     = p->readUInt(); // writeD(....PAPERDOLL_DECO3));
	paperdoll_iid[L2_PAPERDOLL_DECO4]     = p->readUInt(); // writeD(....PAPERDOLL_DECO4));
	paperdoll_iid[L2_PAPERDOLL_DECO5]     = p->readUInt(); // writeD(....PAPERDOLL_DECO5));
	paperdoll_iid[L2_PAPERDOLL_DECO6]     = p->readUInt(); // writeD(....PAPERDOLL_DECO6));
	if( l2_version >= L2_VERSION_T23 )
		paperdoll_iid[L2_PAPERDOLL_BELT]  = p->readUInt(); // PAPERDOLL_BELT

	// paperdoll augmentation IDs (25 items)
	//for( i=0; i<25; i++ ) p->readUInt();
	paperdoll_augid[L2_PAPERDOLL_UNDER]   = p->readUInt(); // Inventory.PAPERDOLL_UNDER));
	paperdoll_augid[L2_PAPERDOLL_REAR]    = p->readUInt(); // writeD(....PAPERDOLL_REAR));
	paperdoll_augid[L2_PAPERDOLL_LEAR]    = p->readUInt(); // writeD(....PAPERDOLL_LEAR));
	paperdoll_augid[L2_PAPERDOLL_NECK]    = p->readUInt(); // writeD(....PAPERDOLL_NECK));
	paperdoll_augid[L2_PAPERDOLL_RFINGER] = p->readUInt(); // writeD(....PAPERDOLL_RFINGER));
	paperdoll_augid[L2_PAPERDOLL_LFINGER] = p->readUInt(); // writeD(....PAPERDOLL_LFINGER));
	paperdoll_augid[L2_PAPERDOLL_HEAD]    = p->readUInt(); // writeD(....PAPERDOLL_HEAD));
	paperdoll_augid[L2_PAPERDOLL_RHAND]   = p->readUInt(); // writeD(....PAPERDOLL_RHAND));
	paperdoll_augid[L2_PAPERDOLL_LHAND]   = p->readUInt(); // writeD(....PAPERDOLL_LHAND));
	paperdoll_augid[L2_PAPERDOLL_GLOVES]  = p->readUInt(); // writeD(....PAPERDOLL_GLOVES));
	paperdoll_augid[L2_PAPERDOLL_CHEST]   = p->readUInt(); // writeD(....PAPERDOLL_CHEST));
	paperdoll_augid[L2_PAPERDOLL_LEGS]    = p->readUInt(); // writeD(....PAPERDOLL_LEGS));
	paperdoll_augid[L2_PAPERDOLL_FEET]    = p->readUInt(); // writeD(....PAPERDOLL_FEET));
	paperdoll_augid[L2_PAPERDOLL_BACK]    = p->readUInt(); // writeD(....PAPERDOLL_BACK));
	paperdoll_augid[L2_PAPERDOLL_LRHAND]  = p->readUInt(); // writeD(....PAPERDOLL_LRHAND));
	paperdoll_augid[L2_PAPERDOLL_HAIR]    = p->readUInt(); // writeD(....PAPERDOLL_HAIR));
	paperdoll_augid[L2_PAPERDOLL_HAIR2]   = p->readUInt(); // writeD(....PAPERDOLL_HAIR2));
	// T1 new D's
	paperdoll_augid[L2_PAPERDOLL_RBRACELET] = p->readUInt(); // writeD(....PAPERDOLL_RBRACELET));
	paperdoll_augid[L2_PAPERDOLL_LBRACELET] = p->readUInt(); // writeD(....PAPERDOLL_LBRACELET));
	paperdoll_augid[L2_PAPERDOLL_DECO1]   = p->readUInt(); // writeD(....PAPERDOLL_DECO1));
	paperdoll_augid[L2_PAPERDOLL_DECO2]   = p->readUInt(); // writeD(....PAPERDOLL_DECO2));
	paperdoll_augid[L2_PAPERDOLL_DECO3]   = p->readUInt(); // writeD(....PAPERDOLL_DECO3));
	paperdoll_augid[L2_PAPERDOLL_DECO4]   = p->readUInt(); // writeD(....PAPERDOLL_DECO4));
	paperdoll_augid[L2_PAPERDOLL_DECO5]   = p->readUInt(); // writeD(....PAPERDOLL_DECO5));
	paperdoll_augid[L2_PAPERDOLL_DECO6]   = p->readUInt(); // writeD(....PAPERDOLL_DECO6));
	if( l2_version >= L2_VERSION_T23 )
		paperdoll_augid[L2_PAPERDOLL_BELT] = p->readUInt(); // PAPERDOLL_BELT

	// T2.3 some 2 Ds
	if( l2_version >= L2_VERSION_T23 )
	{
		p->readD();
		p->readD();
	}

	pAtk      = p->readInt();    // writeD(_activeChar.getPAtk(null));
	pAtkSpd   = p->readInt(); // writeD(_activeChar.getPAtkSpd());
	pDef      = p->readInt();    // writeD(_activeChar.getPDef(null));
	evasion   = p->readInt(); // writeD(_activeChar.getEvasionRate(null));
	accuracy  = p->readInt(); // writeD(_activeChar.getAccuracy());
	critical  = p->readInt(); // writeD(_activeChar.getCriticalHit(null, null));
	mAtk      = p->readInt();     // writeD(_activeChar.getMAtk(null, null));
	mAtkSpd   = p->readInt(); // writeD(_activeChar.getMAtkSpd());
	pAtkSpd   = p->readInt(); // writeD(_activeChar.getPAtkSpd());
	mDef      = p->readInt(); // writeD(_activeChar.getMDef(null, null));

	pvpFlag   = p->readInt(); // writeD(_activeChar.getPvpFlag()); // 0-non-pvp  1-pvp = violett name
	karma     = p->readInt(); // writeD(_activeChar.getKarma());

	runSpeed  = p->readInt();  // writeD(_runSpd);
	walkSpeed = p->readInt(); // writeD(_walkSpd);
	p->readInt(); // writeD(_swimRunSpd); // swimspeed
	p->readInt(); // writeD(_swimWalkSpd); // swimspeed
	p->readInt(); // writeD(_flRunSpd);
	p->readInt(); // writeD(_flWalkSpd);
	p->readInt(); // writeD(_flyRunSpd);
	p->readInt(); // writeD(_flyWalkSpd);
	double moveMultiplier        = p->readDouble(); // writeF(_moveMultiplier);
	double attackSpeedMultiplier = p->readDouble(); // writeF(_activeChar.getAttackSpeedMultiplier());
	if( moveMultiplier > 0.0 )
	{
		runSpeed  = (int)( ((double)(runSpeed)) * moveMultiplier + 0.5 );
		walkSpeed = (int)( ((double)(walkSpeed)) * moveMultiplier + 0.5 );
	}
	if( attackSpeedMultiplier > 0.0 )
	{
		pAtkSpd = (int)( ((double)(pAtkSpd)) * attackSpeedMultiplier);
	}

	collisionRadius = p->readDouble(); // writeF(_activeChar.getBaseTemplate().collisionRadius);
	collisionHeight = p->readDouble(); // writeF(_activeChar.getBaseTemplate().collisionHeight);

	hairStyle = p->readInt(); // writeD(_activeChar.getAppearance().getHairStyle());
	hairColor = p->readInt(); // writeD(_activeChar.getAppearance().getHairColor());
	face      = p->readInt(); // writeD(_activeChar.getAppearance().getFace());
	/*isGm      =*/ p->readInt(); // writeD(_activeChar.isGM() ? 1 : 0); // builder level

	setTitle( p->readS() ); // writeS(title);

	clanID           = p->readUInt(); // writeD(_activeChar.getClanId());
	clanCrestID      = p->readUInt(); // writeD(_activeChar.getClanCrestId());
	allyID           = p->readUInt(); // writeD(_activeChar.getAllyId());
	allyCrestID      = p->readUInt(); // writeD(_activeChar.getAllyCrestId()); // ally crest id
	// 0x40 leader rights
	// siege flags: attacker - 0x180 sword over name, defender - 0x80 shield, 0xC0 crown (|leader), 0x1C0 flag (|leader)
	relation         = p->readUInt(); // writeD(_relation);
	mountType        = p->readUChar(); // writeC(_activeChar.getMountType()); // mount type
	privateStoreType = (int)p->readUChar(); // writeC(_activeChar.getPrivateStoreType());
	hasDwarvenCraft  = p->readUChar(); // writeC(_activeChar.hasDwarvenCraft() ? 1 : 0);
	pkKills          = p->readInt(); // writeD(_activeChar.getPkKills());
	pvpKills         = p->readInt(); // writeD(_activeChar.getPvpKills());

	numCubics        = p->readUShort(); // writeH(_activeChar.getCubics().size());
	if( numCubics > 4 ) numCubics = 4; // safe
	for( i=0; i<numCubics; i++ )
		cubicId[i] = p->readUShort(); //for (int id : _activeChar.getCubics().keySet()) writeH(id);

	p->readUChar(); // writeC(0x00); //1-find party members

	abnormalEffect = p->readUInt(); // writeD(_activeChar.getAbnormalEffect());
	p->readChar(); // writeC(0x00);

	clanPrivs      = p->readUInt(); // writeD(_activeChar.getClanPrivileges());

	recomLeft      = p->readUShort(); // writeH(_activeChar.getRecomLeft());
	recomHave      = p->readUShort(); // writeH(_activeChar.getRecomHave());
	mountNpcId     = p->readUInt(); // writeD(_activeChar.getMountNpcId() + 1000000);
	inventoryLimit = p->readUShort(); // writeH(_activeChar.getInventoryLimit());

	classID        = p->readUInt(); // writeD(_activeChar.getClassId().getId());
	p->readUInt(); // writeD(0x00); // special effects? circles around player...
	maxCp          = (double)p->readInt(); // writeD(_activeChar.getMaxCp());
	curCp          = (double)p->readInt(); // writeD((int) _activeChar.getCurrentCp());
	enchantEffect  = p->readUChar(); // writeC(_activeChar.getEnchantEffect());

	p->readUChar(); // writeC(0x00); //team circle around feet 1= Blue, 2 = red

	clanCrestLargeID = p->readUInt(); // writeD(_activeChar.getClanCrestLargeId());
	isNoble          = p->readUChar(); // writeC(_activeChar.isNoble() ? 1 : 0);
	isHero           = p->readUChar(); // writeC(_activeChar.isHero()

	isFishing = p->readUChar(); // writeC(_activeChar.isFishing() ? 1 : 0); //Fishing Mode
	fishX     = p->readUInt(); // writeD(_activeChar.getFishx()); //fishing x
	fishY     = p->readUInt(); // writeD(_activeChar.getFishy()); //fishing y
	fishZ     = p->readUInt(); // writeD(_activeChar.getFishz()); //fishing z
	nameColor = p->readUInt(); // writeD(_activeChar.getAppearance().getNameColor());

	//new c5
	isRunning = (int)p->readUChar(); // writeC(_activeChar.isRunning() ? 0x01 : 0x00); //changes the Speed display on Status Window

	pledgeClass = p->readUInt(); // writeD(_activeChar.getPledgeClass()); //changes the text above CP on Status Window
	pledgeType  = p->readUInt(); // writeD(_activeChar.getPledgeType());

	titleColor  = p->readUInt(); // writeD(_activeChar.getAppearance().getTitleColor());

    //	writeD(CursedWeaponsManager.getInstance().getLevel(_activeChar.getCursedWeaponEquippedId()));
	cursedWeaponId = p->readUInt(); // cursedWeapon
    
    // T1 Starts
    transformId = p->readUInt(); // writeD(_activeChar.getTransformationId());
    
	if( l2_version < L2_VERSION_T23 )
	{
		elements.attackElementType  = p->readD(); // writeD(_activeChar.getAttackElement());
		elements.attackElementValue = p->readD(); // writeD(_activeChar.getAttackElementValue());
		elements.defAttrFire        = p->readD(); // writeD(_activeChar.getDefAttrFire());
		elements.defAttrWater       = p->readD(); // writeD(_activeChar.getDefAttrWater());
		elements.defAttrWind        = p->readD(); // writeD(_activeChar.getDefAttrWind());
		elements.defAttrEarth       = p->readD(); // writeD(_activeChar.getDefAttrEarth());
		elements.defAttrHoly        = p->readD(); // writeD(_activeChar.getDefAttrHoly());
		elements.defAttrUnholy      = p->readD(); // writeD(_activeChar.getDefAttrUnholy());
	}
	else
	{
		elements.attackElementType  = p->readH(); // writeH(attackAttribute);
		elements.attackElementValue = p->readH(); // writeH(_activeChar.getAttackElementValue(attackAttribute));
		elements.defAttrFire        = p->readH(); // writeH(_activeChar.getDefenseElementValue(Elementals.FIRE));
		elements.defAttrWater       = p->readH(); // writeH(_activeChar.getDefenseElementValue(Elementals.WATER));
		elements.defAttrWind        = p->readH(); // writeH(_activeChar.getDefenseElementValue(Elementals.WIND));
		elements.defAttrEarth       = p->readH(); // writeH(_activeChar.getDefenseElementValue(Elementals.EARTH));
		elements.defAttrHoly        = p->readH(); // writeH(_activeChar.getDefenseElementValue(Elementals.HOLY));
		elements.defAttrUnholy      = p->readH(); // writeH(_activeChar.getDefenseElementValue(Elementals.DARK));
	}
    
    agathionId = p->readUInt();  // writeD(_activeChar.getAgathionId());
    
	// T2
	if( l2_version >= L2_VERSION_T2 )
	{
		fame = p->readUInt();          // writeD(_activeChar.getFame());  // Fame
		p->readUInt();                 // writeD(0x00); // Unknown // 0x01 in CT2.3
		vitalityLevel = p->readUInt(); // writeD(_activeChar.getVitalityLevel());  // Vitality Level
	}

	// T2.3
	//if( l2_version >= L2_VERSION_T23 )
	//{
		//writeD(0x00); // CT2.3
        //writeD(0x00); // CT2.3
        //writeD(0x00); // CT2.3
        //writeD(0x00); // CT2.3
	//}

	// set last time when char coordinates were known exactly
	lastMoveTickTime = OS_GetTickCount();

	return true;
}
