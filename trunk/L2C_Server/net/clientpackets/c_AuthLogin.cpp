#include "pch.h"
#include "Log.h"
#include "l2c_utils.h"
#include "net/GameClient/GameClient.h"
#include "../ClientPacketHandler.h"
#include "../ServerPackets.h"
#include "GS.h"

/* _loginName = readS().toLowerCase();
   _playKey2 = readD();
   _playKey1 = readD();
   _loginKey1 = readD();
   _loginKey2 = readD(); */
L2GamePacket *ClientPacketHandler::AuthLogin( L2GamePacket *pack )
{
	GameServer *gs = GameServer::getInstance();
	pack->getPacketType();
	wchar_t login[128] = {0};
	unsigned char loginKey[8] = {0,0,0,0,0,0,0,0};
	unsigned char playKey[8] = {0,0,0,0,0,0,0,0};
	wcsncpy( login, pack->readUnicodeStringPtr(), 128 ); login[127] = 0;
	pack->readBytes( playKey+4, 4 );
	pack->readBytes( playKey, 4 );
	pack->readBytes( loginKey, 8 );
	//
	m_cl->setAccount( login );
	m_cl->setSessionKeys( playKey, loginKey );
	//
	if( gs->getConfig()->Debug ) LogDebug( L"%s: AuthLogin %s", m_cl->toString(), login );
	gs->addWaitingClientAndSendPlayerAuthRequest( login, loginKey, playKey );
	return NULL;
}

