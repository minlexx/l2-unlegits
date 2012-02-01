#ifndef L2GAME_CHARDELETEFAIL_H_
#define L2GAME_CHARDELETEFAIL_H_

#include "../L2GamePacket.h"

class L2Game_CharDeleteFail: public L2GamePacket
{
public:
	static const int REASON_DELETION_FAILED = 0x01;
	static const int REASON_YOU_MAY_NOT_DELETE_CLAN_MEMBER = 0x02;
	static const int REASON_CLAN_LEADERS_MAY_NOT_BE_DELETED = 0x03;
public:
	L2Game_CharDeleteFail();
	L2Game_CharDeleteFail( const unsigned char *bytes, unsigned int length );
public:
	virtual bool parse( L2_VERSION ver = L2_VERSION_T1 );
	virtual bool create( L2_VERSION ver = L2_VERSION_T1 );
public:
	static void reasonCodeToString( int code, char *outString );
public:
	unsigned int p_reasonCode;
};

#endif
