#include "stdafx.h"
#include "UserAI_QuickTargetOptions.h"

UserAI_QuickTargetOptions::UserAI_QuickTargetOptions()
{
	m_use_pri = 1;
	m_target_dead = 0;
	m_target_party = 0;
	m_target_clan = 0;
	m_target_ally = 0;
	m_target_cw_2_side = 1;
	m_target_cw_1_side = 1;
	m_target_neutral_pk = 1;
	m_target_neutral_flagged = 1;
}
