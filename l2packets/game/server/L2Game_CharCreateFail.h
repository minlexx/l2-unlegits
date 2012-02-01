#ifndef L2GAME_CHARCREATEFAIL_H_
#define L2GAME_CHARCREATEFAIL_H_

#include "../L2GamePacket.h"

class L2Game_CharCreateFail: public L2GamePacket
{
public:
	static const int REASON_CREATION_FAILED     = 0x00;
	static const int REASON_TOO_MANY_CHARACTERS = 0x01;
	static const int REASON_NAME_ALREADY_EXISTS = 0x02;
	static const int REASON_16_ENG_CHARS        = 0x03;
	static const int REASON_INCORRECT_NAME      = 0x04;
	static const int REASON_CREATE_NOT_ALLOWED  = 0x05;
	static const int REASON_CHOOSE_ANOTHER_SVR  = 0x06;
public:
	L2Game_CharCreateFail();
	L2Game_CharCreateFail( const unsigned char *bytes, unsigned int length );
public:
	virtual bool parse( L2_VERSION ver = L2_VERSION_T1 );
	virtual bool create( L2_VERSION ver = L2_VERSION_T1 );
public:
	static void reasonCodeToString( int code, char *outString );
public:
	unsigned int p_reasonCode;
};

#endif
