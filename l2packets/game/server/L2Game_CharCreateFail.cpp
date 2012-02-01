#include "stdafx.h"
#include "L2Game_CharCreateFail.h"

L2Game_CharCreateFail::L2Game_CharCreateFail()
{
	_initNull();
}

L2Game_CharCreateFail::L2Game_CharCreateFail( const unsigned char *bytes, unsigned int length )
{
	_initNull();
	setBytes( bytes, length );
}

bool L2Game_CharCreateFail::parse( L2_VERSION ver )
{
	UNREFERENCED_PARAMETER(ver);
	if( getPacketType() != 0x10 ) return false;
	p_reasonCode = readUInt();
	return true;
}

bool L2Game_CharCreateFail::create( L2_VERSION ver )
{
	UNREFERENCED_PARAMETER(ver);
	setPacketType( 0x10 );
	writeUInt( p_reasonCode );
	return true;
}

void L2Game_CharCreateFail::reasonCodeToString( int code, char *outString )
{
	switch( code )
	{
	case REASON_CREATION_FAILED: strcpy( outString, "REASON_CREATION_FAILED" ); break;
	case REASON_TOO_MANY_CHARACTERS: strcpy( outString, "REASON_TOO_MANY_CHARACTERS" ); break;
	case REASON_NAME_ALREADY_EXISTS: strcpy( outString, "REASON_NAME_ALREADY_EXISTS" ); break;
	case REASON_16_ENG_CHARS: strcpy( outString, "REASON_16_ENG_CHARS" ); break;
	default: strcpy( outString, "REASON_UNKNOWN" ); break;
	}
}

