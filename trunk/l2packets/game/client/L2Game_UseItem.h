#ifndef L2GAME_USEITEM_H_
#define L2GAME_USEITEM_H_

#include "../L2GamePacket.h"

class L2Game_UseItem: public L2GamePacket
{
public:
	L2Game_UseItem();
	L2Game_UseItem( const unsigned char *bytes, unsigned int length );
public:
	bool create( unsigned int oid );
public:
	unsigned int read_objectID();
};

#endif
