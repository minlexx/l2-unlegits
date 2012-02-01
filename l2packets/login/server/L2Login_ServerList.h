#ifndef L2LOGIN_SERVERLIST_H_
#define L2LOGIN_SERVERLIST_H_

#include "../L2LoginPacket.h"

struct L2GameServerInfo
{
	unsigned char      gsID;            // game server #
	unsigned char      gsIP[4];         // IP of game server
	unsigned short int gsPort;          // port of game server
	unsigned char      gsAgeLimit;      // age limit
	unsigned char      gsIsPVP;         // is this PvP server
	unsigned short int gsPlayersOnline; // current online players
	unsigned short int gsPlayersMax;    // max online players
	unsigned char      gsIsUp;          // is game server online?
	unsigned char      gsFlags[5];      // some special flags
};

class L2Login_ServerList : public L2LoginPacket
{
public:
	L2Login_ServerList();
	L2Login_ServerList( const unsigned char *bytes, unsigned int length );
public:
	// lsID can be NULL
	bool read_header( unsigned char *gsCount, unsigned char *lsID );
	bool read_next_GS_Info( struct L2GameServerInfo *gsi );
};

#endif /*L2LOGIN_SERVERLIST_H_*/
