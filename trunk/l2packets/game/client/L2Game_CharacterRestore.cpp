#include "stdafx.h"
#include "L2Game_CharacterRestore.h"

L2Game_CharacterRestore::L2Game_CharacterRestore()
{
	_initNull();
}

L2Game_CharacterRestore::L2Game_CharacterRestore( const unsigned char *bytes, unsigned int length )
{
	_initNull();
	setBytes( bytes, length );
}

bool L2Game_CharacterRestore::create( L2_VERSION ver )
{
	ver = L2_VERSION_T1;
	setPacketType( 0x7B ); // CharacterDelete
	writeD( p_charSlot );
	return true;
}

bool L2Game_CharacterRestore::parse( L2_VERSION ver )
{
	ver = L2_VERSION_T1;
	if( getPacketType() != 0x7B ) return false;
	p_charSlot = readD();
	return true;
}

