#ifndef L2GAME_ENTERWORLD_H_
#define L2GAME_ENTERWORLD_H_

#include "../L2GamePacket.h"

class L2Game_EnterWorld : public L2GamePacket
{
public:
	L2Game_EnterWorld();
	L2Game_EnterWorld( const unsigned char *bytes, unsigned int length );
public:
	virtual bool create( L2_VERSION ver = L2_VERSION_T1 );
	//void read() {} // L2J Server ignores its contents, parsing is unknown :)
};

#endif
