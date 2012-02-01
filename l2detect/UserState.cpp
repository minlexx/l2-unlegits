#include "stdafx.h"
#include "UserState.h"
#include "Logger.h"
#include "ConfigIni.h"
#include "GameClient.h"

extern class CConfig g_cfg;
extern class GameClient *g_game_client;

UserState::UserState()
{
	objectID = 0;
	clear();
}

UserState::~UserState()
{
	clear();
}

void UserState::clear()
{
	charName[0] = 0;
	charTitle[0] = 0;
	level = 0;
	exp = 0;
	sp = 0;
	clanID = allyID = 0;
	relation = 0;
	// hp, mp, cp
	hp = hp_max = 0;
	mp = mp_max = 0;
	cp = cp_max = 0;
	// load weight
	curLoad = maxLoad = 0;
	// some stats
	pAtk = pDef = mAtk = mDef = pAtkSpd = mAtkSpd = 0;
	evasion = accuracy = critical = 0;
	// coords
	x = y = z = 0;
	heading = 0;
	xDst = yDst = zDst = 0x7FFFFFFF; // move stopped
	run_speed = 0; walk_speed = 0;
	isRunning = isSitting = 0;
	isDead = 0;
	// race, sex
	race = sex = 0;
	baseClass = classID = 0;
	// pvp
	pvpflag = 0;
	karma = 0;
	pvpkills = pkkills = 0;
	// target
	targetObjectID = 0;
	targetMaxHP = targetCurHP = 0;
	// timers
	lastTick = GetTickCount();
	// paperdoll
	int i;
	for( i=0; i<32; i++ )
	{
		paperdoll_oid[i] = paperdoll_iid[i] = paperdoll_augid[i] = 0;
	}
}

TCHAR *UserState::getClassStr()
{
#ifdef UNICODE
	wsprintfW( unicode_str, L"%S", L2Data_getClass( classID ) );
	return unicode_str;
#else
	return L2Data_getClass( classID );
#endif
}

TCHAR *UserState::getBaseClassStr()
{
#ifdef UNICODE
	wsprintfW( unicode_str, L"%S", L2Data_getClass( baseClass ) );
	return unicode_str;
#else
	return L2Data_getClass( baseClass );
#endif
}

TCHAR *UserState::getRaceStr()
{
#ifdef UNICODE
	wsprintfW( unicode_str, L"%S", L2Data_getRace( race ) );
	return unicode_str;
#else
	return L2Data_getRace( race );
#endif
}


void UserState::parse_UserInfo( void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	int i = 0;

	// parse
	p->getPacketType();
	x = p->readInt();
	y = p->readInt();
	z = p->readInt();
	// heading from CT2.3 no longer used inside userinfo, here is now vehicle id (boat,airship)
	if( g_cfg.L2_version <= L2_VERSION_T22 )      heading = p->readUInt();
	else if( g_cfg.L2_version >= L2_VERSION_T23 ) vehicleID = p->readUInt();
	objectID = p->readUInt();

	const wchar_t *temp = p->readUnicodeStringPtr();
	if( temp ) { wcscpy( charName, temp ); }
	race      = p->readUInt();
	sex       = p->readUInt();
	baseClass = p->readUInt();
	level     = p->readUInt();
	exp       = p->readUInt64();
	// pass str, dex, con, int, wit, men
	p->readInt(); p->readInt(); p->readInt();
	p->readInt(); p->readInt(); p->readInt();
	// max hp, cur hp, max mp, cur mp
	hp_max = p->readUInt();
	hp     = p->readUInt();
	mp_max = p->readUInt();
	mp     = p->readUInt();
	// sp, currentLoad, maxLoad
	sp      = p->readInt();
	curLoad = p->readInt();
	maxLoad = p->readInt();
	p->readInt(); // 0x20 no weapon, 0x40 weapon equipped

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
	if( g_cfg.L2_version >= (int)L2_VERSION_T23 )
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
	if( g_cfg.L2_version >= (int)L2_VERSION_T23 )
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
	if( g_cfg.L2_version >= (int)L2_VERSION_T23 )
		paperdoll_augid[L2_PAPERDOLL_BELT] = p->readUInt(); // PAPERDOLL_BELT

	// T2.3 some 2 Ds
	if( g_cfg.L2_version >= (int)L2_VERSION_T23 )
	{
		p->readD(); // talisman count
		p->readD(); // cloak status
	}

	// 10 Ds - palyer stats
	// PAtk PAtkSpd PDef EvasionRate Accuracy
	// CriticalHit MAtk MAtkSpd PAtkSpd MDef
	pAtk = p->readD(); pAtkSpd = p->readD(); pDef = p->readD();
	evasion  = p->readD(); // evas
	accuracy = p->readD(); // accu
	critical = p->readD(); // crit
	mAtk = p->readD(); mAtkSpd = p->readD(); p->readD(); /*pAtkSpd?*/ mDef = p->readD();
	// pvpflag, karma
	pvpflag = p->readInt();
	karma   = p->readInt();
	// some 8 Ds - speeds
	// _runSpd   _walkSpd   _swimRunSpd _swimWalkSpd
	// _flRunSpd _flWalkSpd _flyRunSpd  _flyWalkSpd
	run_speed = p->readInt();
	walk_speed = p->readInt();
	for( i=0; i<6; i++ ) p->readInt();

	// 2 doubles
	double moveSpdMult   = p->readDouble(); // _moveMultiplier
	double attackSpdMult = p->readDouble(); // attackSpeedMultiplier
	if( moveSpdMult > 0 ) // apply multiplier
	{
		run_speed  = (int)( ((double)run_speed) * moveSpdMult );
		walk_speed = (int)( ((double)walk_speed) * moveSpdMult );
	}
	if( attackSpdMult > 0 ) // apply multiplier
		pAtkSpd  = (int)( ((double)pAtkSpd) * attackSpdMult );

	// 2 doubles - collisionRadius collisionHeight
	p->readDouble(); p->readDouble();
	// 4 Ds - appearance: hairSt, hairCol, face, is GM
	p->readInt(); p->readInt(); p->readInt(); p->readInt();
	// title
	temp = p->readUnicodeStringPtr();
	if( temp ) { wcscpy( charTitle, temp ); }
	
	clanID = p->readUInt(); //writeD(_activeChar.getClanId());
	p->readUInt(); //writeD(_activeChar.getClanCrestId());
	allyID = p->readUInt(); //writeD(_activeChar.getAllyId());
	p->readUInt(); //writeD(_activeChar.getAllyCrestId());
	relation = p->readUInt(); // writeD(_relation);
	//writeC(_activeChar.getMountType()); // mount type
	//writeC(_activeChar.getPrivateStoreType());
	//writeC(_activeChar.hasDwarvenCraft() ? 1 : 0);
	p->readUChar(); p->readUChar(); p->readUChar();
	// pvpkills, pkkills (order changed in Epilogue, but we ignore: pkkills, pvpkills)
	pvpkills = p->readUInt();
	pkkills  = p->readUInt();
	// num cubics
	int numCubics = (int)p->readUShort();
	for( i=0; i<numCubics; i++ ) p->readUShort();
	
	p->readChar();//writeC(0x00); //1-find party members
	abnormalEffect = p->readInt(); //writeD(_activeChar.getAbnormalEffect());
	p->readChar(); //writeC(0x00); // isFlyingMounted
	p->readInt();  //writeD(_activeChar.getClanPrivileges());
	p->readShort(); //writeH(_activeChar.getRecomLeft()); //c2  recommendations remaining
	p->readShort(); //writeH(_activeChar.getRecomHave()); //c2  recommendations received
	p->readInt();  //writeD(_activeChar.getMountNpcId() + 1000000);
	p->readShort(); //writeH(_activeChar.getInventoryLimit());

	classID = p->readUInt(); //writeD(_activeChar.getClassId().getId());
	p->readInt(); //writeD(0x00); // special effects? circles around player...
	cp_max = p->readUInt(); //writeD(_activeChar.getMaxCp());
	cp = p->readUInt(); //writeD((int) _activeChar.getCurrentCp());
	//
	p->readChar(); // _activeChar.getEnchantEffect());
	p->readChar(); //team circle around feet 1= Blue, 2 = red

	p->readInt(); //writeD(_activeChar.getClanCrestLargeId());
	p->readChar(); //writeC(_activeChar.isNoble() ? 1 : 0); //0x01: symbol on char menu ctrl+I
	p->readChar(); //writeC(_activeChar.isHero() || (_activeChar.isGM() && Config.GM_HERO_AURA) ? 1 : 0); //0x01: Hero Aura
	p->readChar(); //writeC(_activeChar.isFishing() ? 1 : 0); //Fishing Mode
	p->readInt(); p->readInt(); p->readInt(); // fishing x,y,z
	p->readInt(); //writeD(_activeChar.getAppearance().getNameColor());
	////new c5
	isRunning = (int)p->readUChar(); //writeC(_activeChar.isRunning() ? 0x01 : 0x00);
	//writeD(_activeChar.getPledgeClass()); //changes the text above CP on Status Window
	//writeD(_activeChar.getPledgeType());
	//writeD(_activeChar.getAppearance().getTitleColor());
	//writeD(CursedWeaponsManager.getInstance().getLevel(_activeChar.getCursedWeaponEquippedId()));
	//// T1 Starts (change writeD to writeH for >= T23)
	//writeD(_activeChar.getTransformationId());
	//writeD(_activeChar.getAttackElement());
	//writeD(_activeChar.getAttackElementValue());
	//writeD(_activeChar.getDefAttrFire());
	//writeD(_activeChar.getDefAttrWater());
	//writeD(_activeChar.getDefAttrWind());
	//writeD(_activeChar.getDefAttrEarth());
	//writeD(_activeChar.getDefAttrHoly());
	//writeD(_activeChar.getDefAttrUnholy());
	//
	//writeD(_activeChar.getAgathionId());
	//// T2 Starts
	//writeD(_activeChar.getFame());  // Fame
	//writeD(0x00); // Unknown // 0x01 in T23
	// <= T22
	//writeD(_activeChar.getVitalityLevel());  // Vitality Level
	// >= T23
	//writeD(_activeChar.getVitalityPoints());  // Vitality Points
    //writeD(_activeChar.getSpecialEffect());
    //writeD(0x00); // CT2.3
    //writeD(0x00); // CT2.3
    //writeD(0x00); // CT2.3
	//
	lastTick = GetTickCount();

	// log abnormal effect
	//if( abnormalEffect > 0 )
	//{
	//	FILE *f = fopen( "abnormalEffect.log", "at" );
	//	fprintf( f, "%08X (%u)\n", abnormalEffect );
	//	fclose( f );
	//}

	if( g_game_client )
		g_game_client->setCountersStartingExp( exp );
}

//void UserState::parse_StatusUpdate( const unsigned char *bytes, unsigned int length )
void UserState::parse_StatusUpdate( void *l2_game_packet )
{
	const int LEVEL = 0x01;
	const int EXP = 0x02;
	//const int STR = 0x03;
	//const int DEX = 0x04;
	//const int CON = 0x05;
	//const int INT = 0x06;
	//const int WIT = 0x07;
	//const int MEN = 0x08;

	const int CUR_HP = 0x09;
	const int MAX_HP = 0x0a;
	const int CUR_MP = 0x0b;
	const int MAX_MP = 0x0c;

	const int SP = 0x0d;
	const int CUR_LOAD = 0x0e;
	const int MAX_LOAD = 0x0f;

	const int P_ATK = 0x11;
	const int ATK_SPD = 0x12;
	const int P_DEF = 0x13;
	//const int EVASION = 0x14;
	//const int ACCURACY = 0x15;
	//const int CRITICAL = 0x16;
	const int M_ATK = 0x17;
	const int CAST_SPD = 0x18;
	const int M_DEF = 0x19;
	const int PVP_FLAG = 0x1a;
	const int KARMA = 0x1b;

	const int CUR_CP = 0x21;
	const int MAX_CP = 0x22;

	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	int i = 0;
	p->getPacketType();

	// we parse only own statusUpdate or our target's statusUpdate
	unsigned int new_oid = p->readUInt();
	if( (new_oid != objectID ) && (new_oid != targetObjectID) )
	{
		//log_error( LOG_USERAI, "Not my or not our target's statusUpdate!\n" );
		return;
	}

	int numberOfAttributes = p->readInt();
	//if( numberOfAttributes > 1 )
	//	log_error( LOG_WARNING, "StatusUpdate: nAttributes = %d\n", numberOfAttributes );
	int attr_id, attr_val;
	for( i=0; i<numberOfAttributes; i++ )
	{
		attr_id = p->readUInt();
		attr_val = p->readUInt();
		switch( attr_id )
		{
		case CUR_HP:
			{
				if( new_oid == objectID ) this->hp = attr_val;
				else if( new_oid == targetObjectID ) this->targetCurHP = attr_val;
			} break;
		case MAX_HP:
			{
				if( new_oid == objectID ) this->hp_max = attr_val;
				else if( new_oid == targetObjectID ) this->targetMaxHP = attr_val;
			} break;
		case CUR_MP:
			{
				if( new_oid == objectID ) this->mp = attr_val;
			} break;
		case MAX_MP:
			{
				if( new_oid == objectID ) this->mp_max = attr_val;
			} break;
		case CUR_CP:
			{
				if( new_oid == objectID ) this->cp = attr_val;
			} break;
		case MAX_CP:
			{
				if( new_oid == objectID ) this->cp_max = attr_val;
			} break;
		case CUR_LOAD:
			{
				if( new_oid == objectID ) this->curLoad = attr_val;
			} break;
		case MAX_LOAD:
			{
				if( new_oid == objectID ) this->maxLoad = attr_val;
			} break;
		case LEVEL:
			{
				if( new_oid == objectID ) this->level = attr_val;
			} break;
		case EXP:
			{
				if( new_oid == objectID ) this->exp = (unsigned long long int)attr_val;
			} break;
		case SP:
			{
				if( new_oid == objectID ) this->sp = attr_val;
			} break;
		case P_ATK:
			{
				if( new_oid == objectID ) this->pAtk = attr_val;
			} break;
		case ATK_SPD:
			{
				if( new_oid == objectID ) this->pAtkSpd = attr_val;
			} break;
		case P_DEF:
			{
				if( new_oid == objectID ) this->pDef = attr_val;
			} break;
		case M_ATK:
			{
				if( new_oid == objectID ) this->mAtk = attr_val;
			} break;
		case CAST_SPD:
			{
				if( new_oid == objectID ) this->mAtkSpd = attr_val;
			} break;
		case M_DEF:
			{
				if( new_oid == objectID ) this->mDef = attr_val;
			} break;
		case KARMA:
			{
				if( new_oid == objectID ) this->karma = attr_val;
			} break;
		case PVP_FLAG:
			{
				if( new_oid == objectID ) this->pvpflag = attr_val;
			} break;
		}
	}
}

/** protected final void writeImpl()
{
	writeC(0x2f);

	writeD(_charObjId);

	writeD(_xDst);
	writeD(_yDst);
	writeD(_zDst);

	writeD(_x);
	writeD(_y);
	writeD(_z);
} */
bool UserState::parse_MoveToLocation( void *l2_game_packet )
{
	//if( !l2_game_packet ) return;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	p->getPacketType();
	unsigned int oid = p->readUInt();
	if( oid != this->objectID ) return false;
	//log_error( LOG_OK, "My! MoveToLocation!\n" );
	xDst = p->readInt();
	yDst = p->readInt();
	zDst = p->readInt();
	x = p->readInt();
	y = p->readInt();
	z = p->readInt();
	//log_error( LOG_OK, "going (%d,%d,%d) -> (%d,%d,%d)\n", x,y,z, xDst,yDst,zDst );
	lastTick = GetTickCount();
	return true;
}

void UserState::processMoveTick()
{
	unsigned int curTick = GetTickCount();
	unsigned int millisecsPassed = curTick - lastTick;
	if( millisecsPassed < 1000 ) return;
	lastTick = curTick;
	if( (xDst != 0x7FFFFFFF) && (yDst != 0x7FFFFFFF) )
	{
		int dx = xDst - x;
		int dy = yDst - y;
		if( ((dx > -50)&&(dx < 50)) && ((dy > -50)&&(dy < 50)) ) // target ~ reached?
		{
			xDst = 0x7FFFFFFF; // stop "move"
			yDst = 0x7FFFFFFF;
			//log_error( LOG_OK, "UserState::processMoveTick(): stopped move because dx and dy < 50 (%d,%d)\n", dx, dy );
			return;
		}
		float useSpeed = (float)run_speed;
		if( !isRunning ) useSpeed = (float)walk_speed;
		float secs = ((float)millisecsPassed) / 1000;
		float movedDist = useSpeed * secs;
		float totalDist = sqrtf( (float)(dx*dx + dy*dy) );
		float k = totalDist / movedDist;
		if( k < 1.1 ) // cannot be < 1
		{
			xDst = 0x7FFFFFFF; // stop "move"
			yDst = 0x7FFFFFFF;
			//log_error( LOG_OK, "UserState::processMoveTick(): topped move because k < 1.1 (%0.2f)\n", k );
			return;
		}
		float xDelta = (float)dx / k;
		float yDelta = (float)dy / k;
		//log_error( LOG_OK, "move tick: dx,dy %d,%d\n", dx,dy );
		//log_error( LOG_OK, "move tick: useSpeed(%0.2f) secs(%0.2f) movedDist(%0.2f) totalDist(%0.2f) k = %0.2f\n",
		//	useSpeed, secs, movedDist, totalDist, k );
		//log_error( LOG_OK, "move tick: xDelta, yDelta = %0.2f, %0.2f\n", xDelta, yDelta );
		x += (int)xDelta;
		y += (int)yDelta;
	}
}
