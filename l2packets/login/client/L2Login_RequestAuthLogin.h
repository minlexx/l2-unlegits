#ifndef L2LOGIN_REQUESTAUTHLOGIN_H_
#define L2LOGIN_REQUESTAUTHLOGIN_H_

#include "../L2LoginPacket.h"

#define L2_LOGIN_MAXLEN    14
#define L2_PASSWORD_MAXLEN 16

class L2Login_RequestAuthLogin: public L2LoginPacket
{
public:
	L2Login_RequestAuthLogin();
	L2Login_RequestAuthLogin( const unsigned char *bytes, unsigned int length );
public:
	// RSA_pubKeyMod must point to 128-bytes length buffer
	bool create( const char *szL2Login, const char *szL2Password,
				unsigned int ggAuthResponse, const unsigned char *RSA_pubKeyMod );
	//virtual bool create( L2_VERSION ver = L2_VERSION_T1 );
	virtual bool parse( L2_VERSION ver = L2_VERSION_T1 );

public:
	char strLogin[32];
	char strPassword[32];
	unsigned int uGGAuthResponse;

protected:
	// for internal use only
	// encode block with login/password
	// RSA_pubKeyMod must point to 128-bytes length buffer
	bool block_encode_RSA( unsigned char *block , unsigned int blockSize,
		const unsigned char *RSA_pubKeyMod );
	bool block_decode_RSA( unsigned char *block , unsigned int blockSize,
		const unsigned char *RSA_pubKeyMod );
};

#endif /*L2LOGIN_REQUESTAUTHLOGIN_H_*/
