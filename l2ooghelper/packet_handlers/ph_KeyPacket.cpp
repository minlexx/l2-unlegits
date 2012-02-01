#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_KeyPacket( class L2Client *pcls, L2GamePacket *p )
{
	L2Game_KeyPacket *p_game_key = new L2Game_KeyPacket( p->getBytesPtr(), p->getPacketSize() );
	// parse KeyPacket
	if( p_game_key->getPacketSize() == 25 )
	{
		if( !p_game_key->parse( pcls->account.getL2Version() ) )
		{
			pcls->addChatToTab( CHAT_SYS, L"KeyPacket parse error!" );
			log_error( LOG_ERROR, "KeyPacket parse error!\n" );
			pcls->disconnectClient(true);
			return;
		}
	}
	else
	{
		log_error( LOG_WARNING, "KeyPacket: length %u bytes (not 25)\n", p->getPacketSize() );
		pcls->addChatToTabFormat( CHAT_SYS, L"Warning! Short KeyPacket (L2Dream?)" );
		if(p_game_key->getDataSize() >= 10 )
		{
			p_game_key->getPacketType();
			p_game_key->p_protocolIsOK = p_game_key->readUChar();  // 0x01 - proto OK, 0x00 - proto not supported
			p_game_key->readBytes( p_game_key->p_initialKey, 8 );  // first 8 bytes of XOR key
			// short packet - ignore all rest bytes...
			// add last 8 bytes of XOR key - they are constant
		}
		else
		{
			log_error( LOG_ERROR, "KeyPacket: packet len is VERY SHORT! Some protocol error or bot protection!\n" );
			pcls->addChatToTab( CHAT_SYS, L"KeyPacket packet length is VERY SHORT! Some protocol error or bot protection!" );
		}
		L2Game_KeyPacket::createInitialHellboundKey( p_game_key->p_initialKey, p_game_key->p_initialKey );
		// disable opcode obfuscation by default on fucking Java servers
		p_game_key->p_obfuscatorSeed = 0;
		p_game_key->p_serverId = 0;
		p_game_key->p_protocolIsOK = 1;
	}
	// check correct protocol version
	if( !p_game_key->p_protocolIsOK )
	{
		log_error( LOG_WARNING, "KeyPacket: server tells protocolVersion is incorrect? O_o\n" );
		pcls->addChatToTab( CHAT_SYS, L"KeyPacket: server tells protocolVersion is incorrect? O_o" );
	}
#ifdef _DEBUG
	log_error( LOG_DEBUG, "KeyPacket: gs ID: %d\n", (int)p_game_key->p_serverId );
#endif

	// create XOR encryption keys
	L2Game_KeyPacket::createInitialHellboundKey( p_game_key->p_initialKey, p_game_key->p_initialKey );
	// set BOTH recv & send keys to SAME value
	memcpy( pcls->game_key_recv, p_game_key->p_initialKey, 16 );
	memcpy( pcls->game_key_send, pcls->game_key_recv, 16 );
	pcls->game_XOR_enabled = true;

	// remember obfuscator
	pcls->game_opcodeObfuscatorSeed = p_game_key->p_obfuscatorSeed;
#ifdef _DEBUG
	log_error( LOG_DEBUG, "KeyPacket results:\nKey: " );
	int i = 0;
	for( i=0; i<16; i++ ) log_error_np( LOG_DEBUG, "%02X ", pcls->game_key_recv[i] );
	log_error_np( LOG_DEBUG, "\n" );
	log_error( LOG_DEBUG, "Obfuscator seed: %08X\n", pcls->game_opcodeObfuscatorSeed );
#endif
	// free mem
	delete p_game_key; p_game_key = NULL;

	// obfuscator manage
	if( pcls->game_opcodeObfuscatorSeed != 0 )
	{
		pcls->game_pCodeObfuscator = new L2PCodeObfuscator();
		pcls->game_pCodeObfuscator->setVersionMode( pcls->account.getL2Version() );
		pcls->game_pCodeObfuscator->init_tables( pcls->game_opcodeObfuscatorSeed );
		log_error( LOG_WARNING, "Client opcode obfuscation enabled.\n" );
		pcls->addChatToTab( CHAT_SYS, L"Notice: client opcode obfuscation enabled" );
		pcls->addChatToTab( CHAT_SYS, L"Notice: L2 off server >= Hellbound?" );
	}

	// reply with AuthLogin
	pcls->send_AuthLogin();
}
