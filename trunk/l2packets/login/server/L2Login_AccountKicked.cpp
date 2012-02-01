#include "stdafx.h"
#include "L2Login_AccountKicked.h"

/*
writeC(0x02);
writeD(_reason.getCode());
*/

L2Login_AccountKicked::L2Login_AccountKicked()
{
	this->_initNull();
	p_reasonCode = 0;
}

L2Login_AccountKicked::L2Login_AccountKicked( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
	p_reasonCode = 0;
}

unsigned int L2Login_AccountKicked::read_reason()
{
	if( !this->canReadBytes(5) ) return 0;
	this->readReset();
	this->readUChar();
	return this->readUInt();
}

void L2Login_AccountKicked::getReasonStr( unsigned int code, char *str )
{
	if( !str ) return;
	strcpy( str, "REASON_UNKNOWN" );
	switch( code )
	{
	case REASON_DATA_STEALER: strcpy( str, "REASON_DATA_STEALER" ); break;
	case REASON_GENERIC_VIOLATION: strcpy( str, "REASON_GENERIC_VIOLATION" ); break;
	case REASON_7_DAYS_SUSPENDED: strcpy( str, "REASON_7_DAYS_SUSPENDED" ); break;
	case REASON_PERMANENTLY_BANNED: strcpy( str, "REASON_PERMANENTLY_BANNED" ); break;
	}
}

void L2Login_AccountKicked::getReasonStr( char *str )
{
	L2Login_AccountKicked::getReasonStr( p_reasonCode, str );
}

bool L2Login_AccountKicked::parse( L2_VERSION ver )
{
	UNREFERENCED_PARAMETER(ver);
	if( this->getPacketType() != 0x02 ) return false;
	p_reasonCode = readUInt();
	return true;
}

bool L2Login_AccountKicked::create( L2_VERSION ver )
{
	UNREFERENCED_PARAMETER(ver);
	setPacketType( 0x02 );
	writeUInt( p_reasonCode );
	return true;
}
