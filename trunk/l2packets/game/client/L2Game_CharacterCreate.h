#ifndef H_L2GAME_CHARACTERCREATE
#define H_L2GAME_CHARACTERCREATE

#include "../L2GamePacket.h"
#include "../server/L2Game_NewCharacterSuccess.h"

class L2Game_CharacterCreate: public L2GamePacket
{
public:
	L2Game_CharacterCreate();
	L2Game_CharacterCreate( const unsigned char *bytes, unsigned int length );
public:
	bool create( const L2Game_NewCharacterTemplate *tmpl, const wchar_t *name,
		int hairStyle, int hairColor, int face, int gender );
};

#endif
