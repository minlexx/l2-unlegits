#ifndef L2LOGIN_REQUESTSERVERLIST_H_
#define L2LOGIN_REQUESTSERVERLIST_H_

#include "../L2LoginPacket.h"

/*
22 00                    // packet size - 34 bytes
05                       // packet type - RequestServerList
e0 43 ef 46  8e dc 83 f2 // sessionKey #1
04 00 00 00  00 00 00    // some 7 bytes
b1 6a 9f 6c  00 00 00 00 // checksum and 4 0x00 bytes of checksum padding
00 00 00 00  00 00 00 00 // 8 0x00 bytes padding
*/

class L2Login_RequestServerList : public L2LoginPacket
{
public:
	L2Login_RequestServerList();
	L2Login_RequestServerList( const unsigned char *bytes, unsigned int length );
public:
	// sessionKey1 - 8-byte array (from LoginOK packet)
	bool create( const unsigned char *sessionKey1 );
};

#endif /*L2LOGIN_REQUESTSERVERLIST_H_*/
