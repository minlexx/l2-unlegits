#include "stdafx.h"
#include "Resource.h"
#include "windowUtils.h"
#include "UserInventory.h"
#include "IL2Client.h"
#include "logger.h"
#include "windowUtils.h"

extern HINSTANCE g_hInst;

INT_PTR CALLBACK UserInvTabDlg_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );


HWND UserInvTabDlg_Create( HWND hWndParent, IL2Client *pcl )
{
	HWND hDlg = CreateDialogParam( g_hInst, MAKEINTRESOURCE(IDD_USERINV),
		hWndParent, UserInvTabDlg_Proc, 0 );
	SetWindowLongPtr( hDlg, GWLP_USERDATA, (LONG_PTR)pcl );
	return hDlg;
}

long long int UserInv_RequestItemCount( HWND hDlg, UserInventoryItem *it );
int           UserInv_RequestDropDir( HWND hDlg );

INT_PTR CALLBACK UserInvTabDlg_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	TCHAR text[128] = {0};
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
			WULV_SetExtStyle( hwndLV, true, true );
			WULV_InsertColumn( hwndLV, TEXT("Предмет"), 160, 0 );
			WULV_InsertColumn( hwndLV, TEXT("к-во"), 40, 1 );
			WULV_InsertColumn( hwndLV, TEXT("info"), 40, 2 );
			WULV_InsertColumn( hwndLV, TEXT("itemID"), 40, 3 );
		} break;
	case WM_USER:
		{
			UserInventory *inv = (UserInventory *)lParam;
			if( !inv ) return TRUE;
			wsprintf( text, TEXT("%d / %d"), inv->itemCount, inv->invSize );
			SetDlgItemText( hDlg, IDC_ST_INVSIZE, text );
			HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
			// save cur sel item
			int lv_cur_sel = ListView_GetNextItem( hwndLV, -1, LVNI_SELECTED );
			ListView_DeleteAllItems( hwndLV );
			int i = 0, iCol = 0;
			int added_idx;
			TCHAR addstr[32] = {0};
			for( i=0; i<USERINV_MAX_ITEMS; i++ )
			{
				if( inv->item[i].objectID == 0 ) continue;
				iCol = 0;
				// name
				inv->item[i].getItemName( text, 128 );
				added_idx = WULV_InsertItem( hwndLV, text, 0, (LPARAM)inv->item[i].objectID );
				if( added_idx == -1 ) continue;
				iCol++;
				// count
				wsprintf( text, TEXT("%I64d"), inv->item[i].count );
				WULV_SetItem( hwndLV, text, added_idx, iCol );
				iCol++;
				// ext (equipped/quest)...
				text[0] = 0;
				if( inv->item[i].enchantLevel > 0 )
				{
					wsprintf( addstr, TEXT("+%d "), inv->item[i].enchantLevel );
					lstrcat( text, addstr );
				}
				if( inv->item[i].isEquipped )
				{
					lstrcpy( addstr, TEXT("v") );
					lstrcat( text, addstr );
				}
				if( inv->item[i].type2 == 3 )
				{
					lstrcpy( addstr, TEXT("Q") );
					lstrcat( text, addstr );
				}
				WULV_SetItem( hwndLV, text, added_idx, iCol );
				iCol++;
				// itemID
				wsprintf( text, TEXT("%d"), inv->item[i].itemID );
				WULV_SetItem( hwndLV, text, added_idx, iCol );
				iCol++;
			}
			// restore cur sel item
			LVITEM lvi;
			lvi.iItem = lv_cur_sel;
			lvi.iSubItem = 0;
			lvi.mask = LVIF_STATE;
			lvi.stateMask = lvi.state = LVIS_SELECTED;
			ListView_SetItem( hwndLV, &lvi );
		} break;
	case WM_COMMAND:
		{
			IL2Client *pcl = (IL2Client *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
			HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
			int cur_sel = ListView_GetNextItem( hwndLV, -1, LVNI_SELECTED );
			unsigned int objectID = 0;
			if( cur_sel >= 0 ) objectID = (unsigned int)WULV_GetItemLPARAM( hwndLV, cur_sel );
			switch( LOWORD( wParam ) )
			{
			case IDC_DESTROY:
				{
					if( cur_sel >= 0 )
					{
						UserInventoryItem it;
						UserInventory *inv = pcl->get_Inventory();
						inv->getItemInfoByObjectId( objectID, &it );
						long long int count = UserInv_RequestItemCount( hDlg, &it );
						if( count >= 1 ) pcl->game_DestroyItem( objectID, count );
					}
				} break;
			case IDC_DROP:
				{
					if( cur_sel >= 0 )
					{
						int drop_dir = UserInv_RequestDropDir( hDlg );
						if( drop_dir > 0 )
						{
							UserInventoryItem it;
							UserInventory *inv = pcl->get_Inventory();
							inv->getItemInfoByObjectId( objectID, &it );
							long long int count = UserInv_RequestItemCount( hDlg, &it );
							if( count >= 1 )
							{
								UserInfo *usr = pcl->get_UserInfo();
								srand( GetTickCount() );
								const int drop_dist = 20 + (rand() % 20);
								int drop_x = usr->x;
								int drop_y = usr->y;
								int drop_z = usr->z;
								switch( drop_dir )
								{
								case 1: { drop_x -= drop_dist; drop_y -= drop_dist; } break;
								case 3: { drop_x += drop_dist; drop_y -= drop_dist; } break;
								case 7: { drop_x -= drop_dist; drop_y += drop_dist; } break;
								case 9: { drop_x += drop_dist; drop_y += drop_dist; } break;
								case 4: { drop_x -= drop_dist; } break;
								case 6: { drop_x += drop_dist; } break;
								case 2: { drop_y -= drop_dist; } break;
								case 8: { drop_y += drop_dist; } break;
								// case 5: no changes :)
								}
								pcl->game_DropItem( objectID, count, drop_x, drop_y, drop_z );
							}
						}
					}
				} break;
			case IDC_CRYSTALLIZE:
				{
					if( cur_sel >= 0 )
					{
						UserInventoryItem it;
						UserInventory *inv = pcl->get_Inventory();
						inv->getItemInfoByObjectId( objectID, &it );
						long long int count = UserInv_RequestItemCount( hDlg, &it );
						if( count >= 1 ) pcl->game_CrystallizeItem( objectID, count );
					}
				} break;
			case IDC_GIVETOPET:
				{
					if( cur_sel >= 0 )
					{
						UserInventoryItem it;
						UserInventory *inv = pcl->get_Inventory();
						inv->getItemInfoByObjectId( objectID, &it );
						long long int count = UserInv_RequestItemCount( hDlg, &it );
						if( count >= 1 ) pcl->game_GiveItemToPet( objectID, count );
					}
				} break;
			}
		} break;
	case WM_NOTIFY:
		{
			IL2Client *pcl = (IL2Client *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
			LPNMHDR phdr = (LPNMHDR)lParam;
			HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
			if( phdr->hwndFrom == hwndLV )
			{
				if( phdr->code == NM_DBLCLK )
				{
					LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
					unsigned int objectID = (unsigned int)WULV_GetItemLPARAM( hwndLV, lpnmitem->iItem );
					if( objectID > 0 )
					{
						//log_error( LOG_OK, "Use item oid %u\n", objectID );
						pcl->game_UseItemByObjectId( objectID );
					}
				}
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

INT_PTR CALLBACK UserInv_RequestItemCount_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

long long int UserInv_RequestItemCount( HWND hDlg, UserInventoryItem *it )
{
	long long int save_count = it->count;
	INT_PTR ret = DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_USERINV_ITEMCOUNT),
		hDlg, UserInv_RequestItemCount_Proc, (LPARAM)it );
	if( ret == IDCANCEL )
	{
		it->count = save_count;
		return 0;
	}
	long long int ret_count = it->count;
	it->count = save_count;
	return ret_count;
}

INT_PTR CALLBACK UserInv_RequestDropDir_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

int UserInv_RequestDropDir( HWND hDlg )
{
	int dir = 0;
	INT_PTR ret = DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_USERINV_DROPITEM),
		hDlg, UserInv_RequestDropDir_Proc, (LPARAM)&dir );
	if( ret == IDCANCEL ) return 0;
	return dir;
}

INT_PTR CALLBACK UserInv_RequestItemCount_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			SetWindowLongPtrW( hDlg, GWLP_USERDATA, (LONG_PTR)lParam );
			UserInventoryItem *it = (UserInventoryItem *)lParam;
			wchar_t itemName[256] = {0};
			it->getItemName( itemName, 256 );
			SetDlgItemTextW( hDlg, IDC_ST_ITEMNAME, itemName );
			WU_SetDlgItemInt64( hDlg, IDC_E_COUNT, it->count, FALSE );
		} break;
	case WM_COMMAND:
		{
			UserInventoryItem *it = (UserInventoryItem *)GetWindowLongPtrW( hDlg, GWLP_USERDATA );
			switch( LOWORD( wParam ) )
			{
			case IDC_B_ALL:
				{
					WU_SetDlgItemInt64( hDlg, IDC_E_COUNT, it->count, FALSE );
				} break;
			case IDOK:
				{
					//it->count = GetDlgItemInt( hDlg, IDC_E_COUNT, NULL, FALSE );
					it->count = WU_GetDlgItemInt64( hDlg, IDC_E_COUNT, FALSE );
					EndDialog( hDlg, IDOK );
				} break;
			case IDCANCEL: EndDialog( hDlg, IDCANCEL ); break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

INT_PTR CALLBACK UserInv_RequestDropDir_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			SetWindowLongPtrW( hDlg, GWLP_USERDATA, (LONG_PTR)lParam );
			int *res = (int *)lParam;
			(*res) = 0;
		} break;
	case WM_COMMAND:
		{
			int *res = (int *)GetWindowLongPtrW( hDlg, GWLP_USERDATA );
			switch( LOWORD( wParam ) )
			{
			case IDC_B_1: { (*res) = 1; EndDialog( hDlg, IDOK ); } break;
			case IDC_B_2: { (*res) = 2; EndDialog( hDlg, IDOK ); } break;
			case IDC_B_3: { (*res) = 3; EndDialog( hDlg, IDOK ); } break;
			case IDC_B_4: { (*res) = 4; EndDialog( hDlg, IDOK ); } break;
			case IDC_B_5: { (*res) = 5; EndDialog( hDlg, IDOK ); } break;
			case IDC_B_6: { (*res) = 6; EndDialog( hDlg, IDOK ); } break;
			case IDC_B_7: { (*res) = 7; EndDialog( hDlg, IDOK ); } break;
			case IDC_B_8: { (*res) = 8; EndDialog( hDlg, IDOK ); } break;
			case IDC_B_9: { (*res) = 9; EndDialog( hDlg, IDOK ); } break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

