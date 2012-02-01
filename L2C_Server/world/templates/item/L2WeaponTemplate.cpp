#include "pch.h"
#include "L2WeaponTemplate.h"
#include "l2c_utils.h"

L2WeaponTemplate::L2WeaponTemplate( L2ItemSubType subType, StatsSet& set ):
	L2ItemTemplate( TYPE_WEAPON, subType, set )
{
	set.getInt( "soulshots", &m_soulShotCount );
	set.getInt( "soulshots", &m_soulShotCount );
	set.getInt( "spiritshots", &m_spiritShotCount );
	set.getInt( "p_dam", &m_pDam );
	set.getInt( "rnd_dam", &m_rndDam );
	set.getInt( "critical", &m_critical );
	set.getDouble( "hit_modify", &m_hitModifier );
	set.getInt( "avoid_modify", &m_avoidModifier );
	set.getInt( "shield_def", &m_shieldDef );
	set.getDouble( "shield_def_rate", &m_shieldDefRate );
	set.getInt( "atk_speed", &m_atkSpeed );
	int default_atkReuse = 0;
	if( m_subType == WEAPON_BOW ) default_atkReuse = 1500;
	else if( m_subType == WEAPON_CROSSBOW ) default_atkReuse = 1200;
	set.getInt( "atk_reuse", &m_atkReuse, default_atkReuse );
	set.getInt( "mp_consume", &m_mpConsume );
	set.getInt( "m_dam", &m_mDam );
	set.getInt( "change_weaponId", &m_changeWeaponId );
}

L2WeaponTemplate::~L2WeaponTemplate()
{
}
