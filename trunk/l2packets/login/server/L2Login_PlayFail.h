#ifndef L2LOGIN_PLAYFAIL_H_
#define L2LOGIN_PLAYFAIL_H_

#include "../L2LoginPacket.h"

class L2Login_PlayFail : public L2LoginPacket
{
public:
	static const int REASON_SYSTEM_ERROR        = 0x01;
	static const int REASON_USER_OR_PASS_WRONG  = 0x02;
	static const int REASON3                    = 0x03; // same as REASON_USER_OR_PASS_WRONG
	static const int REASON_ACCESS_FAILED       = 0x04;
	static const int REASON_ACCOUNT_INFORMATION_INCORRECT = 0x05;
	static const int REASON_TOO_MANY_PLAYERS    = 0x0f;
public:
	L2Login_PlayFail();
	L2Login_PlayFail( const unsigned char *bytes, unsigned int length );
public:
	unsigned char read_reason();
	// str must point to buffer at least 32 bytes long
	static bool getReasonStr( unsigned char code, char *str );
public:
	bool create( L2_VERSION ver = L2_VERSION_T1 );
	bool parse( L2_VERSION ver = L2_VERSION_T1 );
public:
	unsigned char p_reasonCode;
};

#endif /*L2LOGIN_PLAYFAIL_H_*/
