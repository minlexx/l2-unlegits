#include "stdafx.h"
#include "L2Game_NewCharacter.h"

/**
Client: Len 3 [NewCharacter]
03 00
13        // opcode
*/

L2Game_NewCharacter::L2Game_NewCharacter()
{
	_initNull();
}

L2Game_NewCharacter::L2Game_NewCharacter( const unsigned char *bytes, unsigned int length )
{
	_initNull();
	setBytes( bytes, length );
}

bool L2Game_NewCharacter::create( L2_VERSION ver )
{
	ver = L2_VERSION_T1;
	setPacketType( 0x13 );
	return true;
}

bool L2Game_NewCharacter::parse( L2_VERSION ver )
{
	ver = L2_VERSION_T1;
	// just trugger
	return true;
}
