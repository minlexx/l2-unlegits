#include "stdafx.h"
#include "L2Game_CharacterDelete.h"

L2Game_CharacterDelete::L2Game_CharacterDelete()
{
	_initNull();
}

L2Game_CharacterDelete::L2Game_CharacterDelete( const unsigned char *bytes, unsigned int length )
{
	_initNull();
	setBytes( bytes, length );
}

bool L2Game_CharacterDelete::create( L2_VERSION ver )
{
	ver = L2_VERSION_T1;
	setPacketType( 0x0D ); // CharacterDelete
	writeD( p_charSlot );
	return true;
}

bool L2Game_CharacterDelete::parse( L2_VERSION ver )
{
	ver = L2_VERSION_T1;
	if( getPacketType() != 0x0D ) return false;
	p_charSlot = readD();
	return true;
}
