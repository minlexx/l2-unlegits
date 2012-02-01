#ifndef L2LOGIN_REQUESTSERVERLOGIN_H_
#define L2LOGIN_REQUESTSERVERLOGIN_H_

#include "../L2LoginPacket.h"

class L2Login_RequestServerLogin : public L2LoginPacket
{
public:
	L2Login_RequestServerLogin();
	L2Login_RequestServerLogin( const unsigned char *bytes, unsigned int length );
public:
	// sessionKey1 must point to 8-byte array
	// this is Session Key #1 from LoginOK packet
	bool create( const unsigned char *sessionKey1, unsigned char GameServerID );
public:
	bool read_sessionKey1( unsigned char *bytes );
	unsigned char read_GameServerID();
public:
	bool parse( L2_VERSION ver = L2_VERSION_T1 );
	bool create( L2_VERSION ver = L2_VERSION_T1 );
public:
	unsigned char p_gameServerId;
	unsigned char p_sessionKey1[8];
};

#endif /*L2LOGIN_REQUESTSERVERLOGIN_H_*/
