#include "stdafx.h"
#include "L2Game_AuthLoginFail.h"

L2Game_AuthLoginFail::L2Game_AuthLoginFail()
{
	_initNull();
}

L2Game_AuthLoginFail::L2Game_AuthLoginFail( const unsigned char *bytes, unsigned int length )
{
	_initNull();
	setBytes( bytes, length );
}

bool L2Game_AuthLoginFail::parse( L2_VERSION ver )
{
	UNREFERENCED_PARAMETER(ver);
	if( getPacketType() != 0x0A ) return false;
	p_reasonCode = readUInt();
	return true;
}

bool L2Game_AuthLoginFail::create( L2_VERSION ver )
{
	UNREFERENCED_PARAMETER(ver);
	setPacketType( 0x0A );
	writeUInt( p_reasonCode );
	return true;
}

void L2Game_AuthLoginFail::reasonCodeToString( int code, char *outString )
{
	switch( code )
	{
	case REASON_NO_TEXT: strcpy( outString, "REASON_NO_TEXT" ); break;
	case REASON_SYSTEM_ERROR_LOGIN_LATER: strcpy( outString, "REASON_SYSTEM_ERROR_LOGIN_LATER" ); break;
	case REASON_PASSWORD_DOES_NOT_MATCH_THIS_ACCOUNT: strcpy( outString, "REASON_PASSWORD_DOES_NOT_MATCH_THIS_ACCOUNT" ); break;
	case REASON_PASSWORD_DOES_NOT_MATCH_THIS_ACCOUNT2: strcpy( outString, "REASON_PASSWORD_DOES_NOT_MATCH_THIS_ACCOUNT2" ); break;
	case REASON_ACCESS_FAILED_TRY_LATER: strcpy( outString, "REASON_ACCESS_FAILED_TRY_LATER" ); break;
	case REASON_INCORRECT_ACCOUNT_INFO_CONTACT_CUSTOMER_SUPPORT: strcpy( outString, "REASON_INCORRECT_ACCOUNT_INFO_CONTACT_CUSTOMER_SUPPORT" ); break;
	case REASON_ACCESS_FAILED_TRY_LATER2: strcpy( outString, "REASON_ACCESS_FAILED_TRY_LATER2" ); break;
	case REASON_ACOUNT_ALREADY_IN_USE: strcpy( outString, "REASON_ACOUNT_ALREADY_IN_USE" ); break;
	case REASON_ACCESS_FAILED_TRY_LATER3: strcpy( outString, "REASON_ACCESS_FAILED_TRY_LATER3" ); break;
	case REASON_ACCESS_FAILED_TRY_LATER4: strcpy( outString, "REASON_ACCESS_FAILED_TRY_LATER4" ); break;
	case REASON_ACCESS_FAILED_TRY_LATER5: strcpy( outString, "REASON_ACCESS_FAILED_TRY_LATER5" ); break;
	default: strcpy( outString, "REASON_UNKNOWN" ); break;
	}
}

