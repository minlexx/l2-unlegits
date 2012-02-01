#ifndef L2GAME_ACTION_H_
#define L2GAME_ACTION_H_

#include "../L2GamePacket.h"

class L2Game_Action : public L2GamePacket
{
public:
	L2Game_Action();
	L2Game_Action( const unsigned char *bytes, unsigned int length );
public:
	bool create( unsigned int objectID,
				  unsigned int originX,
				  unsigned int originY,
				  unsigned int originZ,
				  unsigned char bUseShift );
	unsigned int read_objectID();
	int          read_originX();
	int          read_originY();
	int          read_originZ();
	char         read_useShift();
};

#endif /*L2GAME_CHARACTERSELECT_H_*/
