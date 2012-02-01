#include "stdafx.h"
#include "Resource.h"
#include "Logger.h"
#include "L2Client.h"

// √лобальные переменные:
extern HINSTANCE g_hInst; // текущий экземпл€р
extern HWND g_hWnd;       // главное окно приложени€
extern HWND g_hWndTab;    // TabControl приложени€

#define MAX_ACCOUNTS 27
L2Client tabs[MAX_ACCOUNTS];
HFONT hFontTahoma10;
int cur_tab = -1;

void Tabs_Init()
{
	int i;
	for( i=0; i<MAX_ACCOUNTS; i++ ) tabs[i].setUnused();
	hFontTahoma10 = CreateFont( -11, 0, 0, 0, FW_NORMAL, 0, 0, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, FF_DONTCARE, TEXT("Tahoma") );
	SendMessage( g_hWndTab, WM_SETFONT, (WPARAM)hFontTahoma10, TRUE );
}

int Tabs_FindUnused()
{
	int i;
	for( i=0; i<MAX_ACCOUNTS; i++ ) if( tabs[i].isUnused() ) return i;
	return -1;
}

bool Tabs_AccountExists( Account *pAccount )
{
	if( !pAccount ) return false;
	int i;
	Account acc;
	for( i=0; i<MAX_ACCOUNTS; i++ )
	{
		if( tabs[i].isUnused() ) continue;
		acc = tabs[i].getAccount();
		if( (wcscmp( acc.login, pAccount->login ) == 0 ) && 
			(strcmp( acc.loginServerIP, pAccount->loginServerIP ) == 0 ) &&
			(acc.loginServerPort == pAccount->loginServerPort) ) return true;
	}
	return false;
}

void Tabs_AddAccount( Account *pAccount )
{
	if( !pAccount ) return;
	if( Tabs_AccountExists( pAccount ) )
	{
		MessageBox( g_hWnd, TEXT("”же существует"), TEXT("ЁЁ, куда"), MB_ICONSTOP );
		return;
	}
	int idx = Tabs_FindUnused();
	if( idx == -1 )
	{
		MessageBox( g_hWnd, TEXT("“ы и так уже набрал фуллпати!"), TEXT("ЁЁ, куда"), MB_ICONSTOP );
		return;
	}
	tabs[idx].setUsed();
	tabs[idx].setAccount( pAccount );
	tabs[idx].createWindow( g_hWndTab );
	//tabs[idx].showWindow();
	// ad tab to tabcontrol
	WCHAR text[64] = {0};
	wsprintfW( text, L"[%s]", pAccount->login );
	TCITEM tci;
	tci.mask = TCIF_PARAM | TCIF_TEXT;
	tci.lParam = idx;
	tci.pszText = text;
	int inserted_tab_idx = TabCtrl_InsertItem( g_hWndTab, 99, &tci );
	tabs[idx].setTabId( inserted_tab_idx );
	log_error( LOG_DEBUG, "Tabs: Inserted tab to idx %d\n", inserted_tab_idx );
	cur_tab = TabCtrl_GetCurSel( g_hWndTab );
	log_error( LOG_DEBUG, "cur_tab = %d\n", cur_tab );
	if( cur_tab == inserted_tab_idx ) tabs[idx].showWindow();
	tabs[cur_tab].updateWindow();
}

void Tabs_DeleteTab( int tab_id )
{
	if( TabCtrl_DeleteItem( g_hWndTab, tab_id ) ) log_error( LOG_DEBUG, "Tabs: delete tab %d OK\n", tab_id );
	else log_error( LOG_DEBUG, "Tabs: delete tab %d FAIL\n", tab_id );
	int i;
	for( i=0; i<MAX_ACCOUNTS; i++ )
	{
		if( tabs[i].isUnused() ) continue;
		int tid = tabs[i].getTabId();
		if( tid > tab_id )
		{
			log_error( LOG_DEBUG, "Tabs: rearranged tabs[%d].tab_id from %d to %d\n", i, tid, tid-1 );
			tabs[i].setTabId( tid - 1 );
		}
	}
}

void Tabs_Resize( int w, int h )
{
	UNREFERENCED_PARAMETER(w);
	UNREFERENCED_PARAMETER(h);
}

void Tabs_OnPaint()
{
	if( (cur_tab<0) || (cur_tab>=MAX_ACCOUNTS) ) return;
	if( tabs[cur_tab].isUnused() ) return;
	tabs[cur_tab].updateWindow();
}

int Tabs_FindTabIdxByTabId( int tab_id )
{
	int i;
	for( i=0; i<MAX_ACCOUNTS; i++ )
	{
		if( tabs[i].isUnused() ) continue;
		if( tabs[i].getTabId() == tab_id ) return i;
	}
	log_error( LOG_DEBUG, "Tabs: cannot find tab idx for tab id %d\n", tab_id );
	return -1;
}

void Tabs_OnSelChanging( LPNMHDR lpnmhdr )
{
	if( lpnmhdr->hwndFrom != g_hWndTab ) return;
	int cur_sel = TabCtrl_GetCurSel( g_hWndTab );
	//log_error( LOG_DEBUG, "Tabs: hide tab %d\n", cur_sel );
	if( (cur_sel >= 0) && (cur_sel < MAX_ACCOUNTS) )
	{
		int idx = Tabs_FindTabIdxByTabId( cur_sel );
		if( idx == -1 ) return;
		tabs[idx].hideWindow();
	}
}

void Tabs_OnSelChange( LPNMHDR lpnmhdr )
{
	if( lpnmhdr->hwndFrom != g_hWndTab ) return;
	cur_tab = TabCtrl_GetCurSel( g_hWndTab );
	//log_error( LOG_DEBUG, "Tabs: show tab %d\n", cur_tab );
	if( (cur_tab >= 0) && (cur_tab < MAX_ACCOUNTS) )
	{
		int idx = Tabs_FindTabIdxByTabId( cur_tab );
		if( idx == -1 ) return;
		tabs[idx].showWindow();
	}
}
