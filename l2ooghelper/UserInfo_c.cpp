#include "stdafx.h"
#include "UserInfo_c.h"
#include "logger.h"

void UserInfo::setUnused()
{
	L2Player::setUnused();
	// targeting
	targetObjectID = 0;
	targetCurHp = targetMaxHp = targetCurMp = targetMaxMp = 0.0;
	// dead vars
	isDead = 0;
	canResurrectToVillage = canResurrectToCH = canResurrectToCastle = 
		canResurrectToFortress = canResurrectToSiegeHQ = canResurrectFixed = 0;
}
