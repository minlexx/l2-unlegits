#ifndef H_BYTEARRAY
#define H_BYTEARRAY

/** \class ByteArray
* Represents array of bytes with variable size.\n
* L2Packets programmer does not work with ByteArray directly.
* This object is used inside L2BasePacket to hold raw packet data bytes.
*/

class ByteArray
{
public: // constructors/destructors
	/** Default constructor with zero array length */
	ByteArray();
	/** Constructs ByteArray object with specified array length
	* \param length - size in bytes to reserve space
	*/
	ByteArray( unsigned int length );
	/** Constructs ByteArray object with length equal to length of string \n
	 * and with contents of this string. \n
	 * Calls setBytesFromString( string );
	 * \param string - string from which to set contents of created ByteArray
	 */
	ByteArray( const char *string );
	/** Copy constructor */
	ByteArray( const ByteArray& ba );
	/** Constructs ByteArray object and sets its size and contents.
	 * \param newBytes pointer to data to set contents
	 * \param length data size
	 */
	ByteArray( const unsigned char *newBytes, unsigned int length );
	~ByteArray();

public:	// size funcs
	/** \return Current array size. */
	inline unsigned int   getSize() const { return this->byteCount; }
	/** sets new size of buffer. Contents remain unchanged (but may be truncated). \n
	 * In general, calls C runtime realloc()\n
	 * May throw L2P_MemoryError exception, if enabled.
	 * \param newSize new size of buffer. Must be >= 0
	 */
	bool                  setSize( unsigned int newSize );

public: // data functions
	/** \return pointer to internal data buffer */
	inline unsigned char *getBytesPtr() const { return this->bytes; }
	
	/** Get byte value at specified index.\n
	 * May throw L2P_ReadException, if enabled.
	 * \param index Index of byte to retreive. If index is out of array bounds, function returns zero.
	 * \return byte alue at index, or 0 if index does not belong to array.
	 */
	unsigned char         getByteAt( unsigned int index ) const;
	/** Sets byte at specified index.\n
	 * May throw L2P_WriteException, if enabled.
	 * \param index Byte index to set. If index does not belong to array, function does nothing.
	 * \param byteSet new byte value
	 * \return previous byte value */
	unsigned char         setByteAt( unsigned int index, unsigned char byteSet );
	
	/** Fills array with specified byte value.
	 * \param c value to assign to all bytes. */
	void                  memset( unsigned char c );
	/** Sets all array contents. Data is copied to internal storage
	 * \param newBytes data to set.
	 * \param length length of data. If length is > current array length, function resizes array to fit contents.
	 * \return true, if set was ok. false, if there were errors (memory failure?)
	 */
	bool                  setBytes( const unsigned char *newBytes, unsigned int length );
	/** Sets array data from string data. Terminating zero is not appended.
	 * \param string string from which to copy bytes.
	 * \return true/false */
	bool                  setBytesFromString( const char *string );
	/** Sets internal pointer to data buffer and internal bytes counter from specified pointer to data and size.\n
	 * Data is not copied (works much faster)\n
	 * This function is not recommended to use, because:\n
	 * WARNING! buffer must be allocated by the same C runtime as library uses - malloc(), realloc(), calloc(). \n
	 * WARNING! object frees its buffer when destructor executes, so, free() will be called for source buffer.
	 * \param bytes data to set.
	 * \param length length of data. If length is > current array length, function resizes array to fit contents.
	 * \return true, if set was ok.*/
	bool                  setBytesFromPtrNoMemcpy( unsigned char *bytes, unsigned int length );

public: // operators
	/** Calls getByteAt( index ) */
	unsigned char operator[]( int index ) const;

protected: // internal functions
	void                  _initNull();
	void                  _freeSelf();

	// debug functions
#ifdef _DEBUG
public:
	void                   setDebugMode( bool bDebug, FILE *flog );
	int                    _debugPrintf( const char *_Format, ... );
	void                   dumpArrayInfo( FILE * fOut );
#endif
	
protected:
	// data
	unsigned int   byteCount; ///< internal array size counter
	unsigned char *bytes; ///< internal pointer to allocated buffer
	// debug mode vars
#ifdef _DEBUG
	bool           bDebugMode;
	FILE          *fLogFile;
#endif
};

#endif
