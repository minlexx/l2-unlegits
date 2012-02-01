#include "stdafx.h"
#include "TradeP2PDlg.h"
#include "Logger.h"
#include "Resource.h"
#include "windowUtils.h"
#include "L2Client.h"
#include "DBLayer.h"

extern HINSTANCE g_hInst;

// extern
long long int BuySellDlg_RequestItemCount( HWND hDlg, TradeItem *it );

TradeP2PDlg::TradeP2PDlg( void *pvL2Client )
{
	m_pvL2Client = pvL2Client;
	m_hWndParent = NULL;
	m_hDlg = NULL;
	m_ownInv = NULL;
	m_youGive = NULL;
	m_youRecv = NULL;
	m_hWndYourInv = m_hWndYouGive = m_hWndYouRecv = 
		m_hWndStaticOtherPlayerName = NULL;
	m_partnerName[0] = 0;
}

TradeP2PDlg::~TradeP2PDlg()
{
	destroyDialog();
}

BOOL TradeP2PDlg::createDialog( HWND hWndParent )
{
	m_hWndParent = hWndParent;
	m_hDlg = CreateDialogParam( g_hInst, MAKEINTRESOURCE(IDD_TRADEP2P),
		hWndParent, &TradeP2PDlg::TradeP2PDlgProc, (LPARAM)this );
	if( m_hDlg ) return TRUE;
	// error!
	m_hWndParent = NULL;
	return FALSE;
}

void TradeP2PDlg::destroyDialog()
{
	clearContents();
	if( m_hDlg )
	{
		DestroyWindow( m_hDlg );
		m_hDlg = NULL;
	}
}

void TradeP2PDlg::showWindow( bool bShow )
{
	if( m_hDlg )
	{
		if( bShow ) ShowWindow( m_hDlg, SW_SHOWNORMAL );
		else ShowWindow( m_hDlg, SW_HIDE );
	}
}

const wchar_t *TradeP2PDlg::getPartnerName() const
{
	return (const wchar_t *)m_partnerName;
}

void TradeP2PDlg::clearContents()
{
	// clear lists
	if( m_ownInv )
	{
		delete m_ownInv;
		m_ownInv = NULL;
	}
	if( m_youGive )
	{
		delete m_youGive;
		m_youGive = NULL;
	}
	if( m_youRecv )
	{
		delete m_youRecv;
		m_youRecv = NULL;
	}
	// clear partner name
	m_partnerName[0] = 0;
}

void TradeP2PDlg::tradeStart( const TradeItemsList *ownInv, const wchar_t *partnerName )
{
	clearContents();
	// create lists
	m_ownInv = new TradeItemsList();
	m_youGive = new TradeItemsList();
	m_youRecv = new TradeItemsList();
	// copy *ourInv trade list to our inv
	int i;
	for( i=0; i<ownInv->itemCount; i++ )
	{
		TradeItem it = ownInv->item[i];
		m_ownInv->addItem( it );
	}
	// save partner name
	memset( m_partnerName, 0, sizeof(m_partnerName) );
	wcsncpy( m_partnerName, partnerName, 255 );
	m_partnerName[255] = 0;
	// update window
	showWindow();
	wnd_updateLists( this );
}

void TradeP2PDlg::tradeOwnAdd( const TradeItem& it )
{
	m_youGive->addItem( it );
	TradeP2PDlg::wnd_updateLists( this );
}

void TradeP2PDlg::tradeOtherAdd( const TradeItem& it )
{
	m_youRecv->addItem( it );
	TradeP2PDlg::wnd_updateLists( this );
}

void TradeP2PDlg::tradeDone()
{
	clearContents();
	showWindow( false );
}








/*
  CONTROL "",IDC_LV_INV,"SysListView32",...
  CONTROL "",IDC_LV_SEND,"SysListView32",...
  CONTROL "",IDC_LV_RECV,"SysListView32",...
  CONTROL "OK",IDOK,"Button",WS_CHILD|WS_VISIBLE|WS_TABSTOP,8,324,92,24
  CONTROL "Отмена",IDCANCEL,"Button",WS_CHILD|WS_VISIBLE|WS_TABSTOP,146,324,92,24
  CONTROL "Ваш нвентарь:",IDC_STATIC,"Static",WS_CHILD|WS_VISIBLE,4,1,238,9
  CONTROL "Вы отдаете:",IDC_STATIC,"Static",WS_CHILD|WS_VISIBLE,6,121,242,9
  CONTROL "%username% дает вам:",IDC_ST_TRADECHAR,"Static",WS_CHILD|WS_VISIBLE,6,219,242,9
*/

INT_PTR CALLBACK TradeP2PDlg::TradeP2PDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			// store pointer class as window userdata
			class TradeP2PDlg *pcls = (class TradeP2PDlg *)lParam;
			SetWindowLongPtr( hDlg, GWLP_USERDATA, (LONG_PTR)lParam );
			// get handles to child windows
			pcls->m_hWndYourInv = GetDlgItem( hDlg, IDC_LV_INV );
			pcls->m_hWndYouGive = GetDlgItem( hDlg, IDC_LV_SEND );
			pcls->m_hWndYouRecv = GetDlgItem( hDlg, IDC_LV_RECV );
			pcls->m_hWndStaticOtherPlayerName = GetDlgItem( hDlg, IDC_ST_TRADECHAR );
			// initialize child list-views
			// initialize your inv LV
			WULV_SetExtStyle( pcls->m_hWndYourInv, true, true );
			WULV_InsertColumn( pcls->m_hWndYourInv, TEXT("Предмет"), 150, 0 );
			WULV_InsertColumn( pcls->m_hWndYourInv, TEXT("к-во"), 50, 1 );
			// initialize you give LV
			WULV_SetExtStyle( pcls->m_hWndYouGive, true, true );
			WULV_InsertColumn( pcls->m_hWndYouGive, TEXT("Предмет"), 150, 0 );
			WULV_InsertColumn( pcls->m_hWndYouGive, TEXT("к-во"), 50, 1 );
			// initialize you recv LV
			WULV_SetExtStyle( pcls->m_hWndYouRecv, true, true );
			WULV_InsertColumn( pcls->m_hWndYouRecv, TEXT("Предмет"), 150, 0 );
			WULV_InsertColumn( pcls->m_hWndYouRecv, TEXT("к-во"), 50, 1 );
			// last, update lists contents
			pcls->wnd_updateLists( pcls );
		} break;
	case WM_COMMAND:
		{
			class TradeP2PDlg *pcls = (class TradeP2PDlg *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
			switch( LOWORD(wParam) )
			{
			case IDOK: pcls->wnd_OnOK( pcls ); break;
			case IDCANCEL: pcls->wnd_OnCancel( pcls ); break;
			}
		} break;
	case WM_NOTIFY:
		{
			class TradeP2PDlg *pcls = (class TradeP2PDlg *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
			LPNMHDR pnmh = (LPNMHDR)lParam;
			if( pnmh->code == NM_DBLCLK )
			{
				if( pnmh->hwndFrom == pcls->m_hWndYourInv )
				{
					// The iItem, iSubItem, and ptAction members of this structure contain information about the item.
					LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
					wnd_OnInvDblClick( pcls, lpnmitem->iItem );
				}
			}
		} break;
	case WM_KEYDOWN:
		{
			log_error( LOG_ERROR, "TradeP2PDlgProc(): WM_KEYDOWN\n" );
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}







void TradeP2PDlg::wnd_fill_LV_from_list( HWND hWndLV, TradeItemsList *list )
{
	TCHAR text[256] = {0};
	int i = 0;
	ListView_DeleteAllItems( hWndLV );
	if( !list ) return;
	for( i=0; i<list->itemCount; i++ )
	{
		// insert item name
		TCHAR itemName[256] = {0};
		DBLayer_getItemName( list->item[i].itemID, itemName, 256 );
		if( list->item[i].enchantLevel > 0 )
			wsprintf( text, TEXT("+%d %s"), list->item[i].enchantLevel, itemName );
		else
			wcscpy( text, itemName );
		// item lParam... should be item index in array, i think
		int added_item = WULV_InsertItem( hWndLV, text, 999, i );
		if( added_item == -1 ) continue;
		// insert item count
		wsprintf( text, TEXT("%I64d"), list->item[i].count );
		WULV_SetItem( hWndLV, text, added_item, 1 );
	}
}

void TradeP2PDlg::wnd_updateLists( TradeP2PDlg *pcls )
{
	wnd_fill_LV_from_list( pcls->m_hWndYourInv, pcls->m_ownInv );
	wnd_fill_LV_from_list( pcls->m_hWndYouGive, pcls->m_youGive );
	wnd_fill_LV_from_list( pcls->m_hWndYouRecv, pcls->m_youRecv );
	// update partner name
	TCHAR wndText[256] = {0};
	wsprintf( wndText, TEXT("Partner: %s"), pcls->m_partnerName );
	SetWindowText( pcls->m_hWndStaticOtherPlayerName, wndText );
}

void TradeP2PDlg::wnd_OnOK( TradeP2PDlg *pcls )
{
	class L2Client *pcl = (class L2Client *)pcls->m_pvL2Client;
	// confirm trade
	pcl->send_TradeDone( 1 );
}

void TradeP2PDlg::wnd_OnCancel( TradeP2PDlg *pcls )
{
	class L2Client *pcl = (class L2Client *)pcls->m_pvL2Client;
	// cancel trade
	pcl->send_TradeDone( 0 );
}

void TradeP2PDlg::wnd_OnInvDblClick( TradeP2PDlg *pcls, int iItem )
{
	class L2Client *pcl = (class L2Client *)pcls->m_pvL2Client;
	// get TradeItem for clicked item
	int idx = WULV_GetItemLPARAM( pcls->m_hWndYourInv, iItem );
	if( idx < 0  ||  idx >= pcls->m_ownInv->itemCount ) return;
	TradeItem it = pcls->m_ownInv->item[idx];
	// request count to give to other player
	long long move_count = BuySellDlg_RequestItemCount( pcls->m_hDlg, &it );
	if( move_count == 0 ) return; // give cancelled
	// reduce own item count
	pcls->m_ownInv->item[idx].count -= move_count;
	// if own item count zeroed, remove whole cell
	if( pcls->m_ownInv->item[idx].count <= 0 )
		pcls->m_ownInv->removeCellAndShift( idx );
	// send AddTradeItem
	pcl->send_AddTradeItem(
		pcls->m_ownInv->listID, // 0/*tradeID*/, //trade ID - possible partner OID?
		it.objectID,
		move_count );
	// update lists to display reduced own item count (or whole removed item)
	TradeP2PDlg::wnd_updateLists( pcls );
}


