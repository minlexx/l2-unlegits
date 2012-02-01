#include "pch.h"
#include "L2ArmorTemplate.h"
#include "l2c_utils.h"

L2ArmorTemplate::L2ArmorTemplate( L2ItemSubType subType, StatsSet& set ):
	L2ItemTemplate( TYPE_ARMOR, subType, set )
{
	set.getInt( "avoid_modify", &m_avoidModifier );
	set.getInt( "p_def", &m_pDef, 0 );
	set.getInt( "m_def", &m_mDef, 0 );
	set.getInt( "mp_bonus", &m_mpBonus, 0 );
	set.getInt( "hp_bonus", &m_hpBonus, 0 );
	m_enchant4Skill = 0;
	std::string stds;
	set.getString( "enchant4_skill", stds );
	//String[] skill = set.getString("enchant4_skill").split("-");
	//_skill = set.getString("skill").split(";");
	set.getString( "skill", stds );
}

L2ArmorTemplate::~L2ArmorTemplate()
{
}
