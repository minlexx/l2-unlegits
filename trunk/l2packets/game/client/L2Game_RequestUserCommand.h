#ifndef L2GAME_REQUESTUSERCOMMAND_H_
#define L2GAME_REQUESTUSERCOMMAND_H_

#include "../L2GamePacket.h"

class L2Game_RequestUserCommand: public L2GamePacket
{
public:
	static const unsigned int CMD_LOC = 0;
	static const unsigned int CMD_UNSTUCK = 52;
	static const unsigned int CMD_MOUNT = 61;
	static const unsigned int CMD_DISMOUNT = 62;
	static const unsigned int CMD_TIME = 77;
	static const unsigned int CMD_PARTYINFO = 81;
	static const unsigned int CMD_ATTACKLIST = 88;
	static const unsigned int CMD_UNDERATTACKLIST = 89;
	static const unsigned int CMD_WARLIST = 90;
	static const unsigned int CMD_CHANNELDELETE = 93;
	static const unsigned int CMD_CHANNELLEAVE = 96;
	static const unsigned int CMD_CHANNELLISTUPDATE = 97;
	static const unsigned int CMD_CLANPENALTY = 100;
	static const unsigned int CMD_OLYMPIADSTAT = 109;
	static const unsigned int CMD_INSTANCEZONE = 114;
public:
	L2Game_RequestUserCommand();
	L2Game_RequestUserCommand( const unsigned char *bytes, unsigned int length );
public:
	bool create( unsigned int commandID );
public:
	unsigned int read_commandID();
};

#endif
