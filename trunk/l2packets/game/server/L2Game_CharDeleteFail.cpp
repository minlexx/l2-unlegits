#include "stdafx.h"
#include "L2Game_CharDeleteFail.h"

L2Game_CharDeleteFail::L2Game_CharDeleteFail()
{
	_initNull();
}

L2Game_CharDeleteFail::L2Game_CharDeleteFail( const unsigned char *bytes, unsigned int length )
{
	_initNull();
	setBytes( bytes, length );
}

bool L2Game_CharDeleteFail::parse( L2_VERSION ver )
{
	UNREFERENCED_PARAMETER(ver);
	if( getPacketType() != 0x1E ) return false;
	p_reasonCode = readUInt();
	return true;
}

bool L2Game_CharDeleteFail::create( L2_VERSION ver )
{
	UNREFERENCED_PARAMETER(ver);
	setPacketType( 0x1E );
	writeUInt( p_reasonCode );
	return true;
}

void L2Game_CharDeleteFail::reasonCodeToString( int code, char *outString )
{
	switch( code )
	{
	case REASON_DELETION_FAILED: strcpy( outString, "REASON_DELETION_FAILED" ); break;
	case REASON_YOU_MAY_NOT_DELETE_CLAN_MEMBER: strcpy( outString, "REASON_YOU_MAY_NOT_DELETE_CLAN_MEMBER" ); break;
	case REASON_CLAN_LEADERS_MAY_NOT_BE_DELETED: strcpy( outString, "REASON_CLAN_LEADERS_MAY_NOT_BE_DELETED" ); break;
	default: strcpy( outString, "REASON_UNKNOWN" ); break;
	}
}

