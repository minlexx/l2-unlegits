#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**     Client: Len   19 [RequestBuyItem]
13 00
40
90 00 00 00     // list ID
01 00 00 00     // count
  [[ for each item ]]
25 04 00 00     // itemID
64 00 00 00     // count     **/
void L2Client::send_RequestBuyItem( TradeItemsList *plist )
{
	if( !plist ) return;
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket p;
	p.setPacketType( 0x40 ); // RequestBuyItem
	p.writeUInt( plist->listID );
	p.writeInt( plist->itemCount );
	int i;
	for( i=0; i<plist->itemCount; i++ )
	{
		p.writeUInt( plist->item[i].itemID );
		if( getL2Version() < L2_VERSION_T23 ) p.writeD( (int)plist->item[i].count );
		else p.writeQ( plist->item[i].count );
	}
	sendPacket( &p, true );
}

/**    Client: Len   59 [RequestSellItem]
3B 00
37                // pcode
00 00 00 00       // list ID (0 - to NPC?)
04 00 00 00       // count
  [[ for each item ]]
1D 21 01 10       // objectID
F0 0F 00 00       // itemID
01 00 00 00       // count        **/
void L2Client::send_RequestSellItem( TradeItemsList *plist )
{
	if( !plist ) return;
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket p;
	p.setPacketType( 0x37 ); // RequestSellItem
	p.writeUInt( plist->listID );
	p.writeInt( plist->itemCount );
	int i;
	for( i=0; i<plist->itemCount; i++ )
	{
		p.writeUInt( plist->item[i].objectID );
		p.writeUInt( plist->item[i].itemID );
		if( getL2Version() < L2_VERSION_T23 ) p.writeD( (int)plist->item[i].count );
		else p.writeQ( plist->item[i].count );
	}
	sendPacket( &p, true );
}
