#include "pch.h"
#include "Log.h"
#include "l2c_utils.h"
#include "net/GameClient/GameClient.h"
#include "../ClientPacketHandler.h"
#include "../ServerPackets.h"
#include "GS.h"

L2GamePacket *ClientPacketHandler::ProtocolVersion( L2GamePacket *pack )
{
	GameServer *gs = GameServer::getInstance();
	pack->getPacketType();
	int pv = pack->readD();
	// just server ping?
	if( pv == -2 )
	{
		m_cl->setProtocolOk( false );
		m_cl->signalThreadStop( true );
		if( gs->getConfig()->Debug ) LogDebug( L"%s: received server ping from client (%d)", m_cl->toString(), pv );
		return NULL;
	}
	// protocol version validation
	if( gs->getConfig()->Debug ) LogDebug( L"%s: protocol version %d", m_cl->toString(), pv );
	if( pv < gs->getConfig()->min_game_protocol_version ||
		pv > gs->getConfig()->max_game_protocol_version )
	{
		m_cl->setProtocolOk( false );
		m_cl->signalThreadStop( true );
		LogWarning( L"%s: invalid protocol version %d (must be [%d..%d])", 
			m_cl->toString(), pv,
			gs->getConfig()->min_game_protocol_version,
			gs->getConfig()->max_game_protocol_version );
	}
	else m_cl->setProtocolOk( true );
	// reply KeyPacket
	return ServerPackets::KeyPacket( m_cl );
}
