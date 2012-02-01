#include "pch.h"
#include "Log.h"
#include "l2c_utils.h"
#include "net/GameClient/GameClient.h"
#include "../ClientPacketHandler.h"
#include "../ServerPackets.h"
#include "GS.h"

L2GamePacket *ClientPacketHandler::Logout( L2GamePacket *pack )
{
	pack->getPacketType();
	L2GamePacket *ret = NULL;
	switch( m_cl->getState() )
	{
	case CLIENT_STATE_OFFLINE:
	case CLIENT_STATE_CONNECTED:
	case CLIENT_STATE_AUTHED:
		ret = NULL;
		m_cl->signalThreadStop( true ); // we can just disconnect player now
		break;
	case CLIENT_STATE_IN_GAME:
		// TODO: check if player can logout here and now
		// answer with RestartResponse (LogoutOK) or (LogoutFailed)
		break;
	}
	return ret;
}
