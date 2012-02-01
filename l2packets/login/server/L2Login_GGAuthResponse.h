#ifndef LOGIN_GGAUTHRESPONSE_H_
#define LOGIN_GGAUTHRESPONSE_H_

#include "../L2LoginPacket.h"

class L2Login_GGAuthResponse : public L2LoginPacket
{
public:
	L2Login_GGAuthResponse();
	L2Login_GGAuthResponse( const unsigned char *bytes, unsigned int length );
public:
	unsigned int read_Response();
public:
	virtual bool parse( L2_VERSION ver = L2_VERSION_T1 );
	virtual bool create( L2_VERSION ver = L2_VERSION_T1 );
public:
	unsigned int p_ggAuthResponse;
};

#endif /*LOGIN_GGAUTHRESPONSE_H_*/
