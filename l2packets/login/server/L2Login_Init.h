#ifndef LOGIN_INIT_H_
#define LOGIN_INIT_H_

#include "../L2LoginPacket.h"

/**
 * \class L2Login_Init
 * Handles Init packet XOR encryption/decryption, RSA public key modulus scrambling/unscrambling, 
 * general Init packet parsing and creation.
 */

class L2Login_Init: public L2LoginPacket
{
public:
	/** Default constructor */
	L2Login_Init();
	/** Constructs object and calls setBytes() with parameters bytes, length
	 * \param bytes data to set
	 * \param length data size */
	L2Login_Init( const unsigned char *bytes, unsigned int length );

protected:
	void _initPublicMembers();

public: // Init packet related encoding/decoding functions
	/** Used only immediately after BF decoding */
	virtual bool         decodeXOR();
	/** Static function used to decrypt RSA public key modulus from Init packet
	 * \param data must be 128-bytes array */
	static bool          unscramble_RSA_PubKeyMod( unsigned char *data );
	/** Static function used to encrypt RSA public key modulus from Init packet
	 * \param data must be 128-bytes array */
	static bool          scramble_RSA_PubKeyMod( unsigned char *data );
public:	
	/** Called after decoding Init packet (removing Blowfish, XOR)
	 * \param sidBytes must point to array 4 bytes long */
	virtual bool         read_SessionID( unsigned char *sidBytes );
	/** Read login protocol version (after reading sessionID) */
	virtual unsigned int read_ProtoVer();
	/** Read RSA public key modulus after reading login protocol version
	 * \param RSApublicKeyMod must point to buffer 128 bytes len */
	virtual bool         read_RSA_pubKeyMod( unsigned char *RSApublicKeyMod );
	/** Read some 16 bytes, possibly related to GameGuard (contents and meaning unknown). 
	 * Called after reading scrambled RSA public key modulus
	 * \param ggShit - 16 bytes buffer; can be NULL */
	virtual bool         read_GGShit( unsigned char *ggShit );
	/** Read dynamic Blowfish key, which is used to encrypt/decrypt all following packets in session.
	 * Called after reading 16 bytes GGshit.
	 * \param newBFKey - 16 bytes buffer; must NOT be NULL. */
	virtual bool         read_DynamicBFKey( unsigned char *newBFKey );

public: // for debugging
	/** Dumps Init packet to FILE line by line..
	 * \param f - FILE * pointer to opened file. */
	virtual void         displaySelfNice( FILE *f );

public: // easily create/parse Init packet using public class data members
	/** Creates Init packet using public data members: p_sessionId, p_protoVer, p_RSA_pubKeyMod, p_GG_shit, p_BF_dyn_key.
	 * Can be used by server app. */
	virtual bool create( L2_VERSION ver = L2_VERSION_T1 );
	/** Parses Init packet, received from server. Result will be in public data members: 
	 * p_sessionId, p_protoVer, p_RSA_pubKeyMod, p_GG_shit, p_BF_dyn_key.\n
	 * Performs BF & XOR decryption, reading data, unscrambles RSA public key modulus. */
	virtual bool parse( L2_VERSION ver = L2_VERSION_T1 );
public:
	unsigned char p_sessionId[4]; ///< session ID, used in some packets later in session.
	unsigned int  p_protoVer; ///< login server protocol version. For >= Interlude it must be 0x0000c621
	unsigned char p_RSA_pubKeyMod[128]; ///< RSA public key modulus
	unsigned char p_GG_shit[16]; ///< some bytes
	unsigned char p_BF_dyn_key[16]; ///< dynamic Blowfish key
};

#endif /* LOGIN_INIT_H_ */
