#ifndef L2LOGIN_ACCOUNTKICKED_H_
#define L2LOGIN_ACCOUNTKICKED_H_

#include "../L2LoginPacket.h"

class L2Login_AccountKicked : public L2LoginPacket
{
public:
	static const unsigned int REASON_DATA_STEALER       = 0x01;
	static const unsigned int REASON_GENERIC_VIOLATION  = 0x08;
	static const unsigned int REASON_7_DAYS_SUSPENDED   = 0x10;
	static const unsigned int REASON_PERMANENTLY_BANNED = 0x20;
public:
	L2Login_AccountKicked();
	L2Login_AccountKicked( const unsigned char *bytes, unsigned int length );
public:
	unsigned int read_reason();
	// str must point to buffer at 32 bytes long
	static void getReasonStr( unsigned int code, char *str );
	void getReasonStr( char *str );
public:
	virtual bool parse( L2_VERSION ver = L2_VERSION_T1 );
	virtual bool create( L2_VERSION ver = L2_VERSION_T1 );
public:
	unsigned int p_reasonCode;
};

#endif /*L2LOGIN_ACCOUNTKICKED_H_*/
