#include "stdafx.h"
#include "L2Packets_xcpt.h"

L2P_Exception::~L2P_Exception()
{
	if( m_what )
	{
		free( m_what );
		m_what = NULL;
	}
}

const char *L2P_Exception::what() const { return m_what; }

const L2P_Exception& L2P_Exception::operator=( const L2P_Exception& other )
{
	if( this == &other ) return (*this);
	if( m_what ) free( m_what );
	m_what = NULL;
	if( other.m_what )
	{
		size_t ssize = strlen( other.m_what ) + 1;
		m_what = (char *)malloc( ssize );
		if( m_what ) strncpy( m_what, other.m_what, ssize-1 );
	}
	return (*this);
}

L2P_MemoryError::L2P_MemoryError( const char *comment, size_t bytes )
{
	m_what = (char *)malloc( 256 );
	if( m_what )
	{
		sprintf( m_what, "%s: Tried to allocate: %u bytes", comment, (unsigned int)bytes );
	}
}

L2P_ReadException::L2P_ReadException( const char *comment, int nBytesTriedToRead, int nPos, int nSize )
{
	m_what = (char *)malloc( 256 );
	if( m_what )
	{
		sprintf( m_what, "%s: Tried to read: %d bytes; pos: %d/%d", comment, nBytesTriedToRead, nPos, nSize );
	}
}

L2P_WriteException::L2P_WriteException( const char *comment, int nBytesTriedToWrite, int nPos, int nSize )
{
	m_what = (char *)malloc( 256 );
	if( m_what )
	{
		sprintf( m_what, "%s: Tried to write: %d bytes; pos: %d/%d", comment, nBytesTriedToWrite, nPos, nSize );
	}
}

L2P_ObfuscateException::L2P_ObfuscateException( int size, unsigned int wrong_opcode, int error_code )
{
	m_what = (char *)malloc( 256 );
	if( m_what )
	{
		sprintf( m_what, "Obfuscation failed (%d): opcode 0x%02X (table size %d)", error_code, wrong_opcode, size );
	}
}

L2P_DeObfuscateException::L2P_DeObfuscateException( int size, unsigned int wrong_opcode, int error_code )
{
	m_what = (char *)malloc( 256 );
	if( m_what )
	{
		sprintf( m_what, "Deobfuscation failed (%d): opcode 0x%02X (table size %d)", error_code, wrong_opcode, size );
	}
}
