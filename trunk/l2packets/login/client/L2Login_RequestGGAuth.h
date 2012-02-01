#ifndef LOGIN_REQUESTGGAUTH_H_
#define LOGIN_REQUESTGGAUTH_H_

#include "../L2LoginPacket.h"

class L2Login_RequestGGAuth: public L2LoginPacket
{
public:
	L2Login_RequestGGAuth();
	L2Login_RequestGGAuth( const unsigned char *bytes, unsigned int length );
	
public:
	virtual bool create( L2_VERSION ver = L2_VERSION_T1 );
	virtual bool parse( L2_VERSION ver = L2_VERSION_T1 );
public:
	unsigned char sessionId[4];
};

#endif /*LOGIN_REQUESTGGAUTH_H_*/
