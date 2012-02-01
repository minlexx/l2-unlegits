#include "pch.h"
#include "Log.h"
#include "l2c_utils.h"
#include "net/GameClient/GameClient.h"
#include "../ServerPackets.h"
#include "GS.h"

L2GamePacket *ServerPackets::CharCreateOK( GameClient *pl )
{
	pl = NULL; // reference to player not needed
	L2Game_CharCreateSuccess *p = new L2Game_CharCreateSuccess();
	p->create( L2_VERSION_T23 );
	return p;
}
