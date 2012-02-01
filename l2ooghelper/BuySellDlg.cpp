#include "stdafx.h"
#include "Resource.h"
#include "BuySellDlg.h"
#include "logger.h"
#include "windowUtils.h"
#include "DBLayer.h"

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

BuySellDlg::BuySellDlg( HWND hWndNotify, UINT uMessage, int type, TradeItemsList *list )
{
	m_hWndNotify = hWndNotify;
	m_uNotifyMesasage = uMessage;
	m_type = type;
	m_list_src = list;
	m_list_result = new TradeItemsList();
	m_list_result->listID = list->listID; // save listID! important
}

BuySellDlg::~BuySellDlg()
{
	delete m_list_src;
	m_list_src = NULL;
	m_list_result = NULL; // this list is deleted by L2Client reply message receiver
	log_error( LOG_DEBUG, "BuySellDlg::~BuySellDlg()\n" );
}

void BuySellDlg::run()
{
	DWORD dwTID = 0;
	HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0,
		(unsigned int (__stdcall *)(void *))BuySellDlg_Thread, (void *)this,
		0, (unsigned int *)&dwTID );
	if( hThread ) CloseHandle( hThread );
}

DWORD WINAPI BuySellDlg::BuySellDlg_Thread( LPVOID lpParam )
{
	class BuySellDlg *pcls = (BuySellDlg *)lpParam;
	HWND hDlg = CreateDialogParam( g_hInst, MAKEINTRESOURCE(IDD_TRADEITEMS),
		g_hWnd, BuySell_DlgProc, (LPARAM)pcls );
	ShowWindow( hDlg, SW_SHOW );
	MSG msg;
	//while( GetMessage( &msg, hDlg, 0, 0 ) )
	while( GetMessage( &msg, NULL, 0, 0 ) )
	{
		if( !IsDialogMessage( hDlg, &msg ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
	log_error( LOG_DEBUG, "BuySellDlg_Thread(): ended window loop\n" );
	DestroyWindow( hDlg );
	return 0;
}

INT_PTR CALLBACK BuySellDlg::BuySell_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
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

void BuySellDlg::FillListViews( HWND hDlg, BuySellDlg *pcls )
{
	long long int downAdena = 0;
	HWND hwndLV = NULL;
	int i = 0;
	// up LV
	hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
	ListView_DeleteAllItems( hwndLV );
	for( i=0; i<pcls->m_list_src->itemCount; i++ )
	{
		TradeItem it;
		it = pcls->m_list_src->item[i];
		wchar_t wstr[256] = {0};
		DBLayer_getItemName( it.itemID, wstr, 255 );
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
		wsprintfW( wstr, L"%I64d", it.count );
		WULV_SetItem( hwndLV, wstr, added_iid, 2 );
		wsprintfW( wstr, L"%u", it.itemID );
		WULV_SetItem( hwndLV, wstr, added_iid, 3 );
		wsprintfW( wstr, L"%u", it.objectID );
		WULV_SetItem( hwndLV, wstr, added_iid, 4 );
	}
	// down LV
	hwndLV = GetDlgItem( hDlg, IDC_LIST2 );
	ListView_DeleteAllItems( hwndLV );
	for( i=0; i<pcls->m_list_result->itemCount; i++ )
	{
		TradeItem it;
		it = pcls->m_list_result->item[i];
		wchar_t wstr[256] = {0};
		DBLayer_getItemName( it.itemID, wstr, 255 );
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
		wsprintfW( wstr, L"%I64d", it.count );
		WULV_SetItem( hwndLV, wstr, added_iid, 2 );
		wsprintfW( wstr, L"%u", it.itemID );
		WULV_SetItem( hwndLV, wstr, added_iid, 3 );
		wsprintfW( wstr, L"%u", it.objectID );
		WULV_SetItem( hwndLV, wstr, added_iid, 4 );
		//
		downAdena += (it.count * it.price);
	}
	// update adena counters
	WU_SetDlgItemInt64( hDlg, IDC_EDIT1, downAdena, TRUE );
	WU_SetDlgItemInt64( hDlg, IDC_EDIT2, pcls->m_list_src->curMoney, TRUE );
}

void BuySellDlg::OnInitDialog( HWND hDlg, LPARAM lParam )
{
	log_error( LOG_DEBUG, "BuySellDlg::OnInitDialog()\n" );
	// init LV upper
	HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
	WULV_SetExtStyle( hwndLV, true, true );
	WULV_InsertColumn( hwndLV, TEXT("Item"), 150, 0 );
	WULV_InsertColumn( hwndLV, TEXT("$"), 60, 1 );
	WULV_InsertColumn( hwndLV, TEXT("cnt"), 50, 2 );
	WULV_InsertColumn( hwndLV, TEXT("itemID"), 50, 3 );
	WULV_InsertColumn( hwndLV, TEXT("objectID"), 50, 4 );
	// init LV down
	hwndLV = GetDlgItem( hDlg, IDC_LIST2 );
	WULV_SetExtStyle( hwndLV, true, true );
	WULV_InsertColumn( hwndLV, TEXT("Item"), 150, 0 );
	WULV_InsertColumn( hwndLV, TEXT("$"), 60, 1 );
	WULV_InsertColumn( hwndLV, TEXT("cnt"), 50, 2 );
	WULV_InsertColumn( hwndLV, TEXT("itemID"), 50, 3 );
	WULV_InsertColumn( hwndLV, TEXT("objectID"), 50, 4 );
	//
	BuySellDlg *pcls = (BuySellDlg *)lParam;
	//
	switch( pcls->m_type )
	{
	case BuySellDlg::TYPE_BUYLIST:
		{
			SetDlgItemText( hDlg, IDC_ST_UPPER, TEXT("На продаже:") );
			SetDlgItemText( hDlg, IDC_ST_DOWN, TEXT("На покупку:") );
		} break;
	case BuySellDlg::TYPE_SELLLIST:
		{
			SetDlgItemText( hDlg, IDC_ST_UPPER, TEXT("На скупку:") );
			SetDlgItemText( hDlg, IDC_ST_DOWN, TEXT("Вы продаете:") );
		} break;
	case BuySellDlg::TYPE_PRIVATESTOREBUYLIST:
		{
			TCHAR text[256];
			wsprintf( text, TEXT("%s скупает:"), pcls->m_list_src->message );
			SetDlgItemText( hDlg, IDC_ST_UPPER, text );
			SetDlgItemText( hDlg, IDC_ST_DOWN, TEXT("Вы продадите ему:") );
		} break;
	case BuySellDlg::TYPE_PRIVATESTORESELLLIST:
		{
			TCHAR text[256];
			wsprintf( text, TEXT("%s продает:"), pcls->m_list_src->message );
			if( pcls->m_list_src->packageSale )
				wsprintf( text, TEXT("%s продает ОПТОМ(!!!!!!) вы должны купить все:"), pcls->m_list_src->message );
			SetDlgItemText( hDlg, IDC_ST_UPPER, text );
			SetDlgItemText( hDlg, IDC_ST_DOWN, TEXT("Вы купите у него:") );
		} break;
	}
	// UpdateUI
	FillListViews( hDlg, pcls );
}

void BuySellDlg::OnDestroy( HWND hDlg )
{
	log_error( LOG_DEBUG, "BuySellDlg::OnDestroy()\n" );
	BuySellDlg *pcls = (BuySellDlg *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	delete pcls;
}

void BuySellDlg::OnCommand( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam);
	BuySellDlg *pcls = (BuySellDlg *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	switch( LOWORD( wParam ) )
	{
	case IDOK:
		{
			if( pcls->m_list_result->itemCount == 0 )
			{
				log_error( LOG_DEBUG, "BuySellDlg(): OnOk: Did not buy/sell anything!\n" );
				delete pcls->m_list_result;
				pcls->m_list_result = NULL;
			}
			PostMessage( pcls->m_hWndNotify, pcls->m_uNotifyMesasage, (WPARAM)(pcls->m_type),
				(LPARAM)(pcls->m_list_result) );
			PostMessage( hDlg, WM_QUIT, 0, 0 );
		} break;
	case IDCANCEL:
		{
			delete pcls->m_list_result;
			pcls->m_list_result = NULL;
			PostMessage( pcls->m_hWndNotify, pcls->m_uNotifyMesasage, (WPARAM)(pcls->m_type), 0 );
			PostMessage( hDlg, WM_QUIT, 0, 0 );
		} break;
	}
}

// forward decl
// returns 0 if cancelled
long long int BuySellDlg_RequestItemCount( HWND hDlg, TradeItem *it );

void BuySellDlg::OnNotify( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(wParam);
	BuySellDlg *pcls = (BuySellDlg *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
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
			TradeItemsList *tl = pcls->m_list_src;
			TradeItemsList *tl2 = pcls->m_list_result;
			int clicked_iid = lpnmitem->iItem;
			int idx = (int)WULV_GetItemLPARAM( pnmh->hwndFrom, clicked_iid );
			TradeItem it( tl->item[idx] );
			long long int cnt = BuySellDlg_RequestItemCount( hDlg, &it );
			if( cnt > 0 )
			{
				// add to second list (increase)
				int existing_idx = tl2->findItemIdxByItemId( it.itemID );
				if( (existing_idx != -1) && it.isStackableGuess() )
				{
					//log_error( LOG_OK, "Updating existing item idx %d: add count %d\n", existing_idx, cnt );
					tl2->item[existing_idx].count += cnt;
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
				case BuySellDlg::TYPE_BUYLIST: tl->item[idx].count = 0; break;
				case BuySellDlg::TYPE_SELLLIST:
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
			TradeItemsList *tl = pcls->m_list_result;
			TradeItemsList *tl2 = pcls->m_list_src;
			int clicked_iid = lpnmitem->iItem;
			int idx = (int)WULV_GetItemLPARAM( pnmh->hwndFrom, clicked_iid );
			TradeItem it( tl->item[idx] );
			long long int cnt = BuySellDlg_RequestItemCount( hDlg, &it );
			if( cnt > 0 )
			{
				// add to first list (increase)
				int existing_idx = tl2->findItemIdxByItemId( it.itemID );
				if( (existing_idx != -1) && it.isStackableGuess() )
				{
					//log_error( LOG_OK, "Updating existing item in Lv 1 idx %d: add count %d\n", existing_idx, cnt );
					switch( pcls->m_type )
					{
					case BuySellDlg::TYPE_BUYLIST: tl2->item[existing_idx].count = 0; break;
					default: tl2->item[existing_idx].count += cnt; break;
					}
				}
				else
				{
					//log_error( LOG_OK, "Adding new item to Lv 1 to idx %d: count %d\n", tl2->itemCount, cnt );
					it.count = cnt;
					tl2->addItem( it );
				}
				// remove from second list (decrease) TODO: remove on PrivateStoreBuy
				switch( pcls->m_type )
				{
				case BuySellDlg::TYPE_BUYLIST: 
					{
						tl->item[idx].count -= cnt;
						if( tl->item[idx].count <= 0 ) tl->removeCellAndShift( idx );
					} break;
				case BuySellDlg::TYPE_SELLLIST: 
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





INT_PTR CALLBACK BuySellDlg_RequestItemCount_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

// returns 0 if cancelled
long long int BuySellDlg_RequestItemCount( HWND hDlg, TradeItem *it )
{
	long long int save_count = it->count;
	INT_PTR ret = DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_USERINV_ITEMCOUNT),
		hDlg, BuySellDlg_RequestItemCount_Proc, (LPARAM)it );
	if( ret == IDCANCEL )
	{
		it->count = save_count;
		return 0;
	}
	long long int ret_count = it->count;
	it->count = save_count;
	return ret_count;
}

INT_PTR CALLBACK BuySellDlg_RequestItemCount_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			SetWindowLongPtrW( hDlg, GWLP_USERDATA, (LONG_PTR)lParam );
			TradeItem *it = (TradeItem *)lParam;
			wchar_t wstr[256] = {0};
			DBLayer_getItemName( it->itemID, wstr, 255 );
			SetDlgItemTextW( hDlg, IDC_ST_ITEMNAME, wstr );
			WU_SetDlgItemInt64( hDlg, IDC_E_COUNT, it->count, TRUE );
		} break;
	case WM_COMMAND:
		{
			TradeItem *it = (TradeItem *)GetWindowLongPtrW( hDlg, GWLP_USERDATA );
			switch( LOWORD( wParam ) )
			{
			case IDC_B_ALL:
				{
					WU_SetDlgItemInt64( hDlg, IDC_E_COUNT, it->count, TRUE );
				} break;
			case IDOK:
				{
					long long int count = WU_GetDlgItemInt64( hDlg, IDC_E_COUNT, TRUE );
					long long int max_count = it->count;
					if( max_count == 0 ) max_count = 0x00FFFFFF;
					// validate inputted value
					if( (count >= 0) && (count <= max_count) )
					{
						it->count = count; // save inputted value
						EndDialog( hDlg, IDOK );
					}
					else
					{
						TCHAR text[256] = {0};
						wsprintf( text, TEXT("Количество должно быть в пределах: [0..%I64d]"), max_count );
						MessageBox( hDlg, text, TEXT("Ошибка ввода!"), MB_ICONINFORMATION );
					}
				} break;
			case IDCANCEL: EndDialog( hDlg, IDCANCEL ); break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}
