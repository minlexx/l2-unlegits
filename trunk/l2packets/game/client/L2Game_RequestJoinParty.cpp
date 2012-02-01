#include "stdafx.h"
#include "L2Game_RequestJoinParty.h"

/* L2J:
Client: Len   15
0F 00
42
6C 00 6F 00 6C 00 00 00   // target char name
00 00 00 00               // loot type, item distribution
*/

L2Game_RequestJoinParty::L2Game_RequestJoinParty()
{
	this->_initNull();
}

L2Game_RequestJoinParty::L2Game_RequestJoinParty( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

bool L2Game_RequestJoinParty::create( const wchar_t *invitePlayer, unsigned int lootRule )
{
	if( !invitePlayer ) return false;
	writeReset();
	writeUChar( 0x42 );
	writeUnicodeString( invitePlayer );
	writeUInt( lootRule );
	return true;
}

bool L2Game_RequestJoinParty::read_invitePlayer( wchar_t *out )
{
	if( !out ) return false;
	readReset();
	wcscpy( out, readUnicodeStringPtr() );
	return true;
}

unsigned int L2Game_RequestJoinParty::read_lootRule()
{
	return readUInt();
}
