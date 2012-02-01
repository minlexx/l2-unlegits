#include "stdafx.h"
#include "L2Game_CharDeleteSuccess.h"

L2Game_CharDeleteSuccess::L2Game_CharDeleteSuccess()
{
	_initNull();
}

L2Game_CharDeleteSuccess::L2Game_CharDeleteSuccess( const unsigned char *bytes, unsigned int length )
{
	_initNull();
	setBytes( bytes, length );
}



