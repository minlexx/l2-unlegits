#ifndef L2P_XCPT
#define L2P_XCPT

/** \class L2P_Exception
 * Base class for exceptions that may be thrown from deep inside functions from library
 * (ByteArray, L2BasePacket) */
class L2P_Exception
{
public:
	/** Default constructor, no comments */
	L2P_Exception(): m_what(NULL) {}
	/** Copy constructor */
	L2P_Exception( const L2P_Exception& other ): m_what(NULL)
	{
		this->operator=( other );
	}
	/** Destructor, frees internal string buffer if needed */
	virtual ~L2P_Exception();
	/** Returns pointer to string comment buffer
	 \ return pointer to string comment buffer or NULL, if no comment
	*/
	virtual const char *what() const;
	/** Assigns comment from other L2P_Exception object to this
	 * \param other - source to copy from */
	virtual const L2P_Exception& operator=( const L2P_Exception& other );

protected:
	char *m_what; ///< pointer to buffer to keep comment string
};

/** \class L2P_Exception Will be thrown when malloc()/realloc() fails */
class L2P_MemoryError: public L2P_Exception
{
public:
	L2P_MemoryError( const char *comment, size_t bytes );
};

/** \class L2P_Exception Will be thrown when reading past end of packet OR calling 
 * getByteAt() with index not in range of packet size */
class L2P_ReadException: public L2P_Exception
{
public:
	L2P_ReadException( const char *comment, int nBytesTriedToRead, int nPos, int nSize );
};

/** \class L2P_Exception Will be thrown when writing past end of packet (almost impossible situation) OR
 * by calling setByteAt() with index not in range of packet size */
class L2P_WriteException: public L2P_Exception
{
public:
	L2P_WriteException( const char *comment, int nBytesTriedToWrite, int nPos, int nSize );
};

class L2P_ObfuscateException: public L2P_Exception
{
public:
	L2P_ObfuscateException( int size, unsigned int wrong_opcode, int error_code );
};

class L2P_DeObfuscateException: public L2P_Exception
{
public:
	L2P_DeObfuscateException( int size, unsigned int wrong_opcode, int error_code );
};

#endif
