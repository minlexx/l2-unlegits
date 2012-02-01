#include "stdafx.h"
#include "L2Game_CharSelected.h"

L2Game_CharSelected::L2Game_CharSelected()
{
	_initNull();
}

L2Game_CharSelected::L2Game_CharSelected( const unsigned char *bytes, unsigned int length )
{
	_initNull();
	setBytes( bytes, length );
}

bool L2Game_CharSelected::parse( L2_VERSION ver /*= L2_VERSION_T1*/ )
{
	UNREFERENCED_PARAMETER(ver);
	if( !this->canReadBytes( 200 ) ) return false; // must be at least 200 bytes

	readReset();
	readUChar(); // 0x0b CharSelected

	wcscpy( p_char_name, readUnicodeStringPtr() ); // char name
	p_charId    = readUInt();                      // char id (objectId)
	wcscpy( p_title, readUnicodeStringPtr() );     // title
	p_sessionId = readUInt();                      // sessionId
	p_clanId    = readUInt();                      // clanId
	readD();                                       // 0x00 (maybe allyId?)
	p_sex       = readD();
	p_race      = readD();
	p_classId   = readUInt();
	p_isActive  = readD();

	p_x         = readD();
	p_y         = readD();
	p_z         = readD();

	p_curHp     = readF();
	p_curMp     = readF();
	p_sp        = readUInt();
	p_exp       = readUInt64();
	p_level     = readD();
	p_karma     = readD();
	p_PK_kills  = readD();

	p_INT = readD();
	p_STR = readD();
	p_CON = readD();
	p_MEN = readD();
	p_DEX = readD();
	p_WIT = readD();

	readD(); // writeD( 0x0451 ); // game time?
	readD(); // writeD( 0x00 ); // 0x00

	readD(); // writeD( classId ); // classId already read?

	//writeD(0x00); // 4 x 0x00
	//writeD(0x00);
	//writeD(0x00);
	//writeD(0x00); 
	readD(); readD(); readD(); readD();

	//writeB(new byte[64]); // some 64 bytes
	int i;
	for( i=0; i<16; i++ ) readD(); // read 4 x 16 = 64 bytes

	p_opcodeObfuscatorSeed = readUInt();
	return true;
}

bool L2Game_CharSelected::create( L2_VERSION ver /*= L2_VERSION_T1*/ )
{
	UNREFERENCED_PARAMETER(ver);
	writeReset();
	writeC(0x0b);

	writeS( p_char_name );
	writeD( p_charId );
	writeS( p_title );
	writeD( p_sessionId ); // playOkID1
	writeD( p_clanId );
	writeD( 0x00 );  // maybe allyId?
	writeD( p_sex );
	writeD( p_race );
	writeD( p_classId );
	writeD( p_isActive ); // active ??

	writeD( p_x );
	writeD( p_y );
	writeD( p_z );

	writeF( p_curHp );
	writeF( p_curMp );
	writeD( p_sp );
	writeQ( p_exp );
	writeD( p_level );
	writeD( p_karma );
	writeD( p_PK_kills );
	writeD( p_INT );
	writeD( p_STR );
	writeD( p_CON );
	writeD( p_MEN );
	writeD( p_DEX );
	writeD( p_WIT );

	writeD( 0x0451 ); //?
	writeD( 0x00 ); // some constants

	writeD( p_classId );

	writeD(0x00); // 4 x 0x00
	writeD(0x00);
	writeD(0x00);
	writeD(0x00); 

	//writeB(new byte[64]); // some 64 bytes
	unsigned char *rndbuf = (unsigned char *)malloc( 64 );
	writeBytes( rndbuf, 64 );
	free( rndbuf );

	writeD( p_opcodeObfuscatorSeed ); // writeD(0x00);
	return true;
}
