#ifndef L2LOGIN_LOGINOK_H_
#define L2LOGIN_LOGINOK_H_

#include "../L2LoginPacket.h"

class L2Login_LoginOK : public L2LoginPacket
{
public:
	L2Login_LoginOK();
	L2Login_LoginOK( const unsigned char *bytes, unsigned int length );
public:
	// read SessionKey #1 from packet
	// SessionKey is 8-byte array
	bool read_sessionKey1( unsigned char *sessionKey1 );
public:
	bool create( L2_VERSION ver = L2_VERSION_T23 );
	bool parse( L2_VERSION ver = L2_VERSION_T23 );
public:
	unsigned char p_sessionKey1[8];
};

#endif /*L2LOGIN_LOGINOK_H_*/
