#pragma once
#include "world/templates/StatsSet.h"

class L2CharTemplate
{
public:
	L2CharTemplate( StatsSet& set );

public:
	// BaseStats
	int baseSTR;
	int baseCON;
	int baseDEX;
	int baseINT;
	int baseWIT;
	int baseMEN;
	double baseHpMax;
	double baseCpMax;
	double baseMpMax;
	double baseHpReg;
	double baseMpReg;
	
	int basePAtk;
	int baseMAtk;
	int basePDef;
	int baseMDef;
	int basePAtkSpd;
	int baseMAtkSpd;
	double baseMReuseRate;
	int baseShldDef;
	int baseAtkRange;
	int baseShldRate;
	int baseCritRate;
	int baseMCritRate;
	int baseWalkSpd;
	int baseRunSpd;
	// missed base stats
	int baseAccuracy;
	int baseEvasion;
	
	// SpecialStats
	int baseBreath;
	int baseAggression;
	int baseBleed;
	int basePoison;
	int baseStun;
	int baseRoot;
	int baseMovement;
	int baseConfusion;
	int baseSleep;
	int baseFire;
	int baseWind;
	int baseWater;
	int baseEarth;
	int baseHoly;
	int baseDark;
	double baseAggressionVuln;
	double baseBleedVuln;
	double basePoisonVuln;
	double baseStunVuln;
	double baseRootVuln;
	double baseMovementVuln;
	double baseConfusionVuln;
	double baseSleepVuln;
	double baseFireRes;
	double baseWindRes;
	double baseWaterRes;
	double baseEarthRes;
	double baseHolyRes;
	double baseDarkRes;
	double baseCritVuln;
	
	bool isUndead;
	
	//C4 Stats
	int baseMpConsumeRate;
	int baseHpConsumeRate;
	
	int collisionRadius;
	int collisionHeight;
};
