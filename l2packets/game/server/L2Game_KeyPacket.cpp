#include "stdafx.h"
#include "L2Game_KeyPacket.h"

/**
 * ==========================================
Retail NCSoft
19 00                     // p.len 25
2E                        // pcode
01                        // always const
42 55 77 8F  C3 05 69 87  // xorkey first part
01 00 00 00               // always const 
01                        // possible Game Server ID - depends on game server ID
00 00 00 00               // always NULLs
XX XX XX XX               // always different, non-nulls: opcode obfuscator
 */

L2Game_KeyPacket::L2Game_KeyPacket()
{
	this->_initNull();
}

L2Game_KeyPacket::L2Game_KeyPacket( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

// key must point to buffer large enough to hold 8 bytes
bool L2Game_KeyPacket::read_key( unsigned char *key )
{
	if( !key ) return false;
	this->readReset();
	if( this->canReadBytes(10) )
	{
		this->readUChar(); // packet type (0x0e in hellbound, 0x00 - interlude)
		this->readUChar(); // 0x01 ?
		return this->readBytes( key, 8 );
	}
	else return false;
}

unsigned char L2Game_KeyPacket::read_GameServerID()
{
	if( !this->canReadBytes(5) ) return 0x00;
	this->readUInt();         // 01 00 00 00 // always const 
	return this->readUChar(); // 01          // possible Game Server ID
}

unsigned int L2Game_KeyPacket::read_OpcodeObfuscator()
{
	if( !this->canReadBytes(4) ) return 0x00000000;
	this->readUInt();        // 00 00 00 00 // always 0x00000000, read 4 NULLs
	return this->readUInt(); // XX XX XX XX // always different, opcode obfuscator
}

// keyPacket array points to key bytes received from KeyPacket (8 bytes)
// keyResult will hold resultig key (16 bytes)
void L2Game_KeyPacket::createInitialHellboundKey(
		const unsigned char *keyPacket,
		unsigned char *keyResult )
{
	if( !keyPacket || !keyResult ) return;
	// first 8 bytes of key are from KeyPacket
	keyResult[0] = keyPacket[0];
	keyResult[1] = keyPacket[1];
	keyResult[2] = keyPacket[2];
	keyResult[3] = keyPacket[3];
	keyResult[4] = keyPacket[4];
	keyResult[5] = keyPacket[5];
	keyResult[6] = keyPacket[6];
	keyResult[7] = keyPacket[7];
	// last 8 bytes are constant in T1/T1.5/T2/T2.2/T2.3...
	keyResult[8]  = (unsigned char)0xc8;
	keyResult[9]  = (unsigned char)0x27;
	keyResult[10] = (unsigned char)0x93;
	keyResult[11] = (unsigned char)0x01;
	keyResult[12] = (unsigned char)0xa1;
	keyResult[13] = (unsigned char)0x6c;
	keyResult[14] = (unsigned char)0x31;
	keyResult[15] = (unsigned char)0x97;
}

/*** Format:
19 00                      // plen = 25
2E                         // pcode
01                         // 00 - wrong proto, 01 - proto OK
B6 4D 20 15  CE 0E BC 7A   // 1st 8 bytes of crypto key
01 00 00 00                // 0x01
09                         // game serverID
00 00 00 00                // wtf?
E3 D1 10 2F                // obfuscation key  ***/
bool L2Game_KeyPacket::parse( L2_VERSION ver /*= L2_VERSION_T1*/ )
{
	UNREFERENCED_PARAMETER( ver );
	readReset();
	if( !canReadBytes( 19 ) ) return false; // packet size is at least 19 bytes O_o
	getPacketType();
	this->p_protocolIsOK = readUChar();  // 0x01 - proto OK, 0x00 - proto not supported
	readBytes( this->p_initialKey, 8 );  // first 8 bytes of XOR key
	readD();                             // 0x00000001
	this->p_serverId = readUChar();      // server ID
	readD();                             // 0x00000000;
	this->p_obfuscatorSeed = readUInt(); // obfuscator seed
	// add last 8 bytes of XOR key - they are constant
	createInitialHellboundKey( p_initialKey, p_initialKey );
	//
	return true;
}

bool L2Game_KeyPacket::create( L2_VERSION ver )
{
	UNREFERENCED_PARAMETER( ver );
	writeReset();
	setPacketType( 0x2E );
	writeUChar( p_protocolIsOK );
	writeBytes( p_initialKey, 8 );
	writeD( 1 );
	writeUChar( p_serverId );
	writeD( 0 );
	writeUInt( p_obfuscatorSeed );
	return true;
}
