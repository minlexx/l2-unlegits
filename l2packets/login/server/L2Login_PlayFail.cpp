#include "stdafx.h"
#include "L2Login_PlayFail.h"

/*
writeC(0x06);
writeC(_reason.getCode());
*/

L2Login_PlayFail::L2Login_PlayFail()
{
	this->_initNull();
}

L2Login_PlayFail::L2Login_PlayFail( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

unsigned char L2Login_PlayFail::read_reason()
{
	if( this->canReadBytes(2) )
	{
		this->readReset();
		this->readUChar();
		return this->readUChar();
	}
	return 0;
}

// str must point to buffer at least 32 bytes long
bool L2Login_PlayFail::getReasonStr( unsigned char code, char *str )
{
	if( !str ) return false;
	strcpy( str, "REASON_UNKNOWN" );
	switch( code )
	{
	case REASON_SYSTEM_ERROR: strcpy( str, "REASON_SYSTEM_ERROR" ); break;       
	case REASON_USER_OR_PASS_WRONG: strcpy( str, "REASON_USER_OR_PASS_WRONG" ); break; 
	case REASON3: strcpy( str, "REASON3" ); break;
	case REASON_ACCESS_FAILED: strcpy( str, "REASON_ACCESS_FAILED" ); break;
	case REASON_ACCOUNT_INFORMATION_INCORRECT: strcpy( str, "REASON_ACCOUNT_INFORMATION_INCORRECT" ); break;
	case REASON_TOO_MANY_PLAYERS: strcpy( str, "REASON_TOO_MANY_PLAYERS" ); break;
	}
	return true;
}

bool L2Login_PlayFail::create( L2_VERSION ver /*= L2_VERSION_T1*/ )
{
	UNREFERENCED_PARAMETER(ver);
	setPacketType( 0x06 );
	writeUChar( p_reasonCode );
	return true;
}

bool L2Login_PlayFail::parse( L2_VERSION ver /*= L2_VERSION_T1*/ )
{
	UNREFERENCED_PARAMETER(ver);
	if( getPacketType() != 0x06 ) return false;
	if( !canReadBytes(1) ) return false;
	p_reasonCode = readUChar();
	return true;
}

