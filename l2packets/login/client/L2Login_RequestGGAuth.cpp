#include "stdafx.h"
#include "L2Login_RequestGGAuth.h"

L2Login_RequestGGAuth::L2Login_RequestGGAuth()
{
	this->_initNull();
	sessionId[0] = sessionId[1] = sessionId[2] = sessionId[3] = 0;
}

L2Login_RequestGGAuth::L2Login_RequestGGAuth( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
	sessionId[0] = sessionId[1] = sessionId[2] = sessionId[3] = 0;
}

// sesionID - 4 bytes array
bool L2Login_RequestGGAuth::create( L2_VERSION ver )
{
	UNREFERENCED_PARAMETER(ver);
	// set packet type
	this->setPacketType( 0x07 );
	// write sessionID
	this->writeUChar( sessionId[0] );
	this->writeUChar( sessionId[1] );
	this->writeUChar( sessionId[2] );
	this->writeUChar( sessionId[3] );
	// write 16 0x00
	int i;
	for( i=0; i<16; i++ ) this->writeUChar( 0x00 );
	return true;
}

bool L2Login_RequestGGAuth::parse( L2_VERSION ver )
{
	UNREFERENCED_PARAMETER(ver);
	if( getPacketType() != 0x07 ) return false;
	sessionId[0] = readUChar();
	sessionId[1] = readUChar();
	sessionId[2] = readUChar();
	sessionId[3] = readUChar();
	return true;
}
