#include "stdafx.h"
#include "Resource.h"
#include "PrivateStoreDlg.h"
#include "logger.h"
#include "windowUtils.h"
#include "InputStringDlg.h"
#include "RecipeData.h"
#include "DBLayer.h"

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

PrivateStoreManageDlg::PrivateStoreManageDlg( HWND hWndNotify, UINT uMessage, int type, TradeItemsList *list1, TradeItemsList *list2 )
{
	m_hWndNotify = hWndNotify;
	m_uNotifyMesasage = uMessage;
	m_type = type;
	m_list1 = list1;
	m_list2 = list2;
}

PrivateStoreManageDlg::~PrivateStoreManageDlg()
{
	delete m_list1;
	m_list1 = NULL;
	m_list2 = NULL; // this list is deleted by L2Client reply message receiver
	//log_error( LOG_OK, "PrivateStoreManageDlg::~PrivateStoreManageDlg()\n" );
}

void PrivateStoreManageDlg::run()
{
	DWORD dwTID = 0;
	HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0,
		(unsigned int (__stdcall *)(void *))Dlg_Thread, (void *)this,
		0, (unsigned int *)&dwTID );
	if( hThread ) CloseHandle( hThread );
}

DWORD WINAPI PrivateStoreManageDlg::Dlg_Thread( LPVOID lpParam )
{
	class PrivateStoreManageDlg *pcls = (PrivateStoreManageDlg *)lpParam;
	HWND hDlg = CreateDialogParam( g_hInst, MAKEINTRESOURCE(IDD_PRIVATETRADEITEMS),
		g_hWnd, Dlg_Proc, (LPARAM)pcls );
	ShowWindow( hDlg, SW_SHOW );
	MSG msg;
	while( GetMessage( &msg, NULL, 0, 0 ) )
	{
		if( !IsDialogMessage( hDlg, &msg ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
	//log_error( LOG_OK, "PrivateStoreManageDlg::Dlg_Thread(): ended window loop\n" );
	DestroyWindow( hDlg );
	return 0;
}

INT_PTR CALLBACK PrivateStoreManageDlg::Dlg_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			SetWindowLongPtr( hDlg, GWLP_USERDATA, (LONG_PTR)lParam );
			OnInitDialog( hDlg, lParam );
		} break;
	case WM_COMMAND: OnCommand( hDlg, wParam, lParam ); break;
	case WM_NOTIFY: OnNotify( hDlg, wParam, lParam ); break;
	case WM_DESTROY: { OnDestroy( hDlg ); return FALSE; } break;
	default: return FALSE; break;
	}
	return TRUE;
}

void PrivateStoreManageDlg::FillListViews( HWND hDlg, PrivateStoreManageDlg *pcls )
{
	long long int downAdena = 0;
	HWND hwndLV = NULL;
	int i = 0;
	// up LV
	hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
	ListView_DeleteAllItems( hwndLV );
	for( i=0; i<pcls->m_list1->itemCount; i++ )
	{
		TradeItem it;
		it = pcls->m_list1->item[i];
		wchar_t wstr[256] = {0};
		// get item name
		if( (pcls->m_type == TYPE_BUY) || (pcls->m_type == TYPE_SELL) )
		{
			DBLayer_getItemName( it.itemID, wstr, 255 );
		}
		else if( (pcls->m_type == TYPE_CRAFT) || (pcls->m_type == TYPE_COMMONCRAFT) )
		{
			int iid = 0;
			L2Data_RecipeInfo recInfo;
			if( L2Data_GetRecipeInfoForRecID( it.itemID, &recInfo ) ) iid = recInfo.rec_itemID;
			DBLayer_getItemName( iid, wstr, 255 );
		}
		int added_iid = 0;
		if( it.enchantLevel == 0 )
		{
			added_iid = WULV_InsertItem( hwndLV, wstr, 0, i ); // lParam is just index
		}
		else
		{
			wchar_t descr[256] = {0};
			swprintf( descr, 255, L"+%d %s", it.enchantLevel, wstr );
			added_iid = WULV_InsertItem( hwndLV, descr, 0, i ); // lParam is just index
		}
		if( added_iid == -1 ) continue;
		wsprintfW( wstr, L"%I64d", it.price );
		WULV_SetItem( hwndLV, wstr, added_iid, 1 );
		wsprintfW( wstr, L"%I64d", it.storePrice );
		WULV_SetItem( hwndLV, wstr, added_iid, 2 );
		wsprintfW( wstr, L"%I64d", it.count );
		WULV_SetItem( hwndLV, wstr, added_iid, 3 );
		wsprintfW( wstr, L"%u", it.itemID );
		WULV_SetItem( hwndLV, wstr, added_iid, 4 );
		wsprintfW( wstr, L"%u", it.objectID );
		WULV_SetItem( hwndLV, wstr, added_iid, 5 );
	}
	// down LV
	hwndLV = GetDlgItem( hDlg, IDC_LIST2 );
	ListView_DeleteAllItems( hwndLV );
	for( i=0; i<pcls->m_list2->itemCount; i++ )
	{
		TradeItem it;
		it = pcls->m_list2->item[i];
		wchar_t wstr[256] = {0};
		// get item name
		if( (pcls->m_type == TYPE_BUY) || (pcls->m_type == TYPE_SELL) )
		{
			DBLayer_getItemName( it.itemID, wstr, 256 );
		}
		else if( (pcls->m_type == TYPE_CRAFT) || (pcls->m_type == TYPE_COMMONCRAFT) )
		{
			int iid = 0;
			L2Data_RecipeInfo recInfo;
			if( L2Data_GetRecipeInfoForRecID( it.itemID, &recInfo ) ) iid = recInfo.rec_itemID;
			DBLayer_getItemName( iid, wstr, 256 );
		}
		int added_iid = 0;
		if( it.enchantLevel == 0 )
		{
			added_iid = WULV_InsertItem( hwndLV, wstr, 0, i ); // lParam is just index
		}
		else
		{
			wchar_t descr[256] = {0};
			swprintf( descr, 255, L"+%d %s", it.enchantLevel, wstr );
			added_iid = WULV_InsertItem( hwndLV, descr, 0, i ); // lParam is just index
		}
		if( added_iid == -1 ) continue;
		wsprintfW( wstr, L"%I64d", it.price );
		WULV_SetItem( hwndLV, wstr, added_iid, 1 );
		wsprintfW( wstr, L"%I64d", it.storePrice );
		WULV_SetItem( hwndLV, wstr, added_iid, 2 );
		wsprintfW( wstr, L"%I64d", it.count );
		WULV_SetItem( hwndLV, wstr, added_iid, 3 );
		wsprintfW( wstr, L"%u", it.itemID );
		WULV_SetItem( hwndLV, wstr, added_iid, 4 );
		wsprintfW( wstr, L"%u", it.objectID );
		WULV_SetItem( hwndLV, wstr, added_iid, 5 );
		//
		downAdena += (it.count * it.price);
	}
	// update adena counters
	WU_SetDlgItemInt64( hDlg, IDC_EDIT1, downAdena, TRUE );
	WU_SetDlgItemInt64( hDlg, IDC_EDIT2, pcls->m_list1->curMoney, TRUE );
}

void PrivateStoreManageDlg::OnInitDialog( HWND hDlg, LPARAM lParam )
{
	//log_error( LOG_OK, "PrivateStoreManageDlg::OnInitDialog()\n" );
	// init LV upper
	HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
	WULV_SetExtStyle( hwndLV, true, true );
	WULV_InsertColumn( hwndLV, TEXT("Item"), 150, 0 );
	WULV_InsertColumn( hwndLV, TEXT("$"), 60, 1 );
	WULV_InsertColumn( hwndLV, TEXT("$orig"), 60, 2 );
	WULV_InsertColumn( hwndLV, TEXT("cnt"), 50, 3 );
	WULV_InsertColumn( hwndLV, TEXT("itemID"), 50, 4 );
	WULV_InsertColumn( hwndLV, TEXT("objectID"), 50, 5 );
	// init LV down
	hwndLV = GetDlgItem( hDlg, IDC_LIST2 );
	WULV_SetExtStyle( hwndLV, true, true );
	WULV_InsertColumn( hwndLV, TEXT("Item"), 150, 0 );
	WULV_InsertColumn( hwndLV, TEXT("$"), 60, 1 );
	WULV_InsertColumn( hwndLV, TEXT("$orig"), 60, 2 );
	WULV_InsertColumn( hwndLV, TEXT("cnt"), 50, 3 );
	WULV_InsertColumn( hwndLV, TEXT("itemID"), 50, 4 );
	WULV_InsertColumn( hwndLV, TEXT("objectID"), 50, 5 );
	//
	PrivateStoreManageDlg *pcls = (PrivateStoreManageDlg *)lParam;
	//
	switch( pcls->m_type )
	{
	case PrivateStoreManageDlg::TYPE_BUY:
		{
			SetDlgItemText( hDlg, IDC_ST_UPPER, TEXT("Инвентарь:") );
			SetDlgItemText( hDlg, IDC_ST_DOWN, TEXT("Выставить на покупку:") );
		} break;
	case PrivateStoreManageDlg::TYPE_SELL:
		{
			SetDlgItemText( hDlg, IDC_ST_UPPER, TEXT("Инвентарь:") );
			SetDlgItemText( hDlg, IDC_ST_DOWN, TEXT("Выставить на продажу:") );
		} break;
	}
	// UpdateUI
	FillListViews( hDlg, pcls );
}

void PrivateStoreManageDlg::OnDestroy( HWND hDlg )
{
	//log_error( LOG_DEBUG, "PrivateStoreManageDlg::OnDestroy()\n" );
	PrivateStoreManageDlg *pcls = (PrivateStoreManageDlg *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	delete pcls;
}

void PrivateStoreManageDlg::OnCommand( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam);
	PrivateStoreManageDlg *pcls = (PrivateStoreManageDlg *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	switch( LOWORD( wParam ) )
	{
	case IDOK:
		{
			if( pcls->m_list2->itemCount == 0 )
			{
				log_error( LOG_DEBUG, "PrivateStoreManageDlg(): OnOk: Did not set to buy/sell anything!\n" );
				delete pcls->m_list2;
				pcls->m_list2 = NULL;
			}
			PostMessage( pcls->m_hWndNotify, pcls->m_uNotifyMesasage, (WPARAM)(pcls->m_type), (LPARAM)(pcls->m_list2) );
			PostMessage( hDlg, WM_QUIT, 0, 0 );
		} break;
	case IDCANCEL:
		{
			delete pcls->m_list2;
			pcls->m_list2 = NULL;
			PostMessage( pcls->m_hWndNotify, pcls->m_uNotifyMesasage, (WPARAM)(pcls->m_type), 0 );
			PostMessage( hDlg, WM_QUIT, 0, 0 );
		} break;
	case IDC_B_MSGSELL:
		{
			InputStringDlg dlg( TEXT("Соообщение"), TEXT("Введите сообщение:"), pcls->m_list2->message );
			if( dlg.run( hDlg ) ) wcscpy( pcls->m_list2->message, dlg.text );
		} break;
	}
}

// forward decl
// returns 0 if cancelled
extern long long int BuySellDlg_RequestItemCount( HWND hDlg, TradeItem *it );

void PrivateStoreManageDlg::OnNotify( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(wParam);
	PrivateStoreManageDlg *pcls = (PrivateStoreManageDlg *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	LPNMHDR pnmh = (LPNMHDR)lParam;
	HWND hwndLV1 = GetDlgItem( hDlg, IDC_LIST1 );
	HWND hwndLV2 = GetDlgItem( hDlg, IDC_LIST2 );
	if( pnmh->code == NM_DBLCLK )
	{
		// The iItem, iSubItem, and ptAction members of this structure contain information about the item.
		LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
		if( lpnmitem->iSubItem != 0 ) return;
		if( pnmh->hwndFrom == hwndLV1 )
		{
			TradeItemsList *tl = pcls->m_list1;
			TradeItemsList *tl2 = pcls->m_list2;
			int clicked_iid = lpnmitem->iItem;
			int idx = (int)WULV_GetItemLPARAM( pnmh->hwndFrom, clicked_iid );
			TradeItem it( tl->item[idx] );
			// request price
			wchar_t str_price[32] = {0};
			swprintf( str_price, 31, L"%d", it.storePrice );
			InputStringDlg dlg( TEXT("Цена"), TEXT("Введите цену:"), str_price );
			if( dlg.run( hDlg ) ) _stscanf( dlg.text, _T("%d"), &(it.price) ); else it.price = 0;
			// request count
			if( pcls->m_type == PrivateStoreManageDlg::TYPE_BUY ) it.count = 0;
			long long int cnt = 1;
			if( pcls->m_type == TYPE_BUY ) cnt = BuySellDlg_RequestItemCount( hDlg, &it );
			if( pcls->m_type == TYPE_SELL ) cnt = BuySellDlg_RequestItemCount( hDlg, &it );
			/*if( pcls->m_type == TYPE_CRAFT ) // do not request count for recipe shop :)
			{
				unsigned int save_iid = it.itemID;
				RecipeInfo recInfo;
				if( GetRecipeInfoForRecID( it.itemID, &recInfo ) ) it.itemID = recInfo.rec_itemID;
				cnt = BuySellDlg_RequestItemCount( hDlg, &it );
				it.itemID = save_iid;
			}*/
			if( cnt > 0 )
			{
				// add to second list (increase)
				int existing_idx = tl2->findItemIdxByItemId( it.itemID );
				if( (existing_idx != -1) && it.isStackableGuess() )
				{
					//log_error( LOG_OK, "Updating existing item idx %d: add count %d\n", existing_idx, cnt );
					tl2->item[existing_idx].count += cnt;
					tl2->item[existing_idx].price = it.price;
				}
				else
				{
					//log_error( LOG_OK, "Adding new item to idx %d: count %d\n", tl2->itemCount, cnt );
					it.count = cnt;
					tl2->addItem( it );
				}
				// update first list
				switch( pcls->m_type )
				{
				case PrivateStoreManageDlg::TYPE_BUY:
					{
						tl->item[idx].count -= cnt;
						if( tl->item[idx].count <= 0 ) tl->removeCellAndShift( idx );
					} break;
				case PrivateStoreManageDlg::TYPE_SELL:
					{
						tl->item[idx].count -= cnt;
						if( tl->item[idx].count <= 0 ) tl->removeCellAndShift( idx );
					} break;
				default:
					{
						tl->item[idx].count -= cnt;
						if( tl->item[idx].count <= 0 ) tl->removeCellAndShift( idx );
					} break;
				}
			}
		}
		if( pnmh->hwndFrom == hwndLV2 )
		{
			TradeItemsList *tl = pcls->m_list2;
			TradeItemsList *tl2 = pcls->m_list1;
			int clicked_iid = lpnmitem->iItem;
			int idx = (int)WULV_GetItemLPARAM( pnmh->hwndFrom, clicked_iid );
			TradeItem it( tl->item[idx] );
			// request count
			long long int cnt = 1;
			if( pcls->m_type == TYPE_BUY ) cnt = BuySellDlg_RequestItemCount( hDlg, &it );
			if( pcls->m_type == TYPE_SELL ) cnt = BuySellDlg_RequestItemCount( hDlg, &it );
			if( cnt > 0 )
			{
				// add to first list (increase)
				int existing_idx = tl2->findItemIdxByItemId( it.itemID );
				if( (existing_idx != -1) && it.isStackableGuess() )
				{
					//log_error( LOG_OK, "Updating existing item in Lv 1 idx %d: add count %d\n", existing_idx, cnt );
					switch( pcls->m_type )
					{
					case PrivateStoreManageDlg::TYPE_BUY:
						{
							tl2->item[existing_idx].count += cnt;
							tl2->item[existing_idx].price = 0;
						} break;
					case PrivateStoreManageDlg::TYPE_SELL:
						{
							tl2->item[existing_idx].count += cnt;
							tl2->item[existing_idx].price = 0;
						} break;
					default:
						{
							tl2->item[existing_idx].count += cnt;
							tl2->item[existing_idx].price = 0;
						} break;
					}
				}
				else
				{
					//log_error( LOG_OK, "Adding new item to Lv 1 to idx %d: count %d\n", tl2->itemCount, cnt );
					it.count = cnt;
					it.price = 0;
					tl2->addItem( it );
				}
				// remove from second list (decrease) TODO: remove on PrivateStoreBuy
				switch( pcls->m_type )
				{
				case PrivateStoreManageDlg::TYPE_BUY: 
					{
						tl->item[idx].count -= cnt;
						if( tl->item[idx].count <= 0 ) tl->removeCellAndShift( idx );
					} break;
				case PrivateStoreManageDlg::TYPE_SELL: 
					{
						tl->item[idx].count -= cnt;
						if( tl->item[idx].count <= 0 ) tl->removeCellAndShift( idx );
					} break;
				default:
					{
						tl->item[idx].count -= cnt;
						if( tl->item[idx].count <= 0 ) tl->removeCellAndShift( idx );
					} break;
				}
			}
		}
		// update list-views contents
		FillListViews( hDlg, pcls );
	} // NM_DBLCLICK
}



