#ifndef L2GAME_PROTOCOLVERSION_H_
#define L2GAME_PROTOCOLVERSION_H_

#include "../L2GamePacket.h"

class L2Game_ProtocolVersion : public L2GamePacket
{
public:
	L2Game_ProtocolVersion();
	L2Game_ProtocolVersion( const unsigned char *bytes, unsigned int length );
public:
	bool create( L2_VERSION ver = L2_VERSION_T1 );
	bool createDefaultKamael( unsigned int gameProtoVer = 828 );
	bool createDefaultHellbound( unsigned int gameProtoVer = 851 );
	bool createDefaultGracia1( unsigned int gameProtoVer = 12 );
	bool createDefaultGracia2( unsigned int gameProtoVer = 17 );
	bool createDefaultGracia3( unsigned int gameProtoVer = 83 );
	bool createDefaultGracia4( unsigned int gameProtoVer = 146 );
	bool read_protoVer( unsigned int *pver );
};

#endif /*L2GAME_PROTOCOLVERSION_H_*/
