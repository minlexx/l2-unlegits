#include "stdafx.h"
#include "L2Game_CharCreateSuccess.h"

L2Game_CharCreateSuccess::L2Game_CharCreateSuccess()
{
	_initNull();
}

L2Game_CharCreateSuccess::L2Game_CharCreateSuccess( const unsigned char *bytes, unsigned int length )
{
	_initNull();
	setBytes( bytes, length );
}

bool L2Game_CharCreateSuccess::parse( L2_VERSION ver )
{
	UNREFERENCED_PARAMETER(ver);
	if( getPacketType() != 0x0F ) return false;
	if( readUInt() != 0x01 ) return false;
	return true;
}

bool L2Game_CharCreateSuccess::create( L2_VERSION ver )
{
	UNREFERENCED_PARAMETER(ver);
	setPacketType( 0x0F );
	writeUInt( 0x01 );
	return true;
}

