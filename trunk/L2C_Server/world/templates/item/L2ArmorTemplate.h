#pragma once
#include "L2ItemTemplate.h"

class L2ArmorTemplate: public L2ItemTemplate
{
public:
	L2ArmorTemplate( L2ItemSubType subType, StatsSet& set );
	virtual ~L2ArmorTemplate();

public:
	int getAvoidModifier() const { return m_avoidModifier; }
	int getPDef() const          { return m_pDef; }
	int getMDef() const          { return m_mDef; }
	int getMPBonus() const       { return m_mpBonus; }
	int getHPBonus() const       { return m_hpBonus; }
	int getEnchant4Skill() const { return m_enchant4Skill; }

protected:
	int m_avoidModifier;
	int m_pDef;
	int m_mDef;
	int m_mpBonus;
	int m_hpBonus;
	unsigned int m_enchant4Skill; // skill that activates when armor is enchanted +4
};
