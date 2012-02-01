#ifndef H_L2GAME_NEWCHARACTER
#define H_L2GAME_NEWCHARACTER

#include "../L2GamePacket.h"

class L2Game_NewCharacter: public L2GamePacket
{
public:
	L2Game_NewCharacter();
	L2Game_NewCharacter( const unsigned char *bytes, unsigned int length );
public:
	bool create( L2_VERSION ver = L2_VERSION_T1 );
	bool parse( L2_VERSION ver = L2_VERSION_T1 );
};

#endif
