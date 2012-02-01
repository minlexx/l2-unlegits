#include "stdafx.h"
#include "ConfigIni.h"
#include "Logger.h"
#include "GameClient.h"
#include "GameListener.h"

extern CConfig g_cfg;

// TODO: count obfuscator

bool GameClient::InjectPacketToServer( unsigned char *bytes, unsigned int len, bool doObfuscate )
{
	if( !bytes || (len<3) ) return false;
	if( this->sock_server == INVALID_SOCKET ) return false;
	if( g_cfg.EnableModifyGameTraffic == 0 ) return false; // FIXED: do not allow traffic modification if not set
	if( this->state == GCST_OFFLINE )
		log_error( LOG_WARNING, "GameClient::InjectPacketToServer(): trying to send packet to server in state GCST_OFFLINE!\n" );
	unsigned int sentLen = 0;
	// TODO: obfuscate opcode before sending?
	if( doObfuscate )
	{
		if( (this->opcodeObfuscator != 0x00)  &&  (this->clsObfuscator) )
		{
			L2PCodeObfuscator *obf = (L2PCodeObfuscator *)(this->clsObfuscator);
			unsigned char singleOpcode = bytes[2];
			unsigned short doubleOpcode = bytes[3] | (bytes[4] << 8);
			unsigned short tripleOpcode = bytes[5] | (bytes[6] << 8);
			unsigned char prev_singleOpcode = singleOpcode;
			unsigned short prev_doubleOpcode = doubleOpcode;
			try
			{
				obf->encodeOpcode( singleOpcode, doubleOpcode, tripleOpcode );
			}
			catch( L2P_ObfuscateException& e )
			{
				log_error( LOG_ERROR, "GameClient::InjectPacketToServer: ObfuscationException: %s\n", e.what() );
			}
			// log
			if( prev_doubleOpcode == doubleOpcode )
				log_error( LOG_PACKETNAME, "InjectPacketToServer: obfuscated %02X -> %02X\n",
					singleOpcode, prev_singleOpcode );
			else
				log_error( LOG_PACKETNAME, "InjectPacketToServer: obfuscated %02X:%02X -> %02X:%02X\n",
					singleOpcode, doubleOpcode, prev_singleOpcode, prev_doubleOpcode );
			// replace bytes in packet
			bytes[2] = singleOpcode;
			bytes[3] =  doubleOpcode & 0xFF;
			bytes[4] = (doubleOpcode >> 8) & 0xFF;
		}
	}
	// encode XOR if enabled
	if( this->xor_enabled ) L2GamePacket::encodeXOR_buffer( bytes, len, this->key_server_cs );
	LockSend();
	//int ret = L2PacketSend( bytes, sock_server, 5, 0, &sentLen );
	//int ret = L2PacketSend2( bytes, sock_server, 5000, &sentLen );
	L2PacketSend2( bytes, sock_server, 5000, &sentLen );
	UnlockSend();
	return true;
}

bool GameClient::InjectPacketToClient( unsigned char *bytes, unsigned int len )
{
	if( !bytes || (len<3) ) return false;
	if( this->sock_client == INVALID_SOCKET ) return false;
	if( g_cfg.EnableModifyGameTraffic == 0 ) return false; // FIXED: do not allow traffic modification if not set
	if( this->state == GCST_OFFLINE )
		log_error( LOG_WARNING, "GameClient::InjectPacketToClient(): trying to send packet to client in state GCST_OFFLINE!\n" );
	unsigned int sentLen = 0;
	if( this->xor_enabled )
	{
		L2GamePacket::encodeXOR_buffer( bytes, len, this->key_client_sc );
	}
	LockSend();
	int ret = L2PacketSend2( bytes, sock_client, 5000, &sentLen );
	UnlockSend();
	if( ret <= 0 ) log_error( LOG_ERROR, "InjectPacketToClient: send failed (sent %u bytes)\n", sentLen );
	return true;
}
