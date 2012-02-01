#include "stdafx.h"
#include "L2PCodeObfuscator.h"
#ifdef L2P_THROW
#include "../xcpt/L2Packets_xcpt.h"
#endif

L2PCodeObfuscator::L2PCodeObfuscator()
{
	m_version = L2_VERSION_T1;
	m_DecodeTable1 = m_DecodeTable2 = m_DecodeTable3 = NULL;
	m_EncodeTable1 = m_EncodeTable2 = m_EncodeTable3 = NULL;
	m_s1 = m_s2 = m_s3 = 0;
	m_seed = 0;
	m_enabled = 0;
}

L2PCodeObfuscator::~L2PCodeObfuscator()
{
	clear();
}

void L2PCodeObfuscator::clear()
{
	if( m_DecodeTable1 ) free( m_DecodeTable1 );
	if( m_DecodeTable2 ) free( m_DecodeTable2 );
	if( m_DecodeTable3 ) free( m_DecodeTable3 );
	if( m_EncodeTable1 ) free( m_EncodeTable1 );
	if( m_EncodeTable2 ) free( m_EncodeTable2 );
	if( m_EncodeTable3 ) free( m_EncodeTable3 );
	m_version = L2_VERSION_T1;
	m_DecodeTable1 = m_DecodeTable2 = m_DecodeTable3 = NULL;
	m_EncodeTable1 = m_EncodeTable2 = m_EncodeTable3 = NULL;
	m_s1 = m_s2 = m_s3 = 0;
	m_seed = 0;
	m_enabled = 0;
}

void L2PCodeObfuscator::pseudo_srand( unsigned int seed )
{
	m_seed = seed;
}

unsigned int L2PCodeObfuscator::pseudo_rand()
{
	unsigned int a;
	a = ( m_seed * 0x343fd + 0x269EC3 ) & 0xFFFFFFFF;
	m_seed = a;
	unsigned int result = ( m_seed >> 0x10 ) & 0x7FFF;
	return result;
}

void L2PCodeObfuscator::init_tables( unsigned int seed )
{
	unsigned int i = 0;
	unsigned char tmp = 0;
	unsigned int pos;
	unsigned int cpos;

	m_s1 = 0xD0;
	m_s2 = 0x4E; // T1.5 Hellbound
	m_s3 = 0x64; // O_o

/*	if gSys.Protocol = 871 then _init_tables(GInt(_dBuff, $16, 4), $58); // CT2.2
	if gSys.Protocol = 851 then _init_tables(GInt(_dBuff, $16, 4), $55); // CT2
	if gSys.Protocol = 831 then _init_tables(GInt(_dBuff, $16, 4), $4E); // CT1.5+ */
	if( m_version == L2_VERSION_T2  ) m_s2 = 0x55; // T2   Gracia Part 1
	if( m_version == L2_VERSION_T22 ) m_s2 = 0x58; // T2.2 Gracia Part 2
	if( m_version == L2_VERSION_T23 ) m_s2 = 0x63; // T2.3 Gracia Final // TODO: length

	if( m_DecodeTable1 ) free( m_DecodeTable1 );
	if( m_DecodeTable2 ) free( m_DecodeTable2 );
	if( m_DecodeTable3 ) free( m_DecodeTable3 );

	m_DecodeTable1 = (unsigned char *)malloc( sizeof(unsigned char) * (m_s1 + 1) );
	m_DecodeTable2 = (unsigned char *)malloc( sizeof(unsigned char) * (m_s2 + 1) );
	m_DecodeTable3 = (unsigned char *)malloc( sizeof(unsigned char) * (m_s3 + 1) );

	for( i = 0; i <= m_s1; i++ ) m_DecodeTable1[i] = (unsigned char)i;
	for( i = 0; i <= m_s2; i++ ) m_DecodeTable2[i] = (unsigned char)i;
	for( i = 0; i <= m_s3; i++ ) m_DecodeTable3[i] = (unsigned char)i;

	this->pseudo_srand( seed );

	// mix 1-byte opcode table
	for( i = 1; i <= m_s1; i++ )
	{
		pos = this->pseudo_rand() % (i + 1);
		// swap bytes at indexes [pos] and [i] in DecodeTable1
		tmp = m_DecodeTable1[pos];
		m_DecodeTable1[pos] = m_DecodeTable1[i];
		m_DecodeTable1[i] = tmp;
	}

	// mix 2-byte opcode table
	for( i = 1; i <= m_s2; i++ )
	{
		pos = this->pseudo_rand() % (i + 1);
		// swap bytes at indexes [pos] and [i] in DecodeTable2
		tmp = m_DecodeTable2[pos];
		m_DecodeTable2[pos] = m_DecodeTable2[i];
		m_DecodeTable2[i] = tmp;
	}

	cpos = 0;
	while( m_DecodeTable1[cpos] != 0x12 ) cpos++;
	tmp = m_DecodeTable1[0x12];
	m_DecodeTable1[0x12] = 0x12;
	m_DecodeTable1[cpos] = tmp;

	cpos = 0;
	while( m_DecodeTable1[cpos] != 0xB1 ) cpos++;
	tmp = m_DecodeTable1[0xB1];
	m_DecodeTable1[0xB1] = 0xB1;
	m_DecodeTable1[cpos] = tmp;

	m_EncodeTable1 = (unsigned char *)malloc( sizeof(unsigned char) * (m_s1 + 1) );
	m_EncodeTable2 = (unsigned char *)malloc( sizeof(unsigned char) * (m_s2 + 1) );
	m_EncodeTable3 = (unsigned char *)malloc( sizeof(unsigned char) * (m_s3 + 1) );

	for( i = 0; i <= m_s1; i++ ) m_EncodeTable1[ m_DecodeTable1[i] ] = (unsigned char)i;
	for( i = 0; i <= m_s2; i++ ) m_EncodeTable2[ m_DecodeTable2[i] ] = (unsigned char)i;
	for( i = 0; i <= m_s3; i++ ) m_EncodeTable3[ m_DecodeTable3[i] ] = (unsigned char)i;

	m_enabled = 1;
}

int L2PCodeObfuscator::decodeIDs( unsigned char *packet_data_raw )
{
	if( !packet_data_raw ) return 0;
	unsigned char *data = packet_data_raw;
	int ofs = 2; // offset of packet ID in raw data
	int ret_val = 0;
	if( m_DecodeTable1 )
	{
		if( data[ofs] > m_s1 )
		{
#ifdef L2P_THROW
			throw L2P_DeObfuscateException( m_s1, data[ofs], -1 );
#else
			return -1;
#endif
		}
		else data[ofs] = m_DecodeTable1[ data[ofs] ];
		ret_val = 1;
		if( data[ofs] == 0xD0 ) // double-byte packet
		{
			ret_val = 2;
			if( data[ofs + 1] > m_s2 )
			{
#ifdef L2P_THROW
				throw L2P_DeObfuscateException( m_s2, data[ofs + 1], -2 );
#else
				return -2;
#endif
			}
			else data[ofs + 1] = m_DecodeTable2[ data[ofs + 1] ];
		}
	}
	return ret_val;
}

int L2PCodeObfuscator::encodeIDs( unsigned char *packet_data_raw )
{
	if( !packet_data_raw ) return 0;
	unsigned char *data = packet_data_raw;
	int ofs = 2; // offset of packet ID in raw data (ofs+1 is position of extended opcode)
	int ret_val = 0;
	if( m_EncodeTable1 )
	{
		unsigned char prev_opcode = data[ofs]; // opcode BEFORE obfuscation
		if( data[ofs] > m_s1 ) // m_s1 is a valid index in table (allocated as m_s1+1)
		{
#ifdef L2P_THROW
			throw L2P_ObfuscateException( m_s1, data[ofs], -1 );
#else
			return -1;
#endif
		}
		else data[ofs] = m_EncodeTable1[ data[ofs] ];
		ret_val = 1;
		if( prev_opcode == 0xD0 ) // this is a double-byte packet - obfuscate second (ext) opcode
		{
			ret_val = 2;
			if( data[ofs + 1] > m_s2 )
			{
#ifdef L2P_THROW
				throw L2P_ObfuscateException( m_s2, data[ofs + 1], -2 );
#else
				return -2;
#endif
			}
			else data[ofs + 1] = m_EncodeTable2[ data[ofs + 1] ];
		}
	}
	return ret_val;
}

bool L2PCodeObfuscator::decodeOpcode( unsigned char &singleOpcode, unsigned short &doubleOpcode )
{
	unsigned short tripleOpcode = 0;
	return decodeOpcode( singleOpcode, doubleOpcode, tripleOpcode );
}

bool L2PCodeObfuscator::encodeOpcode( unsigned char &singleOpcode, unsigned short &doubleOpcode )
{
	unsigned short tripleOpcode = 0;
	return encodeOpcode( singleOpcode, doubleOpcode, tripleOpcode );
}


bool L2PCodeObfuscator::decodeOpcode( unsigned char &singleOpcode, unsigned short &doubleOpcode, unsigned short &tripleOpcode )
{
	unsigned char  prev_singleOpcode = singleOpcode;
	unsigned short prev_doubleOpcode = doubleOpcode;
	if( singleOpcode > m_s1 )
	{
#ifdef L2P_THROW
		throw L2P_DeObfuscateException( m_s1, singleOpcode, -1 );
#else
		return false;
#endif
	}
	singleOpcode = m_DecodeTable1[ singleOpcode ];
	if( singleOpcode == 0xD0 ) // D0:xx
	{
		if( doubleOpcode > m_s2 )
		{
			singleOpcode = prev_singleOpcode; // restore prev. opcodes
#ifdef L2P_THROW
			throw L2P_DeObfuscateException( m_s2, doubleOpcode, -2 );
#else
			return false;
#endif
		}
		doubleOpcode = (unsigned short)m_DecodeTable2[ doubleOpcode ];
		// TODO: Gracia Final triple opcodes
		if( m_version == L2_VERSION_T23 )
		{
			if( doubleOpcode == 0x51 ) // D0:51:xx
			{
				if( tripleOpcode > m_s3 )
				{
					singleOpcode = prev_singleOpcode; // restore prev. opcodes
					doubleOpcode = prev_doubleOpcode; // restore prev. opcodes
#ifdef L2P_THROW
					throw L2P_DeObfuscateException( m_s3, tripleOpcode, -3 );
#else
					return false;
#endif
				}
				tripleOpcode = (unsigned short)m_DecodeTable3[ tripleOpcode ];
			}
		}
	}
	return true;
}

bool L2PCodeObfuscator::encodeOpcode( unsigned char &singleOpcode, unsigned short &doubleOpcode, unsigned short &tripleOpcode )
{
	unsigned char  prev_singleOpcode = singleOpcode;
	unsigned short prev_doubleOpcode = doubleOpcode;
	if( singleOpcode > m_s1 )
	{
#ifdef L2P_THROW
		throw L2P_ObfuscateException( m_s1, singleOpcode, -1 );
#else
		return false;
#endif
	}
	singleOpcode = m_EncodeTable1[ singleOpcode ];
	if( prev_singleOpcode == 0xD0 ) // D0:xx
	{
		if( doubleOpcode > m_s2 )
		{
			singleOpcode = prev_singleOpcode; // restore prev. opcode
#ifdef L2P_THROW
			throw L2P_ObfuscateException( m_s2, doubleOpcode, -2 );
#else
			return false;
#endif
		}
		doubleOpcode = (unsigned short)m_EncodeTable2[ doubleOpcode ];
		// TODO: Gracia Final triple opcodes
		if( m_version == L2_VERSION_T23 )
		{
			if( prev_doubleOpcode == 0x51 ) // D0:51:xx
			{
				if( tripleOpcode > m_s3 )
				{
					singleOpcode = prev_singleOpcode; // restore prev. opcode
					doubleOpcode = prev_doubleOpcode; // restore prev. opcode
#ifndef L2P_THROW
					throw L2P_ObfuscateException( m_s3, tripleOpcode, -3 );
#else
					return false;
#endif
				}
				tripleOpcode = (unsigned short)m_EncodeTable3[ tripleOpcode ];
			}
		}
	}
	return true;
}
