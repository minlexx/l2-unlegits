#include "stdafx.h"
#include "L2Login_LoginFail.h"

/*
   L2J:
   protected void write()
   {
       writeC(0x01);
       writeD(_reason.getCode());
   }
   
REASON_SYSTEM_ERROR			(0x01),
REASON_PASS_WRONG			(0x02),
REASON_USER_OR_PASS_WRONG	(0x03),
REASON_ACCESS_FAILED		(0x04),
REASON_ACCOUNT_IN_USE		(0x07),
REASON_SERVER_OVERLOADED	(0x0f),
REASON_SERVER_MAINTENANCE	(0x10),
REASON_TEMP_PASS_EXPIRED	(0x11),
REASON_DUAL_BOX				(0x23);
*/

L2Login_LoginFail::L2Login_LoginFail()
{
	this->_initNull();
}

L2Login_LoginFail::L2Login_LoginFail( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

unsigned int L2Login_LoginFail::read_reason()
{
	if( !this->canReadBytes(5) ) return 0;
	this->readReset();
	this->readUChar(); // pass packet type byte
	return this->readUInt();
}

// converts login fail code to human-readable error string :)
// s must point to buffer at least 32 bytes long
void L2Login_LoginFail::getReasonStr( unsigned int code, char *s )
{
	if( !s ) return;
	strcpy( s, "REASON_UNKNOWN" );
	switch( code )
	{
	case REASON_SYSTEM_ERROR       : strcpy( s, "REASON_SYSTEM_ERROR" ); break;
	case REASON_PASS_WRONG         : strcpy( s, "REASON_PASS_WRONG" ); break;
	case REASON_USER_OR_PASS_WRONG : strcpy( s, "REASON_USER_OR_PASS_WRONG" ); break;
	case REASON_ACCESS_FAILED      : strcpy( s, "REASON_ACCESS_FAILED" ); break;
	case REASON_ACCOUNT_IN_USE     : strcpy( s, "REASON_ACCOUNT_IN_USE" ); break;
	case REASON_SERVER_OVERLOADED  : strcpy( s, "REASON_SERVER_OVERLOADED" ); break;
	case REASON_SERVER_MAINTENANCE : strcpy( s, "REASON_SERVER_MAINTENANCE" ); break;
	case REASON_TEMP_PASS_EXPIRED  : strcpy( s, "REASON_TEMP_PASS_EXPIRED" ); break;
	case REASON_DUAL_BOX           : strcpy( s, "REASON_DUAL_BOX" ); break;
	}
}

bool L2Login_LoginFail::create( L2_VERSION ver /*= L2_VERSION_T1*/ )
{
	UNREFERENCED_PARAMETER(ver);
	setPacketType( 0x01 );
	writeUChar( p_reasonCode & 0xFF );
	return true;
}

bool L2Login_LoginFail::parse( L2_VERSION ver /*= L2_VERSION_T1*/ )
{
	UNREFERENCED_PARAMETER(ver);
	if( !canReadBytes( 2 ) ) return false;
	if( getPacketType() != 0x01 ) return false;
	p_reasonCode = (unsigned int)readUChar();
	return true;
}
