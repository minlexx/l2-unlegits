#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"
// dialogs
#include "BuySellDlg.h"
#include "PrivateStoreDlg.h"

void L2Client::handle_BuyList( TradeItemsList *list )
{
	if( !list ) return;
	BuySellDlg *dlg = new BuySellDlg( hWnd, WMMY_UI_BUYSELLDLGREPLY, BuySellDlg::TYPE_BUYLIST, list );
	dlg->run();
}

void L2Client::handle_SellList( TradeItemsList *list )
{
	if( !list ) return;
	BuySellDlg *dlg = new BuySellDlg( hWnd, WMMY_UI_BUYSELLDLGREPLY, BuySellDlg::TYPE_SELLLIST, list );
	dlg->run();
}

void L2Client::handle_PrivateStoreManageListSell( TradeItemsList *list1, TradeItemsList *list2 )
{
	PrivateStoreManageDlg *dlg = new PrivateStoreManageDlg( hWnd, WMMY_UI_PRIVATESTOREDLGREPLY,
		PrivateStoreManageDlg::TYPE_SELL, list1, list2 );
	dlg->run();
}

void L2Client::handle_PrivateStoreManageListBuy( TradeItemsList *list1, TradeItemsList *list2 )
{
	PrivateStoreManageDlg *dlg = new PrivateStoreManageDlg( hWnd, WMMY_UI_PRIVATESTOREDLGREPLY,
		PrivateStoreManageDlg::TYPE_BUY, list1, list2 );
	dlg->run();
}

void L2Client::handle_RecipeShopManageList( TradeItemsList *list1, TradeItemsList *list2 )
{
	int dtype = PrivateStoreManageDlg::TYPE_CRAFT;
	if( list1->isCommonCraft ) dtype = PrivateStoreManageDlg::TYPE_COMMONCRAFT;
	PrivateStoreManageDlg *dlg = new PrivateStoreManageDlg( hWnd, WMMY_UI_PRIVATESTOREDLGREPLY, dtype, list1, list2 );
	dlg->run();
}

void L2Client::handle_PrivateStoreBuyList( TradeItemsList *list )
{
	if( !list ) return;
	BuySellDlg *dlg = new BuySellDlg( hWnd, WMMY_UI_BUYSELLDLGREPLY, BuySellDlg::TYPE_PRIVATESTOREBUYLIST, list );
	dlg->run();
}

void L2Client::handle_PrivateStoreSellList( TradeItemsList *list )
{
	if( !list ) return;
	BuySellDlg *dlg = new BuySellDlg( hWnd, WMMY_UI_BUYSELLDLGREPLY, BuySellDlg::TYPE_PRIVATESTORESELLLIST, list );
	dlg->run();
}


// packet senders

// Client: Len    3 [RequestPrivateStoreQuitSell] | 03 00 / 96 / 
void L2Client::send_RequestPrivateStoreQuitSell()
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket p;
	p.setPacketType( 0x96 ); // RequestPrivateStoreQuitSell
	sendPacket( &p, true );
}

// Client: Len    3 [RequestPrivateStoreQuitBuy] | 03 00 / 9C / 
void L2Client::send_RequestPrivateStoreQuitBuy()
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket p;
	p.setPacketType( 0x9C ); // RequestPrivateStoreQuitBuy
	sendPacket( &p, true );
}

// Client: Len    3 [RequestRecipeShopManageQuit] | 03 00 / BC / 
void L2Client::send_RequestRecipeShopManageQuit()
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket p;
	p.setPacketType( 0xBC ); // RequestRecipeShopManageQuit
	sendPacket( &p, true );
}

// Client: Len   11 [SetPrivateStoreMsgBuy] | 0B 00 / 9D / 53 00 53 00  44 00 00 00  
void L2Client::send_SetPrivateStoreMsgBuy( const wchar_t *message )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket p;
	p.setPacketType( 0x9D ); // SetPrivateStoreMsgBuy
	p.writeUnicodeString( message );
	sendPacket( &p, true );
}

// Client: Len   11 [SetPrivateStoreMsgSell] | 0B 00 / 97 / 53 00 53 00  44 00 00 00  
void L2Client::send_SetPrivateStoreMsgSell( const wchar_t *message )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket p;
	p.setPacketType( 0x97 ); // SetPrivateStoreMsgSell
	p.writeUnicodeString( message );
	sendPacket( &p, true );
}

void L2Client::send_SetPrivateStoreWholeMsg( const wchar_t *message )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket p;
	p.setPacketType( 0xD0 ); // extended packet
	p.writeUShort( 0x4D ); // D0:004D SetPrivateStoreMsgSell
	p.writeUnicodeString( message );
	sendPacket( &p, true );
}

void L2Client::send_RequestRecipeShopMessageSet( const wchar_t *message )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket p;
	p.setPacketType( 0xBA ); // RequestRecipeShopMessageSet
	p.writeUnicodeString( message );
	sendPacket( &p, true );
}

void L2Client::send_SetPrivateStoreListSell( TradeItemsList *list )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	if( !list ) return;
	if( list->packageSale ) send_SetPrivateStoreWholeMsg( list->message );
	else send_SetPrivateStoreMsgSell( list->message );
	int i = 0;
	L2GamePacket p;
	p.setPacketType( 0x31 );
	p.writeUInt( list->packageSale );
	p.writeInt( list->itemCount );
	for( i=0; i<list->itemCount; i++ )
	{
		p.writeUInt( list->item[i].objectID );
		if( getL2Version() < L2_VERSION_T23 )
		{
			p.writeD( (int)list->item[i].count );
			p.writeD( (int)list->item[i].price );
		}
		else
		{
			p.writeQ( list->item[i].count );
			p.writeQ( list->item[i].price );
		}
	}
	sendPacket( &p, true );
}

void L2Client::send_SetPrivateStoreListBuy( TradeItemsList *list )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	if( !list ) return;
	send_SetPrivateStoreMsgBuy( list->message );
	int i = 0;
	L2GamePacket p;
	p.setPacketType( 0x9A );
	p.writeInt( list->itemCount );
	for( i=0; i<list->itemCount; i++ )
	{
		p.writeUInt( list->item[i].itemID );
		p.writeUInt( list->item[i].objectID ); // unknown field
		if( getL2Version() < L2_VERSION_T23 )
		{
			p.writeD( (int)list->item[i].count );
			p.writeD( (int)list->item[i].price );
		}
		else
		{
			p.writeQ( list->item[i].count );
			p.writeQ( list->item[i].price );
			// attributes (?) some 16 bytes, L2J ignores
			p.writeH( (short)list->item[i].attributeAtkType );
			p.writeH( (short)list->item[i].attributeAtkValue );
			p.writeH( (short)list->item[i].attributeDefFire );
			p.writeH( (short)list->item[i].attributeDefWater );
			p.writeH( (short)list->item[i].attributeDefWind );
			p.writeH( (short)list->item[i].attributeDefEarth );
			p.writeH( (short)list->item[i].attributeDefHoly );
			p.writeH( (short)list->item[i].attributeDefUnholy );
		}
	}
	sendPacket( &p, true );
}

void L2Client::send_RequestRecipeShopListSet( TradeItemsList *list )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	if( !list ) return;
	send_RequestRecipeShopMessageSet( list->message );
	int i = 0;
	L2GamePacket p;
	p.setPacketType( 0xBB ); // RequestRecipeShopListSet
	p.writeInt( list->itemCount );
	for( i=0; i<list->itemCount; i++ )
	{
		p.writeUInt( list->item[i].itemID ); // recipe ID
		if( getL2Version() < L2_VERSION_T23 ) p.writeD( (int)list->item[i].price );  // cost
		else p.writeQ( list->item[i].price ); // cost
	}
	sendPacket( &p, true );
}

/**     Client: Len   23 [RequestPrivateStoreBuy]
17 00
83             // pcode
CE 09 00 10    // seller player oid
01 00 00 00    // count
    // for each item
D0 09 00 10  // item oid
01 00 00 00  // count
01 00 00 00  // price        **/
void L2Client::send_RequestPrivateStoreBuy( TradeItemsList *list )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	if( !list ) return;
	int i = 0;
	L2GamePacket p;
	p.setPacketType( 0x83 ); // RequestPrivateStoreBuy
	p.writeUInt( list->listID );
	p.writeInt( list->itemCount );
	for( i=0; i<list->itemCount; i++ )
	{
		p.writeUInt( list->item[i].objectID );
		if( getL2Version() < L2_VERSION_T23 )
		{
			p.writeD( (int)list->item[i].count );
			p.writeD( (int)list->item[i].price );
		}
		else
		{
			p.writeQ( list->item[i].count );
			p.writeQ( list->item[i].price );
		}
	}
	sendPacket( &p, true );
}

/**         Client: Len   31 [RequestPrivateStoreSell]
1F 00
9F            // pcode
CE 09 00 10   // seller oid
01 00 00 00    // count

C7 09 00 10   // oid
F4 25 00 00   // iid
00 00 00 00   // ?? 0x00000000
02 00 00 00   // cnt
01 00 00 00   // price     **/
void L2Client::send_RequestPrivateStoreSell( TradeItemsList *list )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	if( !list ) return;
	int i = 0;
	L2GamePacket p;
	p.setPacketType( 0x9F ); // RequestPrivateStoreSell
	p.writeUInt( list->listID );
	p.writeInt( list->itemCount );
	for( i=0; i<list->itemCount; i++ )
	{
		p.writeUInt( list->item[i].objectID );
		p.writeUInt( list->item[i].itemID );
		p.writeUInt( 0x00000000 ); // ??
		if( getL2Version() < L2_VERSION_T23 )
		{
			p.writeD( (int)list->item[i].count );
			p.writeD( (int)list->item[i].price );
		}
		else
		{
			p.writeQ( list->item[i].count );
			p.writeQ( list->item[i].price );
		}
	}
	sendPacket( &p, true );
}
