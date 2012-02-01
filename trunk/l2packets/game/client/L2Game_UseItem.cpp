#include "stdafx.h"
#include "L2Game_UseItem.h"

/*
0B 00                       // len 11
19                          // ptype - UseItem
57 EA 03 10                 // objectID? 268692055
00 00 00 00                 // wtf nulls?
*/

L2Game_UseItem::L2Game_UseItem()
{
	this->_initNull();
}

L2Game_UseItem::L2Game_UseItem( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

bool L2Game_UseItem::create( unsigned int oid )
{
	this->writeReset();
	this->writeUChar( 0x19 ); // packet type - UseItem
	this->writeUInt( oid );
	this->writeUInt( 0x00000000 );
	return true;
}

unsigned int L2Game_UseItem::read_objectID()
{
	if( !(this->canReadBytes(5)) ) return 0;
	this->readReset();
	this->readUChar(); // pcode
	return this->readUInt();
}
