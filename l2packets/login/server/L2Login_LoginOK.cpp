#include "stdafx.h"
#include "L2Login_LoginOK.h"

L2Login_LoginOK::L2Login_LoginOK()
{
	this->_initNull();
}

L2Login_LoginOK::L2Login_LoginOK( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

// read SessionKey #1 from packet
// sessionKey1 must point to 8-byte array
bool L2Login_LoginOK::read_sessionKey1( unsigned char *sessionKey1 )
{
	if( !sessionKey1 ) return false;
	// check packet size
	if( this->real_size < 9 ) return false;
	this->readReset();
	this->readUChar(); // pass packet code byte
	this->readBytes( sessionKey1, 8 ); // read 8 bytes of sessionKey1
	return true;
}

bool L2Login_LoginOK::create( L2_VERSION ver /*= L2_VERSION_T23*/ )
{
	UNREFERENCED_PARAMETER(ver);
	setPacketType( 0x03 );
	writeBytes( p_sessionKey1, 8 );
	writeD( 0x00 );
	writeD( 0x00 );
	writeD( 0x000003ea ); // wtf?
	writeD( 0x00 );
	writeD( 0x00 );
	writeD( 0x00 );
	//writeB(new byte[16]); // 4 Ds
	writeD( 0x00 );
	writeD( 0x00 );
	writeD( 0x00 );
	writeD( 0x00 );
	return true;
}

bool L2Login_LoginOK::parse( L2_VERSION ver /*= L2_VERSION_T23*/ )
{
	UNREFERENCED_PARAMETER(ver);
	if( getPacketType() != 0x03 ) return false;
	if( !canReadBytes( 8 ) ) return false; // we need at least 8 bytes of sessionKey1
	readBytes( p_sessionKey1, 8 );
	// ignore the rest
	return true;
}

