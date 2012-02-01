#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::send_UseItem( unsigned int objectID )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2Game_UseItem *p = new L2Game_UseItem();
	p->create( objectID );
	sendPacket( p, true );
	delete p; p = NULL;
}

/** Client: Len   11 [RequestDestroyItem]
0B 00
60
71 08 00 10 // objectID
01 00 00 00 // count  **/
void L2Client::send_RequestDestroyItem( unsigned int objectID, long long int count )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2_VERSION ver = getL2Version();
	L2GamePacket *p = new L2GamePacket();
	p->setPacketType( 0x60 ); // RequestDestroyItem
	p->writeUInt( objectID );
	if( ver < L2_VERSION_T23 ) p->writeD( (int)count ); else p->writeQ( count );
	sendPacket( p, true );
	delete p; p = NULL;
}

/**  Client: Len   23 [RequestDropItem]
17 00
17
E4 09 00 10   // objectID
01 00 00 00   // count
F4 AA 00 00   //  x
2F A4 00 00   //  y
43 F2 FF FF   //  z    **/
void L2Client::send_RequestDropItem( unsigned int objectID, long long int count, int x, int y, int z )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2_VERSION ver = getL2Version();
	if( x == 0 ) x = usr.x;
	if( y == 0 ) y = usr.y;
	if( z == 0 ) z = usr.z;
	L2GamePacket *p = new L2GamePacket();
	p->setPacketType( 0x17 ); // RequestDropItem
	p->writeUInt( objectID );
	if( ver < L2_VERSION_T23 ) p->writeD( (int)count ); else p->writeQ( count );
	p->writeInt( x );
	p->writeInt( y );
	p->writeInt( z );
	sendPacket( p, true );
	delete p; p = NULL;
}

// Format: dd (objectID,count)
/**  Client: Len   11
0B 00
2F
13 14 04 10  // objectID  268702739
01 00 00 00  // count     1                  **/
void L2Client::send_RequestCrystallizeItem( unsigned int objectID, long long int count )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2_VERSION ver = getL2Version();
	L2GamePacket *p = new L2GamePacket();
	p->setPacketType( 0x2F ); // RequestCrystallizeItem
	p->writeUInt( objectID );
	if( ver < L2_VERSION_T23 ) p->writeD( (int)count ); else p->writeQ( count );
	sendPacket( p, true );
	delete p; p = NULL;
}

// Format: dd (objectID,count)
void L2Client::send_RequestGiveItemToPet( unsigned int objectID, long long int count )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2_VERSION ver = getL2Version();
	L2GamePacket *p = new L2GamePacket();
	p->setPacketType( 0x95 ); // RequestGiveItemToPet
	p->writeUInt( objectID );
	if( ver < L2_VERSION_T23 ) p->writeD( (int)count ); else p->writeQ( count );
	sendPacket( p, true );
	delete p; p = NULL;
}
