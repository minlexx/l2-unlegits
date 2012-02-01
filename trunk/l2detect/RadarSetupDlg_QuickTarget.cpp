#include "stdafx.h"
#include "Resource.h"
#include "Logger.h"
#include "GameClient.h"
#include "DlgPressKey.h"
#include "windowUtils.h"
#include "RadarSetupDlg_QuickTarget_distPri.h"

extern HINSTANCE g_radardll_hinst;
extern class GameClient *g_game_client; // in main.cpp

INT_PTR CALLBACK QuickTarget_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

HWND RadarSetupDlg_QuickTarget_Create( HWND hWndParent )
{
	return CreateDialogParam( g_radardll_hinst, MAKEINTRESOURCE(IDD_SR_QUICKTARGET),
		hWndParent, QuickTarget_DlgProc, 0 );
}

// =========================================================

void QuickTarget_Init( HWND hDlg );
void QuickTarget_OnDestroy( HWND hDlg );
void QuickTarget_OnApply( HWND hDlg );
void QuickTarget_OnNotify( HWND hDlg, LPARAM lParam );

void QuickTarget_OnLvBeginLabelEdit( HWND hDlg, LPNMHDR pnmh );
void QuickTarget_OnLvEndLabelEdit( HWND hDlg, LPNMHDR pnmh );
void QuickTarget_OnLvDoubleClick( HWND hDlg, LPNMHDR pnmh );
void QuickTarget_OnLvColumnClick( HWND hDlg, LPNMHDR pnmh );

void QuickTarget_FillClassList( HWND hDlg );
void QuickTarget_DisplayKey( HWND hDlg, int vkey );

void QuickTarget_SetKey( HWND hDlg );


INT_PTR CALLBACK QuickTarget_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam);
	switch( uMsg )
	{
	case WM_INITDIALOG: QuickTarget_Init( hDlg ); break;
	case WM_DESTROY: { QuickTarget_OnDestroy( hDlg ); return FALSE; } break;
	case WM_NOTIFY: QuickTarget_OnNotify( hDlg, lParam ); break;
	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			case IDC_APPLY: QuickTarget_OnApply( hDlg ); break;
			case IDC_B_SET_TARGET_KEY: QuickTarget_SetKey( hDlg ); break;
			case IDC_B_DISTANCE_SETUP: RadarSetupDlg_QuickTarget_SetupDistancePriorities( hDlg ); break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

void QuickTarget_Init( HWND hDlg )
{
	// force UserAI to reload config file
	g_game_client->ai.cfgQuickTarg.loadFromFile( "L2Detect_quickTarg.ini" );
	// init class pri list view
	HWND hWndClassList = GetDlgItem( hDlg, IDC_CLASSLIST );
	WULV_SetExtStyle( hWndClassList, true, true );
	WULV_InsertColumn( hWndClassList, TEXT("class"), 120, 0 );
	WULV_InsertColumn( hWndClassList, TEXT("pri"), 35, 1 );
	// init tab
	HWND hwndTab = GetDlgItem( hDlg, IDC_TAB1 );
	WUTab_InsertItem( hwndTab, TEXT("PC list"), 0 );
	WUTab_InsertItem( hwndTab, TEXT("Enemy list"), 1 );
	//
	QuickTarget_FillClassList( hDlg );
	QuickTarget_DisplayKey( hDlg, g_game_client->ai.cfgQuickTarg.getKey() );
	SetDlgItemInt( hDlg, IDC_E_MAXDIST, g_game_client->ai.cfgQuickTarg.getMaxDist(), FALSE );
	CheckDlgButton( hDlg, IDC_C_SHOUT_TARGET_IN_PARTY_CHAT, g_game_client->ai.cfgQuickTarg.getAnnounceTargetInPartyChat() );
	//CheckDlgButton( hDlg, IDC_C_DONT_TARGET_DEAD, g_game_client->ai.cfgQuickTarg.opt_dontTargetDead() );
	//CheckDlgButton( hDlg, IDC_C_DONT_TARGET_PARTY, g_game_client->ai.cfgQuickTarg.opt_dontTargetParty() );
	//CheckDlgButton( hDlg, IDC_C_DONT_TARGET_CLAN, g_game_client->ai.cfgQuickTarg.opt_dontTargetClan() );
	//CheckDlgButton( hDlg, IDC_C_DONT_TARGET_ALLY, g_game_client->ai.cfgQuickTarg.opt_dontTargetAlly() );
	//CheckDlgButton( hDlg, IDC_C_TARGET_ONLY_CW, g_game_client->ai.cfgQuickTarg.opt_targetOnlyCW() );
	//CheckDlgButton( hDlg, IDC_C_TARGET_ALSO_PK, g_game_client->ai.cfgQuickTarg.opt_allowTargetPK() );
	//CheckDlgButton( hDlg, IDC_C_TARGET_ALSO_FLAGGED, g_game_client->ai.cfgQuickTarg.opt_allowTargetFlagged() );
}


void QuickTarget_OnDestroy( HWND hDlg )
{
	UNREFERENCED_PARAMETER(hDlg);
}

void QuickTarget_OnApply( HWND hDlg )
{
	HWND hWndClassList = GetDlgItem( hDlg, IDC_CLASSLIST );
	// apply class priorities
	ClassPriorities *pri = g_game_client->ai.cfgQuickTarg.getClassPriorities();
	wchar_t spri[32];
	int iItem = -1;
	iItem = ListView_GetNextItem( hWndClassList, iItem, LVNI_ALL );
	while( iItem != -1 )
	{
		int classId = WULV_GetItemLPARAM( hWndClassList, iItem );
		*((int *)(&spri[0])) = 0;
		WULV_GetItemText( hWndClassList, iItem, 1, spri, 31 );
		int ipri = 0;
		swscanf( spri, L"%d", &ipri );
		pri->setPriForClass( classId, ipri );
		iItem = ListView_GetNextItem( hWndClassList, iItem, LVNI_ALL );
	}
	// apply hotkey
	int hotkey_vcode = 0;
	GetDlgItemText( hDlg, IDC_E_KEY, spri, 31 );
	swscanf( spri, L"%d", &hotkey_vcode );
	g_game_client->ai.cfgQuickTarg.setKey( hotkey_vcode );
	// apply max dist
	g_game_client->ai.cfgQuickTarg.setMaxDist( (int)GetDlgItemInt( hDlg, IDC_E_MAXDIST, NULL, FALSE ) );
	// apply shout in party
	g_game_client->ai.cfgQuickTarg.setAnnounceTargetInPartyChat( (int)GetDlgItemInt( hDlg, IDC_C_SHOUT_TARGET_IN_PARTY_CHAT, NULL, FALSE ) );
	// apply "don't" checks
	/*g_game_client->ai.cfgQuickTarg.setTargetParams(
		IsDlgButtonChecked( hDlg, IDC_C_DONT_TARGET_DEAD ),
		IsDlgButtonChecked( hDlg, IDC_C_DONT_TARGET_PARTY ),
		IsDlgButtonChecked( hDlg, IDC_C_DONT_TARGET_CLAN ),
		IsDlgButtonChecked( hDlg, IDC_C_DONT_TARGET_ALLY ),
		IsDlgButtonChecked( hDlg, IDC_C_TARGET_ONLY_CW ),
		IsDlgButtonChecked( hDlg, IDC_C_TARGET_ALSO_PK ),
		IsDlgButtonChecked( hDlg, IDC_C_TARGET_ALSO_FLAGGED ),
		IsDlgButtonChecked( hDlg, IDC_C_SHOUT_TARGET_IN_PARTY_CHAT )
		);*/
	// save config to file
	g_game_client->ai.cfgQuickTarg.saveToFile( "L2Detect_quickTarg.ini" );
}

void QuickTarget_FillClassList( HWND hDlg )
{
	HWND hWndClassList = GetDlgItem( hDlg, IDC_CLASSLIST );
	SendMessage( hWndClassList, LVM_DELETEALLITEMS, 0, 0 ); // clear LV
	ClassPriorities *pri = g_game_client->ai.cfgQuickTarg.getClassPriorities();
	int i;
	for( i=0; i<L2MaxClasses; i++ )
	{
		const char *acn = L2Data_getClass( i );
		if( acn )
		{
			wchar_t wcn[128]; wcn[0] = 0;
			MultiByteToWideChar( CP_ACP, 0, acn, -1, wcn, 127 );
			int ipri = pri->getPriForClass( i );
			if( ipri >= 0 )
			{
				int added_item_index = WULV_InsertItem( hWndClassList, wcn, 999, (LPARAM)i );
				swprintf( wcn, 127, L"%d", ipri );
				WULV_SetItem( hWndClassList, wcn, added_item_index, 1 );
			}
		}
	}
}

void QuickTarget_DisplayKey( HWND hDlg, int vkey )
{
	wchar_t sz[64];
	wchar_t szz[32];
	swprintf( sz, 63, L"%d ", vkey ); // vkey code
	if( ( (vkey>='A') && (vkey<='Z') ) || ( (vkey>='0') && (vkey<='9') ) )
	{
		swprintf( szz, 31, _T("'%c'"), vkey );
		wcscat( sz, szz );
	}
	else
	{
		switch( vkey )
		{
		case VK_ESCAPE: wcscat( sz, L"Esc" ); break;
		case VK_OEM_3: wcscat( sz, L"~" ); break;
		case VK_SNAPSHOT: wcscat( sz, L"PrtScr" ); break;
		case VK_SCROLL: wcscat( sz, L"ScrollLock" ); break;
		case VK_PAUSE: wcscat( sz, L"Pause" ); break;
		case VK_RETURN: wcscat( sz, L"Enter" ); break;
		case VK_SPACE: wcscat( sz, L"Spacebar" ); break;
		// TODO: other key codes... вломы, их дохрена - кнопок на клаве
		}
	}
	SetDlgItemTextW( hDlg, IDC_E_KEY, sz );
}

void QuickTarget_SetKey( HWND hDlg )
{
	int vkey = DlgPressKey_InputPressKey( hDlg );
	if( vkey <= 0 ) return;
	QuickTarget_DisplayKey( hDlg, vkey );
}

void QuickTarget_OnNotify( HWND hDlg, LPARAM lParam )
{
	LPNMHDR pnmh = (LPNMHDR)lParam;
	HWND hwndLV = GetDlgItem( hDlg, IDC_CLASSLIST );
	if( hwndLV == pnmh->hwndFrom ) // if notification from ClassList ListView
	{
		switch( pnmh->code )
		{
		case LVN_BEGINLABELEDIT: QuickTarget_OnLvBeginLabelEdit( hDlg, pnmh ); break;
		case LVN_ENDLABELEDIT: QuickTarget_OnLvEndLabelEdit( hDlg, pnmh ); break;
		case NM_DBLCLK: QuickTarget_OnLvDoubleClick( hDlg, pnmh ); break;
		case LVN_COLUMNCLICK: QuickTarget_OnLvColumnClick( hDlg, pnmh ); break;
		}
	}
}

void QuickTarget_OnLvBeginLabelEdit( HWND hDlg, LPNMHDR pnmh )
{
	HWND hwndLV = GetDlgItem( hDlg, IDC_CLASSLIST );
	HWND hwndEdit = (HWND)SendMessage( hwndLV, LVM_GETEDITCONTROL, 0, 0 );
	if( !hwndEdit ) return;
	NMLVDISPINFO *pnmlvdispinfo = (NMLVDISPINFO *)pnmh;
	int iItem = pnmlvdispinfo->item.iItem;
	wchar_t pri_text[128] = {0};
	WULV_GetItemText( hwndLV, iItem, 1, pri_text, 127 );
	DWORD dwOldEditStyle = GetWindowLong( hwndEdit, GWL_STYLE );
	SetWindowLong( hwndEdit, GWL_STYLE, dwOldEditStyle | ES_NUMBER ); // allow only number input
	SetWindowText( hwndEdit, pri_text );
}

void QuickTarget_OnLvEndLabelEdit( HWND hDlg, LPNMHDR pnmh )
{
	HWND hwndLV = GetDlgItem( hDlg, IDC_CLASSLIST );
	NMLVDISPINFO *pnmlvdispinfo = (NMLVDISPINFO *)pnmh;
	int iItem = pnmlvdispinfo->item.iItem;
	wchar_t *newText = pnmlvdispinfo->item.pszText;
	if( !newText ) return;
	int new_pri = 0;
	swscanf( newText, L"%d", &new_pri );
	wchar_t ttext[32];
	swprintf( ttext, 31, L"%d", new_pri );
	WULV_SetItem( hwndLV, ttext, iItem, 1 );
}

void QuickTarget_OnLvDoubleClick( HWND hDlg, LPNMHDR pnmh )
{
	UNREFERENCED_PARAMETER(hDlg);
	NMITEMACTIVATE *p = (NMITEMACTIVATE *)pnmh;
	ListView_EditLabel( pnmh->hwndFrom, p->iItem );
}

int CALLBACK QuickTarget_LVCompareFunc1( LPARAM iItem1, LPARAM iItem2, LPARAM lpHWND );
int CALLBACK QuickTarget_LVCompareFunc2( LPARAM iItem1, LPARAM iItem2, LPARAM lpHWND );

void QuickTarget_OnLvColumnClick( HWND hDlg, LPNMHDR pnmh )
{
	UNREFERENCED_PARAMETER(hDlg);
	int iCol = ((NMLISTVIEW *)pnmh)->iSubItem;
	switch( iCol )
	{
	case 0: ListView_SortItemsEx( pnmh->hwndFrom, QuickTarget_LVCompareFunc1, (LPARAM)pnmh->hwndFrom ); break;
	case 1: ListView_SortItemsEx( pnmh->hwndFrom, QuickTarget_LVCompareFunc2, (LPARAM)pnmh->hwndFrom ); break;
	}
}

// sort by class name A -> Z
int CALLBACK QuickTarget_LVCompareFunc1( LPARAM iItem1, LPARAM iItem2, LPARAM lpHWND )
{
	HWND hwndLV = (HWND)lpHWND;
	TCHAR text1[256], text2[256];
	text1[0] = text2[0] = 0;
	WULV_GetItemText( hwndLV, iItem1, 0, text1, 255 );
	WULV_GetItemText( hwndLV, iItem2, 0, text2, 255 );
	return _tcscmp( text1, text2 );
}

// sort by priority 999 -> 0
int CALLBACK QuickTarget_LVCompareFunc2( LPARAM iItem1, LPARAM iItem2, LPARAM lpHWND )
{
	HWND hwndLV = (HWND)lpHWND;
	TCHAR text1[256], text2[256];
	text1[0] = text2[0] = 0;
	WULV_GetItemText( hwndLV, iItem1, 1, text1, 255 );
	WULV_GetItemText( hwndLV, iItem2, 1, text2, 255 );
	int pri1 = 0, pri2 = 0;
	_stscanf( text1, _T("%d"), &pri1 );
	_stscanf( text2, _T("%d"), &pri2 );
	if( pri1 == pri2 ) return 0;
	if( pri1 > pri2 ) return -1;
	return 1;
}
