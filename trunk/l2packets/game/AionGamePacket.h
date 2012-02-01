#ifndef AIONGAMEPACKET_H_
#define AIONGAMEPACKET_H_

#include "../base/L2BasePacket.h"

class AionGamePacket : public L2BasePacket
{
public:
	static const unsigned char STATIC_SERVER_OPCODE = 0x54;
	static const unsigned char STATIC_CLIENT_OPCODE = 0x5D;
	static unsigned char *STATIC_XOR_KEY;
	//(const unsigned char *)"nKO/WctQ0AVLbpzfBkS6NevDYT8ourG5CRlmdjyJ72aswx4EPq1UgZhFMXH?3iI9";

public:
	AionGamePacket();
	AionGamePacket( const unsigned char *bytes, unsigned int length );

public:
	/* Decodes this packet data with Aion XORgame encryption. key is updated at every dec/enc call
	 * \param key - 8-byte lenght key buffer */
	bool decodeXOR( unsigned char *key );

	/* Encodes this packet data with Aion XORgame encryption. key is updated at every dec/enc call
	 * \param key - 8-byte lenght key buffer */
	bool encodeXOR( unsigned char *key );

	/* Decodes buffer contents with Aion XOR game encryption. key is updated at every dec/enc call
	 * \param decodeBytes pointer to data to be decrypted
	 * \param bytesLen data length in bytes
	 * \param key - 8-byte lenght key buffer */
	static bool decodeXOR_buffer( unsigned char *decodeBytes, unsigned int bytesLen, unsigned char *key );

	/* Encodes buffer contents with Aion XOR game encryption. key is updated at every dec/enc call
	 * \param decodeBytes pointer to data to be decrypted
	 * \param bytesLen data length in bytes
	 * \param key - 8-byte lenght key buffer */
	static bool encodeXOR_buffer( unsigned char *encodeBytes, unsigned int bytesLen, unsigned char *key );

public:
	static unsigned char obfuscateServerOpcode( unsigned char opcode );
	static unsigned char deobfuscateServerOpcode( unsigned char opcode );
	bool writeServerOpcode( unsigned char opcode );
	bool writeClientOpcode( unsigned char opcode );
	bool validateServerOpcode() const;
	bool validateClientOpcode() const;
	unsigned char readOpcode();

public:
	/** Creates initial XOR key - sets last 4 bytes of key. First 4 bytes are dynamic, last 4 bytes are const.\n
	 * key must point to 8-byte buffer.
	 * \param key 8-byte buffer; function updates last 4 of them. */
	static void initStaticXORKey( unsigned char *key );

protected:
	void _checkInitStaticXorKey();
};

#endif /* AIONGAMEPACKET_H_ */
