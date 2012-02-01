#ifndef L2PCODEOBFUSCATOR_H_
#define L2PCODEOBFUSCATOR_H_

#include "../L2_versions.h"

/** \class L2PCodeObfuscator
* Manages opcode obfuscation engine.
*/
class L2PCodeObfuscator
{
public:
	/** Creates default object with version set to T1 */
	L2PCodeObfuscator();
	/** Destructor calls clear() */
	virtual ~L2PCodeObfuscator();

public:
	/** Sets protocol version.\n
	* different protcols have different sizes of opcode obfuscation tables.\n
	* Default is version T1 (Kamael does not have opcode obfuscation at all!)
	* \param new_version - specifies version to set. Must be one of: L2_VERSION_T1, L2_VERSION_T15, L2_VERSION_T2, L2_VERSION_T22, L2_VERSION_T23.
	* \return none
	*/
	void           setVersionMode( L2_VERSION new_version ) { m_version = new_version; }
	/** Set version Hellbound. Equvalent to setVersionMode( L2_VERSION_T15 ); */
	void           setVersionMode_T15() { setVersionMode( L2_VERSION_T15 ); }
	/** Set version Gracia Part 1. Equvalent to setVersionMode( L2_VERSION_T2 ); */
	void           setVersionMode_T2()  { setVersionMode( L2_VERSION_T2  ); }
	/** Set version Gracia Part 2. Equvalent to setVersionMode( L2_VERSION_T22 ); */
	void           setVersionMode_T22() { setVersionMode( L2_VERSION_T22 ); }
	/** Set version Gracia Final. Equvalent to setVersionMode( L2_VERSION_T23 ); */
	void           setVersionMode_T23() { setVersionMode( L2_VERSION_T23 ); }
	
	/** Frees object memory
	* \return none
	*/
	void           clear();
	/** Initializes obfuscation tables using seed (obfuscator code)
	* \param seed - initial PRNG value
	* \return none
	*/
	void           init_tables( unsigned int seed );
	/** Test if obfuscation is enabled. Obfuscator is enabled after init_tables() was called.
	* \return 1, if enabled; 0 otherwise
	*/
	int            isEnabled() const { return m_enabled; }

	/** Decodes opcode in packet (deobfuscates) 
	* \param packet_data_raw - packet bytes (all, starting with 2 first bytes - packet len)
	*/
	int            decodeIDs( unsigned char *packet_data_raw );
	/** Encodes opcode in packet (obfuscates)
	* \param packet_data_raw - packet bytes (all, starting with 2 first bytes - packet len)
	*/
	int            encodeIDs( unsigned char *packet_data_raw );

	bool           decodeOpcode( unsigned char &singleOpcode, unsigned short &doubleOpcode );
	bool           encodeOpcode( unsigned char &singleOpcode, unsigned short &doubleOpcode );
	bool           decodeOpcode( unsigned char &singleOpcode, unsigned short &doubleOpcode, unsigned short &tripleOpcode );
	bool           encodeOpcode( unsigned char &singleOpcode, unsigned short &doubleOpcode, unsigned short &tripleOpcode );

protected:
	/** Internal function used to mix opcode tables */
	void           pseudo_srand( unsigned int seed );
	/** Internal function - generates next pseudo random value. Used to mix opcode tables */
	unsigned int   pseudo_rand();

protected:
	L2_VERSION     m_version; ///< game version.
	int            m_enabled; ///< =1, if enabled (tables were initialized).

	unsigned char *m_DecodeTable1;
	unsigned char *m_DecodeTable2;
	unsigned char *m_DecodeTable3;

	unsigned char *m_EncodeTable1;
	unsigned char *m_EncodeTable2;
	unsigned char *m_EncodeTable3;

	unsigned int   m_seed; ///< current PRNG seed
	unsigned int   m_s1; ///< size of 1-byte opcode obfuscator table (depends on version)
	unsigned int   m_s2; ///< size of 2-byte opcode obfuscator table (depends on version)
	unsigned int   m_s3; ///< size of 2-byte opcode obfuscator table (depends on version)
};

#endif /* L2PCODEOBFUSCATOR_H_ */
