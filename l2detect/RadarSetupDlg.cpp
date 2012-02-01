#include "stdafx.h"
#include "Resource.h"
#include "ConfigIni.h"
//#include "GameClient.h"
#include "ConfigDlg.h"
#include "Logger.h"
// children
#include "RadarSetupDlg_SelfHeal.h"
#include "RadarSetupDlg_OffpartyBDSWS.h"
#include "RadarSetupDlg_SoundAlerts.h"
#include "RadarSetupDlg_Colors.h"
#include "RadarSetupDlg_InvisGM.h"
#include "RadarSetupDlg_QuickTarget.h"
#include "RadarSetupDlg_LT.h"
#include "RadarSetupDlg_FolAsi.h"

extern HINSTANCE      g_radardll_hinst;
extern class CConfig  g_cfg;

INT_PTR CALLBACK RadarSetupDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

void RadarSetupDlgStart( HWND hWndParent )
{
	DialogBoxParam( g_radardll_hinst, MAKEINTRESOURCE(IDD_SETUPRADAR), hWndParent, RadarSetupDlgProc, 0 );
}

HWND rs_hwndTV;
//HTREEITEM

// lParams
#define L_HEAL_USEITEMS         1
#define L_HEAL_USESKILL         2
#define L_QUICK_TARGET          3
#define L_LOCK_TARGET           4
#define L_ASSIST_FOLLOW         5
#define L_OFFPARTY_BDSWS        6
#define L_SOUND_ALERTS          7
#define L_COLORS                8
#define L_INIVIS_GM             9

#define RS_CHILD_COUNT 9
HWND radarSetupDlg_children[ RS_CHILD_COUNT + 1 ];

void RadarSetupDlg_OnInitDialog( HWND hDlg );
void RadarSetupDlg_OnOK( HWND hDlg );
void RadarSetupDlg_OnCancel( HWND hDlg );
void RadarSetupDlg_OnCommand( HWND hDlg, WPARAM wParam, LPARAM lParam );
void RadarSetupDlg_OnNotify( HWND hDlg, WPARAM wParam, LPARAM lParam );
void RadarSetupDlg_OnTreeViewSelChanging( HWND hDlg, LPARAM lParam );
void RadarSetupDlg_OnTreeViewSelChanged( HWND hDlg, LPARAM lParam );

INT_PTR CALLBACK RadarSetupDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG: RadarSetupDlg_OnInitDialog( hDlg ); break;
	case WM_COMMAND: RadarSetupDlg_OnCommand( hDlg, wParam, lParam ); break;
	case WM_NOTIFY: RadarSetupDlg_OnNotify( hDlg, wParam, lParam ); break;
	default: return FALSE; break;
	}
	return true;
}

void RadarSetupDlg_OnInitDialog_InitTreeView( HWND hDlg )
{
	rs_hwndTV = GetDlgItem( hDlg, IDC_T_CONFIG );
	TCHAR text[256] = {0};
	HTREEITEM htiHealing = NULL;
	HTREEITEM htiParty = NULL;
	HTREEITEM htiTargets = NULL;
	HTREEITEM htiLookAndFeel = NULL;
	HTREEITEM htiHacks = NULL;
	TVINSERTSTRUCT tvis;
	// insert at root
	tvis.hParent = NULL;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_CHILDREN | TVIF_PARAM | TVIF_STATE | TVIF_TEXT;
	tvis.item.stateMask = TVIS_BOLD | TVIS_EXPANDED;
	// insert Healing
	tvis.item.lParam = 0;
	tvis.item.cChildren = 1;
	tvis.item.state = TVIS_BOLD | TVIS_EXPANDED;
	tvis.item.pszText = text;
	lstrcpy( text, TEXT("Self") );
	htiHealing = TreeView_InsertItem( rs_hwndTV, &tvis );
	// insert Party
	tvis.item.lParam = 0;
	tvis.item.cChildren = 1;
	tvis.item.state = TVIS_BOLD | TVIS_EXPANDED;
	tvis.item.pszText = text;
	lstrcpy( text, TEXT("Party") );
	htiParty = TreeView_InsertItem( rs_hwndTV, &tvis );
	// insert Targeting
	tvis.item.lParam = 0;
	tvis.item.cChildren = 1;
	tvis.item.state = TVIS_BOLD | TVIS_EXPANDED;
	tvis.item.pszText = text;
	lstrcpy( text, TEXT("PvP settings") );
	htiTargets = TreeView_InsertItem( rs_hwndTV, &tvis );
	// insert LookAndFeel
	tvis.item.lParam = 0;
	tvis.item.cChildren = 1;
	tvis.item.state = TVIS_BOLD | TVIS_EXPANDED;
	tvis.item.pszText = text;
	lstrcpy( text, TEXT("Look And Feel") );
	htiLookAndFeel = TreeView_InsertItem( rs_hwndTV, &tvis );
	// insert hacks
	tvis.item.lParam = 0;
	tvis.item.cChildren = 1;
	tvis.item.state = TVIS_BOLD | TVIS_EXPANDED;
	tvis.item.pszText = text;
	lstrcpy( text, TEXT("Hacks!") );
	htiHacks = TreeView_InsertItem( rs_hwndTV, &tvis );

	// insert children
	// insert Healing children
	tvis.hParent = htiHealing;
	tvis.hInsertAfter = TVI_LAST;
	//
	tvis.item.lParam = L_HEAL_USEITEMS;
	tvis.item.cChildren = 0;
	tvis.item.state = 0;
	tvis.item.pszText = text;
	lstrcpy( text, TEXT("Heal Use Items") );
	TreeView_InsertItem( rs_hwndTV, &tvis );
	//
	tvis.item.lParam = L_HEAL_USESKILL;
	tvis.item.cChildren = 0;
	tvis.item.state = 0;
	tvis.item.pszText = text;
	lstrcpy( text, TEXT("Heal Use Skills") );
	TreeView_InsertItem( rs_hwndTV, &tvis );
	//
	// insert Party children
	tvis.hParent = htiParty;
	tvis.hInsertAfter = TVI_LAST;
	//
	tvis.item.lParam = L_OFFPARTY_BDSWS;
	tvis.item.cChildren = 0;
	tvis.item.state = 0;
	tvis.item.pszText = text;
	lstrcpy( text, TEXT("Offparty dance/song") );
	TreeView_InsertItem( rs_hwndTV, &tvis );
	//
	// insert Targets children
	tvis.hParent = htiTargets;
	tvis.hInsertAfter = TVI_LAST;
	//
	tvis.item.lParam = L_QUICK_TARGET;
	tvis.item.cChildren = 0;
	tvis.item.state = 0;
	tvis.item.pszText = text;
	lstrcpy( text, TEXT("Quick targeting") );
	TreeView_InsertItem( rs_hwndTV, &tvis );
	//
	tvis.item.lParam = L_LOCK_TARGET;
	tvis.item.cChildren = 0;
	tvis.item.state = 0;
	tvis.item.pszText = text;
	lstrcpy( text, TEXT("Lock target") );
	TreeView_InsertItem( rs_hwndTV, &tvis );
	//
	tvis.item.lParam = L_ASSIST_FOLLOW;
	tvis.item.cChildren = 0;
	tvis.item.state = 0;
	tvis.item.pszText = text;
	lstrcpy( text, TEXT("Assist & follow") );
	TreeView_InsertItem( rs_hwndTV, &tvis );
	//
	// insert LookAndFeel children
	tvis.hParent = htiLookAndFeel;
	tvis.hInsertAfter = TVI_LAST;
	// insert colors
	tvis.item.lParam = L_COLORS;
	tvis.item.cChildren = 0;
	tvis.item.state = 0;
	tvis.item.pszText = text;
	lstrcpy( text, TEXT("Colors") );
	TreeView_InsertItem( rs_hwndTV, &tvis );
	// insert Sound Alerts
	tvis.item.lParam = L_SOUND_ALERTS;
	tvis.item.cChildren = 0;
	tvis.item.state = 0;
	tvis.item.pszText = text;
	lstrcpy( text, TEXT("Sound Alerts") );
	TreeView_InsertItem( rs_hwndTV, &tvis );
	//
	// insert Hacks children
	tvis.hParent = htiHacks;
	tvis.hInsertAfter = TVI_LAST;
	//
	tvis.item.lParam = L_INIVIS_GM;
	tvis.item.cChildren = 0;
	tvis.item.state = 0;
	tvis.item.pszText = text;
	lstrcpy( text, TEXT("Invisible objects detection") );
	TreeView_InsertItem( rs_hwndTV, &tvis );
}

void RadarSetupDlg_OnInitDialog_InitChildren( HWND hDlg )
{
	RECT r, rd;
	int tree_x = 0;
	int tree_w = 0;
	int right_w = 0;
	int h = 0;
	HWND htv = GetDlgItem( hDlg, IDC_T_CONFIG );
	GetWindowRect( htv, &r );
	GetClientRect( hDlg, &rd );
	tree_x = 20;
	tree_w = r.right - r.left;
	right_w = rd.right - rd.left - tree_w - tree_x - 10;
	h = r.bottom - r.top;
	int i;
	radarSetupDlg_children[0] = 0;
	radarSetupDlg_children[L_HEAL_USEITEMS]  = RadarSetupDlg_SelfHeal_Create( hDlg );
	radarSetupDlg_children[L_HEAL_USESKILL]  = NULL; // TODO
	radarSetupDlg_children[L_OFFPARTY_BDSWS] = RadarSetupDlg_OFFBDSWS_Create( hDlg );
	radarSetupDlg_children[L_QUICK_TARGET]   = RadarSetupDlg_QuickTarget_Create( hDlg );
	radarSetupDlg_children[L_LOCK_TARGET]    = RadarSetupDlg_LockTarget_Create( hDlg );
	radarSetupDlg_children[L_ASSIST_FOLLOW]  = RadarSetupDlg_FolAsi_Create( hDlg ); // TODO
	radarSetupDlg_children[L_SOUND_ALERTS]   = RadarSetupDlg_SoundAlerts_Create( hDlg );
	radarSetupDlg_children[L_COLORS]         = RadarSetupDlg_Colors_Create( hDlg );
	radarSetupDlg_children[L_INIVIS_GM]      = RadarSetupDlg_InvisGM_Create( hDlg );
	// fix windows
	//DWORD dwStyle;
	for( i=1; i<=RS_CHILD_COUNT; i++ )
	{
		//dwStyle = GetWindowLongPtr( radarSetupDlg_children[i], GWL_STYLE );
		//dwStyle |= WS_CHILD;
		//SetWindowLongPtr( radarSetupDlg_children[i], GWL_STYLE, (LONG)dwStyle );
		MoveWindow( radarSetupDlg_children[i], tree_x + tree_w + 5, 5, right_w, h, TRUE );
	}
}

void RadarSetupDlg_OnInitDialog_DestroyChildren()
{
	int i = RS_CHILD_COUNT;
	while( i>0 )
	{
		if( radarSetupDlg_children[i] )
		{
			SendMessage( radarSetupDlg_children[i], WM_COMMAND, IDC_APPLY, 0 );
			DestroyWindow( radarSetupDlg_children[i] );
		}
		i--;
	}
}

void RadarSetupDlg_OnInitDialog( HWND hDlg )
{
	RadarSetupDlg_OnInitDialog_InitChildren( hDlg );
	RadarSetupDlg_OnInitDialog_InitTreeView( hDlg );
}

void RadarSetupDlg_OnOK( HWND hDlg )
{
	RadarSetupDlg_OnInitDialog_DestroyChildren();
	EndDialog( hDlg, IDOK );
}

void RadarSetupDlg_OnCancel( HWND hDlg )
{
	RadarSetupDlg_OnInitDialog_DestroyChildren();
	EndDialog( hDlg, IDOK );
}

void RadarSetupDlg_OnTreeViewSelChanging( HWND hDlg, LPARAM lParam )
{
	hDlg;
	LPNMTREEVIEW p = (LPNMTREEVIEW)lParam;
	int idx = (int)(p->itemOld.lParam);
	if( (idx >= 1) && (idx <= RS_CHILD_COUNT) )
	{
		if( radarSetupDlg_children[idx] )
			ShowWindow( radarSetupDlg_children[idx], SW_HIDE );
	}
}

void RadarSetupDlg_OnTreeViewSelChanged( HWND hDlg, LPARAM lParam )
{
	hDlg;
	LPNMTREEVIEW p = (LPNMTREEVIEW)lParam;
	int idx = (int)(p->itemNew.lParam);
	if( (idx >= 1) && (idx <= RS_CHILD_COUNT) )
	{
		if( radarSetupDlg_children[idx] )
			ShowWindow( radarSetupDlg_children[idx], SW_SHOW );
	}
}

void RadarSetupDlg_OnCommand( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	lParam;
	switch( LOWORD(wParam) )
	{
	case IDOK: RadarSetupDlg_OnOK( hDlg ); break;
	case IDCANCEL: RadarSetupDlg_OnCancel( hDlg ); break;
	case IDC_NPSETUP: ConfigDialogStart( hDlg ); break;
	}
}

void RadarSetupDlg_OnNotify( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	wParam;
	LPNMHDR pnmh = (LPNMHDR)lParam;
	if( pnmh == NULL ) return;
	switch( pnmh->code )
	{
	case TVN_SELCHANGING:
		{
			if( pnmh->hwndFrom == rs_hwndTV ) RadarSetupDlg_OnTreeViewSelChanging( hDlg, lParam );
		} break;
	case TVN_SELCHANGED:
		{
			if( pnmh->hwndFrom == rs_hwndTV ) RadarSetupDlg_OnTreeViewSelChanged( hDlg, lParam );
		} break;
	}
}
