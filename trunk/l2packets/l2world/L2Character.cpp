#include "stdafx.h"
#include "L2Character.h"
#include "../game/L2GamePacket.h"
#include "../os/os_abstraction.h"

L2Character::L2Character()
{
	setUnused();
}

L2Character::~L2Character()
{
	setUnused();
}

L2Character::L2Character( const L2Character& other ): L2Object()
{
	setUnused();
	this->operator=( other );
}

const L2Character& L2Character::operator=( const L2Character& other )
{
	if( this == &other ) return (*this);
	L2Object::operator=( other );
	// copy
	level = other.level;
	setName( other.charName );
	setTitle( other.charTitle );
	xDst = other.xDst;
	yDst = other.yDst;
	zDst = other.zDst;
	heading = other.heading;
	runSpeed = other.runSpeed;
	walkSpeed = other.walkSpeed;
	isRunning = other.isRunning;
	isSitting = other.isSitting;
	isAlikeDead = other.isAlikeDead;
	isFlying = other.isFlying;
	lastMoveTickTime = other.lastMoveTickTime;
	s_STR = other.s_STR;
	s_DEX = other.s_DEX;
	s_CON = other.s_CON;
	s_INT = other.s_INT;
	s_WIT = other.s_WIT;
	s_MEN = other.s_MEN;
	pAtk = other.pAtk;
	pDef = other.pDef;
	mAtk = other.mAtk;
	mDef = other.mDef;
	pAtkSpd = other.pAtkSpd;
	mAtkSpd = other.mAtkSpd;
	accuracy = other.accuracy;
	evasion = other.evasion;
	critical = other.critical;
	curHp = other.curHp;
	maxHp = other.maxHp;
	curMp = other.curMp;
	maxMp = other.maxMp;
	curCp = other.curCp;
	maxCp = other.maxCp;
	abnormalEffect = other.abnormalEffect;
	targetObjectID = other.targetObjectID;
	isInCombat = other.isInCombat;
	collisionRadius = other.collisionRadius;
	collisionHeight = other.collisionHeight;
	pvpFlag = other.pvpFlag;
	karma = other.karma;
	clanID = other.clanID;
	clanCrestID = other.clanCrestID;
	clanCrestLargeID = other.clanCrestLargeID;
	allyID = other.allyID;
	allyCrestID = other.allyCrestID;
	return (*this);
}

void L2Character::setUnused()
{
	L2Object::setUnused();
	stopMove();
	charName[0] = charTitle[0] = 0;
	level = 0;
	heading = 0;
	runSpeed = walkSpeed = isRunning = isSitting = isAlikeDead = 0;
	lastMoveTickTime = 0;
	s_STR = s_DEX = s_CON = s_INT = s_WIT = s_MEN = 0;
	pAtk = pDef = mAtk = mDef = pAtkSpd = mAtkSpd = accuracy = evasion = critical = 0;
	curHp = maxHp = curMp = maxMp = curCp = maxCp = 0.0;
	abnormalEffect = 0;
	targetObjectID = 0;
	isInCombat = 0;
	collisionRadius = collisionHeight = 1.0;
}

void L2Character::setName( const wchar_t *name )
{
	if( name )
	{
		wcsncpy( charName, name, sizeof(charName)/sizeof(charName[0]) - 1 );
		charName[ sizeof(charName)/sizeof(charName[0]) - 1 ] = 0;
	}
}

void L2Character::setName( const char *name )
{
	if( name )
	{
#ifdef L2PACKETS_WINDOWS
		_snwprintf( charName, sizeof(charName)/sizeof(charName[0]), L"%S", name );
#endif
#ifdef L2PACKETS_LINUX
		swprintf( charName, sizeof(charName)/sizeof(charName[0]), L"%ls", name );
#endif
		charName[ sizeof(charName)/sizeof(charName[0]) - 1 ] = 0;
	}
}

void L2Character::setTitle( const wchar_t *title )
{
	if( title )
	{
		wcsncpy( charTitle, title, sizeof(charTitle)/sizeof(charTitle[0]) - 1 );
		charTitle[ sizeof(charTitle)/sizeof(charTitle[0]) - 1 ] = 0;
	}
}

void L2Character::setTitle( const char *title )
{
	if( title )
	{
#ifdef L2PACKETS_WINDOWS
		_snwprintf( charTitle, sizeof(charTitle)/sizeof(charTitle[0]), L"%S", title );
#endif
#ifdef L2PACKETS_LINUX
		swprintf( charTitle, sizeof(charTitle)/sizeof(charTitle[0]), L"%ls", title );
#endif
		charTitle[ sizeof(charTitle)/sizeof(charTitle[0]) - 1 ] = 0;
	}
}

const wchar_t *L2Character::getName()
{
	this->charName[sizeof(this->charName) - 1] = 0;
	return (const wchar_t *)(this->charName);
}

const wchar_t *L2Character::getTitle()
{
	this->charTitle[sizeof(this->charTitle) - 1] = 0;
	return (const wchar_t *)(this->charTitle);
}

bool L2Character::parse_MoveToLocation( void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	p->getPacketType();
	unsigned int oid = p->readUInt();
	if( oid != this->objectID ) return false; // not my move
	xDst = p->readInt();
	yDst = p->readInt();
	zDst = p->readInt();
	x = p->readInt();
	y = p->readInt();
	z = p->readInt();
	this->lastMoveTickTime = OS_GetTickCount(); // last time when x,y,z, xDst,yDst,zDst were known exactly
	return true;
}

void L2Character::processMoveTick()
{
	unsigned long long int curTick = OS_GetTickCount();
	unsigned long long int millisecsPassed = curTick - lastMoveTickTime;
	if( millisecsPassed < 100 ) return;
	lastMoveTickTime = curTick;
	const int near_range = 50; // range in which point is reached
	if( isMoving() )
	{
		int dx = xDst - x;
		int dy = yDst - y;
		int dz = zDst - z;
		if( ((dx > -near_range)&&(dx < near_range)) && 
			((dy > -near_range)&&(dy < near_range)) && 
			((dz > -near_range)&&(dz < near_range)) ) // target ~~ reached?
		{
			stopMove();
			return;
		}
		float useSpeed = (float)runSpeed;
		if( isRunning ) useSpeed = (float)walkSpeed;
		float secs = ((float)millisecsPassed) / 1000;
		float movedDist = useSpeed * secs;
		float totalDist = sqrtf( (float)(dx*dx + dy*dy) );
		float k = totalDist / movedDist;
		if( k < 1.1 ) // cannot be < 1
		{
			stopMove();
			return;
		}
		float xDelta = (float)dx / k;
		float yDelta = (float)dy / k;
		x += (int)xDelta;
		y += (int)yDelta;
	}
}

void L2Character::startMoveTo( int mxd, int myd, int mzd, int mx, int my, int mz )
{
	x = mx;
	y = my;
	z = mz;
	xDst = mxd;
	yDst = myd;
	zDst = mzd;
	lastMoveTickTime = OS_GetTickCount();
}
