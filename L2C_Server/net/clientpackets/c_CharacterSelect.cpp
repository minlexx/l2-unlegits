#include "pch.h"
#include "Log.h"
#include "l2c_utils.h"
#include "net/GameClient/GameClient.h"
#include "../ClientPacketHandler.h"
#include "../ServerPackets.h"
#include "GS.h"


L2GamePacket *ClientPacketHandler::CharacterSelect( L2GamePacket *pack )
{
	L2GamePacket *ret = NULL;
	pack->getPacketType();
	int charSlot = pack->readD();
	LogDebug( L"CharacterSelect: charSlot : %d", charSlot );
	// create &load player for game client if not exists
	if( m_cl->getPlayer() == NULL )
	{
		GamePlayer *pl = new GamePlayer( m_cl, 0 );
		// TODO: CharacterSelect: load char from base
		m_cl->setPlayer( pl, true, CLIENT_STATE_IN_GAME ); // sets state to IN_GAME
		//
		L2Game_CharSelected *p = new L2Game_CharSelected();
		ret = p;
		p->p_char_name[0] = 0;
		p->p_opcodeObfuscatorSeed = rand();
		p->p_title[0] = 0;
		p->p_classId = 0;
		p->p_level = 1;
		unsigned char playKey[8];
		m_cl->getPlayKey( playKey );
		p->p_sessionId = playKey[0] | (playKey[1] << 8) | (playKey[2] << 16) | (playKey[3] << 24); // playOkID1
		//
		p->create( L2_VERSION_T23 );
		//
		m_cl->setOpcodeObfuscationSeed( p->p_opcodeObfuscatorSeed );
	}
	else
		LogError( L"Net: %s: CharacterSelect: already has player attached to GameClient?? Bad Bad Error or Cheater!",
			m_cl->toString() );
	//
	return ret;
}

