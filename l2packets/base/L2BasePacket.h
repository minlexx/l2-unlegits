#ifndef H_L2BASEPACKET
#define H_L2BASEPACKET

#include "ByteArray.h"
#include "../L2_versions.h"

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(x) // will produce compiler warnings, anyway
#endif

/** \class L2BasePacket
* Represents base class for all L2 packets. Provides way to create packets,
* write data to packets, read data from them.\n
* Internally uses ByteArray object to hold raw packet data bytes.
* Allows writing data/reading data from packet, using readXXXX or writeXXXX functions.\n
* \n
* In Lineage protocol, all packets start with 2 bytes, which hold all packet length (including these first 2 bytes). 
* These 2 bytes are never encrypted, so client application receives first 2 bytes,
* treates them as packet length and then reads the rest length-2 bytes.\n
* \n
* L2BasePacket internally uses ByteArray object to hold packet data. Initially created L2BasePacket object
* has preallocated space for 256 bytes in its internal storage (ByteArray::setSize(256)), so most write
* operations do not cause packet to grow. If 256 bytes is not enough, L2BasePacket automatically doubles
* size of its internal buffer, and so on.\n
* \n
* \n
*<pre>
*  Lineage II packet structure:\n
*        Size field               packet data
*  |----------------------|--------------------------|
*  | low byte | high byte | opcode | ... ... ... ... |
*  |----------------------|--------------------------|
*    0          1           2        3      ..........  << byte ordinal numbers
* </pre>
* \n
* Size field is 16-bit integer, defines size of ALL packet, including size field itself. So,
* maximum length of L2 packet is 65536 bytes (maximum data size is 65534).
*/

class L2BasePacket
{
public:
	/** Default constructor for empty packet. Preallocates buffer size for 256 bytes. */
	L2BasePacket();
	/** Copies length bytes to packet */
	L2BasePacket( const unsigned char *bytes, unsigned int length ); // does memcpy()
	/** Frees object memory */
	virtual ~L2BasePacket();

public:
	/** Reads byte at index from internal storage. Calls ByteArray::getByteAt()\n
	 * May throw L2P_ReadException in case of range error.
	 \param index - byte index to read
	 \return byte value, or 0 if index is out of range.
	 */
	virtual unsigned char  getByteAt( unsigned int index );
	/** Sets byte at specified index to value. Does nothing if index out of bounds. Calls ByteArray::setByteAt()\n
	 * May throw L2P_WriteException in case of range error.
	 * \param index byte index
	 * \param byte byte value to set
	 * \return previous byte value
	 */
	virtual unsigned char  setByteAt( unsigned int index, unsigned char byte );
	/** Copies bytes to internal storage buffer. Resizes internal buffer if needed. Calls ByteArray::setBytes()
	 * \param bytes pointer to source data buffer to copy from
	 * \param length source buffer size
	 * \return success state
	 */
	virtual bool           setBytes( const unsigned char *bytes, unsigned int length );
	/** Sets internal ByteArray's buffer pointer to [bytes], and length to [length]. Does no memcpy()\n
	 * Do not use this function!
	 * \param bytes pointer to new data buffer
	 * \param length source buffer size
	 * \return success state
	 * \see ByteArray::setBytesFromPtrNoMemcpy() */
	virtual bool           setBytesPtr( unsigned char *bytes, unsigned int length );
	/** Sets 3rd byte of packet (which holds packet opcode).
	 * Equivalent to call writeReset() and writeUChar() with parameter type. 
	 * Moves write pointer to byte #3 (next after opcode)
	 * \param type new packet opcode. */
	virtual void           setPacketType( unsigned char type );
	/** Sets 3rd,4th,5th bytes of packet (which holds packet opcode and extended opcode).
	 * Equivalent to call writeReset(); writeUChar(); writeUShort() with parameter type. 
	 * Moves write pointer!
	 * \param opcode new packet opcode.
	 * \param opcode2 extended opcode. */
	virtual void           setPacketType2( unsigned char opcode, unsigned short opcode2 );
	/** Sets 3rd,4th,5th,6th,7th bytes of packet (which holds packet opcode, ext opcode, ext opcode 2).
	 * Equivalent to call writeReset(); writeUChar(); writeUShort() writeUShort() with parameter type. 
	 * Moves write pointer!
	 * \param opcode new packet opcode.
	 * \param opcode2 extended opcode.
	 * \param opcode3 extended opcode 2.
	 */
	virtual void           setPacketType3( unsigned char opcode, unsigned short opcode2, unsigned short opcode3 );
	/** Reads packet opcode. Moves read pointer to byte #3 (next after opcode). Equivalent calls: readReset(); readUChar()
	 * \return read packet opcode. */
	virtual unsigned char  getPacketType() { readReset(); return readUChar(); }
	/** Gets packet length in bytes (not size of internal data buffer, which is larger)
	 * \return packet lentgh */
	virtual unsigned short getPacketSize() const { return (unsigned short)(this->real_size); }
	/** Gets number of data bytes in packet (excluding 1st 2 bytes, which hold all packet size).
	 * Equivalent to getPacketSize()-2.
	 * \return number of <b>data</b> bytes in buffer. */
	virtual unsigned short getDataSize() const { return (unsigned short)(this->datasize); }

public: // write funcs
	// All of read/write funcs can throw subclasses of L2P_Exception in case of read/write errors.
	/** Checks if it is possible to write nBytes bytes to packet; if no, tries to increase buffer size.
	 * Called automatically from write-functions.
	 * \param nBytes checked size
	 * \return false on memory error */
	virtual bool           ensureCanWriteBytes( unsigned int nBytes );
	/** Moves write pointer to the beginning of the packet, to byte #2, where starts opcode. 
	 * First 2 bytes (byte #0, byte #1 - packet size) are always updated automatically, and 
	 * they cannot be accessed by read/write-functions, only by setByteAt() / getByteAt().
	 * Pre-allocates internal buffer, sets packet size to 2. */
	virtual void           writeReset();
	/** Writes single byte to packet. Moves write pointer one char right
	 * \param c byte to write to packet. */
	virtual void           writeChar( char c );
	/** Writes single byte to packet. Moves write pointer one char right
	 * \param c byte to write to packet. */
	virtual void           writeUChar( unsigned char c );
	/** Writes short integer to packet. Moves write pointer two chars right
	 * \param s value to write to packet. */
	virtual void           writeShort( short int s );
	/** Writes short integer to packet. Moves write pointer two chars right
	 * \param s value to write to packet. */
	virtual void           writeUShort( unsigned short int s );
	/** Writes 32-bit integer to packet. Moves write pointer 4 chars right
	 * \param i value to write to packet. */
	virtual void           writeInt( int i );
	/** Writes 32-bit integer to packet. Moves write pointer 4 chars right
	 * \param i value to write to packet. */
	virtual void           writeUInt( unsigned int i );
	/** Writes 64-bit integer to packet. Moves write pointer 8 chars right
	 * \param i64 value to write to packet. */
	virtual void           writeInt64( long long int i64 );
	/** Writes 64-bit integer to packet. Moves write pointer 8 chars right
	 * \param i64 value to write to packet. */
	virtual void           writeUInt64( unsigned long long int i64 );
	/** Writes double value to packet. Moves write pointer 8 chars right
	 * \param d value to write to packet. */
	virtual void           writeDouble( double d );
	/** Writes ANSI string to packet, also NULL terminator. Moves write pointer right by string length
	 * \param str string to write to packet. */
	virtual void           writeString( const char *str );
	/** Writes Unicode string to packet, also Unicode NULL terminator. Moves write pointer right by string length*2
	 * \param ustr string to write to packet. */
	virtual void           writeUnicodeString( const wchar_t *ustr );
	/** Writes bytes array to packet (len bytes). Moves write pointer right by len bytes
	 * \param bytes pointer to data to write to packet.
	 * \param len data length. */
	virtual void           writeBytes( const unsigned char *bytes, unsigned int len );
public: // read funcs
	// All of read/write funcs can throw subclasses of L2P_Exception in case of read/write errors.
	/** Checks if there are some bytes left to read form current read position in packet.
	 * \param nBytes number of bytes to test to readability.
	 * \return true, if nBytes bytes can be read form current read position; false if there are less than 
	 * nBytes left in buffer. */
	virtual bool           canReadBytes( unsigned int nBytes );
	/** Moves read pointer to byte #2 (opcode will be read next). */
	virtual void           readReset();
	/** Reads one byte from packet. Moves read pointer to next position
	 * \return byte */
	virtual char           readChar();
	/** Reads one byte from packet. Moves read pointer to next position
	 * \return byte */
	virtual unsigned char  readUChar();
	/** Reads 16-bit integer from packet. Moves read pointer to next position
	 * \return short int */
	virtual short int      readShort();
	/** Reads 16-bit integer from packet. Moves read pointer to next position
	 * \return short int */
	virtual unsigned short int readUShort();
	/** Reads 32-bit integer from packet. Moves read pointer to next position
	 * \return int */
	virtual int            readInt();
	/** Reads 32-bit integer from packet. Moves read pointer to next position
	 * \return int */
	virtual unsigned int   readUInt();
	/** Reads 64-bit integer from packet. Moves read pointer to next position
	 * \return long long int */
	virtual long long int  readInt64();
	/** Reads 64-bit integer from packet. Moves read pointer to next position
	 * \return long long int */
	virtual unsigned long long int readUInt64();
	/** Reads double value from packet. Moves read pointer to next position
	 * \return double */
	virtual double         readDouble();
	/** Reads ANSI string from packet. Moves read pointer to next read position
	 * \return pointer to allocated string */
	virtual char          *readString();
	/** Reads Unicode string from packet. Moves read pointer to next read position. Allocates buffer.
	 * \return pointer to allocated string. Calling function must free() this buffer. */
	virtual wchar_t       *readUnicodeString();
	/** Reads Unicode string from packet. Moves read pointer to next read position.\n
	 * Function does not allocate buffer,
	 * it just returns casted to (wchar_t *) pointer to internal data buffer as read-only.\n
	 * Warning! Returning pointer will be invalid, because internal data buffer is freed when object is destroyed.\n
	 * Warning! Data is read-only.\n
	 * This function may be faster than readUnicodeString() because it does not do
	 * memory allocation.
	 * \return pointer to allocated string */
	virtual const wchar_t *readUnicodeStringPtr();
	/** Reads num bytes from packet
	 * \param bytes pointer to buffer to store data
	 * \param num number of bytes to read
	 * \returns success state */
	virtual bool           readBytes( unsigned char *bytes, unsigned int num );
public: // L2J aliases
	// writers
	/** Alias to writeChar() */
	virtual inline void writeC( char c )                 { writeChar( c ); }
	/** Alias to writeShort() */
	virtual inline void writeH( short h )                { writeShort( h ); }
	/** Alias to writeInt() */
	virtual inline void writeD( int d )                  { writeInt( d ); }
	/** Alias to writeInt64() */
	virtual inline void writeQ( long long int Q )        { writeInt64( Q ); }
	/** Alias to writeDouble() */
	virtual inline void writeF( double f )               { writeDouble( f ); }
	/** Alias to writeUnicodeString() */
	virtual inline void writeS( const wchar_t *string )  { writeUnicodeString( string ); }
	/** Alias to writeBytes() */
	virtual inline void writeB( const ByteArray& bytes ) { writeBytes( bytes.getBytesPtr(), bytes.getSize() ); }
	// readers
	/** Alias to readChar() */
	virtual inline char           readC() { return readChar(); }
	/** Alias to readShort() */
	virtual inline short          readH() { return readShort(); }
	/** Alias to readInt() */
	virtual inline int            readD() { return readInt(); }
	/** Alias to readDouble() */
	virtual inline double         readF() { return readDouble(); }
	/** Alias to readInt64() */
	virtual inline long long int  readQ() { return readInt64(); }
	/** Alias to readUnicodeString() */
	virtual inline wchar_t       *readS() { return readUnicodeString(); }
	/** Alias to readBytes() */
	virtual        ByteArray     *readB( unsigned int count ); // not inline :)
public: // parsers/creators
	/** Parses packet. Must be overriden in child classes */
	virtual bool parse( L2_VERSION ver = L2_VERSION_T1 );
	/** Default method to create packet. Must be overriden in subclasses */
	virtual bool create( L2_VERSION ver = L2_VERSION_T1 );
	
	// DEBUG funcs
public:
	/** Writes packet dump to file f n HEX
	 * \param f - file to write to */
	virtual void           dumpToFile( FILE *f );
	/** Saves all packet to file as binary data
	 * \param filename - file name to save to */
	virtual void           saveToFileRaw( const char *filename );
	/** here calls dumpToFile(). Must be overriden in child classes
	 * \param f - file to write to */
	virtual void           displaySelfNice( FILE *f );
public: // typeconv
	//virtual operator const unsigned char *() const;
	/** Returns pointer to all internal data buffer. Calls ByteArray::getBytesPtr()
	 * \return pointer to all internal data buffer */
	virtual const unsigned char *getBytesPtr() const;
protected:
	/** Internal function - called from constuctors. Initializes object state:
	 * Sets buffer_size to 256, calls readReset(), writeReset() */
	virtual void _initNull();
	/** Internal function. Pre-allocates internal buffer to buffer_size */
	virtual bool _preAllocateBuffer();
	/** Internal function. Tries to increase buffer size twice */
	virtual bool _growBuffer();
	/** almost the same as _initNull()... */
	virtual void _freeSelf();
protected:
	unsigned int buffer_size; ///< current buffer size
	unsigned int real_size; ///< number of used bytes in buffer
	unsigned int write_ptr; ///< index of next byte that will be written to buffer
	unsigned int read_ptr;  ///< index of next byte that will be read from buffer
	unsigned int datasize;  ///< number of data bytes in packet (excluding first 2 bytes that hold packet size)
	ByteArray b; ///< internal buffer to hold raw packet data
};

#endif
