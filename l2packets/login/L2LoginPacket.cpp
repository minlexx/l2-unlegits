#include "stdafx.h"
#include "L2LoginPacket.h"

//#define L2LOGINP_DEBUGOUT_BLOWFISH // can be defined in L2Packets.h

L2LoginPacket::L2LoginPacket()
{
	_initNull(); // static BF key is initialized there
	NEW_BLOWFISH_KEY_LEN = 0;
	memset( (void *)NEW_BLOWFISH_KEY, 0, sizeof(NEW_BLOWFISH_KEY) );
}

L2LoginPacket::L2LoginPacket( const unsigned char *bytes, unsigned int length )
{
	_initNull(); // static BF key is initialized there
	NEW_BLOWFISH_KEY_LEN = 0;
	memset( (void *)NEW_BLOWFISH_KEY, 0, sizeof(NEW_BLOWFISH_KEY) );
	this->setBytes( bytes, length );
}

L2LoginPacket::~L2LoginPacket()
{
	_freeSelf();
}

void L2LoginPacket::_initNull()
{
	L2BasePacket::_initNull();
	memset( (void *)STATIC_BLOWFISH_KEY, 0, sizeof(STATIC_BLOWFISH_KEY) );
	STATIC_BLOWFISH_KEY[0]  = 0x6B;
	STATIC_BLOWFISH_KEY[1]  = 0x60;
	STATIC_BLOWFISH_KEY[2]  = 0xCB;
	STATIC_BLOWFISH_KEY[3]  = 0x5B;
	STATIC_BLOWFISH_KEY[4]  = 0x82;
	STATIC_BLOWFISH_KEY[5]  = 0xCE;
	STATIC_BLOWFISH_KEY[6]  = 0x90;
	STATIC_BLOWFISH_KEY[7]  = 0xB1;
	STATIC_BLOWFISH_KEY[8]  = 0xCC;
	STATIC_BLOWFISH_KEY[9]  = 0x2B;
	STATIC_BLOWFISH_KEY[10] = 0x6C;
	STATIC_BLOWFISH_KEY[11] = 0x55;
	STATIC_BLOWFISH_KEY[12] = 0x6C;
	STATIC_BLOWFISH_KEY[13] = 0x6C;
	STATIC_BLOWFISH_KEY[14] = 0x6C;
	STATIC_BLOWFISH_KEY[15] = 0x6C;
	STATIC_BLOWFISH_KEY_LEN = 16;
	xor_key = 0;
}

bool L2LoginPacket::decodeBlowfish( bool bUseStaticBFKey )
{
	int blen = (int)getPacketSize(); // all array length
	int datalen = blen - 2;      // only data len, w/o 1st 2 bytes - packet size
	int n8bc = datalen / 8;      // 8-byte blocks count
	int rest = datalen - n8bc*8; // bytes left
	if( rest > 0 ) rest = 8;     // do we need addition?
	int newdatalen = datalen + rest; // dlina novogo u4astka dannih
	int newbuflen = newdatalen + 2;  // dlina novogo paketa
	if( blen < 1 ) return false; // TODO: throw?
	
	unsigned char *buf = b.getBytesPtr();
	if( !buf ) return false; // TODO: throw?
	
	// initialize Blowfish key
	BF_KEY bfkey;
	if( bUseStaticBFKey )
		BF_set_key( &bfkey, (int)this->STATIC_BLOWFISH_KEY_LEN,
			this->STATIC_BLOWFISH_KEY );
	else
		BF_set_key( &bfkey, (int)this->NEW_BLOWFISH_KEY_LEN,
			this->NEW_BLOWFISH_KEY );
	
	int offset = 0;
	int nPasses = 0;
	unsigned char *outbuf = NULL;
	
	// we will decode to this buffer
	outbuf = (unsigned char *)malloc( newbuflen );
	if( !outbuf )
	{
#ifdef L2LOGINP_DEBUGOUT_BLOWFISH
		printf( "L2LoginPacket::decodeBlowfish(): memory error!\n" );
#endif // L2LOGINP_DEBUGOUT_BLOWFISH
		return false;
	}
	memset( outbuf, 0, newbuflen );
	outbuf[0] = buf[0]; // copy packet len
	outbuf[1] = buf[1];

	nPasses = 0;
#ifdef L2LOGINP_DEBUGOUT_BLOWFISH
	if( bUseStaticBFKey ) printf( "L2LoginPacket::decodeBlowfish(): using STATIC BF KEY\n" );
	else printf( "L2LoginPacket::decodeBlowfish(): using DYNAMIC BF KEY\n" );
#endif // L2LOGINP_DEBUGOUT_BLOWFISH
	for( offset=2; offset<newdatalen; offset+=8 )
	{
		unsigned char data[8] = {0,0,0,0,0,0,0,0};
		memcpy( data, buf+offset, 8 );
		BF_decrypt( (BF_LONG *)data, &bfkey );
		memcpy( outbuf+offset, data, 8 );
		nPasses++;
	}
#ifdef L2LOGINP_DEBUGOUT_BLOWFISH
	printf( "L2LoginPacket::decodeBlowfish(): Decode2: %d passes, %d 8-byte blocks\n", nPasses, n8bc );
	//L2LoginPacket *dec2 = new L2LoginPacket( outbuf, blen );
	//dec2->dumpToFile( stdout );
	//dec2->saveToFileRaw( "pdecodedbf.dat" );
	//delete dec2;
#endif // L2LOGINP_DEBUGOUT_BLOWFISH
	
	this->setBytes( outbuf, blen );
	free( outbuf );

#ifdef L2LOGINP_DEBUGOUT_BLOWFISH
	printf( "L2LoginPacket::decodeBlowfish(): decode complete\n" );
#endif // L2LOGINP_DEBUGOUT_BLOWFISH
	
	return true;
}

bool L2LoginPacket::setDynamicBFKey( unsigned char *newKey, unsigned int newKeyLen )
{
	if( !newKey ) return false;
	if( newKeyLen < 1 ) return false;
	memcpy( this->NEW_BLOWFISH_KEY, newKey, newKeyLen );
	this->NEW_BLOWFISH_KEY_LEN = newKeyLen;
#ifdef L2LOGINP_DEBUGOUT_BLOWFISH
	printf( "L2LoginPacket::setDynamicBFKey(): set dynamic BF key of len = %u\n",
		newKeyLen );
#endif // L2LOGINP_DEBUGOUT_BLOWFISH
	return true;
}

bool L2LoginPacket::decodeBlowfish( unsigned char *blowfishKey )
{
	setDynamicBFKey( blowfishKey, 16 );
	return decodeBlowfish( false );
}

/*bool L2LoginPacket::appendChecksum()
{
	// save packet len bytes
	//unsigned char plenbytes[2] = {0,0}
	//plenbytes[0] = b.getByteAt( 0 );
	//plenbytes[1] = b.getByteAt( 1 );
	// get packet length
	unsigned int count = this->getPacketSize();
	if( count > 2 )
	{
		unsigned char *rawbytes = b.getBytesPtr();
		unsigned int chksum = 0;
		unsigned int i = 0;
		for( i=2; i<count; i += 4 ) chksum ^= *( (unsigned int *)&rawbytes[i] );
		this->writeUInt( chksum );
		// restore plen bytes
		//b.setByteAt( 0, plenbytes[0] );
		//b.setByteAt( 1, plenbytes[1] );
		return true;
	}
	return false;
}*/

bool L2LoginPacket::appendChecksum( bool append4bytes )
{
	unsigned char *raw = b.getBytesPtr();
	int size = (int)this->real_size;
	unsigned int chksum = 0;
	int count = size; // size-4; // we do not reserve space for checksum
	unsigned int ecx = 0;
	int i = 0;
	int offset = 2;
#ifdef L2LOGINP_DEBUGOUT_CHKSUM
	printf( "L2LoginPacket::appendChecksum(): for( i=%d; i<%d; i+=4)\n",
		offset, count );
#endif // L2LOGINP_DEBUGOUT_CHKSUM
	for( i = offset; i<count; i+=4 )
	{
		ecx  = (raw[i])           & 0x000000ff;
		ecx |= (raw[i+1] << 0x08) & 0x0000ff00;
		ecx |= (raw[i+2] << 0x10) & 0x00ff0000;
		ecx |= (raw[i+3] << 0x18) & 0xff000000;

		chksum ^= ecx;
	}

	ecx  = raw[i]           & 0x000000ff;
	ecx |= raw[i+1] << 0x08 & 0x0000ff00;
	ecx |= raw[i+2] << 0x10 & 0x00ff0000;
	ecx |= raw[i+3] << 0x18 & 0xff000000;

	// write chksum to end of packet
	/*/// L2J style :)
	raw[i]   = (unsigned char)((chksum)         & 0xff);
	raw[i+1] = (unsigned char)((chksum >> 0x08) & 0xff);
	raw[i+2] = (unsigned char)((chksum >> 0x10) & 0xff);
	raw[i+3] = (unsigned char)((chksum >> 0x18) & 0xff);
	*/
	//this->write_ptr = i+4;
	
#ifdef L2LOGINP_DEBUGOUT_CHKSUM
	printf( "L2LoginPacket::appendChecksum(): writing chksum [%04X] at indexes [%d..%d]\n",
		chksum, this->write_ptr, this->write_ptr+3 );
#endif // L2LOGINP_DEBUGOUT_CHKSUM
	
	writeUChar( (unsigned char)((chksum)         & 0xff) );
	writeUChar( (unsigned char)((chksum >> 0x08) & 0xff) );
	writeUChar( (unsigned char)((chksum >> 0x10) & 0xff) );
	writeUChar( (unsigned char)((chksum >> 0x18) & 0xff) );
	// Should we also automatically append 4 0x00 symbols to packet?
	// Before calling appendChecksum(), packet is considered to be
	// aligned at 8-byte border, so adding 4 bytes of checksum
	// breaks this alignment. And adding more 4 zero bytes will
	// resore 8-byte border alignment.
	// But sometimes this adding is not necessary, for example,
	// when encoding packet from Login Server to Client.
	if( append4bytes )
	{
		writeUChar( 0x00 );
		writeUChar( 0x00 );
		writeUChar( 0x00 );
		writeUChar( 0x00 );
	}
	return true;
}

bool L2LoginPacket::verifyBytesChecksum( const unsigned char *bytes, unsigned int offset, unsigned int size )
{
	if( !bytes || (size<8) ) return false;
	if( size % 4 != 0 ) return false; // data size should be a multiple of 4
	//
	unsigned int chksum = 0;
	unsigned int count = size-4;
	unsigned int check = 0xFFFFFFFF;
	unsigned int i = 0;
	//
	for( i=offset; i<count; i+=4 )
	{
		check = bytes[i] & 0xFF;
		check |= (bytes[i+1] << 8) & 0xFF00;
		check |= (bytes[i+2] << 16) & 0xFF0000;
		check |= (bytes[i+3] << 24) & 0xFF000000;
		chksum ^= check;
	}
	check = bytes[i] & 0xFF;
	check |= (bytes[i+1] << 8) & 0xFF00;
	check |= (bytes[i+2] << 16) & 0xFF0000;
	check |= (bytes[i+3] << 24) & 0xFF000000;
	//
	return (check == chksum);
}

bool L2LoginPacket::verifyChecksum() const
{
	const unsigned char *bytes = getBytesPtr();
	unsigned int data_len = getDataSize();
	return L2LoginPacket::verifyBytesChecksum( bytes, 2, data_len );
}

bool L2LoginPacket::padPacketTo8ByteLen()
{
	unsigned char *packet = b.getBytesPtr();
	unsigned int plen = getPacketSize();
	if( !packet || plen<3 ) return false;
	unsigned int datalen = plen-2;
	
#ifdef L2LOGINP_DEBUGOUT_PADDING
	printf( "L2LoginPacket::padPacketTo8ByteLen(): len = %u (data len = %d)\n",
		plen, datalen );
#endif // L2LOGINP_DEBUGOUT_PADDING
	
	unsigned int rest = datalen % 8;
	unsigned int addsize = 0;
	if( rest > 0 ) addsize = 8 - rest;
	
#ifdef L2LOGINP_DEBUGOUT_PADDING
	printf( "L2LoginPacket::padPacketTo8ByteLen(): We should add %u bytes (rest is %u)\n",
		addsize, rest );
#endif // L2LOGINP_DEBUGOUT_PADDING
	
	// save packet len bytes
	//unsigned char plenbytes[2] = {0,0};
	//plenbytes[0] = b.getByteAt( 0 );
	//plenbytes[1] = b.getByteAt( 1 );
	
	// append by NULLs if we must append something
	if( addsize > 0 )
	{
		unsigned int i;
		for( i=0; i<addsize; i++ ) this->writeUChar( 0x00 );
		// restore plen bytes
		//b.setByteAt( 0, plenbytes[0] );
		//b.setByteAt( 1, plenbytes[1] );
#ifdef L2LOGINP_DEBUGOUT_PADDING
		printf( "L2LoginPacket::padPacketTo8ByteLen(): "
				"after padding real_size = [%u] ==== \n", this->real_size );
		//this->dumpToFile( stdout );
#endif // L2LOGINP_DEBUGOUT_PADDING
	}
	
	return true;
}

bool L2LoginPacket::appendMore8Bytes()
{
	int i;
	for( i=0; i<8; i++ ) writeUChar( 0x00 );
	return true;
}

bool L2LoginPacket::encodeBlowfish( bool bUseStaticBFKey )
{
	unsigned char *buf = this->b.getBytesPtr();
	if( !buf ) return false; // TODO: throw
	unsigned int blen = getPacketSize();
	if( blen < 1 ) return false; // TODO: throw
	//unsigned int datalen = blen - 2; // C4189: local variable is initialized but not referenced
	//this->padPacketTo8ByteLen(); // manually
	
	// initialize Blowfish key
	BF_KEY bfkey;
	if( bUseStaticBFKey )
		BF_set_key( &bfkey, (int)this->STATIC_BLOWFISH_KEY_LEN,
			this->STATIC_BLOWFISH_KEY );
	else
		BF_set_key( &bfkey, (int)this->NEW_BLOWFISH_KEY_LEN,
			this->NEW_BLOWFISH_KEY );
	
	unsigned int offset = 0;
	int nPasses = 0;
	unsigned char *outbuf = NULL;

	// we will decode to this buffer
	outbuf = (unsigned char *)malloc( blen );
	if( !outbuf )
	{
#ifdef L2LOGINP_DEBUGOUT_BLOWFISH
		printf( "L2LoginPacket::encodeBlowfish(): memory error!\n" );
#endif // L2LOGINP_DEBUGOUT_BLOWFISH
		return false;
	}
	memset( outbuf, 0, blen );
	outbuf[0] = buf[0]; // copy packet len
	outbuf[1] = buf[1];

#ifdef L2LOGINP_DEBUGOUT_BLOWFISH
	if( bUseStaticBFKey ) printf( "L2LoginPacket::encodeBlowfish(): using STATIC BF KEY\n" );
	else printf( "L2LoginPacket::encodeBlowfish(): using DYNAMIC BF KEY\n" );
	printf( "L2LoginPacket::encodeBlowfish(): blen = %u, datalen = %u\n",
		blen, datalen );
#endif // L2LOGINP_DEBUGOUT_BLOWFISH
	for( offset=2; offset<real_size-2; offset+=8 )
	{
		unsigned char data[8] = {0,0,0,0,0,0,0,0};
		memcpy( data, buf+offset, 8 );
		BF_encrypt( (BF_LONG *)data, &bfkey );
		memcpy( outbuf+offset, data, 8 );
		nPasses++;
	}
#ifdef L2LOGINP_DEBUGOUT_BLOWFISH
	int n8bc = datalen / 8;
	printf( "L2LoginPacket::encodeBlowfish(): %d passes, %d 8-byte blocks ==\n",
		nPasses, n8bc );
	//L2LoginPacket dec2; // = new L2LoginPacket();
	//printf( "here\n" );
	//bool bret = dec2.setBytes( outbuf, real_size );
	//if( bret ) printf( "true\n" ); else printf( "false\n" );
	//dec2.dumpToFile( stdout );
	//delete dec2;
#endif // L2LOGINP_DEBUGOUT_BLOWFISH
	
	this->setBytes( outbuf, blen );
	free( outbuf );
	
#ifdef L2LOGINP_DEBUGOUT_BLOWFISH
	printf( "L2LoginPacket::encodeBlowfish(): encode complete\n" );
#endif // L2LOGINP_DEBUGOUT_BLOWFISH
	return true;
}

bool L2LoginPacket::encodeAndPrepareToSend( unsigned char *blowfishKey, unsigned int bfKeyLen /*= 16*/ )
{
	if( !setDynamicBFKey( blowfishKey, bfKeyLen ) ) return false;
	if( !padPacketTo8ByteLen() ) return false;
	appendChecksum( true );
	appendMore8Bytes();
	return encodeBlowfish( false );
}
