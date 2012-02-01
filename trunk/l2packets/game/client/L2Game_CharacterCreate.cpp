#include "stdafx.h"
#include "L2Game_CharacterCreate.h"

/*
Client: Len   63 [CharacterCreate]
3F 00
0C             // opcode
41 00 65 00  72 00 69 00  6E 00 00 00     // name
01 00 00 00    // race
01 00 00 00    // sex
19 00 00 00    // class id
1C 00 00 00    // INT
27 00 00 00    // STR
1E 00 00 00    // CON
1B 00 00 00    // MEN
23 00 00 00    // DEX
0B 00 00 00    // WIT
00 00 00 00    // hair style
00 00 00 00    // hair color
00 00 00 00    // face
*/

L2Game_CharacterCreate::L2Game_CharacterCreate()
{
	_initNull();
}

L2Game_CharacterCreate::L2Game_CharacterCreate( const unsigned char *bytes, unsigned int length )
{
	_initNull();
	setBytes( bytes, length );
}

bool L2Game_CharacterCreate::create( const L2Game_NewCharacterTemplate *tmpl, const wchar_t *name,
		int hairStyle, int hairColor, int face, int gender )
{
	if( !tmpl ) return false;
	setPacketType( 0x0C ); // CharacterCreate
	writeS( name );
	writeD( tmpl->race );
	writeD( gender );
	writeD( tmpl->classID );
	writeD( tmpl->base_INT );
	writeD( tmpl->base_STR );
	writeD( tmpl->base_CON );
	writeD( tmpl->base_MEN );
	writeD( tmpl->base_DEX );
	writeD( tmpl->base_WIT );
	writeD( hairStyle );
	writeD( hairColor );
	writeD( face );
	return true;
}
