#include "stdafx.h"
#include "L2Login_RequestServerLogin.h"

/**
 * Format is ddc
 * d: first part of session id
 * d: second part of session id
 * c: server ID
 */

L2Login_RequestServerLogin::L2Login_RequestServerLogin()
{
	this->_initNull();
}

L2Login_RequestServerLogin::L2Login_RequestServerLogin( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

// sessionKey1 must point to 8-byte array
// this is Session Key #1 from LoginOK packet
bool L2Login_RequestServerLogin::create( const unsigned char *sessionKey1,
		unsigned char GameServerID )
{
	if( !sessionKey1 || (GameServerID == 0x00) ) return false;
	// construct packet
	this->writeReset();
	this->setPacketType( 0x02 ); // RequestServerLogin
	this->writeBytes( sessionKey1, 8 );
	this->writeUChar( GameServerID );
	// pad to 8-byte border
	int i;
	for( i=0; i<6; i++ ) this->writeUChar( 0x00 );
	return true;
}

bool L2Login_RequestServerLogin::read_sessionKey1( unsigned char *bytes )
{
	if( !this->canReadBytes(8) ) return false;
	return this->readBytes( bytes, 8 );
}

unsigned char L2Login_RequestServerLogin::read_GameServerID()
{
	return this->readUChar();
}

bool L2Login_RequestServerLogin::parse( L2_VERSION ver /*= L2_VERSION_T1*/ )
{
	UNREFERENCED_PARAMETER(ver);
	if( getPacketType() != 0x02 ) return false;
	if( !canReadBytes( 9 ) ) return false;
	readBytes( p_sessionKey1, 8 );
	p_gameServerId = readUChar();
	return true;
}

bool L2Login_RequestServerLogin::create( L2_VERSION ver /*= L2_VERSION_T1*/ )
{
	UNREFERENCED_PARAMETER(ver);
	setPacketType( 0x02 ); // RequestServerLogin
	writeBytes( p_sessionKey1, 8 );
	writeUChar( p_gameServerId );
	// pad to 8-byte border
	int i;
	for( i=0; i<6; i++ ) writeUChar( 0x00 );
	return true;
}
