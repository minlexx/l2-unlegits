#ifndef H_L2_VERSIONS
#define H_L2_VERSIONS

typedef enum eL2_VERSION
{
	/** constant to set protocol version as Kamael */
	L2_VERSION_T1 = 0,
	/** constant to set protocol version as Hellbound */
	L2_VERSION_T15 = 1,
	/** constant to set protocol version as Gracia Part 1 */
	L2_VERSION_T2 = 2,
	/** constant to set protocol version as Gracia Part 2 */
	L2_VERSION_T22 = 3,
	/** constant to set protocol version as Gracia Final */
	L2_VERSION_T23 = 4,
	/** constant to set protocol version as Gracia Epilogue */
	L2_VERSION_T24 = 5
} L2_VERSION;

#endif
