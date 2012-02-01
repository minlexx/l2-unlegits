#include "stdafx.h"
#include "L2BasePacket.h"
#include "../xcpt/L2Packets_xcpt.h"

L2BasePacket::L2BasePacket()
{
	this->_initNull();
}

L2BasePacket::L2BasePacket( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

L2BasePacket::~L2BasePacket()
{
	_freeSelf();
}

bool L2BasePacket::setBytes( const unsigned char *bytes, unsigned int length )
{
	_initNull();
	if( !bytes || (length < 2) ) return false;
	//
	//printf( "L2BasePacket::setBytes( 0x%p, %u )\n", bytes, length );
	//
	this->ensureCanWriteBytes( length );
	bool ret = this->b.setBytes( bytes, length );
	if( ret )
	{
		this->buffer_size = b.getSize();
		this->real_size = length;
		this->datasize  = length - 2; // length always >= 2 here
		this->write_ptr = length;
		this->read_ptr  = 2;
		// should we update packet size field in packet (0,1 bytes) here?, YES, imo
		this->b.setByteAt( 0, (unsigned char)((length)      & 0xff) );
		this->b.setByteAt( 1, (unsigned char)((length >> 8) & 0xff) );
	}
	return ret;
}

bool L2BasePacket::setBytesPtr( unsigned char *bytes, unsigned int length )
{
	if( !bytes || (length < 2) ) return false;
	if( this->b.setBytesFromPtrNoMemcpy( bytes, length ) )
	{
		this->buffer_size = b.getSize();
		this->real_size = length;
		this->datasize = length - 2;
		this->write_ptr = length;
		this->read_ptr = 2;
		// should we update packet size field in packet (0,1 bytes) here?, YES, imo
		this->b.setByteAt( 0, (unsigned char)((length)      & 0xff) );
		this->b.setByteAt( 1, (unsigned char)((length >> 8) & 0xff) );
		return true;
	}
	return false;
}

void L2BasePacket::setPacketType( unsigned char type )
{
	writeReset();
	writeUChar( type );
}

void L2BasePacket::setPacketType2( unsigned char opcode, unsigned short opcode2 )
{
	writeReset();
	writeUChar( opcode );
	writeUShort( opcode2 );
}

void L2BasePacket::setPacketType3( unsigned char opcode, unsigned short opcode2, unsigned short opcode3 )
{
	writeReset();
	writeUChar( opcode );
	writeUShort( opcode2 );
	writeUShort( opcode3 );
}

/*unsigned char L2BasePacket::getPacketType()
{
	readReset();
	return readUChar();
}*/

/*unsigned short L2BasePacket::getPacketSize() const
{
	return (unsigned short)(this->real_size);
}*/
/*unsigned short L2BasePacket::getDataSize() const// number of DATA bytes in packet
{
	return (unsigned short)(this->datasize);
}*/

bool L2BasePacket::ensureCanWriteBytes( unsigned int nBytes )
{
	if( nBytes == 0 ) return true;
	unsigned int end_ptr = write_ptr + nBytes - 1; // calc write end pointer
	// loop until end_ptr fits in buffer size
	while( end_ptr >= buffer_size ) // not enough place in buffer
	{
		if( !this->_growBuffer() ) return false; // _growBuffer() increases buffer_size
	}
	return true;
}

void L2BasePacket::writeChar( char c )
{
	if( !this->ensureCanWriteBytes( 1 ) ) return; // TODO: Throw exception??
	this->b.setByteAt( this->write_ptr, (unsigned char)c );
	this->write_ptr++;
	this->real_size++;
	this->datasize++;
	// update packet size
	// 1st 2 bytes contain only DATA size? no IMO
	unsigned short psize = (unsigned short)real_size;
	//unsigned short psize = (unsigned short)datasize;
	unsigned char *psaddr = (unsigned char *)&psize;
	b.setByteAt( 0, psaddr[0] );
	b.setByteAt( 1, psaddr[1] );
}

void L2BasePacket::writeUChar( unsigned char c )
{
	if( !this->ensureCanWriteBytes( 1 ) ) return; // TODO: Throw exception??
	//printf( "L2BasePacket::writeUChar(): writing [0x%02X] at idx [%d]\n",
	//	(unsigned int)c, this->write_ptr );
	this->b.setByteAt( this->write_ptr, c );
	this->write_ptr++;
	this->real_size++;
	this->datasize++;
	// update packet size
	unsigned short psize = (unsigned short)real_size;
	unsigned char *psaddr = (unsigned char *)&psize;
	b.setByteAt( 0, psaddr[0] );
	b.setByteAt( 1, psaddr[1] );
}

void L2BasePacket::writeBytes( const unsigned char *bytes, unsigned int len )
{
	if( !bytes || (len<1) ) return;
	unsigned int i;
	for( i=0; i<len; i++ ) this->writeUChar( bytes[i] );
}

void L2BasePacket::writeShort( short int s )
{
	char leftByte = (char)(s & 0x00FF);
	char rightByte = (char)( (s & 0xFF00) >> 8 );
	this->writeChar( leftByte );
	this->writeChar( rightByte );
}

void L2BasePacket::writeUShort( unsigned short int s )
{
	unsigned char leftByte  = (unsigned char)(  s & (unsigned short)0x00FF);
	unsigned char rightByte = (unsigned char)( (s & (unsigned short)0xFF00) >> 8 );
	this->writeUChar( leftByte );
	this->writeUChar( rightByte );
}

void L2BasePacket::writeInt( int i )
{
	char b1 = (char)(  i & (int)0x000000FF);
	char b2 = (char)( (i & (int)0x0000FF00) >> 8 );
	char b3 = (char)( (i & (int)0x00FF0000) >> 16 );
	char b4 = (char)( (i & (int)0xFF000000) >> 24 );
	this->writeChar( b1 );
	this->writeChar( b2 );
	this->writeChar( b3 );
	this->writeChar( b4 );
}

void L2BasePacket::writeUInt( unsigned int i )
{
	unsigned char b1 = (unsigned char)(  i & (unsigned int)0x000000FF);
	unsigned char b2 = (unsigned char)( (i & (unsigned int)0x0000FF00) >> 8 );
	unsigned char b3 = (unsigned char)( (i & (unsigned int)0x00FF0000) >> 16 );
	unsigned char b4 = (unsigned char)( (i & (unsigned int)0xFF000000) >> 24 );
	this->writeUChar( b1 );
	this->writeUChar( b2 );
	this->writeUChar( b3 );
	this->writeUChar( b4 );
}

void L2BasePacket::writeInt64( long long int i64 )
{
	char *i64b = (char *)(&i64);
	unsigned int i;
	for( i=0; i<sizeof(long long int); i++ ) this->writeChar( i64b[i] );
}

void L2BasePacket::writeUInt64( unsigned long long int i64 )
{
	unsigned char *i64b = (unsigned char *)(&i64);
	unsigned int i;
	for( i=0; i<sizeof(unsigned long long int); i++ ) this->writeUChar( i64b[i] );
}

void L2BasePacket::writeDouble( double d )
{
	char *pd = (char *)(&d);
	unsigned int i;
	for( i=0; i<sizeof(double); i++ ) this->writeChar( pd[i] );
}

void L2BasePacket::writeString( const char *str )
{
	if( !str ) return;
	char *p = (char *)str;
	while( (*p) )
	{
		this->writeChar( (*p) );
		p++;
	}
	this->writeChar( 0 );
}

void L2BasePacket::writeUnicodeString( const wchar_t *ustr )
{
	if( !ustr ) return;
	wchar_t *p = (wchar_t *)ustr;
	while( (*p) )
	{
		this->writeShort( (*p) );
		p++;
	}
	this->writeShort( 0x0000 );
}

bool L2BasePacket::canReadBytes( unsigned int nBytes )
{
	if( nBytes == 0 ) return true;
	unsigned int end_ptr = read_ptr + nBytes;
	if( end_ptr <= real_size ) return true;
	return false;
}

char L2BasePacket::readChar()
{
	if( !canReadBytes( 1 ) )
#ifdef L2P_THROW
		throw L2P_ReadException( "L2BasePacket::readChar()", 1, (int)read_ptr, (int)real_size );
#else
		return 0;
#endif
	char ret = b.getByteAt( read_ptr );
	read_ptr++;
	return ret;
}

unsigned char L2BasePacket::readUChar()
{
	if( !canReadBytes( 1 ) )
#ifdef L2P_THROW
		throw L2P_ReadException( "L2BasePacket::readUChar()", 1, (int)read_ptr, (int)real_size );
#else
		return 0;
#endif
	unsigned char ret = b.getByteAt( read_ptr );
	read_ptr++;
	return ret;
}

bool L2BasePacket::readBytes( unsigned char *bytes, unsigned int num )
{
	if( !bytes || (num<1) ) return false;
	unsigned int i;
	for( i=0; i<num; i++ ) bytes[i] = this->readUChar();
	return true;
}

short int L2BasePacket::readShort()
{
	if( canReadBytes( 2 ) )
	{
		unsigned char c1 = readChar();
		unsigned char c2 = readChar();
		unsigned short int ret = ((unsigned short int)c1) | ((unsigned short int)c2 << 8);
		return ret;
	}
#ifdef L2P_THROW
	throw L2P_ReadException( "L2BasePacket::readShort()", 2, (int)read_ptr, (int)real_size );
#else
	return 0;
#endif
}

unsigned short int L2BasePacket::readUShort()
{
	if( canReadBytes( 2 ) )
	{
		unsigned char c1 = readUChar();
		unsigned char c2 = readUChar();
		unsigned short int ret = ((unsigned short int)c1) | ((unsigned short int)c2 << 8);
		return ret;
	}
#ifdef L2P_THROW
	throw L2P_ReadException( "L2BasePacket::readUShort()", 2, (int)read_ptr, (int)real_size );
#else
	return 0;
#endif
}

int L2BasePacket::readInt()
{
	if( canReadBytes( 4 ) )
	{
		unsigned char c1 = readChar();
		unsigned char c2 = readChar();
		unsigned char c3 = readChar();
		unsigned char c4 = readChar();
		unsigned int ret = (unsigned int)c1;
		ret |= ( (unsigned int)c2 << 8 );
		ret |= ( (unsigned int)c3 << 16 );
		ret |= ( (unsigned int)c4 << 24 );
		return ret;
	}
#ifdef L2P_THROW
	throw L2P_ReadException( "L2BasePacket::readInt()", 4, (int)read_ptr, (int)real_size );
#else
	return 0;
#endif
}

unsigned int L2BasePacket::readUInt()
{
	if( canReadBytes( 4 ) )
	{
		unsigned char c1 = readChar();
		unsigned char c2 = readChar();
		unsigned char c3 = readChar();
		unsigned char c4 = readChar();
		unsigned int ret = (unsigned int)c1;
		ret |= ( (unsigned int)c2 << 8 );
		ret |= ( (unsigned int)c3 << 16 );
		ret |= ( (unsigned int)c4 << 24 );
		return ret;
	}
#ifdef L2P_THROW
	throw L2P_ReadException( "L2BasePacket::readUInt()", 4, (int)read_ptr, (int)real_size );
#else
	return 0;
#endif
}

long long int L2BasePacket::readInt64()
{
	long long int ret = 0;
	if( this->canReadBytes( 8 ) )
	{
		unsigned int i;
		unsigned char c = 0;
		unsigned char *retaddr = (unsigned char *)&ret;
		for( i=0; i<sizeof(long long int); i++ )
		{
			c = readChar();
			(*retaddr) = c;
			retaddr++;
		}
		return ret;
	}
#ifdef L2P_THROW
	throw L2P_ReadException( "L2BasePacket::readInt64()", 8, (int)read_ptr, (int)real_size );
#else
	return 0;
#endif
}

unsigned long long int L2BasePacket::readUInt64()
{
	unsigned long long int ret = 0;
	if( this->canReadBytes( 8 ) )
	{
		unsigned int i;
		unsigned char c = 0;
		unsigned char *retaddr = (unsigned char *)&ret;
		for( i=0; i<sizeof(unsigned long long int); i++ )
		{
			c = readUChar();
			(*retaddr) = c;
			retaddr++;
		}
		return ret;
	}
#ifdef L2P_THROW
	throw L2P_ReadException( "L2BasePacket::readUInt64()", 8, (int)read_ptr, (int)real_size );
#else
	return 0;
#endif
}

double L2BasePacket::readDouble()
{
	double ret = 0.0;
	if( canReadBytes( 8 ) )
	{
		unsigned int i = 0;
		unsigned char c = 0;
		unsigned char *retaddr = (unsigned char *)&ret;
		for( i=0; i<sizeof(double); i++ )
		{
			c = readChar();
			(*retaddr) = c;
			retaddr++;
		}
		return ret;
	}
#ifdef L2P_THROW
	throw L2P_ReadException( "L2BasePacket::readDouble()", 8, (int)read_ptr, (int)real_size );
#else
	return 0.0;
#endif
}

char *L2BasePacket::readString()
{
	unsigned int save_read_ptr = this->read_ptr;
	unsigned int str_len = 0;
	char c = 0;
	char *str = NULL;
	// count string length (including terminating 0x00 byte)
	while( this->canReadBytes(1) )
	{
		c = this->readChar();
		str_len++;
		if( c == 0 ) break;
	}
	// restore read_ptr
	this->read_ptr = save_read_ptr;
	// assert
	if( str_len == 0 ) return NULL;
	// allocate buffer
	str = (char *)malloc( str_len+1 ); // +1 for additional safety
	if( !str ) return NULL;
	str[str_len] = 0x00;
	// read bytes to buffer
	if( this->readBytes( (unsigned char *)str, str_len ) ) return str;
	// some error? O_o
	free( str );
	str = NULL;
	return NULL;
}

// returns allocated by malloc() buffer. must be freed later by caller!
wchar_t *L2BasePacket::readUnicodeString()
{
	unsigned int save_read_ptr = this->read_ptr;
	unsigned int str_len = 0;
	unsigned int buffer_len = 0;
	unsigned short int c = 0;
	wchar_t *wstr = NULL;
	// count string length (including terminating 0x0000 word)
	while( this->canReadBytes(2) )
	{
		c = this->readUShort();
		str_len++;
		buffer_len += 2;
		if( c == 0x0000 ) break;
	}
	// restore read_ptr
	this->read_ptr = save_read_ptr;
	// assert
	if( str_len == 0 ) return NULL;
	// allocate buffer
	wstr = (wchar_t *)malloc( buffer_len+2 ); // +2 for more safety
	if( !wstr ) return NULL;
	wstr[str_len] = 0x0000;
	// read bytes to buffer
	if( this->readBytes( (unsigned char *)wstr, buffer_len ) ) return wstr;
	// some error? O_o
	free( wstr );
	wstr = NULL;
	return NULL;
}

// returns read-only pointer to wchar_t inside packet.
const wchar_t *L2BasePacket::readUnicodeStringPtr()
{
	//unsigned int str_len = 0;
	unsigned short int ch = 0;
	// we return poiter to internal buffer
	wchar_t *wstr = (wchar_t *)( this->getBytesPtr() + this->read_ptr );
	// count string length (including terminating 0x0000 word), also moving current read_ptr
	while( this->canReadBytes(2) )
	{
		ch = this->readUShort();
		//str_len++;
		//buffer_len += 2;
		if( ch == 0x0000 ) break;
	}
	//wstr += str_len; // add counted string length to returned ptr
	return wstr;
}


void L2BasePacket::writeReset()
{
	write_ptr = 2; // pass 1st 2 bytes - packet size
	real_size = 2; // think that only 2 bytes written
	read_ptr  = 2; // cannot read more than written :)
	datasize  = 0;
	_preAllocateBuffer();
	// update packet size field
	b.setByteAt( 0, 2 );
	b.setByteAt( 1, 0 );
}

void L2BasePacket::readReset()
{
	read_ptr = 2; // pass 1st 2 bytes - packet size
}

bool L2BasePacket::_preAllocateBuffer()
{
	if( !b.setSize( buffer_size ) ) return false; // TODO: or throw exception?
	return true;
}

bool L2BasePacket::_growBuffer()
{
	buffer_size *= 2;
	return _preAllocateBuffer();
}

void L2BasePacket::_initNull()
{
	buffer_size = 256;
	datasize = 0;
	writeReset();
	readReset();
}

void L2BasePacket::_freeSelf()
{
	buffer_size = 256;
	writeReset();
	readReset();
}

void L2BasePacket::dumpToFile( FILE *f )
{
	if( !f ) return;
	fprintf( f, "L2BasePacket::dumpToFile() begin\n" );
	fprintf( f, "-- read_ptr    : %u\n", this->read_ptr );
	fprintf( f, "-- write_ptr   : %u\n", this->write_ptr );
	fprintf( f, "-- buffer_size : %u\n", this->buffer_size );
	fprintf( f, "-- real_size   : %u\n", this->real_size );
	unsigned int psize = (unsigned int)(this->b[0]) | 
		( ((unsigned int)this->b[1]) << 8 );
	fprintf( f, "-- Packet size : %u (%02X %02X)\n", psize, this->b[0], this->b[1] );
	if( this->real_size > 2 )
	{
		fprintf( f, "-- Packet type : %u (%02X)\n", this->b.getByteAt( 2 ), this->b[2] );
		fprintf( f, "-- ByteArray b :\n" );
		unsigned int i = 0;
		for( i=0; i<this->real_size; i++ )
			fprintf( f, "%02X ", this->b[i] );
		fprintf( f, "\n" );
	}
	fprintf( f, "L2BasePacket::dumpToFile() end\n\n" );
}

void L2BasePacket::saveToFileRaw( const char *filename )
{
	if( !filename ) return;
	FILE *f = fopen( filename, "wb" );
	if( !f ) return;
	fprintf( f, "L2BasePacket::dumpToFile() begin\r\n" );
	fprintf( f, "-- read_ptr    : %u\r\n", this->read_ptr );
	fprintf( f, "-- write_ptr   : %u\r\n", this->write_ptr );
	fprintf( f, "-- buffer_size : %u\r\n", this->buffer_size );
	fprintf( f, "-- real_size   : %u\r\n", this->real_size );
	unsigned int psize = this->b[0] + (this->b[1] << 8);
	fprintf( f, "-- Packet size : %u (%02X %02X)\r\n", psize, this->b[0], this->b[1] );
	if( this->real_size > 2 )
	{
		unsigned char *bytes = b.getBytesPtr();
		unsigned int count = b.getSize();
		fwrite( bytes, 1, count, f );
	}
	fprintf( f, "L2BasePacket::dumpToFile() end\n\n" );
	fclose( f );
}

void L2BasePacket::displaySelfNice( FILE *f )
{
	this->dumpToFile( f );
}

unsigned char L2BasePacket::getByteAt( unsigned int index )
{
	if( index >= real_size )
#ifdef L2P_THROW
		throw L2P_ReadException( "L2BasePacket::getByteAt()", 1, (int)index, (int)real_size );
#else
		return 0;
#endif
	char c = b.getByteAt( index );
	return c;
}
unsigned char L2BasePacket::setByteAt( unsigned int index, unsigned char byte )
{
	// checks are inside of ByteArray::setByteAt() (throwing exceptions if enabled)
	return b.setByteAt( index, byte );
}

inline const unsigned char *L2BasePacket::getBytesPtr() const
{
	return b.getBytesPtr();
}

/*L2BasePacket::operator const unsigned char *() const
{
	return this->getBytesPtr();
}*/

ByteArray *L2BasePacket::readB( unsigned int count )
{
	ByteArray *pByteArray = new ByteArray();
	if( !pByteArray ) return NULL;
	pByteArray->setSize( count );
	if( this->readBytes( pByteArray->getBytesPtr(), count ) ) return pByteArray;
	delete pByteArray;
#ifdef L2P_THROW
	throw L2P_ReadException( "L2BasePacket::readB()", (int)count, (int)read_ptr, (int)real_size );
#else
	return NULL;
#endif
}

// must be overrided in child classes
bool L2BasePacket::parse( L2_VERSION ver )
{
	UNREFERENCED_PARAMETER(ver);
	this->readReset();
	this->readUChar();
	return true;
}

// must be overrided in child classes
bool L2BasePacket::create( L2_VERSION ver )
{
	UNREFERENCED_PARAMETER(ver);
	this->writeReset();
	return true;
}
