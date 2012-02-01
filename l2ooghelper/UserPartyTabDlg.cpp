#include "stdafx.h"
#include "Resource.h"
#include "windowUtils.h"
#include "UserParty.h"
#include "UserPartyTabDlg.h"
#include "IL2Client.h"

extern HINSTANCE g_hInst;

INT_PTR CALLBACK UserPartyTabDlg_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

HWND UserPartyTabDlg_Create( HWND hWndParent, IL2Client *pcl )
{
	HWND hDlg = CreateDialogParam( g_hInst, MAKEINTRESOURCE(IDD_USERPARTY),
		hWndParent, UserPartyTabDlg_Proc, (LPARAM)pcl );
	return hDlg;
}

void UserPartyTabDlg_SetPartyPlayerBuffs( HWND hwndLV, UserBuffs *buffs, int iItem, int iSubItem );

INT_PTR CALLBACK UserPartyTabDlg_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	TCHAR text[256] = {0};
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			SetWindowLongPtr( hDlg, GWLP_USERDATA, (LONG_PTR)lParam );
			HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
			WULV_SetExtStyle( hwndLV, true, true );
			WULV_InsertColumn( hwndLV, TEXT("Имя"), 80, 0 );
			WULV_InsertColumn( hwndLV, TEXT("Lvl"), 30, 1 );
			WULV_InsertColumn( hwndLV, TEXT("Класс"), 70, 2 );
			WULV_InsertColumn( hwndLV, TEXT("HP"), 55, 3 );
			WULV_InsertColumn( hwndLV, TEXT("MP"), 55, 4 );
			WULV_InsertColumn( hwndLV, TEXT("CP"), 55, 5 );
			WULV_InsertColumn( hwndLV, TEXT("баффы..."), 550, 6 );
		} break;
	case WM_COMMAND:
		{
			IL2Client *pcl = (IL2Client *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
			switch( LOWORD( wParam ) )
			{
			case IDC_LEAVEPARTY:
				{
					UserParty *party = pcl->get_Party();
					if( party->getCount() > 0 )
					{
						pcl->game_Party_Leave();
					}
					else
					{
						MessageBox( hDlg, TEXT("Кажись вы не в пати!"), TEXT("Эээ"), MB_ICONWARNING );
					}
				} break;
			case IDC_KICKPARTYMEMBER:
				{
					UserParty *party = pcl->get_Party();
					if( party->getCount() > 0 )
					{
						wchar_t playerName[256] = {0};
						HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
						int cur_sel = ListView_GetNextItem( hwndLV, -1, LVNI_SELECTED );
						if( cur_sel >= 0 )
						{
							WULV_GetItemText( hwndLV, cur_sel, 0, playerName, 255 );
							int yes = MessageBox( hDlg, playerName,
								TEXT("Вы хотите кикнуть чара?"), MB_ICONINFORMATION | MB_YESNO );
							if( yes == IDYES ) pcl->game_Party_KickPlayer( playerName );
						}
					}
					else
					{
						MessageBox( hDlg, TEXT("Кажись вы не в пати!"), TEXT("Эээ"), MB_ICONWARNING );
					}
				} break;
			case IDC_CHANGEPARTYLEADER:
				{
					UserParty *party = pcl->get_Party();
					if( party->getCount() > 0 )
					{
						wchar_t playerName[256] = {0};
						HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
						int cur_sel = ListView_GetNextItem( hwndLV, -1, LVNI_SELECTED );
						if( cur_sel >= 0 )
						{
							WULV_GetItemText( hwndLV, cur_sel, 0, playerName, 255 );
							int yes = MessageBox( hDlg, playerName,
								TEXT("Вы хотите передать корону?"), MB_ICONINFORMATION | MB_YESNO );
							if( yes == IDYES ) pcl->game_Party_ChangeLeader( playerName );
						}
					}
					else
					{
						MessageBox( hDlg, TEXT("Кажись вы не в пати!"), TEXT("Эээ"), MB_ICONWARNING );
					}
				} break;
			}
		} break;
	case WM_NOTIFY:
		{
			LPNMHDR phdr = (LPNMHDR)lParam;
			HWND hWndTab = GetDlgItem( hDlg, IDC_TAB2 );
			HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
			IL2Client *pcl = (IL2Client *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
			if( phdr->hwndFrom == hWndTab )
			{
				int cur_sel = TabCtrl_GetCurSel( hWndTab );
				HWND hwndLVA = GetDlgItem( hWndTab, 100 );
				HWND hwndLVP = GetDlgItem( hWndTab, 101 );
				switch( phdr->code )
				{
				case TCN_SELCHANGING:
					{
						if( cur_sel == 0 ) ShowWindow( hwndLVA, SW_HIDE );
						if( cur_sel == 1 ) ShowWindow( hwndLVP, SW_HIDE );
					} break;
				case TCN_SELCHANGE:
					{
						if( cur_sel == 0 ) ShowWindow( hwndLVA, SW_SHOW );
						if( cur_sel == 1 ) ShowWindow( hwndLVP, SW_SHOW );
					} break;
				}
			}
			if( phdr->hwndFrom == hwndLV )
			{
				if( phdr->code == NM_DBLCLK )
				{
					LPNMITEMACTIVATE lpnmia = (LPNMITEMACTIVATE)lParam;
					unsigned int objectID = (unsigned int)WULV_GetItemLPARAM( hwndLV, lpnmia->iItem );
					if( objectID > 0 ) pcl->game_Action( objectID );
				}
			}
		} break;
	case WM_USER:
		{
			IL2Client *pcl = (IL2Client *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
			UserParty *pty = (UserParty *)lParam;
			if( !pty ) return TRUE;
			HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
			int i = 0;
			ListView_DeleteAllItems( hwndLV );
			int added_item = -1;
			int ipercent = 0;
			for( i=0; i<pty->getCount(); i++ )
			{
				L2Player *member = pty->getPartyPlayer( i );
				if( !member ) continue;
				// name (lParam - objectID)
				added_item = WULV_InsertItem( hwndLV, member->charName, 0, member->objectID );
				// lvl
				wsprintf( text, TEXT("%d"), member->level );
				WULV_SetItem( hwndLV, text, added_item, 1 );
				// class
				wsprintf( text, TEXT("%S"), L2Data_getClass( member->classID ) );
				WULV_SetItem( hwndLV, text, added_item, 2 );
				// HP
				ipercent = (int)( 100.0 * (member->curHp / member->maxHp) );
				swprintf( text, 255, L"%0.0f/%0.0f (%d%%)", member->curHp, member->maxHp, ipercent );
				WULV_SetItem( hwndLV, text, added_item, 3 );
				// MP
				ipercent = (int)( 100.0 * (member->curMp / member->maxMp) );
				swprintf( text, 255, L"%0.0f/%0.0f (%d%%)", member->curMp, member->maxMp, ipercent );
				WULV_SetItem( hwndLV, text, added_item, 4 );
				// CP
				ipercent = (int)( 100.0 * (member->curCp / member->maxCp) );
				swprintf( text, 255, L"%0.0f/%0.0f (%d%%)", member->curCp, member->maxCp, ipercent );
				WULV_SetItem( hwndLV, text, added_item, 5 );
				// buffs
				UserBuffs *buffs = pty->getPartyPlayerBuffs( i );
				if( !buffs ) continue;
				UserPartyTabDlg_SetPartyPlayerBuffs( hwndLV, buffs, added_item, 6 );
			}
			// leader and loot rights
			unsigned int leaderoid = pty->getLeaderObjectId();
			unsigned int useroid = pcl->get_UserInfo()->objectID;
			unsigned int lootType = pty->getItemDistribution();
			int idx = -1;
			if( (leaderoid == useroid) || (leaderoid == 0) )
			{
				wsprintfW( text, L"Лидер вы: %s", pcl->get_UserInfo()->charName );
				SetDlgItemTextW( hDlg, IDC_ST_PARTYLEADER, text );
			}
			else if( pty->isInParty( leaderoid, &idx ) )
			{
				if( idx >=0 )
				{
					L2Player *leader = pty->getPartyPlayer( idx );
					wsprintfW( text, L"Лидер: %s", leader->charName );
					SetDlgItemTextW( hDlg, IDC_ST_PARTYLEADER, text );
				}
				else SetDlgItemTextW( hDlg, IDC_ST_PARTYLEADER, L"<unknown_idx>" );
			}
			else
			{
				wsprintfW( text, L"<unknown_oid_%u>", leaderoid );
				SetDlgItemTextW( hDlg, IDC_ST_PARTYLEADER, text );
			}
			switch( lootType )
			{
			case 0: SetDlgItemTextW( hDlg, IDC_ST_PARTYLOOTTYPE, L"Поднявшему" ); break;
			case 1: SetDlgItemTextW( hDlg, IDC_ST_PARTYLOOTTYPE, L"Случайно" ); break;
			case 2: SetDlgItemTextW( hDlg, IDC_ST_PARTYLOOTTYPE, L"Случайно, включая спойл" ); break;
			case 3: SetDlgItemTextW( hDlg, IDC_ST_PARTYLOOTTYPE, L"По очереди" ); break;
			case 4: SetDlgItemTextW( hDlg, IDC_ST_PARTYLOOTTYPE, L"По очереди, включая спойл" ); break;
			}
		} break;
	case (WM_USER+1):
		{
			UserParty *pty = (UserParty *)lParam;
			if( !pty ) return TRUE;
			HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
			int iItem = 0;
			int idx = -1;
			int count = ListView_GetItemCount( hwndLV );
			for( iItem=0; iItem<count; iItem++ )
			{
				unsigned int objectID = WULV_GetItemLPARAM( hwndLV, iItem );
				if( objectID == 0 ) continue;
				if( !pty->isInParty( objectID, &idx ) ) continue;
				if( idx == -1 ) continue;
				UserBuffs *buffs = pty->getPartyPlayerBuffs( idx );
				if( !buffs ) continue;
				UserPartyTabDlg_SetPartyPlayerBuffs( hwndLV, buffs, iItem, 6 );
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

void UserPartyTabDlg_SetPartyPlayerBuffs( HWND hwndLV, UserBuffs *buffs, int iItem, int iSubItem )
{
	// compose long string with buff names :)
	TCHAR str[2048] = {0};
	TCHAR addstr[64];
	int ii = 0;
	unsigned int buffcnt = buffs->buffCount;
	unsigned int npassed = 0;
	wchar_t skillName[256] = {0};
	for( ii=0; ii<UserBuffs::USER_MAX_BUFFS; ii++ )
	{
		if( buffs->buff[ii].skillID == 0 ) continue;
		npassed++;
		buffs->buff[ii].getSkillName( skillName, 256 );
		wsprintf( addstr, TEXT("%s; "), skillName );
		lstrcat( str, addstr );
		if( npassed >= buffcnt ) break;
	}
	WULV_SetItem( hwndLV, str, iItem, iSubItem );
}
