#include "stdafx.h"
#include "L2Game_RequestGotoLobby.h"

L2Game_RequestGotoLobby::L2Game_RequestGotoLobby()
{
	this->_initNull();
}

L2Game_RequestGotoLobby::L2Game_RequestGotoLobby( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

bool L2Game_RequestGotoLobby::create( L2_VERSION ver )
{
	if( ver <= L2_VERSION_T22 ) setPacketType2( 0xD0, 0x0039 );
	if( ver >= L2_VERSION_T23 ) setPacketType2( 0xD0, 0x0036 );
	return true;
}

bool L2Game_RequestGotoLobby::parse( L2_VERSION ver )
{
	if( getPacketType() != 0xD0 ) return false;
	if( ver <= L2_VERSION_T22 )
		if( readUShort() != 0x0039 ) return false;
	if( ver >= L2_VERSION_T23 )
		if( readUShort() != 0x0036 ) return false;
	return true;
}
