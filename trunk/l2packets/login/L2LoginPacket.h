#ifndef H_L2LOGINPACKET
#define H_L2LOGINPACKET

#include "../base/L2BasePacket.h"

/** \class L2LoginPacket
 * A base class for all login server communication packets.
 * Includes functions to handle Blowfish encryption/decryption and checksum calculations. */

class L2LoginPacket: public L2BasePacket
{
public:
	/** Default constructor */
	L2LoginPacket();
	/** Constructs object and calls setBytes() with parameters bytes, length
	 * \param bytes data to set
	 * \param length data size */
	L2LoginPacket( const unsigned char *bytes, unsigned int length );
	virtual ~L2LoginPacket();
public:
	/** Does Blowfish decryption of packet data.
	 * \param bUseStaticBFKey set to true to use static BF key, or false to dynamic.
	          Dynamic BF key must be set before by setDynamicBFKey()
	 * \return success status */
	virtual bool         decodeBlowfish( bool bUseStaticBFKey );

	/** Does Blowfish decryption of packet data.
	 * \param blowfishKey - 16 bytes array, blowfish key to use to decrypt packet data
	 * \return success status */
	virtual bool         decodeBlowfish( unsigned char *blowfishKey );
public:
	/** Appends checksum to packet
	 * \param append4bytes in Hellbound, set to true to keep packet data length 8-byte aligned */
	virtual bool         appendChecksum( bool append4bytes = true );
	/** Verifies data checksum, XORing count bytes starting from offset.\n
	 * Checksum must be written at last 4 bytes of data, so data size must be >= 8 at least.
	 * \param bytes data array
	 * \param offset starting position
	 * \param size data size in bytes (should be a multiple of 4)
	 * \return true, if checksum is ok */
	static bool          verifyBytesChecksum( const unsigned char *bytes, unsigned int offset, unsigned int size );
	/** Verifies this packet checksum. */
	virtual bool         verifyChecksum() const;
	/** Does padding of packet data by zeroes to 8-byte border (before BF encryption) */
	virtual bool         padPacketTo8ByteLen();
	/** In Hellbound, packets are padded by more 8 zero bytes after adding checksum & 4 bytes */
	virtual bool         appendMore8Bytes();
	/** Sets dynamic Blowfish key (which is taken from Init packet)
	 * \param newKey must point at 16 bytes buffer
	 * \param newKeyLen currently must be = 16 */
	virtual bool         setDynamicBFKey( unsigned char *newKey, unsigned int newKeyLen );
	/** Does Blowfish encryption.
	 * \param bUseStaticBFKey set to true to use static BF key, or false to dynamic.
	          Dynamic BF key must be set before by setDynamicBFKey()
	 * \return success status */
	virtual bool         encodeBlowfish( bool bUseStaticBFKey );

public:
	/** Encrypts packet with Blowfish, does all padding, appends checksum - after that packet may be sent
	 * \param blowfishKey - Blowfish key to use for encryption (16 bytes)
	 * \param bfKeyLen - length of Blowfish key (default is 16)
	 * \return success status */
	virtual bool encodeAndPrepareToSend( unsigned char *blowfishKey, unsigned int bfKeyLen = 16 );

protected:
	/** Also initializes STATIC_BF_KEY, which is constant. */
	virtual void _initNull();
protected:
	unsigned char STATIC_BLOWFISH_KEY[32]; ///< Static Blowfish key (First packet from login server is encrypted by this)
	unsigned int  STATIC_BLOWFISH_KEY_LEN; ///< length of static BF key
	unsigned char NEW_BLOWFISH_KEY[64]; ///< Buffer to hold dynamic blowfish key
	unsigned int  NEW_BLOWFISH_KEY_LEN; ///< Dynamic BF key size
	unsigned int  xor_key; ///< current XOR key (for Init packet)
};

#endif /* H_L2LOGINPACKET */
