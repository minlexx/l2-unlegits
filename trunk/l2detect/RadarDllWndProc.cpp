#include "stdafx.h"
#include "Resource.h"
#include "RadarDllWnd.h"
#include "RadarDllWndCfg.h"
#include "Logger.h"
#include "GameClient.h"
#include "LoginClient.h"
#include "ConfigIni.h"
extern GameClient *g_game_client;
extern CConfig g_cfg;
// lists
#include "CharArray.h"
#include "ClanList.h"
#include "NpcArray.h"
//
#include "RadarSetupDlg.h"
#include "DebugDlg.h"
#include "AboutDialog.h"
#include "RadarDllWndSize.h"
//
#include "PacketInjector.h"
// HWID verification
#include "RadarDllWndHWID.h"
// scripting
#include "ScriptEngine.h"

HWND    g_rw_hWndTab;                      // tab window
HWND    g_rw_hWndCL;                       // chars ListView window
HWND    g_rw_hWndNPC;                      // npcs ListView window
HWND    g_rw_hWndEnemies;                  // enemies ListView window
HWND    g_rw_hWndTargets;                  // target ListView window
#ifdef _DEBUG
HWND    g_rw_hWndDebugBtn;                 // button "Debug"
#endif
DWORD   dwRadarDllLastCharListUpdateTime;  // last CharList update time( GetTickCount() )
int     bRadarWindowIsVisible;             // 1, if window is shown, 0 if hidden
// sizes definitions
int     g_radar_h = 200;                   // width, height of radar area
int     g_radar_h2 = 100;                  // half of width, height of radar area :)
float   g_radar_scale = 0.02f;             // scale of radar display ( 200 / 10'000 )
bool    g_radarwnd_npc_displayEnable = 0;  // enable or disable npc display
int     g_checksrow_h = 25;                // height of checks row
int     g_charlist_h = 480;                // height of chars list
int     g_tabheader_h = 20;                // height of tabs header
//
int     bRadarWindowShrunk;                // is radar windows shrunk
int     g_radarWnd_bigH;                   // высота окна радара в развернутом виде
int     g_radarWnd_titleSet;               // установлен ли заголовок окна радара с именем чара
int     g_radarWnd_curTab;                 // current selected tab
int     g_radarWnd_isInTray;               // is window minimized to tray
HIMAGELIST g_radarWnd_imageList;           // image list with icons, for ListView
// radar window UI config
RADARWNDCFG g_rwCfg;

// display globals
extern int g_radarWnd_drawOnClient;
extern HWND g_hWndL2;

// button IDs
#define IDCB_DEBUGDLG     999
#define IDCB_SHOW_NPC     100
#define IDCB_CONFIG       101
#define IDCB_SHRINKWINDOW 102
#define IDCB_TAB          103
#define IDCB_AUTOHEAL     104
#define IDCB_TRAY         105
#define IDCB_SOUNDENABLE  106
#define IDCB_ABOUT        107
#define IDCB_LUA          108
#define IDCB_TRANSPARENCY 109
#define IDCB_ONTOP        110
#define IDCB_FOLLOW       111
#define IDCB_ASSIST       112

#define TRAY_ICON_ID 1

// GDI objects
HPEN    gr_grayPen;
HPEN    gr_darkGrayPen_dash;
HPEN    gr_redPen;
HPEN    gr_greenPen;
HPEN    gr_bluePen;
HFONT   gr_font;

// message handlers
LRESULT RW_OnCreate( HWND hWnd );
LRESULT RW_OnClose( HWND hWnd );
LRESULT RW_OnCommand( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT RW_OnSize( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT RW_OnKeyUp( HWND hWnd, WPARAM wParam );
LRESULT RW_OnNotify( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT RW_OnPaint( HWND hWnd );
LRESULT RW_OnTimer( HWND hWnd, WPARAM wParam );
LRESULT RW_OnTrayMessage( HWND hWnd, WPARAM wParam, LPARAM lParam );
void    RW_OnButtonLua( HWND hWnd );
// my message handlers
LRESULT RW_OnWindowShowToggle( HWND hWnd );
LRESULT RW_OnUpdateCharsList( HWND hWnd );
LRESULT RW_OnUpdateNpcsList( HWND hWnd );
LRESULT RW_OnSetUserCoords( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT RW_OnGameClientNotify( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT RW_OnLoginClientNotify( HWND hWnd, WPARAM wParam, LPARAM lParam );
// new ways to update lists
LRESULT RW_OnAddObject( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT RW_OnUpdObject( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT RW_OnDelObject( HWND hWnd, WPARAM wParam, LPARAM lParam );
LRESULT RW_OnForceUpdateList( WPARAM wParam );
//
void    RW_UpdateWindowTitle_and_TrayIconTip( HWND hWnd );
LRESULT RW_UpdateChecksState();

// window procedure
LRESULT CALLBACK RadarDllWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_CREATE: return RW_OnCreate( hWnd ); break;
	case WM_CLOSE: return RW_OnClose( hWnd ); break;
	case WM_COMMAND: return RW_OnCommand( hWnd, wParam, lParam ); break;
	case WM_SIZE: return RW_OnSize( hWnd, wParam, lParam ); break;
	case WM_KEYUP: return RW_OnKeyUp( hWnd, wParam ); break;
	case WM_NOTIFY: return RW_OnNotify( hWnd, wParam, lParam ); break;
	case WM_DESTROY: PostQuitMessage( 0 ); break;
	case WM_PAINT: return RW_OnPaint( hWnd ); break;
	case WM_TIMER: return RW_OnTimer( hWnd, wParam ); break;
	case WMMY_WNDSHOWTOGGLE: return RW_OnWindowShowToggle( hWnd ); break;
	//case WMMY_UPDATE_CHARS_LIST: return RW_OnUpdateCharsList( hWnd ); break;
	//case WMMY_UPDATE_NPCS_LIST: return RW_OnUpdateNpcsList( hWnd ); break;
	//case WMMY_SET_USER_COORDS: return RW_OnSetUserCoords( hWnd, wParam, lParam ); break;
	case WMMY_TRAYMESSAGE: return RW_OnTrayMessage( hWnd, wParam, lParam ); break;
	case WMMY_GAMECLIENT_NOTIFY: return RW_OnGameClientNotify( hWnd, wParam, lParam ); break;
	case WMMY_LOGINCLIENT_NOTIFY: return RW_OnLoginClientNotify( hWnd, wParam, lParam ); break;
		// new way to update lists
	case WMMY_ADD_OBJECT:    return RW_OnAddObject( hWnd, wParam, lParam ); break;
	case WMMY_UPDATE_OBJECT: return RW_OnUpdObject( hWnd, wParam, lParam ); break;
	case WMMY_DEL_OBJECT:    return RW_OnDelObject( hWnd, wParam, lParam ); break;
	case WMMY_FORCE_UPDATE_LIST: return RW_OnForceUpdateList( wParam ); break;
		//
	case WMMY_UPDATE_CHECKS: return RW_UpdateChecksState(); break;
	case WMMY_SAVE_WINDOW_SIZE: RadarDllWnd_SaveWidowSize( hWnd ); break;
	default: return DefWindowProc( hWnd, uMsg, wParam, lParam ); break;
	}
	return 0;
}

LRESULT RW_OnClose( HWND hWnd )
{
	//int res = MessageBox( hWnd, TEXT("Are you sure??"), TEXT("Close window???"),
	//	MB_ICONQUESTION | MB_YESNO );
	//if( res == IDNO ) return 0;
	// savr window pos/size
	RadarDllWnd_SaveWidowSize( hWnd );
	// Destroy Window
	DestroyWindow( hWnd );
	// Destroy global GDI objects
	DeleteObject( (HGDIOBJ)gr_grayPen );
	DeleteObject( (HGDIOBJ)gr_darkGrayPen_dash );
	DeleteObject( (HGDIOBJ)gr_redPen );
	DeleteObject( (HGDIOBJ)gr_greenPen );
	DeleteObject( (HGDIOBJ)gr_bluePen );
	DeleteObject( (HGDIOBJ)gr_font );
	return 0;
}

LRESULT RW_OnCreate( HWND hWnd )
{
	g_radardll_hwnd = hWnd; // save radar window handle in global var
	//
	SetWindowText( hWnd, TEXT("L2Detect") );
	HICON hIcon = LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_L2_BLACK) );
	SendMessage( hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon );
	SendMessage( hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon );

	RadarWndCfg_InitDefault( &g_rwCfg );
	RadarWndCfg_LoadConfigFromFile( &g_rwCfg, NULL );

	// create global GDI objects
	gr_grayPen = CreatePen( PS_SOLID, 1, RGB(128,128,128) );
	gr_darkGrayPen_dash = CreatePen( PS_DOT, 1, RGB(75,75,75) );
	gr_redPen = CreatePen( PS_SOLID, 1, RGB(255,128,128) );
	gr_greenPen = CreatePen( PS_SOLID, 1, RGB(128,255,128) );
	gr_bluePen = CreatePen( PS_SOLID, 1, RGB(128,128,255) );
	gr_font = CreateFont( -9, 0, 0, 0, FW_NORMAL, 0, 0, 0, RUSSIAN_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		TEXT("Arial") );
	//
	/*INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_LISTVIEW_CLASSES | ICC_TAB_CLASSES;
	InitCommonControlsEx( &InitCtrls );*/
	//
	HWND hWndB;
	RECT rcClient;
	GetClientRect( hWnd, &rcClient );
	int clientW = rcClient.right - rcClient.left;
	//int clientH = rcClient.bottom - rcClient.top; // local variable is initialized but not referenced

	TCHAR colText[64] = {0};

	// create imagelist for listviews
	g_radarWnd_imageList = ImageList_Create( 10, 10, ILC_COLOR32, 8, 4 );
	ImageList_ReplaceIcon( g_radarWnd_imageList, -1, LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_RED_CIRCLE) ) ); // 0
	ImageList_ReplaceIcon( g_radarWnd_imageList, -1, LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_GREEN_CIRCLE) ) ); // 1
	ImageList_ReplaceIcon( g_radarWnd_imageList, -1, LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_CROSS) ) ); // 2
	ImageList_ReplaceIcon( g_radarWnd_imageList, -1, LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_RED_HALF_CIRCLE) ) ); // 3

	// checks
	int checkrow_x = 0;
	int chechk_w = 50;
	//
	// button "<>"
	hWndB = CreateWindowEx( 0, TEXT("BUTTON"), TEXT("<>"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		1, 1, 20, 20, hWnd, (HMENU)IDCB_SHRINKWINDOW, g_radardll_hinst, NULL );
	SendMessage( hWndB, WM_SETFONT, (WPARAM)gr_font, TRUE );
	// button "Transparency"
	hWndB = CreateWindowEx( 0, TEXT("BUTTON"), TEXT("Tr"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		21, 1, 20, 20, hWnd, (HMENU)IDCB_TRANSPARENCY, g_radardll_hinst, NULL );
	SendMessage( hWndB, WM_SETFONT, (WPARAM)gr_font, TRUE );
	// button "On top"
	hWndB = CreateWindowEx( 0, TEXT("BUTTON"), TEXT("top"),
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		41, 1, 30, 20, hWnd, (HMENU)IDCB_ONTOP, g_radardll_hinst, NULL );
	SendMessage( hWndB, WM_SETFONT, (WPARAM)gr_font, TRUE );
	if( g_cfg.isInGameMode )
		SendMessage( hWndB, BM_SETCHECK, BST_CHECKED, 0 ); // in ingame on top default
	//
	// check "NPC"
	hWndB = CreateWindowEx( 0, TEXT("BUTTON"), TEXT("NPC"),
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		checkrow_x, g_radar_h, (chechk_w-10), g_checksrow_h,
		hWnd, (HMENU)IDCB_SHOW_NPC, g_radardll_hinst, NULL );
	checkrow_x += (chechk_w-10);
	SendMessage( hWndB, WM_SETFONT, (WPARAM)gr_font, TRUE );
	// check "UseItem"
	hWndB = CreateWindowEx( 0, TEXT("BUTTON"), TEXT("UseItem"),
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		checkrow_x, g_radar_h, (chechk_w+5), g_checksrow_h,
		hWnd, (HMENU)IDCB_AUTOHEAL, g_radardll_hinst, NULL );
	checkrow_x += (chechk_w+5);
	SendMessage( hWndB, WM_SETFONT, (WPARAM)gr_font, TRUE );
	SendMessage( hWndB, BM_SETCHECK, (WPARAM)BST_CHECKED, 0 );
	// check "Sounds"
	hWndB = CreateWindowEx( 0, TEXT("BUTTON"), TEXT("Snd"),
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		checkrow_x, g_radar_h, chechk_w-15, g_checksrow_h,
		hWnd, (HMENU)IDCB_SOUNDENABLE, g_radardll_hinst, NULL );
	checkrow_x += (chechk_w-15);
	SendMessage( hWndB, WM_SETFONT, (WPARAM)gr_font, TRUE );
	SendMessage( hWndB, BM_SETCHECK, (WPARAM)BST_CHECKED, 0 );
	// check "Follow"
	hWndB = CreateWindowEx( 0, TEXT("BUTTON"), TEXT("Fol"),
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		checkrow_x, g_radar_h, chechk_w-20, g_checksrow_h,
		hWnd, (HMENU)IDCB_FOLLOW, g_radardll_hinst, NULL );
	checkrow_x += (chechk_w-20);
	SendMessage( hWndB, WM_SETFONT, (WPARAM)gr_font, TRUE );
	SendMessage( hWndB, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0 );
	// check "Assist"
	hWndB = CreateWindowEx( 0, TEXT("BUTTON"), TEXT("Asi"),
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		checkrow_x, g_radar_h, chechk_w-20, g_checksrow_h,
		hWnd, (HMENU)IDCB_ASSIST, g_radardll_hinst, NULL );
	checkrow_x += (chechk_w-20);
	SendMessage( hWndB, WM_SETFONT, (WPARAM)gr_font, TRUE );
	SendMessage( hWndB, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0 );
	//
	// button "Config"
	hWndB = CreateWindowEx( 0, TEXT("BUTTON"), TEXT("Cfg"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		checkrow_x, g_radar_h, (chechk_w-25), g_checksrow_h,
		hWnd, (HMENU)IDCB_CONFIG, g_radardll_hinst, NULL );
	checkrow_x += (chechk_w-25);
	SendMessage( hWndB, WM_SETFONT, (WPARAM)gr_font, TRUE );
	// button "Tray"
	hWndB = CreateWindowEx( 0, TEXT("BUTTON"), TEXT("Tray"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		checkrow_x, g_radar_h, g_checksrow_h, g_checksrow_h,
		hWnd, (HMENU)IDCB_TRAY, g_radardll_hinst, NULL );
	checkrow_x += g_checksrow_h;
	SendMessage( hWndB, WM_SETFONT, (WPARAM)gr_font, TRUE );
	// button "Lua"
	hWndB = CreateWindowEx( 0, TEXT("BUTTON"), TEXT("Lua"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		checkrow_x, g_radar_h, g_checksrow_h, g_checksrow_h,
		hWnd, (HMENU)IDCB_LUA, g_radardll_hinst, NULL );
	checkrow_x += g_checksrow_h;
	SendMessage( hWndB, WM_SETFONT, (WPARAM)gr_font, TRUE );
	// button "About"
	hWndB = CreateWindowEx( 0, TEXT("BUTTON"), TEXT("?"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		checkrow_x, g_radar_h, (g_checksrow_h-5), g_checksrow_h,
		hWnd, (HMENU)IDCB_ABOUT, g_radardll_hinst, NULL );
	checkrow_x += (g_checksrow_h-5);
	SendMessage( hWndB, WM_SETFONT, (WPARAM)gr_font, TRUE );

	// Create tab
	g_rw_hWndTab = CreateWindowEx( 0, WC_TABCONTROL, TEXT(""),
		WS_CHILD | WS_VISIBLE,
		0, g_radar_h + g_checksrow_h, clientW, g_charlist_h + g_tabheader_h,
		hWnd, (HMENU)IDCB_TAB, g_radardll_hinst, NULL );
	SendMessage( g_rw_hWndTab, WM_SETFONT, (WPARAM)gr_font, FALSE );
	g_radarWnd_curTab = 0;
	// add 2 tabs
	TCITEM tcItem;
	memset( &tcItem, 0, sizeof(TCITEM) );
	tcItem.mask = TCIF_TEXT;
	// 1st tab "PC"
	tcItem.pszText = colText;
	lstrcpy( colText, TEXT("PC") );
	SendMessage( g_rw_hWndTab, TCM_INSERTITEM, (WPARAM)0, (LPARAM)&tcItem );
	// 2nd tab "NPC"
	tcItem.pszText = colText;
	lstrcpy( colText, TEXT("NPC") );
	SendMessage( g_rw_hWndTab, TCM_INSERTITEM, (WPARAM)1, (LPARAM)&tcItem );
	// 3rd tab "Enemy"
	tcItem.pszText = colText;
	lstrcpy( colText, TEXT("Enemy") );
	SendMessage( g_rw_hWndTab, TCM_INSERTITEM, (WPARAM)2, (LPARAM)&tcItem );
	// 4th tab "Targets"
	tcItem.pszText = colText;
	lstrcpy( colText, TEXT("Targets") );
	SendMessage( g_rw_hWndTab, TCM_INSERTITEM, (WPARAM)3, (LPARAM)&tcItem );

	// Now create List-Views
	// chars list
	g_rw_hWndCL = CreateWindowEx( 0, WC_LISTVIEW, TEXT(""),
		WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_SORTASCENDING | LVS_SHAREIMAGELISTS,
		5, g_tabheader_h, clientW - 10, g_charlist_h-5,
		g_rw_hWndTab, NULL, g_radardll_hinst, NULL );
	DWORD dwLvExStyle = 0;
	dwLvExStyle |= (LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	SendMessage( g_rw_hWndCL, LVM_SETEXTENDEDLISTVIEWSTYLE, (WPARAM)dwLvExStyle, (LPARAM)dwLvExStyle );
	SendMessage( g_rw_hWndCL, WM_SETFONT, (WPARAM)gr_font, FALSE ); // set font
	SendMessage( g_rw_hWndCL, LVM_SETIMAGELIST, LVSIL_NORMAL, (LPARAM)g_radarWnd_imageList ); // set ImageList for control
	SendMessage( g_rw_hWndCL, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)g_radarWnd_imageList ); // set ImageList for control
	// columns
	LVCOLUMN lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_FMT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.pszText = colText;
	lvc.cchTextMax = sizeof( colText ) / sizeof( colText[0] );
	// col 1
	lstrcpy( colText, TEXT("Name") );
	lvc.cx = 70;
	lvc.iSubItem = 0;
	SendMessage( g_rw_hWndCL, LVM_INSERTCOLUMN, (WPARAM)0, (LPARAM)&lvc );
	// col 2
	lstrcpy( colText, TEXT("cls/base cls") );
	lvc.cx = 100;
	lvc.iSubItem = 1;
	SendMessage( g_rw_hWndCL, LVM_INSERTCOLUMN, (WPARAM)1, (LPARAM)&lvc );
	// col 3
	lstrcpy( colText, TEXT("Clan") );
	lvc.cx = 70;
	lvc.iSubItem = 2;
	SendMessage( g_rw_hWndCL, LVM_INSERTCOLUMN, (WPARAM)2, (LPARAM)&lvc );
	// col 4
	lstrcpy( colText, TEXT("dZ") );
	lvc.cx = 30;
	lvc.iSubItem = 3;
	SendMessage( g_rw_hWndCL, LVM_INSERTCOLUMN, (WPARAM)3, (LPARAM)&lvc );
	// col 5
	lstrcpy( colText, TEXT("CW") );
	lvc.cx = 30;
	lvc.iSubItem = 4;
	SendMessage( g_rw_hWndCL, LVM_INSERTCOLUMN, (WPARAM)4, (LPARAM)&lvc );
	//

	// NPCs listview
	g_rw_hWndNPC = CreateWindowEx( 0, WC_LISTVIEW, TEXT(""),
		WS_CHILD | /*WS_VISIBLE |*/ LVS_REPORT | LVS_SINGLESEL | LVS_SORTASCENDING | LVS_SHAREIMAGELISTS,
		5, g_tabheader_h, clientW - 10, g_charlist_h-5,
		g_rw_hWndTab, NULL, g_radardll_hinst, NULL );
	dwLvExStyle = 0;
	dwLvExStyle = (LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	SendMessage( g_rw_hWndNPC, LVM_SETEXTENDEDLISTVIEWSTYLE, (WPARAM)dwLvExStyle, (LPARAM)dwLvExStyle );
	SendMessage( g_rw_hWndNPC, WM_SETFONT, (WPARAM)gr_font, FALSE );
	SendMessage( g_rw_hWndNPC, LVM_SETIMAGELIST, LVSIL_NORMAL, (LPARAM)g_radarWnd_imageList ); // set ImageList for control
	SendMessage( g_rw_hWndNPC, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)g_radarWnd_imageList ); // set ImageList for control
	// columns
	memset( &lvc, 0, sizeof(lvc) );
	lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_FMT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.pszText = colText;
	lvc.cchTextMax = sizeof( colText ) / sizeof( colText[0] );
	// col 1
	lstrcpy( colText, TEXT("Name") );
	lvc.cx = 70;
	lvc.iSubItem = 0;
	SendMessage( g_rw_hWndNPC, LVM_INSERTCOLUMN, (WPARAM)0, (LPARAM)&lvc );
	// col 2
	lstrcpy( colText, TEXT("Title") );
	lvc.cx = 100;
	lvc.iSubItem = 1;
	SendMessage( g_rw_hWndNPC, LVM_INSERTCOLUMN, (WPARAM)1, (LPARAM)&lvc );
	// col 3
	lstrcpy( colText, TEXT("dz") );
	lvc.cx = 30;
	lvc.iSubItem = 2;
	SendMessage( g_rw_hWndNPC, LVM_INSERTCOLUMN, (WPARAM)2, (LPARAM)&lvc );
	// col 4
	lstrcpy( colText, TEXT("npcID") );
	lvc.cx = 40;
	lvc.iSubItem = 3;
	SendMessage( g_rw_hWndNPC, LVM_INSERTCOLUMN, (WPARAM)3, (LPARAM)&lvc );
	//

	// enemies list
	g_rw_hWndEnemies = CreateWindowEx( 0, WC_LISTVIEW, TEXT(""),
		WS_CHILD | /*WS_VISIBLE |*/ LVS_REPORT | LVS_SINGLESEL | LVS_SORTASCENDING ,
		5, g_tabheader_h, clientW - 10, g_charlist_h-5,
		g_rw_hWndTab, NULL, g_radardll_hinst, NULL );
	dwLvExStyle = 0;
	dwLvExStyle |= (LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	SendMessage( g_rw_hWndEnemies, LVM_SETEXTENDEDLISTVIEWSTYLE, (WPARAM)dwLvExStyle, (LPARAM)dwLvExStyle );
	SendMessage( g_rw_hWndEnemies, WM_SETFONT, (WPARAM)gr_font, FALSE ); // set font
	SendMessage( g_rw_hWndEnemies, LVM_SETIMAGELIST, LVSIL_NORMAL, (LPARAM)g_radarWnd_imageList ); // set ImageList for control
	SendMessage( g_rw_hWndEnemies, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)g_radarWnd_imageList ); // set ImageList for control
	// columns
	lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_FMT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.pszText = colText;
	lvc.cchTextMax = sizeof( colText ) / sizeof( colText[0] );
	// col 1
	lstrcpy( colText, TEXT("Name") );
	lvc.cx = 70;
	lvc.iSubItem = 0;
	SendMessage( g_rw_hWndEnemies, LVM_INSERTCOLUMN, (WPARAM)0, (LPARAM)&lvc );
	// col 2
	lstrcpy( colText, TEXT("cls/base cls") );
	lvc.cx = 100;
	lvc.iSubItem = 1;
	SendMessage( g_rw_hWndEnemies, LVM_INSERTCOLUMN, (WPARAM)1, (LPARAM)&lvc );
	// col 3
	lstrcpy( colText, TEXT("Clan") );
	lvc.cx = 70;
	lvc.iSubItem = 2;
	SendMessage( g_rw_hWndEnemies, LVM_INSERTCOLUMN, (WPARAM)2, (LPARAM)&lvc );
	// col 4
	lstrcpy( colText, TEXT("dZ") );
	lvc.cx = 30;
	lvc.iSubItem = 3;
	SendMessage( g_rw_hWndEnemies, LVM_INSERTCOLUMN, (WPARAM)3, (LPARAM)&lvc );
	// col 5
	lstrcpy( colText, TEXT("CW") );
	lvc.cx = 30;
	lvc.iSubItem = 4;
	SendMessage( g_rw_hWndEnemies, LVM_INSERTCOLUMN, (WPARAM)4, (LPARAM)&lvc );
	//

	// targets list
	g_rw_hWndTargets = CreateWindowEx( 0, WC_LISTVIEW, TEXT(""),
		WS_CHILD | /*WS_VISIBLE |*/ LVS_REPORT | LVS_SINGLESEL | LVS_SORTASCENDING ,
		5, g_tabheader_h, clientW - 10, g_charlist_h-5,
		g_rw_hWndTab, NULL, g_radardll_hinst, NULL );
	dwLvExStyle = 0;
	dwLvExStyle |= (LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	SendMessage( g_rw_hWndTargets, LVM_SETEXTENDEDLISTVIEWSTYLE, (WPARAM)dwLvExStyle, (LPARAM)dwLvExStyle );
	SendMessage( g_rw_hWndTargets, WM_SETFONT, (WPARAM)gr_font, FALSE ); // set font
	// columns
	lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_FMT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.pszText = colText;
	lvc.cchTextMax = sizeof( colText ) / sizeof( colText[0] );
	// col 1
	lstrcpy( colText, TEXT("Source") );
	lvc.cx = 100;
	lvc.iSubItem = 0;
	SendMessage( g_rw_hWndTargets, LVM_INSERTCOLUMN, (WPARAM)0, (LPARAM)&lvc );
	// col 2
	lstrcpy( colText, TEXT("Dest") );
	lvc.cx = 150;
	lvc.iSubItem = 1;
	SendMessage( g_rw_hWndTargets, LVM_INSERTCOLUMN, (WPARAM)1, (LPARAM)&lvc );
	//

	// button - debug dlg
#ifdef _DEBUG
	g_rw_hWndDebugBtn = CreateWindowEx( 0, TEXT("BUTTON"), TEXT("Debug"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		5, g_radar_h + g_checksrow_h + g_charlist_h + 5 + g_tabheader_h, clientW - 10, 20,
		hWnd, (HMENU)IDCB_DEBUGDLG, g_radardll_hinst, NULL );
#endif
	// globals init
	bRadarWindowIsVisible = FALSE;
	bRadarWindowShrunk = FALSE;
	RECT r;
	GetWindowRect( hWnd, &r );
	g_radarWnd_bigH = r.bottom - r.top;
	g_radarWnd_titleSet = 0;
	g_radarWnd_isInTray = 0;
	// set timer
	SetTimer( hWnd, 1, 1000, NULL );
	// setup update commands
	dwRadarDllLastCharListUpdateTime = GetTickCount();
	//CharArray_SetUpdateCommand( hWnd, WMMY_UPDATE_CHARS_LIST ); // removed
	//NpcArray_SetUpdateCommand( hWnd, WMMY_UPDATE_NPCS_LIST ); // removed
	g_game_client->setNotifyWindow( hWnd, WMMY_GAMECLIENT_NOTIFY );
	//g_login_client->setNotifyWindow( hWnd, WMMY_LOGINCLIENT_NOTIFY );

	// force UserAI to load settings
	g_game_client->ai.init();

	// start HWID check
	//RadarDllWnd_Start_HWID_Check( hWnd ); // OMG!!!

	// update checks state
	RW_UpdateChecksState();

	RadarDllWnd_RestoreWindowSize( hWnd );
	return 0;
}

LRESULT RW_OnWindowShowToggle( HWND hWnd )
{
	DWORD curStyle = GetWindowLong( hWnd, GWL_STYLE );
	if( curStyle & WS_VISIBLE )
	{
		ShowWindow( hWnd, SW_HIDE );
		bRadarWindowIsVisible = FALSE;
	}
	else
	{
		ShowWindow( hWnd, SW_SHOWNORMAL );
		bRadarWindowIsVisible = TRUE;
	}
	return 0;
}

/*LRESULT RW_OnSetUserCoords( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	// not used
	return 0;
}*/


LRESULT RW_OnCommand( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam);
	int ctrlID = LOWORD(wParam);
	int notifyCode = HIWORD(wParam);
	switch( ctrlID )
	{
	case IDCB_DEBUGDLG: DebugDialogStart( hWnd, g_radardll_hinst ); break;
	case IDCB_CONFIG:
		RadarSetupDlgStart( hWnd );
		RW_UpdateChecksState(); // update all checks to new radar config
		break;
	case IDCB_ABOUT:    AboutDialogStart( hWnd ); break;
	case IDCB_SHOW_NPC:
		{
			g_radarwnd_npc_displayEnable = false;
			if( IsDlgButtonChecked( hWnd, IDCB_SHOW_NPC ) == BST_CHECKED )
			{
				g_radarwnd_npc_displayEnable = true;
				RW_OnUpdateNpcsList( hWnd );
			}
		} break;
	case IDCB_AUTOHEAL:
		{
			int enable = IsDlgButtonChecked( hWnd, IDCB_AUTOHEAL );
			g_game_client->ai.setEnableHealCP( enable );
			g_game_client->ai.setEnableHealHP( enable );
			g_game_client->ai.setEnableHealMP( enable );
		} break;
	case IDCB_SOUNDENABLE:
		{
			int enable = IsDlgButtonChecked( hWnd, IDCB_SOUNDENABLE );
			if( enable ) g_game_client->ai.soundAlert_enabled = true;
			else g_game_client->ai.soundAlert_enabled = false;
		} break;
	case IDCB_FOLLOW:
		{
			int enable = IsDlgButtonChecked( hWnd, IDCB_FOLLOW );
			g_game_client->ai.folAsiCfg.m_follow_enable = enable;
			g_game_client->ai.folAsiCfg.saveToFile( "L2Detect_folAsi.ini" );
			if( g_game_client->ai.folAsiCfg.m_follow_enable == 0 )
				g_game_client->ai.followDisable(); // zero follow objectID
		} break;
	case IDCB_ASSIST:
		{
			int enable = IsDlgButtonChecked( hWnd, IDCB_ASSIST );
			g_game_client->ai.folAsiCfg.m_assist_enable = enable;
			g_game_client->ai.folAsiCfg.saveToFile( "L2Detect_folAsi.ini" );
			if( g_game_client->ai.folAsiCfg.m_assist_enable == 0 )
				g_game_client->ai.assistDisable(); // zero assist objectID
		} break;
	case IDCB_SHRINKWINDOW:
		{
			RECT r;
			int clientW;
			GetWindowRect( hWnd, &r );
			clientW = r.right - r.left;
			int small_H = g_radar_h +               // H of radar area
				GetSystemMetrics( SM_CYCAPTION ) +  // window title bar
				GetSystemMetrics( SM_CYFRAME ) +    // window frame
				g_checksrow_h;                      // H of checks row
			int new_val;
			if( bRadarWindowShrunk )
			{
				// enlarge
				MoveWindow( hWnd, r.left, r.top, clientW, g_radarWnd_bigH, TRUE );
				// g_radarWnd_bigH is saved at window creation on WM_CREATE
				new_val = FALSE;
#ifdef _DEBUG // show "Debug" button
				ShowWindow( g_rw_hWndDebugBtn, SW_SHOW );
#endif
			}
			else
			{
				// srink
				MoveWindow( hWnd, r.left, r.top, clientW, small_H, TRUE );
				new_val = TRUE;
#ifdef _DEBUG // hide "Debug" button
				ShowWindow( g_rw_hWndDebugBtn, SW_HIDE );
#endif
			}
			bRadarWindowShrunk = new_val;
		} break;
	case IDCB_TRAY:
		{
			NOTIFYICONDATA data;
			data.cbSize = sizeof( NOTIFYICONDATA );
			data.hWnd = hWnd;
			data.uID = TRAY_ICON_ID;
			data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			data.uCallbackMessage = WMMY_TRAYMESSAGE;
			data.hIcon = LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_L2_BLACK) );
			lstrcpy( data.szTip, TEXT("L2Detect") );
			//
			if( g_game_client->getState() == GCST_IN_GAME )
			{
				data.hIcon = LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_L2_GREEN) );
				wsprintf( data.szTip, TEXT("%s"), g_game_client->ai.usr.charName );
			}
			else if( g_game_client->getState() == GCST_AUTHED )
			{
				data.hIcon = LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_L2_PURPLE) );
				lstrcpy( data.szTip, TEXT("L2Detect") );
			}
			// add icon to tray
			if( Shell_NotifyIcon( NIM_ADD, &data ) )
			{
				ShowWindow( hWnd, SW_HIDE );
				g_radarWnd_isInTray = 1;
			}
		} break;
	case IDCB_TRANSPARENCY:
		{
			HMENU hMenu = CreatePopupMenu();
			int i;
			for( i=0; i<10; i++ )
			{
				TCHAR appStr[64];
				wsprintf( appStr, TEXT("%d%%"), 10+(i*10) ); // 10, 20, 30, ..., 100
				AppendMenu( hMenu, MF_STRING, 1010 + i*10, appStr ); // 1010, 1020, 1030, ..., 1100
			}
			POINT pt; GetCursorPos( &pt );
			int cmd = TrackPopupMenu( hMenu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, 0, hWnd, NULL );
			if( cmd>=1000 && cmd<=1100 )
			{
				// cmd [1000..1100] % transp
				float transp = 255.0f * (float)(cmd-1000) * 0.01f;
				// alpha: 0 - transparent, 255 - opaque
				SetLayeredWindowAttributes( hWnd, 0, (BYTE)(transp), LWA_ALPHA );
			}
			DestroyMenu( hMenu );
		} break;
	case IDCB_ONTOP:
		{
			//HWND hb = GetDlgItem( hWnd, IDCB_ONTOP );
			if( notifyCode == BN_CLICKED )
			{
				DWORD dwExStyle = GetWindowLong( hWnd, GWL_EXSTYLE );
				if( dwExStyle & WS_EX_TOPMOST )
				//if( st & BST_PUSHED )
				{
					dwExStyle |= (dwExStyle & !WS_EX_TOPMOST);
					SetWindowPos( hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
					//Button_SetState( hb, FALSE );
				}
				else
				{
					dwExStyle |= WS_EX_TOPMOST;
					SetWindowPos( hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
					//Button_SetState( hb, TRUE );
				}
				SetWindowLong( hWnd, GWL_EXSTYLE, dwExStyle );
			}
		} break;
	case IDCB_LUA: RW_OnButtonLua( hWnd ); break;
	}
	return 0;
}

LRESULT RW_OnSize( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(hWnd);
	if( wParam != SIZE_RESTORED ) return 0;
	int clientW = LOWORD(lParam);
	int clientH = HIWORD(lParam);
	//
	g_charlist_h = clientH - (g_radar_h + g_checksrow_h + g_tabheader_h);
#ifdef _DEBUG
	g_charlist_h -= 25;
#endif
	// tab: 0, g_radar_h + g_checksrow_h, clientW, charlist_h + tabheader_h,
	SetWindowPos( g_rw_hWndTab, NULL, 0, g_radar_h + g_checksrow_h, clientW, g_charlist_h + g_tabheader_h, SWP_NOACTIVATE | SWP_NOZORDER );
	// listviews: 5, g_tabheader_h, clientW - 10, g_charlist_h-5,
	SetWindowPos( g_rw_hWndCL,      NULL, 2, g_tabheader_h, clientW-8, g_charlist_h-5, SWP_NOACTIVATE | SWP_NOZORDER );
	SetWindowPos( g_rw_hWndNPC,     NULL, 2, g_tabheader_h, clientW-8, g_charlist_h-5, SWP_NOACTIVATE | SWP_NOZORDER );
	SetWindowPos( g_rw_hWndEnemies, NULL, 2, g_tabheader_h, clientW-8, g_charlist_h-5, SWP_NOACTIVATE | SWP_NOZORDER );
	SetWindowPos( g_rw_hWndTargets, NULL, 2, g_tabheader_h, clientW-8, g_charlist_h-5, SWP_NOACTIVATE | SWP_NOZORDER );
#ifdef _DEBUG
	SetWindowPos( g_rw_hWndDebugBtn, NULL, 5, clientH-25+2, clientW-10, 20, SWP_NOACTIVATE | SWP_NOZORDER );
	//5, g_radar_h + g_checksrow_h + g_charlist_h + 5 + g_tabheader_h, clientW - 10, 20,
#endif
	return 0;
}

LRESULT RW_OnKeyUp( HWND hWnd, WPARAM wParam )
{
	int vk_code = (int)wParam;
	// в ингейм режиме кнопка Пауза показывает/прячет окно радара
	if( (g_cfg.isInGameMode == true) && (vk_code == VK_PAUSE) )
	{
		PostMessage( hWnd, WMMY_WNDSHOWTOGGLE, 0, 0 );
		return 0;
	}
	g_game_client->ai.notifyUIKeyUp( vk_code );
	return 0;
}

LRESULT RW_OnNotify( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(wParam);
	NMHDR *pnmh = (NMHDR *)lParam;
	if( !pnmh ) return 0;
	// from chars list
	if( pnmh->hwndFrom == g_rw_hWndCL )
	{
		switch( pnmh->code )
		{
		case NM_DBLCLK:
			{
				NMITEMACTIVATE *pnmia = (NMITEMACTIVATE *)lParam;
				if( pnmia->iItem < 0 ) return 0;
				LVITEM lvi;
				lvi.iItem = pnmia->iItem;
				lvi.iSubItem = 0;
				lvi.mask = LVIF_PARAM;
				lvi.lParam = 0;
				SendMessage( g_rw_hWndCL, LVM_GETITEM, 0, (LPARAM)&lvi );
				//log_error( LOG_OK, "LV NM_DBLCLK: item activate =  %d, lParam = %u\n",
				//	pnmia->iItem, (unsigned int)lvi.lParam );
				g_game_client->ai.lockTargetMgr.onUserRequestAction( (unsigned int)lvi.lParam );
				// TODO: Action coordinates!
				PGen_Action( (unsigned int)lvi.lParam,
					g_game_client->ai.usr.x,
					g_game_client->ai.usr.y,
					g_game_client->ai.usr.z, 0 );
			} break;
		}
	}

	// from NPCs list
	if( pnmh->hwndFrom == g_rw_hWndNPC )
	{
		switch( pnmh->code )
		{
		case NM_DBLCLK:
			{
				NMITEMACTIVATE *pnmia = (NMITEMACTIVATE *)lParam;
				if( pnmia->iItem < 0 ) return 0;
				LVITEM lvi;
				lvi.iItem = pnmia->iItem;
				lvi.iSubItem = 0;
				lvi.mask = LVIF_PARAM;
				lvi.lParam = 0;
				SendMessage( g_rw_hWndNPC, LVM_GETITEM, 0, (LPARAM)&lvi );
				//log_error( LOG_OK, "LV NM_DBLCLK: item activate =  %d, lParam = %u\n",
				//	pnmia->iItem, (unsigned int)lvi.lParam );
				g_game_client->ai.lockTargetMgr.onUserRequestAction( (unsigned int)lvi.lParam );
				// TODO: Action coordinates!
				PGen_Action( (unsigned int)lvi.lParam,
					g_game_client->ai.usr.x,
					g_game_client->ai.usr.y,
					g_game_client->ai.usr.z, 0 );
			} break;
		}
	}

	// from Enemies list
	if( pnmh->hwndFrom == g_rw_hWndEnemies )
	{
		switch( pnmh->code )
		{
		case NM_DBLCLK:
			{
				NMITEMACTIVATE *pnmia = (NMITEMACTIVATE *)lParam;
				if( pnmia->iItem < 0 ) return 0;
				LVITEM lvi;
				lvi.iItem = pnmia->iItem;
				lvi.iSubItem = 0;
				lvi.mask = LVIF_PARAM;
				lvi.lParam = 0;
				SendMessage( g_rw_hWndCL, LVM_GETITEM, 0, (LPARAM)&lvi );
				//log_error( LOG_OK, "LV NM_DBLCLK: item activate =  %d, lParam = %u\n",
				//	pnmia->iItem, (unsigned int)lvi.lParam );
				g_game_client->ai.lockTargetMgr.onUserRequestAction( (unsigned int)lvi.lParam );
				// TODO: Action coordinates!
				PGen_Action( (unsigned int)lvi.lParam,
					g_game_client->ai.usr.x,
					g_game_client->ai.usr.y,
					g_game_client->ai.usr.z, 0 );
			} break;
		}
	}

	// tab changes
	if( pnmh->hwndFrom == g_rw_hWndTab )
	{
		int cur_tab = TabCtrl_GetCurSel( g_rw_hWndTab );
		switch( pnmh->code )
		{
		case TCN_SELCHANGING: // tab is hiding
			{
				switch( cur_tab )
				{
				case 0: ShowWindow( g_rw_hWndCL, SW_HIDE ); break;
				case 1: ShowWindow( g_rw_hWndNPC, SW_HIDE ); break;
				case 2: ShowWindow( g_rw_hWndEnemies, SW_HIDE ); break;
				case 3: ShowWindow( g_rw_hWndTargets, SW_HIDE ); break;
				}
			} break;
		case TCN_SELCHANGE: // tab changed to
			{
				switch( cur_tab )
				{
				case 0: ShowWindow( g_rw_hWndCL, SW_SHOW ); break;
				case 1: ShowWindow( g_rw_hWndNPC, SW_SHOW ); break;
				case 2: ShowWindow( g_rw_hWndEnemies, SW_SHOW ); break;
				case 3: ShowWindow( g_rw_hWndTargets, SW_SHOW ); break;
				}
				g_radarWnd_curTab = cur_tab;
			} break;
		}
	}

	return 0;
}


LRESULT RW_OnPaint( HWND hWnd )
{
	//
	RECT rcClient;
	GetClientRect( hWnd, &rcClient );
	//int clientW = rcClient.right - rcClient.left; // local variable is initialized but not referenced
	// old GDI objects
	HPEN oldPen;
	HBRUSH oldBrush;
	// begin paint
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint( hWnd, &ps );
	// save old GDI objects
	oldPen = (HPEN)SelectObject( hdc, (HGDIOBJ)gr_grayPen ); // gray pen,
	oldBrush = (HBRUSH)SelectObject( hdc, GetStockObject(BLACK_BRUSH) ); // black brush
	// radar circle
	Ellipse( hdc,
		rcClient.right/2 - g_radar_h2, 0,
		rcClient.right/2 + g_radar_h2, g_radar_h );
	// restore old GDI objects, end paint
	SelectObject( hdc, (HGDIOBJ)oldBrush );
	SelectObject( hdc, (HGDIOBJ)oldPen );
	EndPaint( hWnd, &ps );
	//ValidateRect( hWnd, NULL ); // BeginPaint/EndPaint pair already do ValidateRect
	return 0;
}

//int rw_timer_calls = 0;
LRESULT RW_OnTimer( HWND hWnd, WPARAM wParam )
{
	// in game?
	if( g_game_client->getState() != GCST_IN_GAME ) return 0;
	if( g_game_client->getState() == GCST_IN_GAME && !g_radarWnd_titleSet )
	{
		if( g_game_client->ai.usr.charName[0] != 0 )
			RW_UpdateWindowTitle_and_TrayIconTip( hWnd );
	}

	switch( wParam )
	{
	case 1:
	{

	//
	int user_x = g_game_client->ai.usr.x;
	int user_y = g_game_client->ai.usr.y;
	int user_xDst = g_game_client->ai.usr.xDst;
	int user_yDst = g_game_client->ai.usr.yDst;
	int delta_coord_x, delta_coord_y;
	int delta_wnd_x, delta_wnd_y;
	int i;
	RECT rcClient;
	GetClientRect( hWnd, &rcClient );
	int clientW = rcClient.right - rcClient.left;
	//int clientH = rcClient.bottom - rcClient.top;
	int clientW2 = clientW/2;
	int radar_cross_len = 5;
	HDC hdc = NULL;
	if( !g_radarWnd_drawOnClient ) hdc = GetDC( hWnd );
	else
	{
		log_error( LOG_DEBUG, "... drawing on client ...\n" );
		hdc = GetDC( g_hWndL2 );
		hWnd = g_hWndL2;
	}

	// draw color
	COLORREF crColor = 0;

	// old GDI objects
	HPEN oldPen;
	HBRUSH oldBrush;
	// save old GDI objects
	oldPen   = (HPEN)SelectObject( hdc, GetStockObject(NULL_PEN) ); // empty pen,
	oldBrush = (HBRUSH)SelectObject( hdc, GetStockObject(BLACK_BRUSH) ); // black brush

	// clear radar area
	//Rectangle( hdc, 0, 0, rcClient.right, g_radar_h );
	Rectangle( hdc, 70, 0, clientW, g_radar_h );
	Rectangle( hdc, 0, 20, clientW, g_radar_h );
	// radar circle
	SelectObject( hdc, (HGDIOBJ)gr_grayPen ); // gray pen,
	Ellipse( hdc,
		rcClient.right/2 - g_radar_h2, 0,
		rcClient.right/2 + g_radar_h2, g_radar_h );
	// radar center cross
	MoveToEx( hdc, clientW2, g_radar_h2 - radar_cross_len, NULL );
	LineTo( hdc, clientW2, g_radar_h2 + radar_cross_len + 1 );
	MoveToEx( hdc, clientW2 - radar_cross_len, g_radar_h2, NULL );
	LineTo( hdc, clientW2  + radar_cross_len + 1, g_radar_h2 );
	SetPixel( hdc, clientW2, g_radar_h2, RGB(200,200,200) );
	// user move vector
	if( (user_xDst != 0x7FFFFFFF) && (user_yDst != 0x7FFFFFFF) )
	{
		delta_coord_x = user_xDst - user_x;
		delta_coord_y = user_yDst - user_y;
		delta_wnd_x = (int)( (float)delta_coord_x * g_radar_scale );
		delta_wnd_y = (int)( (float)delta_coord_y * g_radar_scale );
		SelectObject( hdc, (HGDIOBJ)gr_darkGrayPen_dash );
		MoveToEx( hdc, clientW2, g_radar_h2, NULL );
		LineTo( hdc, clientW2 + delta_wnd_x, g_radar_h2 + delta_wnd_y );
	}

	// npcs
	if( g_radarwnd_npc_displayEnable )
	{
		NpcArray_Lock();
		for( i=0; i<NPCA_MAX_NPCS; i++ )
		{
			if( npc_array[i] == NULL ) continue;
			delta_coord_x = npc_array[i]->x - user_x;
			delta_coord_y = npc_array[i]->y - user_y;
			delta_wnd_x = (int)( (float)delta_coord_x * g_radar_scale );
			delta_wnd_y = (int)( (float)delta_coord_y * g_radar_scale );
			// "show" NPC :)
			crColor = g_rwCfg.ui.colors.npcColor; // simple NPC
			if( npc_array[i]->isAttackable ) crColor = g_rwCfg.ui.colors.mobColor; // mob!
			SetPixel( hdc,
				clientW2 + delta_wnd_x,
				g_radar_h2 + delta_wnd_y,
				crColor );
			SetPixel( hdc,
				clientW2 + delta_wnd_x + 1,
				g_radar_h2 + delta_wnd_y,
				crColor );
			SetPixel( hdc,
				clientW2 + delta_wnd_x,
				g_radar_h2 + delta_wnd_y + 1,
				crColor );
			SetPixel( hdc,
				clientW2 + delta_wnd_x + 1,
				g_radar_h2 + delta_wnd_y + 1,
				crColor );
		}
		NpcArray_Unlock();
	}

	// chars
	unsigned int RELATION_ENEMY      = 0x01000;
	unsigned int RELATION_MUTUAL_WAR = 0x08000;
	unsigned int RELATION_1SIDED_WAR = 0x10000;
	if( g_cfg.L2_version == L2_VERSION_T23 )
	{
		RELATION_MUTUAL_WAR = 0x04000; // gracia final constants changed
		RELATION_1SIDED_WAR = 0x08000;
	}
	SelectObject( hdc, (HGDIOBJ)gr_bluePen );
	CharArray_Lock();
	for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
	{
		//CharInfo *cha = chars_array[i];
		L2Player *cha = chars_array[i];
		if( !cha ) continue;
		if( cha->isUnused() ) continue;
		delta_coord_x = chars_array[i]->x - user_x;
		delta_coord_y = chars_array[i]->y - user_y;
		delta_wnd_x = (int)( (float)delta_coord_x * g_radar_scale );
		delta_wnd_y = (int)( (float)delta_coord_y * g_radar_scale );
		// "show" char :)
		crColor = g_rwCfg.ui.colors.pcColor; // default pc color
		// maybe char dead?
		if( cha->isAlikeDead ) crColor = g_rwCfg.ui.colors.pcColorDead; // if character is dead
		// maybe war color?
		if( (cha->relation & RELATION_1SIDED_WAR) || (cha->relation & RELATION_MUTUAL_WAR)
			|| (cha->relation & RELATION_ENEMY) )
		{
			crColor = g_rwCfg.ui.colors.pcColorWar;
			if( cha->isAlikeDead ) crColor = g_rwCfg.ui.colors.pcColorWarDead; // if character is enemy and dead
		}
		// maybe party member?
		if( g_game_client->ai.party.isInParty( cha->objectID, NULL ) )
		{
			crColor = g_rwCfg.ui.colors.pcColorParty;
			if( cha->isAlikeDead ) crColor = g_rwCfg.ui.colors.pcColorPartyDead; // dead party member
		}
		SetPixel( hdc,
			clientW2 + delta_wnd_x,
			g_radar_h2 + delta_wnd_y,    crColor );
		SetPixel( hdc,
			clientW2 + delta_wnd_x + 1,
			g_radar_h2 + delta_wnd_y,    crColor );
		SetPixel( hdc,
			clientW2 + delta_wnd_x,
			g_radar_h2 + delta_wnd_y + 1,    crColor );
		SetPixel( hdc,
			clientW2 + delta_wnd_x + 1,
			g_radar_h2 + delta_wnd_y + 1,    crColor );
		// is this char on user target?
		if( g_game_client->ai.usr.targetObjectID == cha->objectID )
		{
			// display circle around
			SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
			Ellipse( hdc, clientW2 + delta_wnd_x - 2, g_radar_h2 + delta_wnd_y - 2,
				clientW2 + delta_wnd_x + 3, g_radar_h2 + delta_wnd_y + 3 );
		}
	}
	CharArray_Unlock();

	// current User target
	if( g_game_client->ai.usr.targetObjectID != 0 )
	{
		SelectObject( hdc, (HGDIOBJ)gr_font );
		SetBkMode( hdc, TRANSPARENT );
		L2OBJECT_TYPE objType = L2OT_NONE;
		int idx = -1;
		TCHAR targetstr[256] = {0};
		TCHAR tstradd[64] = {0};
		// targeting self?
		int target_text_x = 1;
		int target_text_y = g_radar_h - 20;
		if( g_game_client->ai.usr.objectID == g_game_client->ai.usr.targetObjectID )
		{
			TextOut( hdc, target_text_x, target_text_y, TEXT("Self"), 4 );
		} // else targeting someone else in the world?
		else if( WorldObjectTree_GetInfoByObjectID( g_game_client->ai.usr.targetObjectID, &objType, &idx ) )
		{
			// target name
			if( objType == L2OT_NPC )
			{
				SetTextColor( hdc, RGB(0,255,0));
				_tcscat( targetstr, TEXT("NPC: ") );
				if( npc_array[idx]->isUnused() )
				{
					_tcscpy( tstradd, TEXT("<Unused NPC!!!>") );
				}
				else
				{
					_tcscpy( tstradd, npc_array[idx]->getName() );
					if( tstradd[0] == 0 ) wsprintf( tstradd, TEXT("npcid_%u"), npc_array[idx]->templateID );
				}
				_tcscat( targetstr, tstradd );
			}
			else if( objType == L2OT_PC )
			{
				SetTextColor( hdc, RGB(255,255,255));
				if( chars_array[idx]->isUnused() )
				{
					_tcscat( targetstr, TEXT("<Unused char!!!>") );
				}
				else
				{
					_tcscat( targetstr, chars_array[idx]->getName() );
				}
				// add clanwar state to targetstr
				unsigned int relation = chars_array[idx]->relation;
				if( relation != L2Player::RELATION_NONE )
				{
					tstradd[0] = 0;
					if( relation & L2Player::RELATION_1SIDED_WAR ) _tcscpy( tstradd, TEXT("<") );
					if( relation & L2Player::RELATION_MUTUAL_WAR ) _tcscpy( tstradd, TEXT("<>") );
					if( tstradd[0] ) _tcscat( targetstr, tstradd );
				}
			}
			// target HP
			if( (g_game_client->ai.usr.targetCurHP > 0) || (g_game_client->ai.usr.targetMaxHP > 0) )
			{
				tstradd[0] = 0;
				if( g_game_client->ai.usr.targetMaxHP > 0 )
				{
					int ipercent = (int)( 100.0 * ((double)g_game_client->ai.usr.targetCurHP) / 
						((double)g_game_client->ai.usr.targetMaxHP) );
					wsprintf( tstradd, TEXT(" HP: [%d / %d] (%d%%)"),
						g_game_client->ai.usr.targetCurHP, g_game_client->ai.usr.targetMaxHP,
						ipercent );
				}
				else
				{
					wsprintf( tstradd, TEXT(" HP: [%d]"), g_game_client->ai.usr.targetCurHP );
				}
				_tcscat( targetstr, tstradd );
			}
			// draw :)
			TextOut( hdc, target_text_x, target_text_y, targetstr, _tcslen(targetstr) );
		}
	}

	// restore old GDI objects, end paint
	SelectObject( hdc, (HGDIOBJ)oldBrush );
	SelectObject( hdc, (HGDIOBJ)oldPen );
	ReleaseDC( hWnd, hdc );
	ValidateRect( hWnd, NULL );
	}

	break;
	} // switch

	return 0;
}

LRESULT RW_OnTrayMessage( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	switch( wParam )
	{
	case TRAY_ICON_ID:
		{
			switch( lParam )
			{
			case WM_LBUTTONUP:
				{
					NOTIFYICONDATA data;
					data.cbSize = sizeof( NOTIFYICONDATA );
					data.hWnd = hWnd;
					data.uID = TRAY_ICON_ID;
					data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
					data.uCallbackMessage = WMMY_TRAYMESSAGE;
					data.hIcon = LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_MFC) );
					data.szTip[0] = 0;
					Shell_NotifyIcon( NIM_DELETE, &data );
					ShowWindow( hWnd, SW_SHOW );
					g_radarWnd_isInTray = 0;
				} break;
			}
		} break;
	}
	return 0;
}

void RW_UpdateWindowTitle_and_TrayIconTip( HWND hWnd )
{
	g_radarWnd_titleSet = 0;
	TCHAR text[256] = {0};
	HICON hIcon = LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_L2_BLACK) );
	TCHAR l2detect_text[64];
	if( g_cfg.isInGameMode )
		wsprintf( l2detect_text, TEXT("IG L2Detect") );
	else
		wsprintf( l2detect_text, TEXT("L2Detect") );
	if( g_game_client->getState() == GCST_OFFLINE )
	{
		SetWindowText( hWnd, l2detect_text );
	}
	else if( g_game_client->getState() == GCST_CONNECTED )
	{
		hIcon = LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_L2_PURPLE) );
		wsprintf( text, TEXT("<Connected> - %s"), l2detect_text );
		SetWindowText( hWnd, text );
	}
	else if( g_game_client->getState() == GCST_AUTHED )
	{
		hIcon = LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_L2_PURPLE) );
		wsprintf( text, TEXT("<Char select> - %s"), l2detect_text );
		SetWindowText( hWnd, text );
	}
	else if( g_game_client->getState() == GCST_IN_GAME )
	{
		hIcon = LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_L2_GREEN) );
		if( g_game_client->ai.usr.charName[0] == 0 )
		{
			wsprintf( text, TEXT("<Entering game> - %s"), l2detect_text );
		}
		else
		{
			wsprintf( text, TEXT("%s - %s"), g_game_client->ai.usr.charName, l2detect_text );
			g_radarWnd_titleSet = 1; // got UserInfo... now we know charName
		}
		SetWindowText( hWnd, text );
	}
	SendMessage( hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon );
	SendMessage( hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon );

	// update tray icon only if window is minimized to tray
	if( !g_radarWnd_isInTray ) return;
	NOTIFYICONDATA data;
	data.cbSize = sizeof( NOTIFYICONDATA );
	data.hWnd = hWnd;
	data.uID = TRAY_ICON_ID;
	data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	data.uCallbackMessage = WMMY_TRAYMESSAGE;
	data.hIcon = LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_L2_BLACK) );
	lstrcpy( data.szTip, TEXT("L2Detect") );
	if( g_game_client->getState() == GCST_IN_GAME )
	{
		data.hIcon = LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_L2_GREEN) );
		wsprintf( data.szTip, TEXT("%s"), g_game_client->ai.usr.charName );
	}
	else if( g_game_client->getState() == GCST_AUTHED )
	{
		data.hIcon = LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_L2_PURPLE) );
		lstrcpy( data.szTip, TEXT("<Char select> - L2Detect") );
	}

	// check for login
	if( g_game_client->getState() == GCST_OFFLINE )
	{
		if( g_login_client )
		{
			log_error( LOG_OK, "RW_UpdateWindowTitle_and_TrayIconTip(): g_login_client state %d\n", g_login_client->getState() );
			switch( g_login_client->getState() )
			{
			case LCST_CONNECTED:
			case LCST_AUTHED_GG:
			case LCST_AUTHED_LOGIN:
				SetWindowText( hWnd, TEXT("<Login> - L2Detect") );
				data.hIcon = LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_L2_BLUE) );
				lstrcpy( data.szTip, TEXT("Login...") );
				break;
			}
		}
	}
	Shell_NotifyIcon( NIM_MODIFY, &data );
}

LRESULT RW_OnGameClientNotify( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam);
	switch( wParam )
	{
	case GCN_STATECHANGE: RW_UpdateWindowTitle_and_TrayIconTip( hWnd ); break;
	}
	return 0;
}

LRESULT RW_OnLoginClientNotify( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	switch( wParam )
	{
	case LCN_STATECHANGE:
		{
			//log_error( LOG_DEBUG, "RW_OnLoginClientNotify(): lParam %d\n", (int)lParam );
			switch( lParam )
			{
			case 1:
				{
					SetWindowText( hWnd, TEXT("<Login> - L2Detect") );
					HICON hIcon = LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_L2_BLUE) );
					SendMessage( hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon );
					SendMessage( hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon );
					// update tray icon only if window is minimized to tray
					if( g_radarWnd_isInTray )
					{
						NOTIFYICONDATA data;
						data.cbSize = sizeof( NOTIFYICONDATA );
						data.hWnd = hWnd;
						data.uID = TRAY_ICON_ID;
						data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
						data.uCallbackMessage = WMMY_TRAYMESSAGE;
						data.hIcon = hIcon;
						lstrcpy( data.szTip, TEXT("<Login> - L2Detect") );
						Shell_NotifyIcon( NIM_MODIFY, &data );
					}
				} break;
			case 0:
				{
					SetWindowText( hWnd, TEXT("L2Detect") );
					HICON hIcon = LoadIcon( g_radardll_hinst, MAKEINTRESOURCE(IDI_L2_BLACK) );
					SendMessage( hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon );
					SendMessage( hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon );
					// update tray icon only if window is minimized to tray
					if( g_radarWnd_isInTray )
					{
						NOTIFYICONDATA data;
						data.cbSize = sizeof( NOTIFYICONDATA );
						data.hWnd = hWnd;
						data.uID = TRAY_ICON_ID;
						data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
						data.uCallbackMessage = WMMY_TRAYMESSAGE;
						data.hIcon = hIcon;
						lstrcpy( data.szTip, TEXT("L2Detect") );
						Shell_NotifyIcon( NIM_MODIFY, &data );
					}
				} break;
			}
		} break;
	}
	return 0;
}


bool RadarWnd_NotifyTarget_Update( unsigned int oid_from, unsigned int oid_to );
bool RadarWnd_NotifyTarget_Add( unsigned int oid_from, unsigned int oid_to );

void RadarWnd_NotifyTarget( unsigned int oid_from, unsigned int oid_to, bool selected /*= true*/ )
{
	if( selected )
	{
		// first try to update
		if( RadarWnd_NotifyTarget_Update( oid_from, oid_to ) ) return;
		// update failed - no pair existed
		// add new pair Src -> Dst
		TCHAR src_name[256] = {0};
		TCHAR dst_name[256] = {0};
		_tcscpy( src_name, _T("<?>") );
		_tcscpy( dst_name, _T("<?>") );
		int added_item = -1;

		L2OBJECT_TYPE objType = L2OT_NONE;
		int idx = -1;
		if( oid_from == g_game_client->ai.usr.objectID )
			_tcscpy( src_name, _T(" <You> ") );
		else
		{
			WorldObjectTree_GetInfoByObjectID( oid_from, &objType, &idx );
			if( (objType == L2OT_PC) && (idx>=0) )
			{
				CharArray_Lock();
				L2Player *cha = chars_array[idx];
				if( cha )
					if( !cha->isUnused() )
						_tcscpy( src_name, cha->getName() );
				CharArray_Unlock();
			}
			else if( (objType == L2OT_NPC) && (idx>=0) )
			{
				NpcArray_Lock();
				L2Npc *cha = npc_array[idx];
				if( cha )
					if( !cha->isUnused() )
						_tcscat( _tcscpy( src_name, _T("NPC: ") ), cha->getName() );
				NpcArray_Unlock();
			}
		}

		if( oid_to == g_game_client->ai.usr.objectID )
			_tcscpy( dst_name, _T(" <You> ") );
		else
		{
			WorldObjectTree_GetInfoByObjectID( oid_to, &objType, &idx );
			if( (objType == L2OT_PC) && (idx>=0) )
			{
				CharArray_Lock();
				L2Player *cha = chars_array[idx];
				if( cha )
					if( !cha->isUnused() )
						_tcscpy( dst_name, cha->getName() );
				CharArray_Unlock();
			}
			else if( (objType == L2OT_NPC) && (idx>=0) )
			{
				NpcArray_Lock();
				L2Npc *cha = npc_array[idx];
				if( cha )
					if( !cha->isUnused() )
						_tcscat( _tcscpy( dst_name, _T("NPC: ") ), cha->getName() );
				NpcArray_Unlock();
			}
		}

		LVITEM lvi;
		memset( &lvi, 0, sizeof(lvi) );
		lvi.cchTextMax = 128;
		lvi.pszText = NULL;
		// name src
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iItem = 999;
		lvi.iSubItem = 0;
		lvi.lParam = oid_from;
		lvi.pszText = src_name;
		added_item = (int)SendMessage( g_rw_hWndTargets, LVM_INSERTITEM, 0, (LPARAM)&lvi );
		if( added_item == -1 ) return;
		// name dst
		lvi.mask = LVIF_TEXT;
		lvi.iItem = added_item;
		lvi.iSubItem = 1;
		lvi.pszText = dst_name;
		SendMessage( g_rw_hWndTargets, LVM_SETITEM, 0, (LPARAM)&lvi );
	}
	else
	{
		// delete pair
		LVITEM lvi;
		memset( &lvi, 0, sizeof(lvi) );
		int iItem = -1;
		// loop all listview items, find our oid_from and delete whole row
		do
		{
			iItem = (int)SendMessage( g_rw_hWndTargets, LVM_GETNEXTITEM, iItem, LVNI_ALL );
			if( iItem == -1 ) break;
			lvi.mask = LVIF_PARAM;
			lvi.iItem = iItem;
			lvi.iSubItem = 0;
			if( SendMessage( g_rw_hWndTargets, LVM_GETITEM, 0, (LPARAM)&lvi ) )
			{
				if( ((unsigned int)lvi.lParam) == oid_from )
				{
					SendMessage( g_rw_hWndTargets, LVM_DELETEITEM, iItem, 0 );
					return;
				}
			}
		} while( iItem >= 0 );
		// normally should not reach here...
		//log_error( LOG_WARNING, "RadarWnd_NotifyTarget(): cannot find objectId %u to remove target\n", oid_from );
	}
}

// update target oid_from -> oid_to
// if updated, returns true
// if not existed, returns false
bool RadarWnd_NotifyTarget_Update( unsigned int oid_from, unsigned int oid_to )
{
	LVITEM lvi;
	memset( &lvi, 0, sizeof(lvi) );
	int iItem = -1;
	// loop all listview items, find our oid_from and delte whole row
	do
	{
		iItem = (int)SendMessage( g_rw_hWndTargets, LVM_GETNEXTITEM, iItem, LVNI_ALL );
		if( iItem == -1 ) break;
		lvi.mask = LVIF_PARAM;
		lvi.iItem = iItem;
		lvi.iSubItem = 0;
		if( SendMessage( g_rw_hWndTargets, LVM_GETITEM, 0, (LPARAM)&lvi ) )
		{
			if( ((unsigned int)lvi.lParam) == oid_from )
			{
				TCHAR dst_name[256] = {0};
				_tcscpy( dst_name, _T("<?>") );
				if( oid_to == g_game_client->ai.usr.objectID )
					_tcscpy( dst_name, _T(" <You> ") );
				else
				{
					// get target name
					L2OBJECT_TYPE objType = L2OT_NONE;
					int idx = -1;
					WorldObjectTree_GetInfoByObjectID( oid_to, &objType, &idx );
					if( (objType == L2OT_PC) && (idx>=0) )
					{
						CharArray_Lock();
						L2Player *cha = chars_array[idx];
						if( cha )
							if( !cha->isUnused() )
								_tcscpy( dst_name, cha->getName() );
						CharArray_Unlock();
					}
					else if( (objType == L2OT_NPC) && (idx>=0) )
					{
						NpcArray_Lock();
						L2Npc *cha = npc_array[idx];
						if( cha )
							if( !cha->isUnused() )
								_tcscat( _tcscpy( dst_name, _T("NPC: ") ), cha->getName() );
						NpcArray_Unlock();
					}
				}
				// name dst
				lvi.mask = LVIF_TEXT;
				lvi.iItem = iItem;
				lvi.iSubItem = 1;
				lvi.pszText = dst_name;
				SendMessage( g_rw_hWndTargets, LVM_SETITEM, 0, (LPARAM)&lvi );
				return true;
			}
		}
	}
	while( iItem >= 0 );
	return false; // not found :(
}


LRESULT RW_OnForceUpdateList( WPARAM wParam )
{
	switch( wParam )
	{
	case FORCE_UPDATE_CHARS_LIST: RW_OnUpdateCharsList( g_radardll_hwnd ); break;
	case FORCE_UPDATE_NPCS_LIST:  RW_OnUpdateNpcsList( g_radardll_hwnd ); break;
	}
	return 0;
}

void RadarWnd_UpdateChecksState()
{
	PostMessage( g_radardll_hwnd, WMMY_UPDATE_CHECKS, 0, 0 );
}

LRESULT RW_UpdateChecksState()
{
	if( !g_game_client ) return 0; // assert
	//log_error( LOG_OK, "RW_UpdateChecksState()\n" );
	// update state of checks: 'NPC', 'UseItem', 'Snd', 'Follow', 'Assist'
	HWND hWndB = NULL;
	// NPC
	hWndB = GetDlgItem( g_radardll_hwnd, IDCB_SHOW_NPC );
	if( g_radarwnd_npc_displayEnable ) Button_SetCheck( hWndB, TRUE ); else Button_SetCheck( hWndB, FALSE );
	// UseItem
	hWndB = GetDlgItem( g_radardll_hwnd, IDCB_AUTOHEAL );
	if( g_game_client->ai.isHealingEnabledAny() ) Button_SetCheck( hWndB, TRUE ); else Button_SetCheck( hWndB, FALSE );
	// Sounds
	hWndB = GetDlgItem( g_radardll_hwnd, IDCB_SOUNDENABLE );
	if( g_game_client->ai.soundAlert_enabled ) Button_SetCheck( hWndB, TRUE ); else Button_SetCheck( hWndB, FALSE );
	// Follow
	hWndB = GetDlgItem( g_radardll_hwnd, IDCB_FOLLOW );
	if( g_game_client->ai.folAsiCfg.m_follow_enable ) Button_SetCheck( hWndB, TRUE ); else Button_SetCheck( hWndB, FALSE );
	// Assist
	hWndB = GetDlgItem( g_radardll_hwnd, IDCB_ASSIST );
	if( g_game_client->ai.folAsiCfg.m_assist_enable ) Button_SetCheck( hWndB, TRUE ); else Button_SetCheck( hWndB, FALSE );
	return 0;
}

void RW_OnButtonLua( HWND hWnd )
{
	ScriptEngine *se = ScriptEngine::getInstance();
	HMENU hMenu = CreatePopupMenu();
	std::map<int, std::string> files;
	if( se->isScriptThreadRunning() )
	{
		UINT ufl = MF_STRING;
		if( se->isScriptThreadPaused() ) ufl = MF_STRING | MF_GRAYED;
		AppendMenu( hMenu, ufl, 100, TEXT("Stop script") );
		if( se->isScriptThreadPaused() )
			AppendMenu( hMenu, MF_STRING, 102, TEXT("Resume script") );
		else
			AppendMenu( hMenu, MF_STRING, 101, TEXT("Pause script") );
	}
	else
	{
		// create files list
		char curDir[256] = {0};
		char tmpl[256] = {0};
		GetCurrentDirectoryA( 256, curDir );
		sprintf( tmpl, "%s\\*.lua*", curDir );
		WIN32_FIND_DATAA data;
		HANDLE hFind = FindFirstFileA( tmpl, &data );
		if( hFind == INVALID_HANDLE_VALUE ) return;
		sprintf( tmpl, "%s\\%s", curDir, data.cFileName );
		files[0] = std::string( tmpl );
		int idx = 1;
		while( FindNextFileA( hFind, &data ) )
		{
			sprintf( tmpl, "%s\\%s", curDir, data.cFileName );
			files[idx] = std::string( tmpl );
			idx++;
		}
		FindClose( hFind );
		//
		std::map<int, std::string>::const_iterator iter;
		for( iter=files.begin(); iter!=files.end(); iter++ )
		{
			//log_error( LOG_OK, "%S\n", iter->second.c_str() );
			AppendMenuA( hMenu, MF_STRING, 1000+iter->first, iter->second.c_str() );
		}
	}
	POINT pt;
	GetCursorPos( &pt );
	int cmd = TrackPopupMenu( hMenu,
		TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD | TPM_NONOTIFY,
		pt.x, pt.y, 0, hWnd, NULL );
	DestroyMenu( hMenu );
	//
	if( cmd >= 1000 )
	{
		// run script
		int idx = cmd - 1000;
		std::string stds = files[idx];
		se->startScriptThread( stds.c_str() );
	}
	else
	{
		switch( cmd )
		{
		case 100: // stop
			{
				if( !se->stopScriptThread( 2000, false ) )
				{
					int yes = MessageBox( hWnd, TEXT("Wait failed, terminate script?"),
						TEXT("ScriptEngine warning"), MB_ICONQUESTION | MB_YESNO );
					if( yes == IDYES )
						se->stopScriptThread( 5000, true );
				}
			} break;
		case 101: // pause
			se->pauseScriptThread( true );
			break;
		case 102: // resume
			se->pauseScriptThread( false );
			break;
		}
	}
}
