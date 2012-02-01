#include "stdafx.h"
#include "L2Login_RequestServerList.h"

/*
22 00                    // packet size - 34 bytes
05                       // packet type - RequestServerList
e0 43 ef 46  8e dc 83 f2 // sessionKey #1
04 00 00 00  00 00 00    // some 7 bytes
b1 6a 9f 6c  00 00 00 00 // checksum and 4 0x00 bytes of checksum padding
00 00 00 00  00 00 00 00 // 8 0x00 bytes padding
*/

L2Login_RequestServerList::L2Login_RequestServerList()
{
	this->_initNull();
}

L2Login_RequestServerList::L2Login_RequestServerList( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

bool L2Login_RequestServerList::create( const unsigned char *sessionKey1 )
{
	if( !sessionKey1 ) return false;
	this->writeReset();
	this->setPacketType( 0x05 );
	this->writeBytes( sessionKey1, 8 );
	unsigned char somebytes[7] = { 4,0,0,0, 0,0,0 };
	this->writeBytes( somebytes, sizeof(somebytes) );
	// now packet is padded at 8-byte border. all left to is add checksum
	return true;
}
