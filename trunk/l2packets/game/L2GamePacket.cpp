#include "stdafx.h"
#include "L2GamePacket.h"

L2GamePacket::L2GamePacket()
{
	this->_initNull();
}

L2GamePacket::L2GamePacket( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

// in Hellbound key must be 16-byte array
// key is changed after enc/dec procedure
bool L2GamePacket::decodeXOR( unsigned char *key )
{
	if( !key ) return false;
	unsigned int sz  = this->getPacketSize();
	unsigned int dsz = this->getDataSize();
	if( sz < 3 ) return false;
	unsigned char temp = 0, temp2 = 0;
	unsigned int i = 0;
	unsigned char *data = (unsigned char *)this->getBytesPtr();
	// xor enc/dec from index 2 (pass 2 bytes - packet size)
	data += 2;
	for( i = 0; i < dsz; i++ )
	{
		temp2 = data[i];
		data[i] = (temp2 ^ (key[i & 15]) ^ temp);
		temp = temp2;
	}

	unsigned int old = 0;
	old |=  key[8]           & 0x000000ff;
	old |= (key[9]  << 0x08) & 0x0000ff00;
	old |= (key[10] << 0x10) & 0x00ff0000;
	old |= (key[11] << 0x18) & 0xff000000;

	//old += sz;
	old += dsz; // key increases by data size

	key[8]  = (unsigned char)((old)         & 0xff);
	key[9]  = (unsigned char)((old >> 0x08) & 0xff);
	key[10] = (unsigned char)((old >> 0x10) & 0xff);
	key[11] = (unsigned char)((old >> 0x18) & 0xff);
	return true;
}

// in Hellbound key must be 16-byte array
// key is changed after enc/dec procedure
bool L2GamePacket::encodeXOR( unsigned char *key )
{
	if( !key ) return false;
	unsigned int  sz  = this->getPacketSize();
	unsigned int  dsz = this->getDataSize();
	if( sz < 3 ) return false;
	unsigned char temp = 0, temp2 = 0;
	unsigned int  i = 0;
	unsigned char *data = (unsigned char *)this->getBytesPtr();
	// xor enc/dec from index 2 (pass 2 bytes - packet size)
	data += 2;
	for( i = 0; i < dsz; i++ )
	{
		temp2 = data[i];
		data[i] = (temp2 ^ (key[i & 15]) ^ temp );
		temp = data[i];
	}

	unsigned int old = 0;
	old |=  key[8]           & 0x000000ff;
	old |= (key[9]  << 0x08) & 0x0000ff00;
	old |= (key[10] << 0x10) & 0x00ff0000;
	old |= (key[11] << 0x18) & 0xff000000;

	//old += sz;
	old += dsz; // key increases by data size

	key[8]  = (unsigned char)((old)         & 0xff);
	key[9]  = (unsigned char)((old >> 0x08) & 0xff);
	key[10] = (unsigned char)((old >> 0x10) & 0xff);
	key[11] = (unsigned char)((old >> 0x18) & 0xff);
	return true;
}

bool L2GamePacket::decodeXOR_buffer( unsigned char *decodeBytes, unsigned int bytesLen,
										   unsigned char *key )
{
	//if( !key ) return false;
	unsigned int dsz = bytesLen - 2;
	//if( sz < 3 ) return false;
	unsigned char temp = 0, temp2 = 0;
	unsigned int i = 0;
	unsigned char *data = decodeBytes;
	// xor enc/dec from index 2 (pass 2 bytes - packet size)
	data += 2;
	for( i = 0; i < dsz; i++ )
	{
		temp2 = data[i];
		data[i] = (temp2 ^ (key[i & 15]) ^ temp);
		temp = temp2;
	}

	unsigned int old = 0;
	old |=  key[8]           & 0x000000ff;
	old |= (key[9]  << 0x08) & 0x0000ff00;
	old |= (key[10] << 0x10) & 0x00ff0000;
	old |= (key[11] << 0x18) & 0xff000000;

	//old += sz;
	old += dsz; // key increases by data size

	key[8]  = (unsigned char)((old)         & 0xff);
	key[9]  = (unsigned char)((old >> 0x08) & 0xff);
	key[10] = (unsigned char)((old >> 0x10) & 0xff);
	key[11] = (unsigned char)((old >> 0x18) & 0xff);
	return true;
}

bool L2GamePacket::encodeXOR_buffer( unsigned char *encodeBytes, unsigned int bytesLen,
										   unsigned char *key )
{
	//if( !key ) return false;
	unsigned int  dsz = bytesLen - 2;
	//if( sz < 3 ) return false;
	unsigned char temp = 0, temp2 = 0;
	unsigned int  i = 0;
	unsigned char *data = encodeBytes;
	// xor enc/dec from index 2 (pass 2 bytes - packet size)
	data += 2;
	for( i = 0; i < dsz; i++ )
	{
		temp2 = data[i];
		data[i] = (temp2 ^ (key[i & 15]) ^ temp );
		temp = data[i];
	}

	unsigned int old = 0;
	old |=  key[8]           & 0x000000ff;
	old |= (key[9]  << 0x08) & 0x0000ff00;
	old |= (key[10] << 0x10) & 0x00ff0000;
	old |= (key[11] << 0x18) & 0xff000000;

	//old += sz;
	old += dsz; // key increases by data size

	key[8]  = (unsigned char)((old)         & 0xff);
	key[9]  = (unsigned char)((old >> 0x08) & 0xff);
	key[10] = (unsigned char)((old >> 0x10) & 0xff);
	key[11] = (unsigned char)((old >> 0x18) & 0xff);
	return true;
}
