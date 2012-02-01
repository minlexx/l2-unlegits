#include "pch.h"
#include "Log.h"
#include "l2c_utils.h"
#include "net/GameClient/GameClient.h"
#include "../ServerPackets.h"
#include "GS.h"

L2GamePacket *ServerPackets::KeyPacket( GameClient *pl )
{
#ifdef ENABLE_OBF
	unsigned int obf_seed = rand();
#else
	unsigned int obf_seed = 0;
#endif
	pl->setOpcodeObfuscationSeed( obf_seed );
	//
	L2Game_KeyPacket *p = new L2Game_KeyPacket();
	p->p_protocolIsOK   = pl->isProtocolOk();
	p->p_obfuscatorSeed = obf_seed;
	p->p_serverId       = (unsigned char)GameServer::getInstance()->getServerId();
	// initial key is random bytes...
	L2Game_KeyPacket::createInitialHellboundKey( p->p_initialKey, p->p_initialKey );
	p->create( L2_VERSION_T23 );
	// enable crypt on client
	pl->enable_XOR_crypt( true, p->p_initialKey );
	return p;
}
