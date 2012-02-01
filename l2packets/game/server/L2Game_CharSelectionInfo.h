#ifndef L2GAME_CHARSELECTIONINFO_H_
#define L2GAME_CHARSELECTIONINFO_H_

#include "../L2GamePacket.h"

struct L2Game_CharSelectionInfoBlock
{
	wchar_t charName[32];
	unsigned int charID;
	wchar_t accountName[32];
	unsigned int sessionID;
	unsigned int clanID;
	unsigned int sex;
	unsigned int race;
	unsigned int baseClassID;
	unsigned int isActive;
	int x;
	int y;
	int z;
	double HP_cur;
	double MP_cur;
	unsigned int SP;
	unsigned long long Exp;
	unsigned int level;
	unsigned int karma;
	unsigned int PK_kills;
	unsigned int PVP_kills;
	unsigned int iid_hair_all;
	unsigned int iid_R_ear;
	unsigned int iid_L_ear;
	unsigned int iid_neck;
	unsigned int iid_R_finger;
	unsigned int iid_L_finger;
	unsigned int iid_head;
	unsigned int iid_R_hand;
	unsigned int iid_L_hand;
	unsigned int iid_gloves;
	unsigned int iid_chest;
	unsigned int iid_legs;
	unsigned int iid_feet;
	unsigned int iid_back;
	unsigned int iid_LR_hand;
	unsigned int iid_hair;
	unsigned int iid_hair_2;
	unsigned int iid_R_bracelet;
	unsigned int iid_L_bracelet;
	unsigned int iid_belt; // T2.3
	unsigned int hairStyle;
	unsigned int hairColor;
	unsigned int face;
	double HP_max;
	double MP_max;
	unsigned int deleteSeconds; ///< seconds left before char will be deleted, or 0 if char is not marked to deletion
	unsigned int classID;
	unsigned int lastUsedChar;
	unsigned char enchantEffect;
	unsigned short int augmentID;
	unsigned short int augmentSmth;
	unsigned int transformID;
};

class L2Game_CharSelectionInfo : public L2GamePacket
{
public:
	L2Game_CharSelectionInfo();
	L2Game_CharSelectionInfo( const unsigned char *bytes, unsigned int length );
public:
	bool read_nChars( unsigned int *ret );
	bool read_server_maxChars( unsigned int *ret ); // ret can be NULL
	bool read_next_charSelectInfoBlock( L2_VERSION l2_version,
		struct L2Game_CharSelectionInfoBlock *c );
};

#endif /*L2GAME_CHARSELECTIONINFO_H_*/
