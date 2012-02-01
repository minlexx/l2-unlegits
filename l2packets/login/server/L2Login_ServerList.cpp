#include "stdafx.h"
#include "L2Login_ServerList.h"

/*
FA 00   // packet size - 250 bytes
04      // packet type (ServerList)
0B      // game servers count - 11
09      // WTF? Login server # ?

// repeated block for each server
01             // game server ID
D8 6B F4 82    // game server IP
61 1E 00 00    // game server port
00             // age limit?
01             // is_pvp?
6E 09          // players online   (2414)
70 17          // max players      (6000)
01             // is_up? 01 - up, 00 - down
00 00 00 00 00 // some flags here

packet ends with:
00 00 00 00 00 DA A9 B4 02 85 1A E1 26 E0
^^ dunno what is it :)

* Server will be considered as Good when the number of  online players
* is less than half the maximum. as Normal between half and 4/5
* and Full when there's more than 4/5 of the maximum number of players
*/

L2Login_ServerList::L2Login_ServerList()
{
	this->_initNull();
}

L2Login_ServerList::L2Login_ServerList( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

// lsID can be NULL
bool L2Login_ServerList::read_header( unsigned char *gsCount, unsigned char *lsID )
{
	if( gsCount == NULL ) return false;
	if( !this->canReadBytes(2) ) return false;
	this->readReset();
	this->getPacketType();
	(*gsCount) = this->readUChar();
	if( lsID ) (*lsID) = this->readUChar(); else this->readUChar();
	return true;
}

bool L2Login_ServerList::read_next_GS_Info( struct L2GameServerInfo *gsi )
{
	if( !gsi ) return false;
	// game server info consists of 21 bytes-block
	if( !this->canReadBytes( 21 ) )
	{
#ifdef _DEBUG
		printf( "L2Login_ServerList::read_next_GS_Info(): error: cannot"
			" read next 21 bytes\n" );
#endif
		return false;
	}
	//this->readBytes( (unsigned char *)gsi, sizeof(struct L2GameServerInfo) );
	unsigned char c1 = 0, c2 = 0;
	// server ID - 1 byte
	gsi->gsID = this->readUChar();
	// server IP - 4 bytes
	gsi->gsIP[0] = this->readUChar();
	gsi->gsIP[1] = this->readUChar();
	gsi->gsIP[2] = this->readUChar();
	gsi->gsIP[3] = this->readUChar();
	// server port - 2 + 2 bytes
	c1 = this->readUChar();
	c2 = this->readUChar();
	gsi->gsPort  = ((unsigned char)c1);
	gsi->gsPort |= ((unsigned char)c2) << 8;
	this->readUChar();
	this->readUChar();
	// age limit - 1 byte
	gsi->gsAgeLimit = this->readUChar();
	// is pvp - 1 byte
	gsi->gsIsPVP = this->readUChar();
	// players online (2 bytes)
	c1 = this->readUChar();
	c2 = this->readUChar();
	gsi->gsPlayersOnline  = ((unsigned char)c1);
	gsi->gsPlayersOnline |= ((unsigned char)c2) << 8;
	// max players online (2 bytes)
	c1 = this->readUChar();
	c2 = this->readUChar();
	gsi->gsPlayersMax  = ((unsigned char)c1);
	gsi->gsPlayersMax |= ((unsigned char)c2) << 8;
	// server is up?
	gsi->gsIsUp = this->readUChar();
	// next 5 bytes - some special flags, not very interesting
	this->readBytes( gsi->gsFlags, 5 );
	return true;
}
