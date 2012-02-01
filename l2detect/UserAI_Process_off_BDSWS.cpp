#include "stdafx.h"
#include "UserAI.h"
#include "Logger.h"
#include "PacketInjector.h"

void UserAI::UAI_ProcessOffpartyBDSWS( class UserAI *cls )
{
	//if( !cls ) return;
	DWORD curTick = GetTickCount();
	// invite bd?
	if( curTick >= cls->offBDSWS_nextBDInvTime )
	{
		if( cls->off_BD_SWS.inv_bd_enable )
		{
			if( cls->offBD_inrange )
			{
				log_error( LOG_USERAI, "OFFBDSWS: Time to invite BD...\n" );
				cls->offBDSWS_nextBDInvTime = curTick + (cls->off_BD_SWS.inv_bd_secs * 1000);
				// next dismiss time = next invite time + dismiss timeout // WRONGG мать твою
				//cls->offBDSWS_nextBDDisTime = cls->offBDSWS_nextBDInvTime +
				//	(cls->off_BD_SWS.dismiss_bd_secs * 1000);
				cls->offBDSWS_nextBDDisTime = curTick + (cls->off_BD_SWS.dismiss_bd_secs * 1000);
				//log_error( LOG_USERAI, "OFFBDSWS: cur %u, next dis %u, next inv %u\n",
				//	curTick, cls->offBDSWS_nextBDDisTime, cls->offBDSWS_nextBDInvTime );
				PGen_RequestJoinParty( cls->off_BD_SWS.nameBD, L2_PARTY_LOOT_ITEM_LOOTER );
			}
			else log_error( LOG_USERAI, "OFFBDSWS: Time to invite BD... BD is not in range :(\n" );
		}
	}
	// invite sws?
	if( curTick >= cls->offBDSWS_nextSWSInvTime )
	{
		if( cls->off_BD_SWS.inv_sws_enable )
		{
			if( cls->offSWS_inrange )
			{
				log_error( LOG_USERAI, "OFFBDSWS: Time to invite SWS...\n" );
				cls->offBDSWS_nextSWSInvTime = curTick + (cls->off_BD_SWS.inv_sws_secs * 1000);
				// next dismiss time = next invite time + dismiss timeout
				//cls->offBDSWS_nextSWSDisTime = cls->offBDSWS_nextSWSInvTime +
				//	(cls->off_BD_SWS.dismiss_sws_secs * 1000);
				cls->offBDSWS_nextSWSDisTime = curTick + (cls->off_BD_SWS.dismiss_sws_secs * 1000);
				//log_error( LOG_USERAI, "OFFBDSWS: cur %u, next dis %u, next inv %u\n",
				//	curTick, cls->offBDSWS_nextSWSDisTime, cls->offBDSWS_nextSWSInvTime );
				PGen_RequestJoinParty( cls->off_BD_SWS.nameSWS, L2_PARTY_LOOT_ITEM_LOOTER );
			}
			else log_error( LOG_USERAI, "OFFBDSWS: Time to invite SWS... SWS is not in range :(\n" );
		}
	}

	// dismiss bd?
	if( curTick >= cls->offBDSWS_nextBDDisTime )
	{
		if( cls->off_BD_SWS.inv_bd_enable )
		{
			log_error( LOG_USERAI, "OFFBDSWS: Time to dismiss BD...\n" );
			PGen_RequestOustPartyMember( cls->off_BD_SWS.nameBD );
			// dismiss time is updated on invite
		}
	}
	// dismiss sws?
	if( curTick >= cls->offBDSWS_nextSWSDisTime )
	{
		if( cls->off_BD_SWS.inv_sws_enable )
		{
			log_error( LOG_USERAI, "OFFBDSWS: Time to dismiss SWS...\n" );
			PGen_RequestOustPartyMember( cls->off_BD_SWS.nameSWS );
			// dismiss time is updated on invite
		}
	}
}
