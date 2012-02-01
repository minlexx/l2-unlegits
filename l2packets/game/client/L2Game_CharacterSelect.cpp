#include "stdafx.h"
#include "L2Game_CharacterSelect.h"

/* L2J:
protected void readImpl()
{
	_charSlot = readD();
	_unk1 = readH();
	_unk2 = readD();
	_unk3 = readD();
	_unk4 = readD();
}
*/

L2Game_CharacterSelect::L2Game_CharacterSelect()
{
	this->_initNull();
}

L2Game_CharacterSelect::L2Game_CharacterSelect( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

bool L2Game_CharacterSelect::create( unsigned int charSlot )
{
	this->writeReset();
	this->writeUChar( 0x12 ); // packet type
	this->writeUInt( charSlot );
	this->writeUShort( 0x0000 );
	this->writeUInt( 0x00000000 );
	this->writeUInt( 0x00000000 );
	this->writeUInt( 0x00000000 );
	return true;
}

bool L2Game_CharacterSelect::read_charSlot( unsigned int *charSlot )
{
	if( !charSlot || !(this->canReadBytes(5)) ) return false;
	(*charSlot) = 0;
	this->readReset();
	this->readUChar(); // pcode
	(*charSlot) = this->readUInt();
	return true;
}
