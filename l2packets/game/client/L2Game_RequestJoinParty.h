#ifndef L2GAME_REQUESTJOINPARY_H_
#define L2GAME_REQUESTJOINPARY_H_

#include "../L2GamePacket.h"

#define L2_PARTY_LOOT_ITEM_LOOTER       0
#define L2_PARTY_LOOT_ITEM_RANDOM       1
#define L2_PARTY_LOOT_ITEM_RANDOM_SPOIL 2
#define L2_PARTY_LOOT_ITEM_ORDER        3
#define L2_PARTY_LOOT_ITEM_ORDER_SPOIL  4

class L2Game_RequestJoinParty: public L2GamePacket
{
public:
	static const unsigned int ITEM_LOOTER = 0;
	static const unsigned int ITEM_RANDOM = 1;
	static const unsigned int ITEM_RANDOM_SPOIL = 2;
	static const unsigned int ITEM_ORDER = 3;
	static const unsigned int ITEM_ORDER_SPOIL = 4;
public:
	L2Game_RequestJoinParty();
	L2Game_RequestJoinParty( const unsigned char *bytes, unsigned int length );
public:
	bool create( const wchar_t *invitePlayer, unsigned int lootRule );
public:
	bool         read_invitePlayer( wchar_t *out );
	unsigned int read_lootRule();
};

#endif
