#ifndef L2LOGIN_LOGINFAIL_H_
#define L2LOGIN_LOGINFAIL_H_

#include "../L2LoginPacket.h"

class L2Login_LoginFail : public L2LoginPacket
{
public:
	static const unsigned int REASON_SYSTEM_ERROR =        0x01;
	static const unsigned int REASON_PASS_WRONG =          0x02;
	static const unsigned int REASON_USER_OR_PASS_WRONG =  0x03;
	static const unsigned int REASON_ACCESS_FAILED =       0x04;
	static const unsigned int REASON_ACCOUNT_IN_USE =      0x07;
	static const unsigned int REASON_SERVER_OVERLOADED =   0x0f;
	static const unsigned int REASON_SERVER_MAINTENANCE =  0x10;
	static const unsigned int REASON_TEMP_PASS_EXPIRED =   0x11;
	static const unsigned int REASON_DUAL_BOX =            0x23;
public:
	L2Login_LoginFail();
	L2Login_LoginFail( const unsigned char *bytes, unsigned int length );
public:
	unsigned int read_reason();
	// converts login fail code to human-readable error string :)
	// s must point to buffer at least 32 bytes long
	static void getReasonStr( unsigned int code, char *s );
public:
	bool create( L2_VERSION ver = L2_VERSION_T1 );
	bool parse( L2_VERSION ver = L2_VERSION_T1 );
public:
	unsigned int p_reasonCode;
};

#endif /*L2LOGIN_LOGINFAIL_H_*/
