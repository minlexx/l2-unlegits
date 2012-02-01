#include "stdafx.h"
#include "L2Login_PlayOK.h"

/*
1A 00                     // packet size - 26 bytes
07                        // packet code - LoginOK
8C BF 00 00  C7 41 0B 00  // Session Key #2
09 2E 5C 18  41 04 00     // padding??
1F 03 D4 57  00 44 B7 82  // checksum??
*/

L2Login_PlayOK::L2Login_PlayOK()
{
	this->_initNull();
}

L2Login_PlayOK::L2Login_PlayOK( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

// sessionKey2 must point to 8-byte len buffer
bool L2Login_PlayOK::read_sessionKey2( unsigned char *sessionKey2 )
{
	if( !sessionKey2 ) return false;
	if( !this->canReadBytes( 9 ) ) return false;
	this->getPacketType();
	return this->readBytes( sessionKey2, 8 );
}

bool L2Login_PlayOK::create( L2_VERSION ver /*= L2_VERSION_T1*/ )
{
	UNREFERENCED_PARAMETER(ver);
	setPacketType( 0x07 );
	writeBytes( p_sessionKey2, 8 );
	return true;
}

bool L2Login_PlayOK::parse( L2_VERSION ver /*= L2_VERSION_T1*/ )
{
	UNREFERENCED_PARAMETER(ver);
	if( getPacketType() != 0x07 ) return false;
	if( !canReadBytes( 8 ) ) return false;
	readBytes( p_sessionKey2, 8 );
	return true;
}
