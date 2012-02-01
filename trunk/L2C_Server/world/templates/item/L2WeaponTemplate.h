#pragma once
#include "L2ItemTemplate.h"

class L2WeaponTemplate: public L2ItemTemplate
{
public:
	L2WeaponTemplate( L2ItemSubType subType, StatsSet& set );
	virtual ~L2WeaponTemplate();

public:
	int     getSoulShotCount() const    { return m_soulShotCount; }
	int     getSpiritShotCount() const  { return m_spiritShotCount; }
	int     getPDam() const             { return m_pDam; }
	int     getRndDam() const           { return m_rndDam; }
	int     getCritical() const         { return m_critical; }
	double  getHitModifier() const      { return m_hitModifier; }
	int     getAvoidModifier() const    { return m_avoidModifier; }
	int     getShieldDef() const        { return m_shieldDef; }
	double  getShieldDefRate() const    { return m_shieldDefRate; }
	int     getAtkSpeed() const         { return m_atkSpeed; }
	int     getAtkReuse() const         { return m_atkReuse; }
	int     getMpConsume() const        { return m_mpConsume; }
	int     getMDam() const             { return m_mDam; }
	int     getChangeWeaponId() const   { return m_changeWeaponId; }

protected:
	int     m_soulShotCount;
	int     m_spiritShotCount;
	int     m_pDam;
	int     m_rndDam;
	int     m_critical;
	double  m_hitModifier;
	int     m_avoidModifier;
	int     m_shieldDef;
	double  m_shieldDefRate;
	int     m_atkSpeed;
	int     m_atkReuse;
	int     m_mpConsume;
	int     m_mDam;
	int     m_changeWeaponId;

	// TODO: skills
	//private L2Skill _enchant4Skill = null; // skill that activates when item is enchanted +4 (for duals)
	//String[] _skill;
	// Attached skills for Special Abilities
	//protected L2Skill _skillsOnCast;
	//protected Condition _skillsOnCastCondition;
	//protected L2Skill _skillsOnCrit;
	//protected Condition _skillsOnCritCondition;
};
