#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/***     Client: Len    7 [TradeRequest] | 07 00
1A              // pcode
C5 09 00 10     // objectID of partner      ***/
void L2Client::send_TradeRequest( unsigned int targetObjectId/* = 0*/ )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	if( (this->usr.targetObjectID == 0) && (targetObjectId == 0) ) return; // target must be set
	if( targetObjectId == 0 ) targetObjectId = this->usr.targetObjectID;
	L2GamePacket p;
	p.setPacketType( 0x1A ); // TradeRequest
	p.writeUInt( targetObjectId );
	sendPacket( &p, true );
}

/***    Client: Len    7 [AnswerTradeRequest]
07 00
55             // pcode
01 00 00 00    // 1 - ok, 0 - cancel      **/
void L2Client::send_AnswerTradeRequest( bool accept/* = true*/ )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket p;
	p.setPacketType( 0x55 ); // AnswerTradeRequest
	if( accept ) p.writeUInt( 0x00000001 );
	else p.writeUInt( 0x00000000 );
	sendPacket( &p, true );
}

/** Client: Len   15 [AddTradeItem]
0F 00
1B              // pcode
01 00 00 00     // tradeId? O_o wtf L2J ignores this
D1 09 00 10     // objectID
01 00 00 00     // count  **/
void L2Client::send_AddTradeItem( unsigned int tradeID, unsigned int objectID, long long int count )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket p;
	p.setPacketType( 0x1B ); // AddTradeItem
	p.writeUInt( tradeID );
	p.writeUInt( objectID );
	if( getL2Version() < L2_VERSION_T23 )
		p.writeD( (int)count );
	else p.writeQ( count );
	sendPacket( &p, true );
}

/***    Client: Len    7 [TradeDone]
07 00
1C              // pcode
01 00 00 00     // OK/cancel trade? **/
void L2Client::send_TradeDone( int bConfirm )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket p;
	p.setPacketType( 0x1C ); // TradeDone
	p.writeInt( bConfirm );
	sendPacket( &p, true );
}
