#include "stdafx.h"
#include "Resource.h"
#include "windowUtils.h"
#include "logger.h"
#include "L2Client.h"
#include "IL2Client.h"
#include "BotCfgDlg.h"

extern HINSTANCE g_hInst;

INT_PTR CALLBACK BotCfgDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

void BotCfgDlg_Run( HWND hWndParent, L2Client *pcl )
{
	DialogBoxParamW( g_hInst, MAKEINTRESOURCE(IDD_BOTCONFIG), hWndParent, BotCfgDlgProc, (LPARAM)pcl );
}

#define NUM_CHILD_TABS    6
#define T_SELF            1
#define T_SELF_HEAL       2
#define T_SELF_BUFFS      3
#define T_PARTY           4
#define T_PARTY_HEAL      5
#define T_PARTY_BUFFS     6
struct BotCfgDlgSt
{
	L2Client *cl;
	BotConfig myCfg;
	HWND hWndChildTab[ NUM_CHILD_TABS + 1 ];
	// tree items
	// self
	HTREEITEM htiSelf;
	//HTREEITEM htiSelfHeal;
	// party
	HTREEITEM htiParty;
};

void BotCfg_InitChildTabs( HWND hDlg, BotCfgDlgSt *st );
void BotCfg_DestroyChildTabs( HWND hDlg, BotCfgDlgSt *st );
void BotCfg_InitTree( HWND hDlg, BotCfgDlgSt *st );

BOOL BotCfg_OnNotify( HWND hDlg, LPARAM lParam );
BOOL BotCfg_OnCommand( HWND hDlg, WPARAM wParam, LPARAM lParam );
void BotCfg_OnOK( HWND hDlg, BotCfgDlgSt *st );
void BotCfg_OnApply( HWND hDlg, BotCfgDlgSt *st );
void BotCfg_OnLoad( HWND hDlg, BotCfgDlgSt *st );
void BotCfg_OnSave( HWND hDlg, BotCfgDlgSt *st );

INT_PTR CALLBACK BotCfgDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			BotCfgDlgSt *st = (BotCfgDlgSt *)HeapAlloc( GetProcessHeap(), 0, sizeof(BotCfgDlgSt) );
			st->cl = (L2Client *)lParam;
			st->myCfg = st->cl->getConfig(); // create copy of client config
			SetWindowLongPtrW( hDlg, GWLP_USERDATA, (LONG_PTR)st );
			BotCfg_InitChildTabs( hDlg, st );
			BotCfg_InitTree( hDlg, st );
		} break;
	case WM_DESTROY:
		{
			BotCfgDlgSt *st = (BotCfgDlgSt *)GetWindowLongPtrW( hDlg, GWLP_USERDATA );
			SetWindowLongPtrW( hDlg, GWLP_USERDATA, 0 ); // no user-data now
			BotCfg_DestroyChildTabs( hDlg, st );
			HeapFree( GetProcessHeap(), 0, st ); // delete struct
			//log_error( LOG_OK, "BotCfgDlgProc() WM_DESTROY\n" );
			return FALSE; // call default handler
		} break;
	case WM_COMMAND: return BotCfg_OnCommand( hDlg, wParam, lParam ); break;
	case WM_NOTIFY: return BotCfg_OnNotify( hDlg, lParam ); break;
	default: return FALSE; break;
	}
	return TRUE;
}

BOOL BotCfg_OnCommand( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam);
	BotCfgDlgSt *st = (BotCfgDlgSt *)GetWindowLongPtrW( hDlg, GWLP_USERDATA );
	int ctrlID = LOWORD(wParam);
	switch( ctrlID )
	{
	case IDCANCEL: EndDialog( hDlg, IDCANCEL ); break;
	case IDOK: BotCfg_OnOK( hDlg, st ); break;
	case IDC_APPLY: BotCfg_OnApply( hDlg, st ); break;
	case IDC_LOAD: BotCfg_OnLoad( hDlg, st ); break;
	case IDC_SAVE: BotCfg_OnSave( hDlg, st ); break;
	default: return FALSE; break; // call default handler
	}
	return TRUE;
}

void BotCfg_InitChildTabs( HWND hDlg, BotCfgDlgSt *st )
{
	int i;
	for( i=0; i<=NUM_CHILD_TABS; i++ ) st->hWndChildTab[i] = NULL;
	// create individual children, passing pointer to our copy of config
	st->hWndChildTab[ T_SELF ] = BotCfgDlg_Self_Create( hDlg, &(st->myCfg) );
	st->hWndChildTab[ T_SELF_HEAL ] = BotCfgDlg_SelfHeal_Create( hDlg, &(st->myCfg) );
	st->hWndChildTab[ T_PARTY ] = BotCfgDlg_Party_Create( hDlg, &(st->myCfg) );
	// resize all children to fit
	RECT r, rd;
	int tree_x = 0, tree_y = 0;
	int tree_w = 0;
	int right_w = 0;
	int h = 0;
	HWND htv = GetDlgItem( hDlg, IDC_TREE1 );
	GetWindowRect( htv, &r );
	GetClientRect( hDlg, &rd );
	tree_x = 10;
	tree_y = 10;
	tree_w = r.right - r.left;
	right_w = rd.right - rd.left - tree_w - tree_x - 10;
	h = r.bottom - r.top;
	for( i=1; i<=NUM_CHILD_TABS; i++ )
	{
		if( st->hWndChildTab[i] )
			MoveWindow( st->hWndChildTab[i], tree_x + tree_w + 5, tree_y, right_w, h, TRUE );
	}
}

void BotCfg_DestroyChildTabs( HWND hDlg, BotCfgDlgSt *st )
{
	UNREFERENCED_PARAMETER(hDlg);
	int i = NUM_CHILD_TABS;
	while( i > 0 )
	{
		if( st->hWndChildTab[i] ) DestroyWindow( st->hWndChildTab[i] );
		st->hWndChildTab[i] = 0;
		i--;
	}
}

void BotCfg_InitTree( HWND hDlg, BotCfgDlgSt *st )
{
	HWND hwndTV = GetDlgItem( hDlg, IDC_TREE1 );
	// self
	st->htiSelf = WUTV_InsertItem( hwndTV, NULL, TVI_LAST, TEXT("Перс"), T_SELF, true, true, true );
	/*st->htiSelfHeal = */
	WUTV_InsertItem( hwndTV, st->htiSelf, TVI_LAST, TEXT("СамоХил"), T_SELF_HEAL, false, false, false );
	// party
	st->htiParty = WUTV_InsertItem( hwndTV, NULL, TVI_LAST, TEXT("Пати"), T_PARTY, true, true, true );
}

BOOL BotCfg_OnNotify( HWND hDlg, LPARAM lParam )
{
	BotCfgDlgSt *st = (BotCfgDlgSt *)GetWindowLongPtrW( hDlg, GWLP_USERDATA );
	LPNMHDR pnmh = (LPNMHDR)lParam;
	HWND hwndTV = GetDlgItem( hDlg, IDC_TREE1 );
	if( pnmh == NULL ) return FALSE; // default handler
	switch( pnmh->code )
	{
	case TVN_SELCHANGING:
		{
			if( pnmh->hwndFrom == hwndTV )
			{
				LPNMTREEVIEW p = (LPNMTREEVIEW)lParam;
				int idx = (int)(p->itemOld.lParam);
				if( (idx >= 1) && (idx <= NUM_CHILD_TABS) )
				{
					if( st->hWndChildTab[idx] ) ShowWindow( st->hWndChildTab[idx], SW_HIDE );
				}
			}
		} break;
	case TVN_SELCHANGED:
		{
			if( pnmh->hwndFrom == hwndTV )
			{
				LPNMTREEVIEW p = (LPNMTREEVIEW)lParam;
				int idx = (int)(p->itemNew.lParam);
				if( (idx >= 1) && (idx <= NUM_CHILD_TABS) )
				{
					if( st->hWndChildTab[idx] ) ShowWindow( st->hWndChildTab[idx], SW_SHOW );
				}
			}
		} break;
	default: return FALSE; break; // call default handler
	}
	return TRUE;
}

void BotCfg_OnOK( HWND hDlg, BotCfgDlgSt *st )
{
	BotCfg_OnApply( hDlg, st );
	EndDialog( hDlg, IDOK );
}

void BotCfg_OnApply( HWND hDlg, BotCfgDlgSt *st )
{
	UNREFERENCED_PARAMETER(hDlg);
	// pass APPLY to all child windows
	int i = 0;
	for( i=1; i<=NUM_CHILD_TABS; i++ )
	{
		if( st->hWndChildTab[i] ) SendMessage( st->hWndChildTab[i], WM_COMMAND, IDC_APPLY, 0 );
	}
	// save our copy of config to L2Client
	st->cl->setBotConfig( st->myCfg );
}

void BotCfg_OnLoad( HWND hDlg, BotCfgDlgSt *st )
{
	// change subdir to '.\configs'
	char curDir[256] = {0};
	char newDir[256] = {0};
	GetCurrentDirectoryA( 200, curDir );
	wsprintfA( newDir, "%s\\configs", curDir );
	SetCurrentDirectoryA( newDir );
	//
	char fileName[256] = {0};
	OPENFILENAMEA ofn;
	memset( &ofn, 0, sizeof(ofn) );
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hDlg;
	ofn.hInstance = g_hInst;
	ofn.lpstrFilter = "Config files\0*.config\0All files\0*.*\0\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = 255;
	ofn.lpstrTitle = "Откуда читать?";
	ofn.lpstrInitialDir = newDir;
	ofn.Flags = OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	if( GetOpenFileNameA( &ofn ) )
	{
		if( !st->myCfg.readConfig( fileName ) )
		{
			MessageBox( hDlg, TEXT("Конфиг не загрузился!"), TEXT("Ашыпка!"), MB_ICONSTOP );
		}
		else // update all child tabs
		{
			// pass APPLY to all child windows
			int i = 0;
			for( i=1; i<=NUM_CHILD_TABS; i++ )
			{
				if( st->hWndChildTab[i] )
					SendMessage( st->hWndChildTab[i], WM_COMMAND, IDC_LOAD, 0 );
			}
		}
	}
	// restore cur dir (required to load maps, for example)
	SetCurrentDirectoryA( curDir );
}

void BotCfg_OnSave( HWND hDlg, BotCfgDlgSt *st )
{
	// change subdir to '.\configs'
	char curDir[256] = {0};
	char newDir[256] = {0};
	GetCurrentDirectoryA( 200, curDir );
	wsprintfA( newDir, "%s\\configs", curDir );
	SetCurrentDirectoryA( newDir );
	//
	char fileName[256] = {0};
	OPENFILENAMEA ofn;
	memset( &ofn, 0, sizeof(ofn) );
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hDlg;
	ofn.hInstance = g_hInst;
	ofn.lpstrFilter = "Config files\0*.config\0All files\0*.*\0\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = 255;
	ofn.lpstrTitle = "Куда сохранять?";
	ofn.lpstrInitialDir = newDir;
	ofn.Flags = OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	if( GetSaveFileNameA( &ofn ) )
	{
		if( strstr( fileName, ".config" ) == NULL ) strcat( fileName, ".config" );
		if( !st->myCfg.saveConfig( fileName ) )
		{
			MessageBox( hDlg, TEXT("Конфиг не сохранился!"), TEXT("Ашыпка!"), MB_ICONSTOP );
		}
	}
	// restore cur dir (required to load maps, for example)
	SetCurrentDirectoryA( curDir );
}
