#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::send_Appearing()
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket pack;
	pack.setPacketType( 0x3A ); // pcode Appearing
	sendPacket( &pack, true );
}

void L2Client::send_MoveBackwardToLocation( int xDst, int yDst, int zDst /*= 0x7FFFFFFF*/ )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	if( zDst == 0x7FFFFFFF ) zDst = usr.z;
	L2GamePacket *pack = new L2GamePacket();
	pack->setPacketType( 0x0F ); // MoveBackwardToLocation
	pack->writeInt( xDst );
	pack->writeInt( yDst );
	pack->writeInt( zDst );
	pack->writeInt( usr.x );
	pack->writeInt( usr.y );
	pack->writeInt( usr.z );
	pack->writeInt( 1 ); // for movement used mouse
	sendPacket( pack, true );
	delete pack; pack = NULL;
}

void L2Client::game_MoveToDelta( int dx, int dy, int dz /*= 0*/ )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	int xDst = usr.x + dx;
	int yDst = usr.y + dy;
	int zDst = usr.z + dz;
	send_MoveBackwardToLocation( xDst, yDst, zDst );
}

/**  Client: Len   23 [ValidatePosition]
17 00
59
B3 A5 00 00    // x 42419
39 A4 00 00    // y 42041
5B F2 FF FF    // z -3493
E2 24 00 00    // heading 9442
00 00 00 00    // wtf data? L2J ignores   **/
void L2Client::send_ValidatePosition()
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket p;
	p.setPacketType( 0x59 ); // ValidatePosition
	p.writeInt( usr.x );
	p.writeInt( usr.y );
	p.writeInt( usr.z );
	p.writeInt( usr.heading ); // L2J ignores...?
	p.writeInt( 0x00 );
	sendPacket( &p, true );
}
