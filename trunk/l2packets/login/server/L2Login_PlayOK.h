#ifndef L2LOGIN_PLAYOK_H_
#define L2LOGIN_PLAYOK_H_

#include "../L2LoginPacket.h"

class L2Login_PlayOK : public L2LoginPacket
{
public:
	L2Login_PlayOK();
	L2Login_PlayOK( const unsigned char *bytes, unsigned int length );
public:
	// sessionKey2 must point to 8-byte len buffer
	bool read_sessionKey2( unsigned char *sessionKey2 );
public:
	bool create( L2_VERSION ver = L2_VERSION_T1 );
	bool parse( L2_VERSION ver = L2_VERSION_T1 );
public:
	unsigned char p_sessionKey2[8];
};

#endif /*L2LOGIN_PLAYOK_H_*/
