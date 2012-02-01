#ifndef L2GAMEPACKET_H_
#define L2GAMEPACKET_H_

#include "../base/L2BasePacket.h"

class L2GamePacket : public L2BasePacket
{
public:
	L2GamePacket();
	L2GamePacket( const unsigned char *bytes, unsigned int length );
	//virtual ~L2GamePacket();
public:
	// in Hellbound key must be 16-byte array
	// key is changed after enc/dec procedure
	bool decodeXOR( unsigned char *key );
	// in Hellbound key must be 16-byte array
	// key is changed after enc/dec procedure
	bool encodeXOR( unsigned char *key );
public:
	static bool decodeXOR_buffer( unsigned char *decodeBytes, unsigned int bytesLen, unsigned char *key );
	static bool encodeXOR_buffer( unsigned char *encodeBytes, unsigned int bytesLen, unsigned char *key );
};

#endif /*L2GAMEPACKET_H_*/
