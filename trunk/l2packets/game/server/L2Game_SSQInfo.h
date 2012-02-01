#ifndef L2GAME_SSQINFO_H_
#define L2GAME_SSQINFO_H_

#include "../L2GamePacket.h"

class L2Game_SSQInfo : public L2GamePacket
{
public:
	static const unsigned short int SSQ_DAWN_SKY = 258;
	static const unsigned short int SSQ_DUSK_SKY = 257;
	static const unsigned short int SSQ_NORMAL_SKY = 256;
public:
	L2Game_SSQInfo();
	L2Game_SSQInfo( const unsigned char *bytes, unsigned int length );
public:
	unsigned short int read_SSQ_SkySatus();
};

#endif /* L2GAME_SSQINFO_H_*/
