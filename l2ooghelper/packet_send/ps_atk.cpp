#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::send_RequestMagicSkillUse( unsigned int skillID,
		unsigned int ctrlPressed, unsigned char shiftPressed )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket *pack = new L2GamePacket();
	pack->setPacketType( 0x39 ); // RequestMagicSkillUse
	pack->writeUInt( skillID );
	pack->writeUInt( ctrlPressed );
	pack->writeUChar( shiftPressed );
	sendPacket( pack, true );
	delete pack; pack = NULL;
}

void L2Client::send_Action( unsigned int objectID, int x, int y, int z, unsigned char useShift )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	if( x == 0 ) x = usr.x;
	if( y == 0 ) y = usr.y;
	if( z == 0 ) z = usr.z;
	L2GamePacket *pack = new L2GamePacket();
	pack->setPacketType( 0x1F ); // Action
	pack->writeUInt( objectID );
	pack->writeInt( x );
	pack->writeInt( y );
	pack->writeInt( z );
	pack->writeUChar( useShift );
	sendPacket( pack, true );
	delete pack; pack = NULL;
}

void L2Client::send_RequestTargetCanceld()
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket *pack = new L2GamePacket();
	pack->setPacketType( 0x48 ); // RequestTargetCanceld
	pack->writeUShort( 0x0000 );
	sendPacket( pack, true );
	delete pack; pack = NULL;
}

void L2Client::send_AttackRequest( unsigned int objectID, int x, int y, int z, unsigned char useShift )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	if( objectID == 0 ) objectID = usr.targetObjectID;
	if( objectID == 0 ) return;
	//
	if( x == 0 ) x = usr.x;
	if( y == 0 ) y = usr.y;
	if( z == 0 ) z = usr.z;
	//
	L2GamePacket *pack = new L2GamePacket();
	pack->setPacketType( 0x01 ); // AttackRequest
	pack->writeUInt( objectID );
	pack->writeInt( x );
	pack->writeInt( y );
	pack->writeInt( z );
	pack->writeUChar( useShift );
	sendPacket( pack, true );
	delete pack; pack = NULL;
}


void L2Client::send_RequestAutoSoulshot( unsigned int itemID, bool enable )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket *pack = new L2GamePacket();
	pack->setPacketType( 0xD0 );
	pack->writeUShort( 0x000D ); // D0:000D RequestAutoSoulshot
	pack->writeUInt( itemID );
	enable ? pack->writeUInt( 1 ) : pack->writeUInt( 0 );
	sendPacket( pack, true );
	delete pack; pack = NULL;
}

