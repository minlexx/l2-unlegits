#include "stdafx.h"
#include "L2Game_ProtocolVersion.h"

/* L2J:
// "[C] 00 ProtocolVersion";
 * pcode 0c00 is in interlude
 * pcode 0x0e is in hellbound
protected void readImpl()
{
	_version  = readD();
}
*/

L2Game_ProtocolVersion::L2Game_ProtocolVersion()
{
	this->_initNull();
}

L2Game_ProtocolVersion::L2Game_ProtocolVersion( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

bool L2Game_ProtocolVersion::create( L2_VERSION ver )
{
	switch( ver )
	{
	case L2_VERSION_T1:  this->createDefaultKamael();    break;
	case L2_VERSION_T15: this->createDefaultHellbound(); break;
	case L2_VERSION_T2:  this->createDefaultGracia1();   break;
	case L2_VERSION_T22: this->createDefaultGracia2();   break;
	case L2_VERSION_T23: this->createDefaultGracia3();   break;
	case L2_VERSION_T24: this->createDefaultGracia4();   break;
	default: return false; break;
	}
	return true;
}

/** Kamael
C->S
0B 01       // len
0E          // p.code ProtocolVersion
3C 03 00 00 // protocol - 828
09 07 54 56  03 09 0B 01  07 02 54 54  56 07 00 02
55 56 00 51  00 53 57 04  07 55 08 54  01 07 01 53
00 56 55 56  01 06 05 04  51 03 08 51  08 51 56 04
54 06 55 08  02 09 51 56  01 53 06 55  04 53 00 56
56 53 01 09  02 09 01 51  54 51 09 55  56 09 03 04
07 05 55 04  06 55 04 06  09 04 51 01  08 08 06 05
52 06 04 01  07 54 03 06  52 55 06 55  55 51 01 02
04 54 03 55  54 01 57 51  55 05 52 05  54 07 51 51
55 07 02 53  53 00 52 05  52 07 01 54  00 03 05 05
08 06 05 05  06 03 00 0D  08 01 07 09  03 51 03 07
53 09 51 06  07 54 0A 50  56 02 52 04  05 55 51 02
53 00 08 54  04 52 56 06  02 09 00 08  03 53 56 01
05 00 55 06  08 56 04 0D  06 07 52 06  07 04 0A 06
01 04 54 04  00 05 02 04  54 00 09 52  53 05 04 01
04 05 05 01  52 51 52 0D  06 51 08 09  54 53 00 0D
01 02 03 54  53 01 05 03  08 56 54 07  02 54 0B 06
8D 71 5F 08
*/

bool L2Game_ProtocolVersion::createDefaultKamael( unsigned int gameProtoVer /*= 828*/ )
{
	this->writeReset();
	// packet code 0x00 was in Interlude!!!!
	// this->writeUChar( 0x00 ); // packet type - ProtocolVersion
	// in Hellbound this code is 0x0e
	this->writeUChar( 0x0e ); // packet type - ProtocolVersion
	this->writeUInt( gameProtoVer );
	unsigned char pv_array[] = {
	0x09, 0x07, 0x54, 0x56,  0x03, 0x09, 0x0B, 0x01, 0x07, 0x02, 0x54, 0x54, 0x56, 0x07, 0x00, 0x02, 
	0x55, 0x56, 0x00, 0x51,  0x00, 0x53, 0x57, 0x04, 0x07, 0x55, 0x08, 0x54, 0x01, 0x07, 0x01, 0x53, 
	0x00, 0x56, 0x55, 0x56,  0x01, 0x06, 0x05, 0x04, 0x51, 0x03, 0x08, 0x51, 0x08, 0x51, 0x56, 0x04, 
	0x54, 0x06, 0x55, 0x08,  0x02, 0x09, 0x51, 0x56, 0x01, 0x53, 0x06, 0x55, 0x04, 0x53, 0x00, 0x56, 
	0x56, 0x53, 0x01, 0x09,  0x02, 0x09, 0x01, 0x51, 0x54, 0x51, 0x09, 0x55, 0x56, 0x09, 0x03, 0x04, 
	0x07, 0x05, 0x55, 0x04,  0x06, 0x55, 0x04, 0x06, 0x09, 0x04, 0x51, 0x01, 0x08, 0x08, 0x06, 0x05, 
	0x52, 0x06, 0x04, 0x01,  0x07, 0x54, 0x03, 0x06, 0x52, 0x55, 0x06, 0x55, 0x55, 0x51, 0x01, 0x02, 
	0x04, 0x54, 0x03, 0x55,  0x54, 0x01, 0x57, 0x51, 0x55, 0x05, 0x52, 0x05, 0x54, 0x07, 0x51, 0x51, 
	0x55, 0x07, 0x02, 0x53,  0x53, 0x00, 0x52, 0x05, 0x52, 0x07, 0x01, 0x54, 0x00, 0x03, 0x05, 0x05, 
	0x08, 0x06, 0x05, 0x05,  0x06, 0x03, 0x00, 0x0D, 0x08, 0x01, 0x07, 0x09, 0x03, 0x51, 0x03, 0x07, 
	0x53, 0x09, 0x51, 0x06,  0x07, 0x54, 0x0A, 0x50, 0x56, 0x02, 0x52, 0x04, 0x05, 0x55, 0x51, 0x02, 
	0x53, 0x00, 0x08, 0x54,  0x04, 0x52, 0x56, 0x06, 0x02, 0x09, 0x00, 0x08, 0x03, 0x53, 0x56, 0x01, 
	0x05, 0x00, 0x55, 0x06,  0x08, 0x56, 0x04, 0x0D, 0x06, 0x07, 0x52, 0x06, 0x07, 0x04, 0x0A, 0x06, 
	0x01, 0x04, 0x54, 0x04,  0x00, 0x05, 0x02, 0x04, 0x54, 0x00, 0x09, 0x52, 0x53, 0x05, 0x04, 0x01, 
	0x04, 0x05, 0x05, 0x01,  0x52, 0x51, 0x52, 0x0D, 0x06, 0x51, 0x08, 0x09, 0x54, 0x53, 0x00, 0x0D, 
	0x01, 0x02, 0x03, 0x54,  0x53, 0x01, 0x05, 0x03, 0x08, 0x56, 0x54, 0x07, 0x02, 0x54, 0x0B, 0x06, 
	0x8D, 0x71, 0x5F, 0x08  };
	writeBytes( pv_array, sizeof(pv_array) );
	return true;
}

/* Hellbound
C->S Len  267
0B 01         // len
0E            // p. code
3F 03 00 00   // protocol - 831 (Hellbound?)
09 07 54 56  03 09 0B 01  07 02 54 54  56 07 00 02
55 56 00 51  00 53 57 04  07 55 08 54  01 07 01 53
00 56 55 56  01 06 05 04  51 03 08 51  08 51 56 04
54 06 55 08  02 09 51 56  01 53 06 55  04 53 00 56
56 53 01 09  02 09 01 51  54 51 09 55  56 09 03 04
07 05 55 04  06 55 04 06  09 04 51 01  08 08 06 05
52 06 04 01  07 54 03 06  52 55 06 55  55 51 01 02
04 54 03 55  54 01 57 51  55 05 52 05  54 07 51 51
55 07 02 53  53 00 52 05  52 07 01 54  00 03 05 05
08 06 05 05  06 03 00 0D  08 01 07 09  03 51 03 07
53 09 51 06  07 54 0A 50  56 02 52 04  05 55 51 02
53 00 08 54  04 52 56 06  02 09 00 08  03 53 56 01
05 00 55 06  08 56 04 0D  06 07 52 06  07 04 0A 06
01 04 54 04  00 05 02 04  54 00 09 52  53 05 04 01
04 05 05 01  52 51 52 0D  06 51 08 09  54 53 00 0D
01 02 03 54  53 01 05 03  08 56 54 07  02 54 0B 06
FB 87 B9 4A 
*/

bool L2Game_ProtocolVersion::createDefaultHellbound( unsigned int gameProtoVer /*= 851*/ )
{
	this->writeReset();
	// packet code 0x00 was in Interlude!!!!
	// this->writeUChar( 0x00 ); // packet type - ProtocolVersion
	// in Hellbound this code is 0x0e
	this->writeUChar( 0x0e ); // packet type - ProtocolVersion
	this->writeUInt( gameProtoVer );
	unsigned char pv_array[] = {
	0x09, 0x07, 0x54, 0x56,  0x03, 0x09, 0x0B, 0x01, 0x07, 0x02, 0x54, 0x54, 0x56, 0x07, 0x00, 0x02, 
	0x55, 0x56, 0x00, 0x51,  0x00, 0x53, 0x57, 0x04, 0x07, 0x55, 0x08, 0x54, 0x01, 0x07, 0x01, 0x53, 
	0x00, 0x56, 0x55, 0x56,  0x01, 0x06, 0x05, 0x04, 0x51, 0x03, 0x08, 0x51, 0x08, 0x51, 0x56, 0x04, 
	0x54, 0x06, 0x55, 0x08,  0x02, 0x09, 0x51, 0x56, 0x01, 0x53, 0x06, 0x55, 0x04, 0x53, 0x00, 0x56, 
	0x56, 0x53, 0x01, 0x09,  0x02, 0x09, 0x01, 0x51, 0x54, 0x51, 0x09, 0x55, 0x56, 0x09, 0x03, 0x04, 
	0x07, 0x05, 0x55, 0x04,  0x06, 0x55, 0x04, 0x06, 0x09, 0x04, 0x51, 0x01, 0x08, 0x08, 0x06, 0x05, 
	0x52, 0x06, 0x04, 0x01,  0x07, 0x54, 0x03, 0x06, 0x52, 0x55, 0x06, 0x55, 0x55, 0x51, 0x01, 0x02, 
	0x04, 0x54, 0x03, 0x55,  0x54, 0x01, 0x57, 0x51, 0x55, 0x05, 0x52, 0x05, 0x54, 0x07, 0x51, 0x51, 
	0x55, 0x07, 0x02, 0x53,  0x53, 0x00, 0x52, 0x05, 0x52, 0x07, 0x01, 0x54, 0x00, 0x03, 0x05, 0x05, 
	0x08, 0x06, 0x05, 0x05,  0x06, 0x03, 0x00, 0x0D, 0x08, 0x01, 0x07, 0x09, 0x03, 0x51, 0x03, 0x07, 
	0x53, 0x09, 0x51, 0x06,  0x07, 0x54, 0x0A, 0x50, 0x56, 0x02, 0x52, 0x04, 0x05, 0x55, 0x51, 0x02, 
	0x53, 0x00, 0x08, 0x54,  0x04, 0x52, 0x56, 0x06, 0x02, 0x09, 0x00, 0x08, 0x03, 0x53, 0x56, 0x01, 
	0x05, 0x00, 0x55, 0x06,  0x08, 0x56, 0x04, 0x0D, 0x06, 0x07, 0x52, 0x06, 0x07, 0x04, 0x0A, 0x06, 
	0x01, 0x04, 0x54, 0x04,  0x00, 0x05, 0x02, 0x04, 0x54, 0x00, 0x09, 0x52, 0x53, 0x05, 0x04, 0x01, 
	0x04, 0x05, 0x05, 0x01,  0x52, 0x51, 0x52, 0x0D, 0x06, 0x51, 0x08, 0x09, 0x54, 0x53, 0x00, 0x0D, 
	0x01, 0x02, 0x03, 0x54,  0x53, 0x01, 0x05, 0x03, 0x08, 0x56, 0x54, 0x07, 0x02, 0x54, 0x0B, 0x06, 
	0xFB, 0x87, 0xB9, 0x4A  };
	writeBytes( pv_array, sizeof(pv_array) );
	return true;
}

bool L2Game_ProtocolVersion::createDefaultGracia1( unsigned int gameProtoVer /*= 12*/ )
{
	this->writeReset();
	// packet code 0x00 was in Interlude!!!!
	// this->writeUChar( 0x00 ); // packet type - ProtocolVersion
	// in Hellbound this code is 0x0e
	this->writeUChar( 0x0e ); // packet type - ProtocolVersion
	this->writeUInt( gameProtoVer );
	unsigned char pv_array[] = {
	0x09, 0x07, 0x54, 0x56,  0x03, 0x09, 0x0b, 0x01, 0x07, 0x02, 0x54, 0x54, 0x56, 0x07, 0x00, 0x02, 
	0x55, 0x56, 0x00, 0x51,  0x00, 0x53, 0x57, 0x04, 0x07, 0x55, 0x08, 0x54, 0x01, 0x07, 0x01, 0x53, 
	0x00, 0x56, 0x55, 0x56,  0x01, 0x06, 0x05, 0x04, 0x51, 0x03, 0x08, 0x51, 0x08, 0x51, 0x56, 0x04, 
	0x54, 0x06, 0x55, 0x08,  0x02, 0x09, 0x51, 0x56, 0x01, 0x53, 0x06, 0x55, 0x04, 0x53, 0x00, 0x56, 
	0x56, 0x53, 0x01, 0x09,  0x02, 0x09, 0x01, 0x51, 0x54, 0x51, 0x09, 0x55, 0x56, 0x09, 0x03, 0x04, 
	0x07, 0x05, 0x55, 0x04,  0x06, 0x55, 0x04, 0x06, 0x09, 0x04, 0x51, 0x01, 0x08, 0x08, 0x06, 0x05, 
	0x52, 0x06, 0x04, 0x01,  0x07, 0x54, 0x03, 0x06, 0x52, 0x55, 0x06, 0x55, 0x55, 0x51, 0x01, 0x02, 
	0x04, 0x54, 0x03, 0x55,  0x54, 0x01, 0x57, 0x51, 0x55, 0x05, 0x52, 0x05, 0x54, 0x07, 0x51, 0x51, 
	0x55, 0x07, 0x02, 0x53,  0x53, 0x00, 0x52, 0x05, 0x52, 0x07, 0x01, 0x54, 0x00, 0x03, 0x05, 0x05, 
	0x08, 0x06, 0x05, 0x05,  0x06, 0x03, 0x00, 0x0d, 0x08, 0x01, 0x07, 0x09, 0x03, 0x51, 0x03, 0x07, 
	0x53, 0x09, 0x51, 0x06,  0x07, 0x54, 0x0a, 0x50, 0x56, 0x02, 0x52, 0x04, 0x05, 0x55, 0x51, 0x02, 
	0x53, 0x00, 0x08, 0x54,  0x04, 0x52, 0x56, 0x06, 0x02, 0x09, 0x00, 0x08, 0x03, 0x53, 0x56, 0x01, 
	0x05, 0x00, 0x55, 0x06,  0x08, 0x56, 0x04, 0x0d, 0x06, 0x07, 0x52, 0x06, 0x07, 0x04, 0x0a, 0x06, 
	0x01, 0x04, 0x54, 0x04,  0x00, 0x05, 0x02, 0x04, 0x54, 0x00, 0x09, 0x52, 0x53, 0x05, 0x04, 0x01, 
	0x04, 0x05, 0x05, 0x01,  0x52, 0x51, 0x52, 0x0d, 0x06, 0x51, 0x08, 0x09, 0x54, 0x53, 0x00, 0x0d, 
	0x01, 0x02, 0x03, 0x54,  0x53, 0x01, 0x05, 0x03, 0x08, 0x56, 0x54, 0x07, 0x02, 0x54, 0x0b, 0x06, 
	0xdf, 0xb8, 0x3b, 0x54  };
	writeBytes( pv_array, sizeof(pv_array) );
	return true;
}

// TODO: validate
bool L2Game_ProtocolVersion::createDefaultGracia2( unsigned int gameProtoVer /*= 17*/ )
{
	this->writeReset();
	// packet code 0x00 was in Interlude!!!!
	// this->writeUChar( 0x00 ); // packet type - ProtocolVersion
	// in Hellbound this code is 0x0e
	this->writeUChar( 0x0e ); // packet type - ProtocolVersion
	this->writeUInt( gameProtoVer );
	unsigned char pv_array[] = {
	0x09, 0x07, 0x54, 0x56,  0x03, 0x09, 0x0B, 0x01,  0x07, 0x02, 0x54, 0x54,  0x56, 0x07, 0x00, 0x02, 
	0x55, 0x56, 0x00, 0x51,  0x00, 0x53, 0x57, 0x04,  0x07, 0x55, 0x08, 0x54,  0x01, 0x07, 0x01, 0x53, 
	0x00, 0x56, 0x55, 0x56,  0x01, 0x06, 0x05, 0x04,  0x51, 0x03, 0x08, 0x51,  0x08, 0x51, 0x56, 0x04, 
	0x54, 0x06, 0x55, 0x08,  0x02, 0x09, 0x51, 0x56,  0x01, 0x53, 0x06, 0x55,  0x04, 0x53, 0x00, 0x56, 
	0x56, 0x53, 0x01, 0x09,  0x02, 0x09, 0x01, 0x51,  0x54, 0x51, 0x09, 0x55,  0x56, 0x09, 0x03, 0x04, 
	0x07, 0x05, 0x55, 0x04,  0x06, 0x55, 0x04, 0x06,  0x09, 0x04, 0x51, 0x01,  0x08, 0x08, 0x06, 0x05, 
	0x52, 0x06, 0x04, 0x01,  0x07, 0x54, 0x03, 0x06,  0x52, 0x55, 0x06, 0x55,  0x55, 0x51, 0x01, 0x02, 
	0x04, 0x54, 0x03, 0x55,  0x54, 0x01, 0x57, 0x51,  0x55, 0x05, 0x52, 0x05,  0x54, 0x07, 0x51, 0x51, 
	0x55, 0x07, 0x02, 0x53,  0x53, 0x00, 0x52, 0x05,  0x52, 0x07, 0x01, 0x54,  0x00, 0x03, 0x05, 0x05, 
	0x08, 0x06, 0x05, 0x05,  0x06, 0x03, 0x00, 0x0D,  0x08, 0x01, 0x07, 0x09,  0x03, 0x51, 0x03, 0x07, 
	0x53, 0x09, 0x51, 0x06,  0x07, 0x54, 0x0A, 0x50,  0x56, 0x02, 0x52, 0x04,  0x05, 0x55, 0x51, 0x02, 
	0x53, 0x00, 0x08, 0x54,  0x04, 0x52, 0x56, 0x06,  0x02, 0x09, 0x00, 0x08,  0x03, 0x53, 0x56, 0x01, 
	0x05, 0x00, 0x55, 0x06,  0x08, 0x56, 0x04, 0x0D,  0x06, 0x07, 0x52, 0x06,  0x07, 0x04, 0x0A, 0x06, 
	0x01, 0x04, 0x54, 0x04,  0x00, 0x05, 0x02, 0x04,  0x54, 0x00, 0x09, 0x52,  0x53, 0x05, 0x04, 0x01, 
	0x04, 0x05, 0x05, 0x01,  0x52, 0x51, 0x52, 0x0D,  0x06, 0x51, 0x08, 0x09,  0x54, 0x53, 0x00, 0x0D, 
	0x01, 0x02, 0x03, 0x54,  0x53, 0x01, 0x05, 0x03,  0x08, 0x56, 0x54, 0x07,  0x02, 0x54, 0x0B, 0x06, 
	0xEB, 0xEF, 0x3D, 0xE6  };
	writeBytes( pv_array, sizeof(pv_array) );
	return true;
}

bool L2Game_ProtocolVersion::createDefaultGracia3( unsigned int gameProtoVer /*= 83*/ )
{
	this->writeReset();
	// packet code 0x00 was in Interlude!!!!
	// this->writeUChar( 0x00 ); // packet type - ProtocolVersion
	// in Hellbound this code is 0x0e
	this->writeUChar( 0x0e ); // packet type - ProtocolVersion
	this->writeUInt( gameProtoVer );
	unsigned char pv_array[] = {
	0x09, 0x07, 0x54, 0x56,  0x03, 0x09, 0x0B, 0x01,  0x07, 0x02, 0x54, 0x54,  0x56, 0x07, 0x00, 0x02, 
	0x55, 0x56, 0x00, 0x51,  0x00, 0x53, 0x57, 0x04,  0x07, 0x55, 0x08, 0x54,  0x01, 0x07, 0x01, 0x53, 
	0x00, 0x56, 0x55, 0x56,  0x01, 0x06, 0x05, 0x04,  0x51, 0x03, 0x08, 0x51,  0x08, 0x51, 0x56, 0x04, 
	0x54, 0x06, 0x55, 0x08,  0x02, 0x09, 0x51, 0x56,  0x01, 0x53, 0x06, 0x55,  0x04, 0x53, 0x00, 0x56, 
	0x56, 0x53, 0x01, 0x09,  0x02, 0x09, 0x01, 0x51,  0x54, 0x51, 0x09, 0x55,  0x56, 0x09, 0x03, 0x04, 
	0x07, 0x05, 0x55, 0x04,  0x06, 0x55, 0x04, 0x06,  0x09, 0x04, 0x51, 0x01,  0x08, 0x08, 0x06, 0x05, 
	0x52, 0x06, 0x04, 0x01,  0x07, 0x54, 0x03, 0x06,  0x52, 0x55, 0x06, 0x55,  0x55, 0x51, 0x01, 0x02, 
	0x04, 0x54, 0x03, 0x55,  0x54, 0x01, 0x57, 0x51,  0x55, 0x05, 0x52, 0x05,  0x54, 0x07, 0x51, 0x51, 
	0x55, 0x07, 0x02, 0x53,  0x53, 0x00, 0x52, 0x05,  0x52, 0x07, 0x01, 0x54,  0x00, 0x03, 0x05, 0x05, 
	0x08, 0x06, 0x05, 0x05,  0x06, 0x03, 0x00, 0x0D,  0x08, 0x01, 0x07, 0x09,  0x03, 0x51, 0x03, 0x07, 
	0x53, 0x09, 0x51, 0x06,  0x07, 0x54, 0x0A, 0x50,  0x56, 0x02, 0x52, 0x04,  0x05, 0x55, 0x51, 0x02, 
	0x53, 0x00, 0x08, 0x54,  0x04, 0x52, 0x56, 0x06,  0x02, 0x09, 0x00, 0x08,  0x03, 0x53, 0x56, 0x01, 
	0x05, 0x00, 0x55, 0x06,  0x08, 0x56, 0x04, 0x0D,  0x06, 0x07, 0x52, 0x06,  0x07, 0x04, 0x0A, 0x06, 
	0x01, 0x04, 0x54, 0x04,  0x00, 0x05, 0x02, 0x04,  0x54, 0x00, 0x09, 0x52,  0x53, 0x05, 0x04, 0x01, 
	0x04, 0x05, 0x05, 0x01,  0x52, 0x51, 0x52, 0x0D,  0x06, 0x51, 0x08, 0x09,  0x54, 0x53, 0x00, 0x0D, 
	0x01, 0x02, 0x03, 0x54,  0x53, 0x01, 0x05, 0x03,  0x08, 0x56, 0x54, 0x07,  0x02, 0x54, 0x0B, 0x06, 
	0x11, 0x5D, 0x1F, 0x60  }; // these last 4 bytes always differ :)
	writeBytes( pv_array, sizeof(pv_array) );
	return true;
}

bool L2Game_ProtocolVersion::createDefaultGracia4( unsigned int gameProtoVer /*= 146*/ )
{
	this->writeReset();
	this->writeUChar( 0x0e ); // packet type - ProtocolVersion
	this->writeUInt( gameProtoVer );
	unsigned char pv_array[] = {
	0x09, 0x07, 0x54, 0x56,  0x03, 0x09, 0x0B, 0x01,  0x07, 0x02, 0x54, 0x54,  0x56, 0x07, 0x00, 0x02, 
	0x55, 0x56, 0x00, 0x51,  0x00, 0x53, 0x57, 0x04,  0x07, 0x55, 0x08, 0x54,  0x01, 0x07, 0x01, 0x53, 
	0x00, 0x56, 0x55, 0x56,  0x01, 0x06, 0x05, 0x04,  0x51, 0x03, 0x08, 0x51,  0x08, 0x51, 0x56, 0x04, 
	0x54, 0x06, 0x55, 0x08,  0x02, 0x09, 0x51, 0x56,  0x01, 0x53, 0x06, 0x55,  0x04, 0x53, 0x00, 0x56, 
	0x56, 0x53, 0x01, 0x09,  0x02, 0x09, 0x01, 0x51,  0x54, 0x51, 0x09, 0x55,  0x56, 0x09, 0x03, 0x04, 
	0x07, 0x05, 0x55, 0x04,  0x06, 0x55, 0x04, 0x06,  0x09, 0x04, 0x51, 0x01,  0x08, 0x08, 0x06, 0x05, 
	0x52, 0x06, 0x04, 0x01,  0x07, 0x54, 0x03, 0x06,  0x52, 0x55, 0x06, 0x55,  0x55, 0x51, 0x01, 0x02, 
	0x04, 0x54, 0x03, 0x55,  0x54, 0x01, 0x57, 0x51,  0x55, 0x05, 0x52, 0x05,  0x54, 0x07, 0x51, 0x51, 
	0x55, 0x07, 0x02, 0x53,  0x53, 0x00, 0x52, 0x05,  0x52, 0x07, 0x01, 0x54,  0x00, 0x03, 0x05, 0x05, 
	0x08, 0x06, 0x05, 0x05,  0x06, 0x03, 0x00, 0x0D,  0x08, 0x01, 0x07, 0x09,  0x03, 0x51, 0x03, 0x07, 
	0x53, 0x09, 0x51, 0x06,  0x07, 0x54, 0x0A, 0x50,  0x56, 0x02, 0x52, 0x04,  0x05, 0x55, 0x51, 0x02, 
	0x53, 0x00, 0x08, 0x54,  0x04, 0x52, 0x56, 0x06,  0x02, 0x09, 0x00, 0x08,  0x03, 0x53, 0x56, 0x01, 
	0x05, 0x00, 0x55, 0x06,  0x08, 0x56, 0x04, 0x0D,  0x06, 0x07, 0x52, 0x06,  0x07, 0x04, 0x0A, 0x06, 
	0x01, 0x04, 0x54, 0x04,  0x00, 0x05, 0x02, 0x04,  0x54, 0x00, 0x09, 0x52,  0x53, 0x05, 0x04, 0x01, 
	0x04, 0x05, 0x05, 0x01,  0x52, 0x51, 0x52, 0x0D,  0x06, 0x51, 0x08, 0x09,  0x54, 0x53, 0x00, 0x0D, 
	0x01, 0x02, 0x03, 0x54,  0x53, 0x01, 0x05, 0x03,  0x08, 0x56, 0x54, 0x07,  0x02, 0x54, 0x0B, 0x06, 
	0xA6, 0x23, 0xF4, 0xFE  }; // these last 4 bytes always differ :)
	writeBytes( pv_array, sizeof(pv_array) );
	return true;
}

bool L2Game_ProtocolVersion::read_protoVer( unsigned int *pver )
{
	if( !pver || !(this->canReadBytes(5)) ) return false;
	(*pver) = 0;
	this->readReset();
	this->readUChar();
	(*pver) = this->readUInt();
	return true;
}
