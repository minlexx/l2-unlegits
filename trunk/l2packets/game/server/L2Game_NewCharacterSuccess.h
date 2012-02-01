#ifndef H_L2GAME_NEWCHARACTERSUCCESS
#define H_L2GAME_NEWCHARACTERSUCCESS

#include "../L2GamePacket.h"

struct L2Game_NewCharacterTemplate
{
	int race;
	int classID;
	int base_STR;
	int base_DEX;
	int base_CON;
	int base_INT;
	int base_WIT;
	int base_MEN;
};

class L2Game_NewCharacterSuccess: public L2GamePacket
{
public:
	L2Game_NewCharacterSuccess();
	L2Game_NewCharacterSuccess( const unsigned char *bytes, unsigned int length );
public:
	int  read_templatesCount();
	bool read_nextCharacterTemplate( struct L2Game_NewCharacterTemplate *t );
};

#endif
