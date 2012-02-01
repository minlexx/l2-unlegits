#ifndef L2GAME_AUTHLOGINFAIL_H_
#define L2GAME_AUTHLOGINFAIL_H_

#include "../L2GamePacket.h"

class L2Game_AuthLoginFail: public L2GamePacket
{
public:
	static const int REASON_NO_TEXT = 0;
	static const int REASON_SYSTEM_ERROR_LOGIN_LATER = 1;
	static const int REASON_PASSWORD_DOES_NOT_MATCH_THIS_ACCOUNT = 2;
	static const int REASON_PASSWORD_DOES_NOT_MATCH_THIS_ACCOUNT2 = 3;
	static const int REASON_ACCESS_FAILED_TRY_LATER = 4;
	static const int REASON_INCORRECT_ACCOUNT_INFO_CONTACT_CUSTOMER_SUPPORT = 5;
	static const int REASON_ACCESS_FAILED_TRY_LATER2 = 6;
	static const int REASON_ACOUNT_ALREADY_IN_USE = 7;
	static const int REASON_ACCESS_FAILED_TRY_LATER3 = 8;
	static const int REASON_ACCESS_FAILED_TRY_LATER4 = 9;
	static const int REASON_ACCESS_FAILED_TRY_LATER5 = 10;
public:
	L2Game_AuthLoginFail();
	L2Game_AuthLoginFail( const unsigned char *bytes, unsigned int length );
public:
	virtual bool parse( L2_VERSION ver = L2_VERSION_T1 );
	virtual bool create( L2_VERSION ver = L2_VERSION_T1 );
public:
	static void reasonCodeToString( int code, char *outString );
public:
	unsigned int p_reasonCode;
};

#endif
