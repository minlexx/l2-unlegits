#include "pch.h"
#include "Log.h"
#include "l2c_utils.h"
#include "net/GameClient/GameClient.h"
#include "../ClientPacketHandler.h"
#include "../ServerPackets.h"
#include "GS.h"


L2GamePacket *ClientPacketHandler::RequestGotoLobby( L2GamePacket *pack )
{
	pack = NULL; // unused here
	return ServerPackets::CharacterSelectionInfo( m_cl );
}

