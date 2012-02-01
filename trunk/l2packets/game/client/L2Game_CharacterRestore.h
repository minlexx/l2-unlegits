#ifndef H_L2GAME_CHARACTERRESTORE
#define H_L2GAME_CHARACTERRESTORE

#include "../L2GamePacket.h"

class L2Game_CharacterRestore: public L2GamePacket
{
public:
	L2Game_CharacterRestore();
	L2Game_CharacterRestore( const unsigned char *bytes, unsigned int length );
public:
	bool create( L2_VERSION ver = L2_VERSION_T1 );
	bool parse( L2_VERSION ver = L2_VERSION_T1 );
public:
	int p_charSlot;
};

#endif
