#include "stdafx.h"
#include "UserSkills.h"
#include "logger.h"
#include "DBLayer.h"

UserSkill::UserSkill()
{
	skillID = level = coolTimeReuse = coolTimeRemaining = 0;
	isPassive = 1;
	castStartTick = 0;
	isDisabled = isEnchanted = 0;
}

UserSkill::~UserSkill()
{
	skillID = level = coolTimeReuse = coolTimeRemaining = 0;
	isPassive = 1;
	isDisabled = isEnchanted = 0;
}

UserSkill::UserSkill( const UserSkill& other )
{
	this->operator=( other );
}

UserSkill& UserSkill::operator=( const UserSkill& other )
{
	this->skillID = other.skillID;
	this->level = other.level;
	this->isPassive = other.isPassive;
	this->coolTimeReuse = other.coolTimeReuse;
	this->coolTimeRemaining = other.coolTimeRemaining;
	this->isDisabled = other.isDisabled;
	this->isEnchanted = other.isEnchanted;
	return (*this);
}

void UserSkill::setUnused()
{
	skillID = 0;
	level = 0;
	isPassive = 1;
	coolTimeReuse = coolTimeRemaining = 0;
	castStartTick = 0;
	isDisabled = isEnchanted = 0;
}

void UserSkill::getSkillName( wchar_t *out, size_t maxCount )
{
	DBLayer_getSkillName( skillID, out, maxCount );
}

void UserSkill::process_coolTime( unsigned int curTick )
{
	//if( skillID == 0 ) return;
	//if( castStartTick < 1 ) return;
	unsigned int passed = curTick - castStartTick;
	if( passed > coolTimeReuse )
	{
		//coolTimeReuse = 0;
		coolTimeRemaining = 0;
		castStartTick = 0;
	}
	else
	{
		coolTimeRemaining = coolTimeReuse - passed;
	}
}




UserSkills::UserSkills() { clear(); }
UserSkills::~UserSkills() { clear(); }

void UserSkills::clear()
{
	castMsecLeft = 0;
	castMsecStart = 0;
	castMsecTickStart = 0;
	skillCount = 0;
	int i;
	for( i=0; i<USERSKILL_MAX_SKILLS; i++ ) skill[i].setUnused();
}

int UserSkills::addSkill( UserSkill *addskill )
{
	int add_idx = -1;
	int i;
	for( i=0; i<USERSKILL_MAX_SKILLS; i++ )
	{
		if( skill[i].skillID == 0 )
		{
			add_idx = i;
			break;
		}
	}
	if( add_idx == -1 ) return add_idx;
	skillCount++;
	//skill[add_idx].setUnused();
	skill[add_idx] = (*addskill);
	//log_error( LOG_DEBUG, "Added skill: %u [%S] lv %u (passive: %d)\n",
	//	addskill->skillID, addskill->getSkillNameW(), addskill->level, addskill->isPassive );
	return add_idx;
}

int UserSkills::getSkillInfoBySkillId( unsigned int skillID, UserSkill *outskill )
{
	int i;
	int idx = -1;
	int nPassed = 0;
	for( i=0; i<USERSKILL_MAX_SKILLS; i++ )
	{
		if( skill[i].isUnused() ) continue;
		nPassed++;
		if( this->skill[i].skillID == skillID )
		{
			idx = i;
			break;
		}
		// we watched all skills we have? no need to look other USERSKILL_MAX_SKILLS
		if( nPassed >= skillCount ) break;
	}
	if( idx == -1 ) return idx;
	(*outskill) = this->skill[idx];
	return idx;
}

void UserSkills::parse_SkillList( L2GamePacket *p, L2_VERSION ver )
{
	if( !p ) return;
	this->clear();
	p->getPacketType();             // writeC(0x5f);
	int nSkills = p->readInt();     // writeD(_skills.size());
	int i;
	for( i=0; i<nSkills; i++ )      // for (Skill temp : _skills)
	{
		UserSkill s;
		s.isPassive = p->readInt(); // writeD(temp.passive ? 1 : 0);
		s.level = p->readUInt();    // writeD(temp.level);
		s.skillID = p->readUInt();  // writeD(temp.id);
		s.isDisabled = p->readChar(); // writeC(temp.disabled ? 1 : 0);
		if( ver >= L2_VERSION_T24 )
			s.isEnchanted = p->readChar(); // writeC(temp.enchanted ? 1 : 0);
		addSkill( &s );
	}
}

void UserSkills::parse_SkillCoolTime( L2GamePacket *p )
{
	if( !p ) return;
	unsigned int i, n, skillID, reuse, remaining;
	p->getPacketType();             // writeC(0xc7);
	n = p->readInt();               // writeD(_reuseTimeStamps.size()); // list size
	//log_error( LOG_OK, "SkillCoolTime for %u skills\n", n );
	unsigned int curTick = GetTickCount();
	for( i=0; i<n; i++ )            // for (TimeStamp ts : _reuseTimeStamps)
	{
		skillID = p->readUInt();    // writeD(ts.getSkill());
		p->readUInt();              // writeD(0x00);
		reuse = p->readUInt();      // writeD((int) ts.getReuse() / 1000);
		remaining = p->readUInt();  // writeD((int) ts.getRemaining() / 1000);
		//log_error( LOG_OK, "SkillCoolTime for skill: %u (%u / %u)\n", skillID, remaining, reuse );
		//             [++] SkillCoolTime for skill: 1355 (19 / 37)
		int ii = 0;
		for( ii=0; ii<USERSKILL_MAX_SKILLS; ii++ )
		{
			if( skill[ii].skillID == skillID )
			{
				skill[ii].coolTimeReuse = reuse * 1000;
				skill[ii].coolTimeRemaining = remaining * 1000;
				skill[ii].castStartTick = curTick - ( reuse - remaining ) * 1000;
				break;
			}
		}
	}
}

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
void UserSkills::parse_MagicSkillUse( L2GamePacket *p )
{
	if( !p ) return;
	p->getPacketType();
	p->readUInt(); // objectId
	p->readUInt(); // target object id
	unsigned int skillID = p->readUInt(); // skillID
	/*int skillLvl         =*/ p->readInt();  // skillLvl
	/*unsigned int hitTime =*/ p->readUInt(); // hitTime
	unsigned int reuseDelay = p->readUInt(); // reuseDelay
	//if( reuseDelay == 0 ) return;
	//
	int i;
	int nPassed = 0;
	for( i=0; i<USERSKILL_MAX_SKILLS; i++ )
	{
		if( skill[i].skillID == 0 ) continue;
		nPassed++;
		if( skill[i].skillID == skillID )
		{
			skill[i].coolTimeRemaining = skill[i].coolTimeReuse = reuseDelay;
			skill[i].castStartTick = GetTickCount();
			break;
		}
		if( nPassed >= skillCount ) break;
	}
	//
	//char skillname[256] = {0};
	//L2Data_DB_GetSkillNameByID( skillID, skillname );
	//log_error( LOG_OK, "skillID %d [%s] lvl %d reuseDelay %u\n", skillID, skillname, skillLvl, reuseDelay );
}

void UserSkills::startCasting( unsigned int time )
{
	castMsecStart = time; // общее время каста скилла
	castMsecLeft = time;  // время, оставшееся до конца каста
	castMsecTickStart = GetTickCount(); // таймер начала каста
}

void UserSkills::stopCasting()
{
	castMsecLeft = 0;
	castMsecStart = 0;
	castMsecTickStart = 0;
}

void UserSkills::processCasting( unsigned int curTick )
{
	if( !isCasting() ) return;
	unsigned int passed = curTick - castMsecTickStart; // сколько тиков прошло с начала каста
	if( passed > castMsecStart ) // прошло больше тиков, чем общее время каста
	{
		// каст закончен
		stopCasting();
	}
	else // прошло меньше тиков, чем общее время каста
	{
		// оставшееся время каста уменьшилось на величину прощедшего промежутка времени
		castMsecLeft = castMsecStart - passed;
	}
}

void UserSkills::processCoolTimes( unsigned int curTick )
{
	int i;
	int nPassed = 0;
	for( i=0; i<USERSKILL_MAX_SKILLS; i++ )
	{
		if( skill[i].skillID == 0 ) continue;
		nPassed++;
		//if( (skill[i].coolTimeRemaining == 0) & (skill[i].coolTimeReuse > 0) ) skill[i].coolTimeReuse = 0;
		if( skill[i].castStartTick > 0 ) skill[i].process_coolTime( curTick );
	}
}
