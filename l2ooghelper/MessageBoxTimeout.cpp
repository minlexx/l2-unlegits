#include "stdafx.h"
#include "Resource.h"
#include "MessageBoxTimeout.h"
#include "logger.h"
#include "L2Client.h"

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

#define USE_MODELESS

MessageBoxTimeout::MessageBoxTimeout(
	HWND hWndNotify, UINT uMessage, int type,
	LPTSTR szQuestiion, int timeoutSec )
{
	m_hWndNotify = hWndNotify;
	m_uNotifyMesasage = uMessage;
	m_type = type;
	m_question[0] = 0;
	if( szQuestiion ) lstrcpy( m_question, szQuestiion );
	m_timeout = timeoutSec;
	m_curChar[0] = 0;
	m_requestId = 0;
	m_sm_ID = 0;
	m_pvL2Client = NULL;
}

MessageBoxTimeout::~MessageBoxTimeout()
{
	//log_error( LOG_OK, "MessageBoxTimeout::~MessageBoxTimeout()\n" );
}

void MessageBoxTimeout::run( const wchar_t *curCharName )
{
	wcscpy( m_curChar, curCharName );
	// thread
	DWORD dwTID = 0;
	HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0,
		(unsigned int (__stdcall *)(void *))MBTimeout_Thread, (void *)this,
		0, (unsigned int *)&dwTID );
	if( hThread ) CloseHandle( hThread );
}

void MessageBoxTimeout::runConfirmDlg( const wchar_t *curCharName,
		unsigned int requestId, unsigned int sm_ID,
		void *pvActionL2Client )
{
	wcscpy( m_curChar, curCharName );
	m_requestId = requestId;
	m_sm_ID = sm_ID;
	m_pvL2Client = pvActionL2Client;
	// thread
	DWORD dwTID = 0;
	HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0,
		(unsigned int (__stdcall *)(void *))MBTimeout_Thread, (void *)this,
		0, (unsigned int *)&dwTID );
	if( hThread ) CloseHandle( hThread );
}

DWORD WINAPI MessageBoxTimeout::MBTimeout_Thread( LPVOID lpParam )
{
	class MessageBoxTimeout *mb = (MessageBoxTimeout *)lpParam;
#ifndef USE_MODELESS
	DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_MBTIMEOUT), g_hWnd,
		MBTimeout_DlgProc, (LPARAM)mb );
#else
	HWND hDlg = CreateDialogParam( g_hInst, MAKEINTRESOURCE(IDD_MBTIMEOUT),
		g_hWnd, MBTimeout_DlgProc, (LPARAM)mb );
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
	//log_error( LOG_OK, "MBTimeout_Thread(): ended window loop\n" );
	DestroyWindow( hDlg );
#endif
	return 0;
}

INT_PTR CALLBACK MessageBoxTimeout::MBTimeout_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			SetWindowLongPtr( hDlg, GWLP_USERDATA, (LONG_PTR)lParam );
			OnInitDialog( hDlg, lParam );
		} break;
	case WM_COMMAND: OnCommand( hDlg, wParam, lParam ); break;
	case WM_DESTROY: { OnDestroy( hDlg ); return FALSE; } break;
	case WM_TIMER: OnTimer( hDlg, wParam ); break;
	default: return FALSE; break;
	}
	return TRUE;
}

void MessageBoxTimeout::OnInitDialog( HWND hDlg, LPARAM lParam )
{
	//log_error( LOG_OK, "MessageBoxTimeout::OnInitDialog()\n" );
	MessageBoxTimeout *pcls = (MessageBoxTimeout *)lParam;
	wchar_t text[256] = {0};
	wsprintfW( text, L"%s - выбери", pcls->m_curChar );
	SetWindowTextW( hDlg, text );
	SetDlgItemText( hDlg, IDC_ST_Q, pcls->m_question );
	HWND hWndProgress = GetDlgItem( hDlg, IDC_PROGRESS1 );
	SendMessage( hWndProgress, PBM_SETRANGE, 0, MAKELPARAM(0, pcls->m_timeout) );
	SendMessage( hWndProgress, PBM_SETPOS, pcls->m_timeout, 0 );
	SendMessage( hWndProgress, PBM_SETSTEP, (WPARAM)-1, 0 );
	if( pcls->m_timeout > 0 )
	{
		SetTimer( hDlg, 1, 1000, NULL );
	}
}

void MessageBoxTimeout::OnDestroy( HWND hDlg )
{
	//log_error( LOG_OK, "MessageBoxTimeout::OnDestroy()\n" );
	KillTimer( hDlg, 1 );
	MessageBoxTimeout *pcls = (MessageBoxTimeout *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	delete pcls;
}

void MessageBoxTimeout::OnCommand( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam);
	MessageBoxTimeout *pcls = (MessageBoxTimeout *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	switch( LOWORD( wParam ) )
	{
	case IDOK:
		{
			if( pcls->m_type < MessageBoxTimeout::TYPE_CONFIRMDLG )
			{
				PostMessage( pcls->m_hWndNotify, pcls->m_uNotifyMesasage, TRUE, (pcls->m_type) );
			}
			if( pcls->m_type == MessageBoxTimeout::TYPE_CONFIRMDLG )
			{
				L2Client *pcl = (L2Client *)pcls->m_pvL2Client;
				if( pcl ) pcl->send_DlgAnswer( pcls->m_sm_ID, 1, pcls->m_requestId );
			}
#ifdef USE_MODELESS
			PostMessage( hDlg, WM_QUIT, 0, 0 );
			//PostQuitMessage( 0 );
			//DestroyWindow( hDlg );
#else
			EndDialog( hDlg, IDOK );
#endif
		} break;
	case IDCANCEL:
		{
			if( pcls->m_type < MessageBoxTimeout::TYPE_CONFIRMDLG )
			{
				PostMessage( pcls->m_hWndNotify, pcls->m_uNotifyMesasage, FALSE, (pcls->m_type) );
			}
			if( pcls->m_type == MessageBoxTimeout::TYPE_CONFIRMDLG )
			{
				L2Client *pcl = (L2Client *)pcls->m_pvL2Client;
				if( pcl ) pcl->send_DlgAnswer( pcls->m_sm_ID, 0, pcls->m_requestId );
			}
#ifdef USE_MODELESS
			PostMessage( hDlg, WM_QUIT, 0, 0 );
			//DestroyWindow( hDlg );
#else
			EndDialog( hDlg, IDCANCEL );
#endif
		} break;
	}
}

void MessageBoxTimeout::OnTimer( HWND hDlg, WPARAM wParam )
{
	switch( wParam )
	{
	case 1:
		{
			HWND hWndProgress = GetDlgItem( hDlg, IDC_PROGRESS1 );
			SendMessage( hWndProgress, PBM_STEPIT, 0, 0 );
			int pos = (int)SendMessage( hWndProgress, PBM_GETPOS, 0, 0 );
			//log_error( LOG_OK, "OnTimer(): pos = %d\n", pos );
			if( pos <= 0 ) PostMessage( hDlg, WM_COMMAND, IDCANCEL, 0 );
		} break;
	}
}
