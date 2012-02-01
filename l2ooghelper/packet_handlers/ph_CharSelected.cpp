#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_CharSelected( class L2Client *pcls, L2GamePacket *p )
{
	pcls->setState( STATE_IN_GAME );
	pcls->addChatToTab( CHAT_SYS, L"Entering game..." );

	L2Game_CharSelected *p_charSel =
		new L2Game_CharSelected( p->getBytesPtr(), p->getPacketSize() );
	if( p_charSel->parse( pcls->account.getL2Version() ) )
	{
		// check obfuscator
		if( p_charSel->p_opcodeObfuscatorSeed > 0 )
		{
			log_error( LOG_WARNING, "CharSelected: client opcode obfusation enabled, seed %08X\n",
				p_charSel->p_opcodeObfuscatorSeed );
			pcls->addChatToTab( CHAT_SYS, L"Notice: client opcode obfusation enabled!" );
			// remember obfuscator
			pcls->game_opcodeObfuscatorSeed = p_charSel->p_opcodeObfuscatorSeed;
			// init enc/dec tables
			if( pcls->game_pCodeObfuscator )
			{
#ifdef _DEBUG
				log_error( LOG_DEBUG, "CharSelected: deleting existing obfuscator class\n" );
#endif
				delete pcls->game_pCodeObfuscator;
				pcls->game_pCodeObfuscator = NULL;
			}
			pcls->game_pCodeObfuscator = new L2PCodeObfuscator();
			pcls->game_pCodeObfuscator->setVersionMode( pcls->account.getL2Version() );
			pcls->game_pCodeObfuscator->init_tables( pcls->game_opcodeObfuscatorSeed );
		}
	}
	else
	{
		delete p_charSel;
		log_error( LOG_ERROR, "CharSelected parse error!\n" );
		pcls->disconnectClient( true ); // FIXME: is it correct here?
		return;
	}

	delete p_charSel;
	p_charSel = NULL;

	// now we must request some data from server...
	// and indicate that client initialization complete (EnterWorld)
	// RequestManorList
	// RequestKeyMapping
	// EnterWorld

	unsigned char pack[32];
	unsigned int plen = 0;

	// lock sending...
	EnterCriticalSection( &(pcls->cs_sendPacket) );

	// send: D0:0001 RequestManorList
	pack[0] = 5;    pack[1] = 0; // length 5
	pack[2] = 0xD0; pack[3] = 0x01; pack[4] = 0x00; // opcode D0:0001
	pcls->pack_OpcodeObfuscate( pack, 5 );
	L2GamePacket::encodeXOR_buffer( pack, 5, pcls->game_key_send );
	L2PacketSend2( pack, pcls->sock, pcls->game_sendTimeoutMsec, &plen );

	// send: D0:0021 RequestKeyMapping
	pack[0] = 5;    pack[1] = 0; // length 5
	pack[2] = 0xD0; pack[3] = 0x21; pack[4] = 0x00; // opcode D0:0021
	pcls->pack_OpcodeObfuscate( pack, 5 );
	L2GamePacket::encodeXOR_buffer( pack, 5, pcls->game_key_send );
	L2PacketSend2( pack, pcls->sock, pcls->game_sendTimeoutMsec, &plen );

	// send 11 EnterWorld
	L2Game_EnterWorld *ew = new L2Game_EnterWorld();
	ew->create();
	pcls->pack_OpcodeObfuscate( ew );
	ew->encodeXOR( pcls->game_key_send );
	L2PacketSend( pcls->sock, ew, pcls->game_sendTimeoutMsec, &plen );

	// unlock sending
	LeaveCriticalSection( &(pcls->cs_sendPacket) );

	delete ew; ew = NULL;
}
