#ifndef L2GAME_REQEUSTGOTOLOBBY_H_
#define L2GAME_REQUESTGOTOLOBBY_H_

#include "../L2GamePacket.h"

class L2Game_RequestGotoLobby : public L2GamePacket
{
public:
	L2Game_RequestGotoLobby();
	L2Game_RequestGotoLobby( const unsigned char *bytes, unsigned int length );
public:
	bool create( L2_VERSION ver = L2_VERSION_T1 );
	bool parse( L2_VERSION ver = L2_VERSION_T1 );
};

#endif /*L2GAME_CHARACTERSELECT_H_*/
