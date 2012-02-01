#include "stdafx.h"
#include "L2Game_NewCharacterSuccess.h"

/*
Server: Len  967 [NewCharacterSuccess]
C7 03
0D              // opcode
0C 00 00 00     // number of templates

//    [ for each template ]

00 00 00 00    // race
00 00 00 00    // class id
46 00 00 00    // 0x46
28 00 00 00    // base STR
0A 00 00 00    // 0x0A
46 00 00 00    // 0x46
1E 00 00 00    // base DEX
0A 00 00 00
46 00 00 00
2B 00 00 00    // base CON
0A 00 00 00
46 00 00 00
15 00 00 00    // base INT
0A 00 00 00
46 00 00 00
0B 00 00 00    // base WIT
0A 00 00 00
46 00 00 00
19 00 00 00    // base MEN
0A 00 00 00
*/

L2Game_NewCharacterSuccess::L2Game_NewCharacterSuccess()
{
	_initNull();
}

L2Game_NewCharacterSuccess::L2Game_NewCharacterSuccess( const unsigned char *bytes, unsigned int length )
{
	_initNull();
	setBytes( bytes, length );
}

int L2Game_NewCharacterSuccess::read_templatesCount()
{
	readReset();
	getPacketType();
	return readInt();
}

bool L2Game_NewCharacterSuccess::read_nextCharacterTemplate( struct L2Game_NewCharacterTemplate *t )
{
	if( !t ) return false;
	// each template is 80 bytes length (20Ds)
	if( !canReadBytes( 80 ) ) return false;
	// read
	t->race      = readD();
	t->classID   = readD();
	readD(); // 0x46
	t->base_STR  = readD();
	readD(); // 0x0A
	readD(); // 0x46
	t->base_DEX  = readD();
	readD(); // 0x0A
	readD(); // 0x46
	t->base_CON  = readD();
	readD(); // 0x0A
	readD(); // 0x46
	t->base_INT  = readD();
	readD(); // 0x0A
	readD(); // 0x46
	t->base_WIT  = readD();
	readD(); // 0x0A
	readD(); // 0x46
	t->base_MEN  = readD();
	readD(); // 0x0A
	return true;
}

