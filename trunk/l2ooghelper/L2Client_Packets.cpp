#include "stdafx.h"
#include "logger.h"
#include "L2Client.h"

int L2Client::sendPacket( L2GamePacket *pack, bool obfuscateAndXOREncode )
{
	// this function can be used only during GS connection
	if( (this->state != STATE_IN_GAME) &&
		(this->state != STATE_AUTHED_GAME) &&
		(this->state != STATE_CONNECTING_GAME) &&
		(this->state != STATE_CONNECTED_GAME) )
	{
#ifdef _DEBUG
		log_error( LOG_ERROR, "L2Client::sendPacket(): ERROR: function used "
			"not for GS connection! (state = %d)\n", (int)(this->state) );
#endif
		return 0;
	}
	// check socket
	if( this->sock == INVALID_SOCKET )
	{
#ifdef _DEBUG
		log_error( LOG_ERROR, "L2Client::sendPacket(): sock == INVALID_SOCKET!\n" );
#endif
		return false;
	}
	//
	unsigned int nBytesSent = 0;
	if( obfuscateAndXOREncode )
	{
		this->pack_OpcodeObfuscate( pack );
		if( game_XOR_enabled ) pack->encodeXOR( this->game_key_send );
	}
	EnterCriticalSection( &(this->cs_sendPacket) );
	int r = L2PacketSend( this->sock, pack, this->game_sendTimeoutMsec, &nBytesSent );
	LeaveCriticalSection( &(this->cs_sendPacket) );
#ifdef _DEBUG
	if( r <= 0 )
	{
		log_error( LOG_ERROR, "L2Client::sendPacket(): ERROR return: %d (sent %u bytes)\n",
			r, nBytesSent );
	}
#endif
	return r;
}

bool L2Client::pack_OpcodeDeObfuscate( unsigned char *bytes, unsigned int len )
{
	if( !game_pCodeObfuscator ) return false;
	if( len < 3 ) return false;
	// gracia final does not use opcode obfuscation for S->C packets
	if( this->account.getL2Version() == L2_VERSION_T23 ) return false; // ? sure?
	int ret = 0;
	try
	{
		game_pCodeObfuscator->decodeIDs( bytes );
	}
	catch( L2P_Exception& e )
	{
		log_error( LOG_ERROR, "OpcodeDeObfuscate: L2P_Exception: %s\n", e.what() );
	}
	if( ret < 0 )
		log_error( LOG_WARNING, "L2Client: opcode deobfuscation failed: error code: %d\n", ret );
	return (ret > 0);
}

bool L2Client::pack_OpcodeObfuscate( unsigned char *bytes, unsigned int len )
{
	if( !game_pCodeObfuscator ) return false;
	if( len < 3 ) return false;
	int ret = 0;
	try
	{
		game_pCodeObfuscator->encodeIDs( bytes );
	}
	catch( L2P_Exception& e )
	{
		log_error( LOG_ERROR, "OpcodeObfuscate: L2P_Exception: %s\n", e.what() );
	}
	if( ret < 0 )
		log_error( LOG_WARNING, "L2Client: opcode obfuscation failed: error code: %d\n", ret );
	return (ret > 0);
}

bool L2Client::pack_OpcodeDeObfuscate( L2GamePacket *p )
{
	if( !game_pCodeObfuscator ) return false;
	if( !p ) return false;
	return pack_OpcodeDeObfuscate( (unsigned char *)p->getBytesPtr(), p->getPacketSize() );
}

bool L2Client::pack_OpcodeObfuscate( L2GamePacket *p )
{
	if( !game_pCodeObfuscator ) return false;
	if( !p ) return false;
	return pack_OpcodeObfuscate( (unsigned char *)p->getBytesPtr(), p->getPacketSize() );
}
