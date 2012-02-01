#include "pch.h"
#include "L2CharTemplate.h"

L2CharTemplate::L2CharTemplate( StatsSet& set )
{
	// Base stats
	set.getInt( "baseSTR", &baseSTR );
	set.getInt( "baseCON", &baseCON );
	set.getInt( "baseDEX", &baseDEX );
	set.getInt( "baseINT", &baseINT );
	set.getInt( "baseWIT", &baseWIT );
	set.getInt( "baseMEN", &baseMEN );
	set.getDouble( "baseHpMax", &baseHpMax );
	set.getDouble( "baseCpMax", &baseCpMax );
	set.getDouble( "baseMpMax", &baseMpMax );
	set.getDouble( "baseHpReg", &baseHpReg );
	set.getDouble( "baseMpReg", &baseMpReg );
	set.getInt( "basePAtk", &basePAtk );
	set.getInt( "baseMAtk", &baseMAtk );
	set.getInt( "basePDef", &basePDef );
	set.getInt( "baseMDef", &baseMDef );
	set.getInt( "basePAtkSpd", &basePAtkSpd );
	set.getInt( "baseMAtkSpd", &baseMAtkSpd );
	set.getDouble( "baseMReuseDelay", &baseMReuseRate, 1.0 );
	set.getInt( "baseShldDef", &baseShldDef );
	set.getInt( "baseAtkRange", &baseAtkRange );
	set.getInt( "baseShldRate", &baseShldRate );
	set.getInt( "baseCritRate", &baseCritRate );
	set.getInt( "baseMCritRate", &baseMCritRate, 80 ); // CT2: The magic critical rate has been increased to 10 times.
	set.getInt( "baseWalkSpd", &baseWalkSpd );
	set.getInt( "baseRunSpd", &baseRunSpd );

	// missed base stats
	set.getInt( "baseAccuracy", &baseAccuracy );
	set.getInt( "baseEvasion", &baseEvasion );
	
	// SpecialStats
	set.getInt( "baseBreath", &baseBreath, 100 );
	set.getInt( "baseAggression", &baseAggression, 0 );
	set.getInt( "baseBleed", &baseBleed, 0 );
	set.getInt( "basePoison", &basePoison, 0 );
	set.getInt( "baseStun", &baseStun, 0 );
	set.getInt( "baseRoot", &baseRoot, 0 );
	set.getInt( "baseMovement", &baseMovement, 0 );
	set.getInt( "baseConfusion", &baseConfusion, 0 );
	set.getInt( "baseSleep", &baseSleep, 0 );
	set.getInt( "baseFire", &baseFire, 0 );
	set.getInt( "baseWind", &baseWind, 0 );
	set.getInt( "baseWater", &baseWater, 0 );
	set.getInt( "baseEarth", &baseEarth, 0 );
	set.getInt( "baseHoly", &baseHoly, 0 );
	set.getInt( "baseDark", &baseDark, 0 );
	set.getDouble( "baseAaggressionVuln", &baseAggressionVuln, 1.0 );
	set.getDouble( "baseBleedVuln", &baseBleedVuln, 1.0 );
	set.getDouble( "basePoisonVuln", &basePoisonVuln, 1.0 );
	set.getDouble( "baseStunVuln", &baseStunVuln, 1.0 );
	set.getDouble( "baseRootVuln", &baseRootVuln, 1.0 );
	set.getDouble( "baseMovementVuln", &baseMovementVuln, 1.0 );
	set.getDouble( "baseConfusionVuln", &baseConfusionVuln, 1.0 );
	set.getDouble( "baseSleepVuln", &baseSleepVuln, 1.0 );
	set.getDouble( "baseFireRes", &baseFireRes, 0.0 );
	set.getDouble( "baseWindRes", &baseWindRes, 0.0 );
	set.getDouble( "baseWaterRes", &baseWaterRes, 0.0 );
	set.getDouble( "baseEarthRes", &baseEarthRes, 0.0 );
	set.getDouble( "baseHolyRes", &baseHolyRes, 0.0 );
	set.getDouble( "baseDarkRes", &baseDarkRes, 0.0 );
	set.getDouble( "baseCritVuln", &baseCritVuln, 1.0 );
	
	// undead? default NO
	set.getBool( "isUndead", &isUndead, false );
	
	//C4 Stats
	set.getInt( "baseMpConsumeRate", &baseMpConsumeRate, 0 );
	set.getInt( "baseHpConsumeRate", &baseHpConsumeRate, 0 );
	
	// Geometry
	set.getInt( "collision_radius", &collisionRadius, 10 );
	set.getInt( "collision_height", &collisionHeight, 10 );
}
