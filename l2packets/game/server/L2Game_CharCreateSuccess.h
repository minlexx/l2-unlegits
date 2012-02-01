#ifndef L2GAME_CHARCREATEOK_H_
#define L2GAME_CHARCREATEOK_H_

#include "../L2GamePacket.h"

class L2Game_CharCreateSuccess: public L2GamePacket
{
public:
	L2Game_CharCreateSuccess();
	L2Game_CharCreateSuccess( const unsigned char *bytes, unsigned int length );
public:
	virtual bool parse( L2_VERSION ver = L2_VERSION_T1 );
	virtual bool create( L2_VERSION ver = L2_VERSION_T1 );
};

#endif
