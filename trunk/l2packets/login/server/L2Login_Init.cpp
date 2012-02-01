#include "stdafx.h"
#include "L2Login_Init.h"

L2Login_Init::L2Login_Init()
{
	_initNull();
	_initPublicMembers();
}

L2Login_Init::L2Login_Init( const unsigned char *bytes, unsigned int length )
{
	_initNull();
	_initPublicMembers();
	this->setBytes( bytes, length );
}

bool L2Login_Init::decodeXOR()
{
	unsigned int   blen   = getPacketSize();
	unsigned char *packet = b.getBytesPtr();
	if( blen < 1 || !packet ) return false; // TODO: throw?
	if( blen < 186 ) return false; // TODO: throw?
	// get xor key
	// XOR key position to the left from End-of-packet
	unsigned int xor_offset = 8;
	xor_key = 0;
	unsigned char b = 0;
	b = packet[ blen - xor_offset ];
	xor_key |= (unsigned int)b;
	b = packet[ blen - xor_offset + 1 ];
	xor_key |= ( (unsigned int)b << 8 );
	b = packet[ blen - xor_offset + 2 ];
	xor_key |= ( (unsigned int)b << 16 );
	b = packet[ blen - xor_offset + 3 ];
	xor_key |= ( (unsigned int)b << 24 );
	
	// enc xor?
	unsigned int offset = 2;
	unsigned int edx = 0;
	unsigned int ecx = xor_key;
	
	offset = blen - xor_offset - 4;
	while( offset > 2 ) // offset > 3 ?
	{
		edx  = (packet[offset+0] & 0xFF);
		edx |= (packet[offset+1] & 0xFF) << 8;
		edx |= (packet[offset+2] & 0xFF) << 16;
		edx |= (packet[offset+3] & 0xFF) << 24;

		edx ^= ecx;
		ecx -= edx;

		packet[offset+0] = (unsigned char)((edx)       & 0xFF);
		packet[offset+1] = (unsigned char)((edx >>  8) & 0xFF);
		packet[offset+2] = (unsigned char)((edx >> 16) & 0xFF);
		packet[offset+3] = (unsigned char)((edx >> 24) & 0xFF);
		offset -= 4;
	}
	return true;
}

// static function used to decrypt RSA public key modulus from Init packet
// data must be 128-byte long array
bool L2Login_Init::unscramble_RSA_PubKeyMod( unsigned char *data )
{
	int i;
	// step 4 xor last 0x40 bytes with first 0x40 bytes
	for( i=0; i<0x40; i++ )
		data[0x40 + i] = (unsigned char)(data[0x40 + i] ^ data[i]);
	// step 3 xor bytes 0x0d-0x10 with bytes 0x34-0x38
	for( i=0; i<4; i++ )
		data[0x0d + i] = (unsigned char)(data[0x0d + i] ^ data[0x34 + i]);
	// step 2 xor first 0x40 bytes with last 0x40 bytes
	for( i=0; i<0x40; i++ )
		data[i] = (unsigned char)(data[i] ^ data[0x40 + i]);
	// step 1
	for( i=0; i<4; i++ )
	{
		unsigned char temp = data[0x00 + i];
		data[0x00 + i] = data[0x4d + i];
		data[0x4d + i] = temp;
	}
	return true;
}

bool L2Login_Init::scramble_RSA_PubKeyMod( unsigned char *data )
{
	int i;
	for( i=0; i<4; i++ )
	{
		unsigned char temp = data[0x00 + i];
		data[0x00 + i] = data[0x4d + i];
		data[0x4d + i] = temp;
	}
	// step 2 xor first 0x40 bytes with last 0x40 bytes
	for( i=0; i<0x40; i++ )
		data[i] = (unsigned char)(data[i] ^ data[0x40 + i]);
	// step 3 xor bytes 0x0d-0x10 with bytes 0x34-0x38
	for( i=0; i<4; i++ )
		data[0x0d + i] = (unsigned char)(data[0x0d + i] ^ data[0x34 + i]);
	// step 4 xor last 0x40 bytes with first 0x40 bytes
	for( i=0; i<0x40; i++ )
		data[0x40 + i] = (unsigned char)(data[0x40 + i] ^ data[i]);
	return true;
}

bool L2Login_Init::read_SessionID( unsigned char *sidBytes )
{
	if( !sidBytes ) return false;
	if( !canReadBytes( 4 ) ) return false;
	this->readReset();
	this->readChar();  // pass packet type
	sidBytes[0] = this->readUChar();
	sidBytes[1] = this->readUChar();
	sidBytes[2] = this->readUChar();
	sidBytes[3] = this->readUChar();
	return true;
}

unsigned int L2Login_Init::read_ProtoVer()
{
	unsigned int ret = 0;
	ret = this->readUInt();
	return ret;
}

// RSApublicKeyMod must point to buffer 128 bytes len
bool L2Login_Init::read_RSA_pubKeyMod( unsigned char *RSApublicKeyMod )
{
	if( !RSApublicKeyMod ) return false;
	if( !canReadBytes(128) ) return false;
	int i = 0;
	while( i < 128 ) RSApublicKeyMod[i++] = readUChar();
	return true;
}

// ggShit - 16 bytes buffer; can be NULL
bool L2Login_Init::read_GGShit( unsigned char *ggShit )
{
	if( !canReadBytes( 16 ) ) return false;
	int i = 0;
	char c = 0;
	while( i < 16 )
	{
		c = this->readUChar();
		if( ggShit ) ggShit[i] = c;
		i++;
	}
	return true;
}

// newBFKey - 16 bytes buffer; can NOT be NULL
bool L2Login_Init::read_DynamicBFKey( unsigned char *newBFKey )
{
	if( !newBFKey ) return false;
	if( !canReadBytes(16) ) return false;
	int i = 0;
	while( i < 16 ) newBFKey[i++] = this->readUChar();
	return true;
}

void L2Login_Init::displaySelfNice( FILE *f )
{
	fprintf( f, "L2Login_Init: displaying self\n" );
	if( this->getPacketSize() != 186 )
	{
		fprintf( f, "L2Login_Init: this is not standard Interlude Init packet! (wrong size)\n" );
		this->dumpToFile( stdout );
		return;
	}
	char ptype = this->getPacketType();
	if( ptype != 0 )
	{
		fprintf( f, "L2Login_Init: this is not standard Interlude Init packet! (wrong packet type)\n" );
		this->dumpToFile( stdout );
	}
	
	// vars
	unsigned char sidBytes[4]    = {0,0,0,0};
	unsigned int  protoRev       = 0;
	unsigned char RSA_pubKeyMod[128];
	unsigned char GG_Shit[16]    = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
	unsigned char dyn_BF_key[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
	unsigned char nullTerm       = 0;
	
	// memsets
	memset( RSA_pubKeyMod, 0, sizeof(RSA_pubKeyMod) );
	
	// read ...
	this->read_SessionID( sidBytes );
	protoRev = this->read_ProtoVer();
	this->read_RSA_pubKeyMod( RSA_pubKeyMod );
	this->read_GGShit( GG_Shit );
	this->read_DynamicBFKey( dyn_BF_key );
	nullTerm = this->readUChar();
	
	// print
	int i = 0;
	fprintf( f, "Packet type     : Init (0)\n" );
	fprintf( f, "Session ID      : %02X %02X %02X %02X\n",
		sidBytes[0], sidBytes[1], sidBytes[2], sidBytes[3] );
	fprintf( f, "Protocol Rev    : %04X\n", protoRev );
	fprintf( f, "RSA pub key mod :\n" );
	for( i=0; i<128; i++ ) fprintf( f, "%02X ", (unsigned int)RSA_pubKeyMod[i] );
	fprintf( f, "\n" );
	fprintf( f, "GG Shit         : " );
	for( i=0; i<16; i++ ) fprintf( f, "%02X ", (unsigned int)GG_Shit[i] );
	fprintf( f, "\n" );
	fprintf( f, "Dynamic BF Key  : " );
	for( i=0; i<16; i++ ) fprintf( f, "%02X ", (unsigned int)dyn_BF_key[i] );
	fprintf( f, "\n" );
	fprintf( f, "NULL terminator : %u (0x%02X)\n",
		(unsigned int)nullTerm, (unsigned int)nullTerm );
	fprintf( f, "Init packet dump end ;)\n\n" );
}

void L2Login_Init::_initPublicMembers()
{
	p_sessionId[0] = p_sessionId[1] = p_sessionId[2] = p_sessionId[3] = 0x00;
	p_protoVer = 0x00000000;
	memset( p_RSA_pubKeyMod, 0, sizeof(p_RSA_pubKeyMod) );
	memset( p_GG_shit, 0, sizeof(p_GG_shit) );
	memset( p_BF_dyn_key, 0, sizeof(p_BF_dyn_key) );
}

bool L2Login_Init::create( L2_VERSION ver )
{
	UNREFERENCED_PARAMETER(ver); // login server packets may ignore this
	//int i;
	scramble_RSA_PubKeyMod( p_RSA_pubKeyMod );
	// write data
	writeReset();
	setPacketType( 0x00 );
	writeBytes( p_sessionId, 4 );
	writeUInt( p_protoVer );
	writeBytes( p_RSA_pubKeyMod, 128 );
	writeUInt( 0x29dd954e ); // 4e 95 dd 29 // GGshit? O_o
	writeUInt( 0x77c39cfc ); // fc 9c c3 77
	writeUInt( 0x97adb620 ); // 20 b6 ad 97
	writeUInt( 0x07bde0f7 ); // f7 e0 bd 07
	writeBytes( p_BF_dyn_key, 16 );
	writeChar( 0x00 );
	// Init packet has no checksum or padding, but it has XOR encryption
	// fill packet up to xor key position
	writeD( 0x00000000 ); // write some 6 bytes, they are ignored? trash?
	writeH( 0x0000 );     // tested, works OK
	// now, XOR all previous bytes except first 2 bytes with packet len
	// and next 4 bytes with login session id?
	unsigned int xor_key = rand(); // O_o generate random XOR "key"
	unsigned char *raw_bytes = b.getBytesPtr(); // we need to modify almost all bytes...
	int psize = getPacketSize();
	int xor_offset = 6; //2; // should we XOR login session id also?
	while( xor_offset < psize )
	{
		// read current dword
		unsigned int xoring_dword =
			(unsigned int)raw_bytes[xor_offset] | 
			((unsigned int)raw_bytes[xor_offset+1]) << 8 |
			((unsigned int)raw_bytes[xor_offset+2]) << 16 |
			((unsigned int)raw_bytes[xor_offset+3]) << 24;
		xor_key += xoring_dword; // increase xor key by xoring dword before XOR
		xoring_dword ^= xor_key; // XOR
		// decodeXOR() first XORs, then substracts :) and it goes from end of packet to beginning
		// update dword inside packet
		raw_bytes[xor_offset]   = (unsigned char)(xoring_dword & 0xFF);
		raw_bytes[xor_offset+1] = (unsigned char)(xoring_dword >> 8 & 0xFF);
		raw_bytes[xor_offset+2] = (unsigned char)(xoring_dword >> 16 & 0xFF);
		raw_bytes[xor_offset+3] = (unsigned char)(xoring_dword >> 24 & 0xFF);
		// move to next dword
		xor_offset += 4;
	}
	// append resulting xor key to packet
	writeUInt( xor_key );
	// add some 4 (trash?) bytes up to size 186
	writeUInt( 0x00001234 );
	// now Init packet size is 186 bytes long, all OK
	// next step before sending packet to client is to encrypt Init packet
	// by STATIC blowfish key (can be done by calling L2LoginPacket::encodeBlowfish( true );)
	// NO need to call padPacketTo8ByteLen(), appendChecksum(), appendMore8Bytes().
	return true;
}

bool L2Login_Init::parse( L2_VERSION ver )
{
	UNREFERENCED_PARAMETER(ver); // login server packets may ignore this
	if( getPacketSize() != 186 ) return false;
	// first remove blowfish using static BF key (hardcoded)
	if( !decodeBlowfish( true ) ) return false;
	if( !decodeXOR() ) return false;
	// read data
	_initPublicMembers();
	if( getPacketType() != 0x00 ) return false;
	if( !read_SessionID( p_sessionId ) ) return false;
	p_protoVer = read_ProtoVer();
	if( !read_RSA_pubKeyMod( p_RSA_pubKeyMod ) ) return false;
	if( !read_GGShit( p_GG_shit ) ) return false;
	if( !read_DynamicBFKey( p_BF_dyn_key ) ) return false;
	unscramble_RSA_PubKeyMod( p_RSA_pubKeyMod );
	return true;
}
