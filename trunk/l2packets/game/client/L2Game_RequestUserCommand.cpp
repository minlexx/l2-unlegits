#include "stdafx.h"
#include "L2Game_RequestUserCommand.h"

/* L2J:
Client Len    7 [            RequestUserCommand]
07 00
B3
00 00 00 00 // command ID

================================

Decimal command numbers:

0 - /loc
52 - /unstuck (Escape)
61 - /Mount
62 - /DisMount
77 - /time
81 - /partyinfo
88 - /AttackList
89 - /underattacklist
90 - /warlist
93 - /ChannelDelete
96 - /ChannelLeave
97 - /ChannelListUpdate
100 - /ClanPenalty
109 - /OlympiadStat
114 - /InstanceZone
*/

L2Game_RequestUserCommand::L2Game_RequestUserCommand()
{
	this->_initNull();
}

L2Game_RequestUserCommand::L2Game_RequestUserCommand( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

bool L2Game_RequestUserCommand::create( unsigned int commandID )
{
	writeReset();
	writeUChar( 0xB3 );
	writeUInt( commandID );
	return true;
}

unsigned int L2Game_RequestUserCommand::read_commandID()
{
	readReset();
	getPacketType();
	return readUInt();
}

