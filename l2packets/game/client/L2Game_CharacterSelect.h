#ifndef L2GAME_CHARACTERSELECT_H_
#define L2GAME_CHARACTERSELECT_H_

#include "../L2GamePacket.h"

class L2Game_CharacterSelect : public L2GamePacket
{
public:
	L2Game_CharacterSelect();
	L2Game_CharacterSelect( const unsigned char *bytes, unsigned int length );
public:
	bool create( unsigned int charSlot );
	bool read_charSlot( unsigned int *charSlot );
};

#endif /*L2GAME_CHARACTERSELECT_H_*/
