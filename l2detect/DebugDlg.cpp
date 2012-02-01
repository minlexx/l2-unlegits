#include "stdafx.h"
#include "Resource.h"
#include "ConfigIni.h"
#include "DebugDlg.h"
#include "ConfigDlg.h"

#include "Logger.h"
#include "LoginListener.h"
#include "GameListener.h"
#include "GameClient.h"

#include "net_hook.h"

// EXTERNALS
// global app configuration
extern class CConfig g_cfg;             // in main.cpp
extern class GameClient *g_game_client; // in main.cpp

// global listeners
extern class LoginListener *g_plogin;
extern class GameListener *g_pgame;

#define TIMER_UPDATEINFO 1

BOOL DebugDlg_OnInitDialog( HWND hDlg );
void DebugDlg_OnBnClickedRunconfig( HWND hDlg );
void DebugDlg_OnBnClickedStartLL( HWND hDlg );
void DebugDlg_OnBnClickedStartGL( HWND hDlg );
void DebugDlg_OnBnClickedStopLL( HWND hDlg );
void DebugDlg_OnBnClickedStopGL( HWND hDlg );
void DebugDlg_OnBnClickedFlushLog( HWND hDlg );
void DebugDlg_OnBnClickedEnableConsole( HWND hDlg );
void DebugDlg_OnBnClickedDisableConsole( HWND hDlg );
void DebugDlg_OnBnClickedLoadWalker( HWND hDlg );
void DebugDlg_OnBnClickedUnloadWalker( HWND hDlg );
void DebugDlg_OnTimer( HWND hDlg, UINT_PTR nIDEvent );
void DebugDlg_updateInfo( HWND hDlg );
void DebugDlg_OnBnClickedValidateInterception( HWND hDlg );
void DebugDlg_OnBnClickedInterceptConnect( HWND hDlg );
void DebugDlg_OnBnClickedCheckVP( HWND hDlg );
void DebugDlg_OnBnClickedDumpAllRelations( HWND hDlg );
void DebugDlg_OnBnClickedPrintAddrTid( HWND hDlg );


INT_PTR CALLBACK DebugDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam);
	switch( uMsg )
	{
	case WM_INITDIALOG: DebugDlg_OnInitDialog( hDlg ); break;
	case WM_TIMER: DebugDlg_OnTimer( hDlg, (UINT_PTR)wParam ); break;
	case WM_COMMAND:
		{
			switch( LOWORD( wParam ) )
			{
			//case IDC_RUNCONFIG: DebugDlg_OnBnClickedRunconfig( hDlg ); break;
			case IDC_STARTLL: DebugDlg_OnBnClickedStartLL( hDlg ); break;
			case IDC_STOPLL: DebugDlg_OnBnClickedStopLL( hDlg ); break;
			case IDC_STARTGL: DebugDlg_OnBnClickedStartGL( hDlg ); break;
			case IDC_STOPGL: DebugDlg_OnBnClickedStopGL( hDlg ); break;
			case IDC_FLUSH_LOG: DebugDlg_OnBnClickedFlushLog( hDlg ); break;
			case IDC_B_CONENABLE: DebugDlg_OnBnClickedEnableConsole( hDlg ); break;
			case IDC_B_CONDISABLE: DebugDlg_OnBnClickedDisableConsole( hDlg ); break;
			case IDC_B_VALIDATEINTERCEPT: DebugDlg_OnBnClickedValidateInterception( hDlg ); break;
			case IDC_B_INTERCEPTCONNECT: DebugDlg_OnBnClickedInterceptConnect( hDlg ); break;
			case IDC_B_CHECK_VIRTUALPROTECTEX: DebugDlg_OnBnClickedCheckVP( hDlg ); break;
			case IDC_B_LOADWALKER: DebugDlg_OnBnClickedLoadWalker( hDlg ); break;
			case IDC_B_UNLOADWALKER: DebugDlg_OnBnClickedUnloadWalker( hDlg ); break;
			case IDC_B_DUMP_ALL_RELATIONS: DebugDlg_OnBnClickedDumpAllRelations( hDlg ); break;
			case IDC_B_PRINTADDRTID: DebugDlg_OnBnClickedPrintAddrTid( hDlg ); break;
			case IDOK: EndDialog( hDlg, IDOK ); break;
			case IDCANCEL: EndDialog( hDlg, IDCANCEL ); break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

void DebugDialogStart( HWND hWndParent, HINSTANCE hInst )
{
	DialogBoxParam( hInst, MAKEINTRESOURCE(IDD_DEBUG), hWndParent, DebugDlgProc, NULL );
}

// обработчики сообщений DebugDlg
BOOL DebugDlg_OnInitDialog( HWND hDlg )
{
	DebugDlg_updateInfo( hDlg );
	SetTimer( hDlg, TIMER_UPDATEINFO, 1000, NULL );
	return TRUE;
}

//void DebugDlg_OnBnClickedRunconfig( HWND hDlg )
//{
//	ConfigDialogStart( hDlg, g_radardll_hinst );
//}

void DebugDlg_OnBnClickedStartLL( HWND hDlg )
{
	if( g_plogin )
	{
		if( g_plogin->isRunning() )
		{
			MessageBox( hDlg, TEXT("Login listener already running!"), TEXT("Ops!"), MB_ICONINFORMATION );
			return;
		}
		if( !g_plogin->start() )
			MessageBox( hDlg, TEXT("Login listener start failed!"), TEXT("Ops!"), MB_ICONINFORMATION );
	}
}

void DebugDlg_OnBnClickedStartGL( HWND hDlg )
{
	if( g_pgame )
	{
		if( g_pgame->isRunning() )
		{
			MessageBox( hDlg, TEXT("Game listener already running!"), TEXT("Ops!"), MB_ICONINFORMATION );
			return;
		}
		if( !g_pgame->start() )
			MessageBox( hDlg, TEXT("Game listener start failed!"), TEXT("Ops!"), MB_ICONINFORMATION );
	}
}

void DebugDlg_OnBnClickedStopLL( HWND hDlg )
{
	if( g_plogin )
	{
		if( !g_plogin->isRunning() )
		{
			MessageBox( hDlg, TEXT("Login listener already stopped!"), TEXT("Ops!"), MB_ICONINFORMATION );
			return;
		}
		g_plogin->signalStop();
		// wait 10 secs to stop
		if( !g_plogin->waitStopped( 10000 ) )
		{
			g_plogin->terminateThread();
			MessageBox( hDlg, TEXT("Login listener stop wait failed, terminated :("), TEXT("Ops!"), MB_ICONINFORMATION );
		}
	}
}

void DebugDlg_OnBnClickedStopGL( HWND hDlg )
{
	if( g_pgame )
	{
		if( !g_pgame->isRunning() )
		{
			MessageBox( hDlg, TEXT("Game listener already stopped!"), TEXT("Ops!"), MB_ICONINFORMATION );
			return;
		}
		g_pgame->signalStop();
		// wait 10 secs to stop
		if( !g_pgame->waitStopped( 10000 ) )
		{
			g_pgame->terminateThread();
			MessageBox( hDlg, TEXT("Game listener stop wait failed, terminated :("), TEXT("Ops!"), MB_ICONINFORMATION );
		}
	}
}

void DebugDlg_OnBnClickedEnableConsole( HWND hDlg )
{
	UNREFERENCED_PARAMETER(hDlg);
	ErrorLogger_EnableLoggingToConsole( true );
}

void DebugDlg_OnBnClickedDisableConsole( HWND hDlg )
{
	UNREFERENCED_PARAMETER(hDlg);
	ErrorLogger_EnableLoggingToConsole( false );
}

void DebugDlg_OnTimer( HWND hDlg, UINT_PTR nIDEvent )
{
	if( nIDEvent == TIMER_UPDATEINFO )
	{
		DebugDlg_updateInfo( hDlg );
	}
}

void DebugDlg_updateInfo( HWND hDlg )
{
	int run;
	HWND hWndB;
	// update LoginListener info
	// LoginListener may not be started in ingame mode at all
	if( g_plogin )
	{
		run = g_plogin->isRunning();
		if( run )
		{
			SetDlgItemText( hDlg, IDC_ELLSTATUS, TEXT("ON") );
			// start button
			hWndB = GetDlgItem( hDlg, IDC_STARTLL );
			if( hWndB ) EnableWindow( hWndB, FALSE );
			// stop button
			hWndB = GetDlgItem( hDlg, IDC_STOPLL );
			if( hWndB ) EnableWindow( hWndB, TRUE );
		}
		else
		{
			SetDlgItemText( hDlg, IDC_ELLSTATUS, TEXT("Off") );
			// start button
			hWndB = GetDlgItem( hDlg, IDC_STARTLL );
			if( hWndB ) EnableWindow( hWndB, TRUE );
			// stop button
			hWndB = GetDlgItem( hDlg, IDC_STOPLL );
			if( hWndB ) EnableWindow( hWndB, FALSE );
		}
	}
	else // all start & stop LL buttons disabled
	{
		// start button
		hWndB = GetDlgItem( hDlg, IDC_STARTLL );
		if( hWndB ) EnableWindow( hWndB, FALSE );
		// stop button
		hWndB = GetDlgItem( hDlg, IDC_STOPLL );
		if( hWndB ) EnableWindow( hWndB, FALSE );
	}

	// update GameListener info
	if( g_pgame )
	{
		run = g_pgame->isRunning();
		if( run )
		{
			SetDlgItemText( hDlg, IDC_EGLSTATUS, TEXT("ON") );
			// start button
			hWndB = GetDlgItem( hDlg, IDC_STARTGL );
			if( hWndB ) EnableWindow( hWndB, FALSE );
			// stop button
			hWndB = GetDlgItem( hDlg, IDC_STOPGL );
			if( hWndB ) EnableWindow( hWndB, TRUE );
		}
		else
		{
			SetDlgItemText( hDlg, IDC_EGLSTATUS, TEXT("Off") );
			// start button
			hWndB = GetDlgItem( hDlg, IDC_STARTGL );
			if( hWndB ) EnableWindow( hWndB, TRUE );
			// stop button
			hWndB = GetDlgItem( hDlg, IDC_STOPGL );
			if( hWndB ) EnableWindow( hWndB, FALSE );
		}
	}
	else // all start & stop GL buttons disabled
	{
		// start button
		hWndB = GetDlgItem( hDlg, IDC_STARTGL );
		if( hWndB ) EnableWindow( hWndB, FALSE );
		// stop button
		hWndB = GetDlgItem( hDlg, IDC_STOPGL );
		if( hWndB ) EnableWindow( hWndB, FALSE );
	}

	if( g_game_client->getState() == GCST_IN_GAME )
	{
		TCHAR tstr[256];
		// hp
		wsprintf( tstr, _T("%d / %d"), g_game_client->ai.usr.hp, g_game_client->ai.usr.hp_max );
		SetDlgItemText( hDlg, IDC_HP, tstr );
		// mp
		wsprintf( tstr, _T("%d / %d"), g_game_client->ai.usr.mp, g_game_client->ai.usr.mp_max );
		SetDlgItemText( hDlg, IDC_MP, tstr );
		// cp
		wsprintf( tstr, _T("%d / %d"), g_game_client->ai.usr.cp, g_game_client->ai.usr.cp_max );
		SetDlgItemText( hDlg, IDC_CP, tstr );
		// char name
		wsprintf( tstr, _T("%s (tit %s), Lv %d (%s/base %s)"),
			g_game_client->ai.usr.charName, g_game_client->ai.usr.charTitle,
			g_game_client->ai.usr.level,
			g_game_client->ai.usr.getClassStr(), g_game_client->ai.usr.getBaseClassStr() );
		SetDlgItemText( hDlg, IDC_CHARNAME, tstr );
	}
	else SetDlgItemText( hDlg, IDC_CHARNAME, TEXT("<Not in game!>") );
}

void DebugDlg_OnBnClickedFlushLog( HWND hDlg )
{
	UNREFERENCED_PARAMETER( hDlg );
	ErrorLogger_FlushLogFile();
}

void DebugDlg_OnBnClickedValidateInterception( HWND hDlg )
{
	hDlg = NULL;
	Hook_ValidateInterception_my();
}

void DebugDlg_OnBnClickedInterceptConnect( HWND hDlg )
{
	hDlg = NULL;
	Hook_InterceptConnect_my();
}

void DebugDlg_OnBnClickedCheckVP( HWND hDlg )
{
	hDlg = NULL;
	Hook_CheckVirtualProtect();
}

void DebugDlg_OnBnClickedLoadWalker( HWND hDlg )
{
	HINSTANCE hDllL2Walker = GetModuleHandle( TEXT("L2Walker.dll") );
	TCHAR text[256];
	if( hDllL2Walker != NULL )
	{
		log_error( LOG_OK, "L2Walker.dll already loaded at 0x%p\n", (void *)hDllL2Walker );
		wsprintf( text, TEXT("L2Walker.dll already loaded at 0x%p"), (void *)hDllL2Walker );
		MessageBox( hDlg, text, TEXT("Cannot!"), MB_ICONWARNING );
		return;
	}
	hDllL2Walker = LoadLibrary( TEXT("L2Walker.dll") );
	if( !hDllL2Walker )
	{
		ErrorLogger_LogLastError( "LoadLibrary l2walker", GetLastError() );
		MessageBox( hDlg, TEXT("Cannot load L2Walker.dll!\nError is in log."), TEXT("Cannot!"), MB_ICONSTOP );
		return;
	}
	log_error( LOG_OK, "L2Walker.dll loaded at 0x%p\n", (void *)hDllL2Walker );
	ErrorLogger_FlushLogFile();
	// RVA 000196E0
}

void DebugDlg_OnBnClickedUnloadWalker( HWND hDlg )
{
	HINSTANCE hDllL2Walker = GetModuleHandle( TEXT("L2Walker.dll") );
	if( hDllL2Walker == NULL )
	{
		MessageBox( hDlg, TEXT("L2Walker.dll not loaded!"), TEXT("Cannot!"), MB_ICONWARNING );
		return;
	}
	log_error( LOG_DEBUG, "FreeLibrary() for L2Walker.dll...\n" );
	ErrorLogger_FlushLogFile();
	FreeLibrary( hDllL2Walker );
	log_error( LOG_OK, "L2Walker.dll unloaded.\n" );
	ErrorLogger_FlushLogFile();
}

void str_relation( wchar_t *out, unsigned int relation )
{
	out[0] = 0;
	const unsigned int RELATION_PARTY1       = 0x00001; // party member
	const unsigned int RELATION_PARTY2       = 0x00002; // party member
	const unsigned int RELATION_PARTY3       = 0x00004; // party member
	const unsigned int RELATION_PARTY4       = 0x00008; // party member (for information, see L2PcInstance.getRelation())
	const unsigned int RELATION_PARTYLEADER  = 0x00010; // true if is party leader
	const unsigned int RELATION_HAS_PARTY    = 0x00020; // true if is in party
	const unsigned int RELATION_CLAN_MEMBER  = 0x00040; // true if is in clan
	const unsigned int RELATION_LEADER 	     = 0x00080; // true if is clan leader
	const unsigned int RELATION_CLAN_MATE    = 0x00100; // true if is in same clan
	const unsigned int RELATION_INSIEGE   	 = 0x00200; // true if in siege
	const unsigned int RELATION_ATTACKER     = 0x00400; // true when attacker
	const unsigned int RELATION_ALLY         = 0x00800; // blue siege icon, cannot have if red
	const unsigned int RELATION_ENEMY        = 0x01000; // true when red icon, doesn't matter with blue
	const unsigned int RELATION_1SIDED_WAR   = 0x08000; // single fist
	const unsigned int RELATION_MUTUAL_WAR   = 0x04000; // double fist
	const unsigned int RELATION_ALLY_MEMBER  = 0x10000; // clan is in alliance
	const unsigned int RELATION_TERRITORY_WAR= 0x80000; // show Territory War icon
	if( relation & RELATION_PARTY1 )      wcscat( out, L"RELATION_PARTY1 " );
	if( relation & RELATION_PARTY2 )      wcscat( out, L"RELATION_PARTY2 " );
	if( relation & RELATION_PARTY3 )      wcscat( out, L"RELATION_PARTY3 " );
	if( relation & RELATION_PARTY4 )      wcscat( out, L"RELATION_PARTY4 " );
	if( relation & RELATION_PARTYLEADER ) wcscat( out, L"RELATION_PARTYLEADER " );
	if( relation & RELATION_HAS_PARTY )   wcscat( out, L"RELATION_HAS_PARTY " );
	if( relation & RELATION_CLAN_MEMBER ) wcscat( out, L"RELATION_CLAN_MEMBER " );
	if( relation & RELATION_LEADER )      wcscat( out, L"RELATION_LEADER " );
	if( relation & RELATION_CLAN_MATE )   wcscat( out, L"RELATION_CLAN_MATE " );
	if( relation & RELATION_INSIEGE )     wcscat( out, L"RELATION_INSIEGE " );
	if( relation & RELATION_ATTACKER )    wcscat( out, L"RELATION_ATTACKER " );
	if( relation & RELATION_ALLY )        wcscat( out, L"RELATION_ALLY " );
	if( relation & RELATION_ENEMY )       wcscat( out, L"RELATION_ENEMY " );
	if( relation & RELATION_1SIDED_WAR )  wcscat( out, L"RELATION_1SIDED_WAR " );
	if( relation & RELATION_MUTUAL_WAR )  wcscat( out, L"RELATION_MUTUAL_WAR " );
	if( relation & RELATION_ALLY_MEMBER ) wcscat( out, L"RELATION_ALLY_MEMBER " );
	if( relation & RELATION_TERRITORY_WAR ) wcscat( out, L"RELATION_TERRITORY_WAR " );
}

void DebugDlg_OnBnClickedDumpAllRelations( HWND hDlg )
{
	UNREFERENCED_PARAMETER( hDlg );
	CharArray_Lock();
	int i;
	FILE *f = fopen( "Relations.txt", "at" );
	fwprintf( f, L"User: [%s] 0x%08X\n", g_game_client->ai.usr.charName, g_game_client->ai.usr.relation );
	for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
	{
		if( !chars_array[i] ) continue;
		if( chars_array[i]->isUnused() ) continue;
		wchar_t rel[512] = {0};
		str_relation( rel, chars_array[i]->relation );
		fwprintf( f, L"Char[%d]: [%s] 0x%08X [%s]\n", i, chars_array[i]->charName, chars_array[i]->relation, rel );
	}
	fprintf( f, "====== END =====\n" );
	fclose( f );
	CharArray_Unlock();
}

void DebugDlg_OnBnClickedPrintAddrTid( HWND hDlg )
{
	UNREFERENCED_PARAMETER( hDlg );
	void *addr = (void *)&(g_game_client->ai.dwThreadID);
	log_error( LOG_OK, "&(g_game_client->ai.dwThreadID) = 0x%p\n", addr );
}
