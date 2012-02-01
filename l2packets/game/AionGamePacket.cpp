#include "stdafx.h"
#include "AionGamePacket.h"

unsigned char *AionGamePacket::STATIC_XOR_KEY = NULL;

void AionGamePacket::_checkInitStaticXorKey()
{
	if( !AionGamePacket::STATIC_XOR_KEY )
		AionGamePacket::STATIC_XOR_KEY = (unsigned char *)"nKO/WctQ0AVLbpzfBkS6NevDYT8ourG5CRlmdjyJ72aswx4EPq1UgZhFMXH?3iI9";
}

AionGamePacket::AionGamePacket()
{
	this->_initNull();
	this->_checkInitStaticXorKey();
}

AionGamePacket::AionGamePacket( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->_checkInitStaticXorKey();
	this->setBytes( bytes, length );
}

bool AionGamePacket::decodeXOR( unsigned char *key )
{
	if( !key ) return false;
	bool r = AionGamePacket::decodeXOR_buffer( b.getBytesPtr(), getPacketSize(), key );
	return r;
}

bool AionGamePacket::encodeXOR( unsigned char *key )
{
	if( !key ) return false;
	bool r = AionGamePacket::encodeXOR_buffer( b.getBytesPtr(), getPacketSize(), key );
	return r;
}

bool AionGamePacket::decodeXOR_buffer( unsigned char *decodeBytes, unsigned int bytesLen, unsigned char *key )
{
	if( !key ) return false;
	const int data_offset = 2; // do not touch 1st 2 bytes - with packet length; pass them
	unsigned int i = 0;
	unsigned char cur = 0; // current byte
	unsigned char prev = decodeBytes[data_offset]; // prev. encrypted byte
	decodeBytes[data_offset] ^= key[0]; // decrypt 1st byte
	// decrypt loop
	for( i=(data_offset + 1); i<bytesLen; i++ )
	{
		cur = decodeBytes[i];
		decodeBytes[i] ^= AionGamePacket::STATIC_XOR_KEY[(i-data_offset) & 63] ^ key[(i-data_offset) & 7] ^ prev;
		prev = cur;
	}
	// old key value as 64-bit unsigned int
	unsigned long long oldKey = (unsigned long long)key[0] | ((unsigned long long)key[1] << 8) |
		((unsigned long long)key[2] << 16) | ((unsigned long long)key[3] << 24) |
		((unsigned long long)key[4] << 32) | ((unsigned long long)key[5] << 40) |
		((unsigned long long)key[6] << 48) | ((unsigned long long)key[7] << 56);
	// packet size is added to old key value
	oldKey += (bytesLen - 2); // data size = whole packet length - 2
	// update key
	key[0] = (unsigned char)(oldKey & 0xFF);
	key[1] = (unsigned char)((oldKey >> 8) & 0xFF);
	key[2] = (unsigned char)((oldKey >> 16) & 0xFF);
	key[3] = (unsigned char)((oldKey >> 24) & 0xFF);
	key[4] = (unsigned char)((oldKey >> 32) & 0xFF);
	key[5] = (unsigned char)((oldKey >> 40) & 0xFF);
	key[6] = (unsigned char)((oldKey >> 48) & 0xFF);
	key[7] = (unsigned char)((oldKey >> 56) & 0xFF);
	return true;
}

bool AionGamePacket::encodeXOR_buffer( unsigned char *encodeBytes, unsigned int bytesLen, unsigned char *key )
{
	if( !key ) return false;
	unsigned int i = 0;
	unsigned char prev = 0; // prev. encoded byte
	const int data_offset = 2; // do not touch 1st 2 bytes - with packet length; pass them
	// encrypt 1st byte
	encodeBytes[data_offset] ^= key[0];
	prev = encodeBytes[data_offset];
	// encrypt loop
	for( i=(data_offset+1); i<bytesLen; i++ )
	{
		encodeBytes[i] ^= AionGamePacket::STATIC_XOR_KEY[(i-data_offset) & 63] ^ key[(i-data_offset) & 7] ^ prev;
		prev = encodeBytes[i];
	}
	// old key value as 64-bit unsigned int
	unsigned long long oldKey = (unsigned long long)key[0] | ((unsigned long long)key[1] << 8) |
		((unsigned long long)key[2] << 16) | ((unsigned long long)key[3] << 24) |
		((unsigned long long)key[4] << 32) | ((unsigned long long)key[5] << 40) |
		((unsigned long long)key[6] << 48) | ((unsigned long long)key[7] << 56);
	// packet size is added to old key value
	oldKey += (bytesLen - 2); // data size = whole packet length - 2
	// update key
	key[0] = (unsigned char)(oldKey & 0xFF);
	key[1] = (unsigned char)((oldKey >> 8) & 0xFF);
	key[2] = (unsigned char)((oldKey >> 16) & 0xFF);
	key[3] = (unsigned char)((oldKey >> 24) & 0xFF);
	key[4] = (unsigned char)((oldKey >> 32) & 0xFF);
	key[5] = (unsigned char)((oldKey >> 40) & 0xFF);
	key[6] = (unsigned char)((oldKey >> 48) & 0xFF);
	key[7] = (unsigned char)((oldKey >> 56) & 0xFF);
	return true;
}

unsigned char AionGamePacket::obfuscateServerOpcode( unsigned char opcode )
{
	return ((opcode + 0xAE) ^ 0xEE);
}

unsigned char AionGamePacket::deobfuscateServerOpcode( unsigned char opcode )
{
	return ((opcode ^ 0xEE) - 0xAE);
}

bool AionGamePacket::writeServerOpcode( unsigned char opcode )
{
	writeReset();
	unsigned char scrambled_opcode = obfuscateServerOpcode( opcode );
	writeUChar( scrambled_opcode );
	writeUChar( AionGamePacket::STATIC_SERVER_OPCODE );
	writeUChar( ~scrambled_opcode );
	return true;
}

// TODO: verify this
bool AionGamePacket::writeClientOpcode( unsigned char opcode )
{
	writeReset();
	unsigned char scrambled_opcode = obfuscateServerOpcode( opcode ); // REALLYY??????????
	writeUChar( scrambled_opcode ); // ARE YOU SURE??
	writeUChar( AionGamePacket::STATIC_CLIENT_OPCODE );
	writeUChar( ~scrambled_opcode );
	return true;
}

bool AionGamePacket::validateClientOpcode() const
{
	if( getDataSize() >= 3 )
	{
		unsigned char o1 = b[2];
		unsigned char o2 = b[3];
		unsigned char o3 = b[4];
		if( (o1 == (unsigned char)(~o3)) && (o2 == AionGamePacket::STATIC_CLIENT_OPCODE) ) return true;
	}
	return false;
}

bool AionGamePacket::validateServerOpcode() const
{
	if( getDataSize() >= 3 )
	{
		unsigned char o1 = b[2];
		unsigned char o2 = b[3];
		unsigned char o3 = b[4];
		if( (o1 == (unsigned char)(~o3)) && (o2 == AionGamePacket::STATIC_SERVER_OPCODE) ) return true;
	}
	return false;
}

void AionGamePacket::initStaticXORKey( unsigned char *key )
{
	key[4] = 0xA1;
	key[5] = 0x6C;
	key[6] = 0x54;
	key[7] = 0x87;
}

unsigned char AionGamePacket::readOpcode()
{
	unsigned char opcode = 0;
	readReset();
	if( canReadBytes( 3 ) )
	{
		opcode = readUChar();
		readUChar(); // static opcode
		readUChar(); // ~opcode
	}
	return opcode;
}
