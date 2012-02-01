#include "pch.h"
#include "Log.h"
#include "l2c_utils.h"
#include "net/GameClient/GameClient.h"
#include "../ServerPackets.h"
#include "GS.h"

L2GamePacket *ServerPackets::CharCreateFail( GameClient *pl, int reason )
{
	pl = NULL; // reference to player not needed
	L2Game_CharCreateFail *p = new L2Game_CharCreateFail();
	p->p_reasonCode = (unsigned int)reason;
	p->create( L2_VERSION_T23 );
	return p;
}
