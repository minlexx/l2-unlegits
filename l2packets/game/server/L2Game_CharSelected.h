#ifndef L2GAME_CHARSELECTED_H_
#define L2GAME_CHARSELECTED_H_

#include "../L2GamePacket.h"

class L2Game_CharSelected : public L2GamePacket
{
public:
	L2Game_CharSelected();
	L2Game_CharSelected( const unsigned char *bytes, unsigned int length );
public:
	virtual bool parse( L2_VERSION ver = L2_VERSION_T1 );
	virtual bool create( L2_VERSION ver = L2_VERSION_T1 );
public:
	wchar_t      p_char_name[128]; // char name
	unsigned int p_charId; // char id (objectId)
	wchar_t      p_title[128];  // title (empty here)
	unsigned int p_sessionId;  // sessionId
	unsigned int p_clanId; // clan id
	int          p_sex; // sex
	int          p_race; // race 0x01 - elf
	unsigned int p_classId;  // class Id
	int          p_isActive;  // is Active
	int          p_x;
	int          p_y;
	int          p_z;
	double       p_curHp; // currentHp
	double       p_curMp; // currentMp
	unsigned int p_sp; // sp
	unsigned long long int p_exp; // exp
	int          p_level; // level (64)
	int          p_karma; // karma
	int          p_PK_kills; // PK kills
	int          p_INT; // INT
	int          p_STR; // STR
	int          p_CON; // CON
	int          p_MEN; // MEN
	int          p_DEX; // DEX
	int          p_WIT; // WIT
	unsigned int p_opcodeObfuscatorSeed; // opcode obfuscator seed
};

#endif
