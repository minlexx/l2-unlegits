#include "stdafx.h"
#include "ByteArray.h"
#include "../xcpt/L2Packets_xcpt.h"

#ifdef _DEBUG
int ByteArray::_debugPrintf( const char *_Format, ... )
{
	if( !this->bDebugMode ) return 0;
	if( !_Format ) return 0;
	if( !this->fLogFile ) return 0;
	va_list args;
	int     ret = 0;
	va_start( args, _Format );
	ret = vfprintf( this->fLogFile, _Format, args );
	return ret;
}

void ByteArray::setDebugMode( bool bDebug, FILE *flog )
{
	if( bDebug && flog )
	{
		this->bDebugMode = bDebug;
		this->fLogFile = flog;
		return;
	}
	if( !bDebug )
	{
		this->bDebugMode = false;
		this->fLogFile = NULL;
	}
}

void ByteArray::dumpArrayInfo( FILE * fOut )
{
	if( !fOut ) return;
	fprintf( fOut, "==== Dump of Array info ====\n" );
	fprintf( fOut, "     Size: %u\n", this->byteCount );
	if( this->byteCount > 0 )
	{
		unsigned i = 0;
		while( i < this->byteCount )
		{
			fprintf( fOut, "%c:%02X ", this->bytes[i], this->bytes[i] );
			i++;
		}
	}
	fprintf( fOut, "\n==== end dump ====\n" );
}
#endif

void ByteArray::_initNull()
{
	// data
	this->byteCount = 0;
	this->bytes = NULL;
	// debug mode vars
#ifdef _DEBUG
	this->bDebugMode = false;
	this->fLogFile = NULL;
#endif
}

void ByteArray::_freeSelf()
{
	if( (this->byteCount > 0) && (this->bytes) )
	{
		free( this->bytes );
		this->bytes = NULL;
		this->byteCount = 0;
#ifdef _DEBUG
		_debugPrintf( "ByteArray::_freeSelf() freeed\n" );
#endif
	}
#ifdef _DEBUG
	else _debugPrintf( "ByteArray::_freeSelf() nothing to free\n" );
#endif
}

ByteArray::ByteArray()
{
	this->_initNull();
}

ByteArray::ByteArray( unsigned int length )
{
	this->_initNull();
	if( length > 0 ) this->setSize( length );
}

ByteArray::ByteArray( const char *string )
{
	this->_initNull();
	if( string ) this->setBytesFromString( string );
}

ByteArray::ByteArray( const unsigned char *newBytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( newBytes, length );
}

ByteArray::ByteArray( const ByteArray& ba )
{
	this->_initNull();
	this->setBytes( ba.bytes, ba.byteCount );
}

ByteArray::~ByteArray()
{
	this->_freeSelf();
#ifdef _DEBUG
	_debugPrintf( "ByteArray::~ByteArray() worked\n" );
#endif
}

bool ByteArray::setSize( unsigned int newSize )
{
	if( newSize == 0 )
	{
		this->_freeSelf();
		return true;
	}
	if( newSize == this->byteCount ) return true; // already have that size
	if( newSize > 0 )
	{
		unsigned char *newPtr = (unsigned char *)realloc( this->bytes, newSize );
		if( newPtr )
		{
			this->bytes = newPtr;
			this->byteCount = newSize;
			return true;
		}
#ifdef L2P_THROW
		else throw L2P_MemoryError( "ByteArray::setSize()", newSize );
#else
		return false;
#endif
	}
	return false;
}


unsigned char ByteArray::getByteAt( unsigned int index ) const
{
	if( (this->byteCount > 0) && (this->bytes) )
	{
		if( index < (this->byteCount) ) return this->bytes[index];
		return 0;
	}
#ifdef L2P_THROW
	throw L2P_ReadException( "ByteArray::getByteAt()", 1, (int)index, (int)byteCount );
#else
	return 0;
#endif
}

unsigned char ByteArray::setByteAt( unsigned int index, unsigned char byteSet )
{
	if( (this->byteCount > 0) && (this->bytes) )
	{
		unsigned char retVal = 0;
		if( index < (this->byteCount) )
		{
			retVal = this->bytes[index];
			this->bytes[index] = byteSet;
		}
#ifdef L2P_THROW
		else throw L2P_WriteException( "ByteArray::setByteAt()", 1, (int)index, (int)byteCount );
#endif
		return retVal;
	}
	else
	{
#ifdef L2P_THROW
		throw L2P_WriteException( "ByteArray::setByteAt()", 1, (int)index, (int)byteCount );
#else
		return 0;
#endif
	}
}

void ByteArray::memset( unsigned char c )
{
	if( (this->byteCount > 0) && (this->bytes) )
	{
		/*unsigned int i = 0;
		while( i < this->byteCount )
			this->bytes[i++] = c;*/
		::memset( this->bytes, c, this->byteCount );
	}
}

bool ByteArray::setBytes( const unsigned char *newBytes, unsigned int length )
{
	if( length < 1 ) return false;
	//unsigned int i = 0;
	//
	//printf( "ByteArray::setBytes( 0x%p, %u )\n", newBytes, length );
	//printf( "ByteArray::setBytes(): %u < %u?\n", this->byteCount, length );
	//
	// check if it is enough space in 'bytes' now
	if( this->byteCount < length ) // not enough
	{
		// we should try to grow to required size
		if( !this->setSize( length ) ) return false; // growing failed :(
	}
	// if we're here, space in 'bytes' buffer is enough to hold 'length' bytes
	this->memset( 0x00 );
	//while( i < length ) this->bytes[i] = newBytes[i++];
	memcpy( this->bytes, newBytes, length );
	//printf( "ByteArray::setBytes(): %u bytes set OK (buffer len %u)\n", length, this->byteCount );
	return true;
}

bool ByteArray::setBytesFromString( const char *string )
{
	if( !string ) return false;
	unsigned int str_len = strlen( string );
	if( str_len < 1 ) return false;
	return this->setBytes( (const unsigned char *)string, str_len );
}

bool ByteArray::setBytesFromPtrNoMemcpy( unsigned char *bytes, unsigned int length )
{
	if( !bytes || (length<1) ) return false;
	this->byteCount = length;
	this->bytes = bytes;
	return true;
}

unsigned char ByteArray::operator[]( int index ) const
{
	if( (this->byteCount > 0) && (this->bytes) )
	{
		if( (index>=0) && (index<((int)(this->byteCount))) ) return this->bytes[index];
		return 0;
	}
	return 0;
}
