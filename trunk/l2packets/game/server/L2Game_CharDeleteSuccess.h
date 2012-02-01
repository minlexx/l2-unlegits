#ifndef L2GAME_CHARDELETESUCCESS_H_
#define L2GAME_CHARDELETESUCCESS_H_

#include "../L2GamePacket.h"

class L2Game_CharDeleteSuccess: public L2GamePacket
{
public:
	L2Game_CharDeleteSuccess();
	L2Game_CharDeleteSuccess( const unsigned char *bytes, unsigned int length );
// nothing to parse - just trigger
};

#endif
