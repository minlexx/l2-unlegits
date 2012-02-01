#include "stdafx.h"
#include "Logger.h"
#include "UserInfo_c.h"

WNDPROC OriginalProgressProc = NULL;
LRESULT CALLBACK ProgressHPDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK ProgressMPDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK ProgressCPDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK ProgressWeightDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK ProgressTargetHPDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


void SubclassProgressHP( HWND hWnd, UserInfo *pusr )
{
	if( OriginalProgressProc == NULL )
	{
		OriginalProgressProc = (WNDPROC)(LPVOID)GetWindowLongPtr( hWnd, GWLP_WNDPROC );
		//log_error( LOG_DEBUG, "Progress user data: 0x%08X\n", GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
	}
	SetWindowLongPtr( hWnd, GWLP_WNDPROC, (LONG_PTR)(LPVOID)ProgressHPDlgProc );
	SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)pusr );
}

void SubclassProgressMP( HWND hWnd, UserInfo *pusr )
{
	if( OriginalProgressProc == NULL ) OriginalProgressProc = (WNDPROC)(LPVOID)GetWindowLongPtr( hWnd, GWLP_WNDPROC );
	SetWindowLongPtr( hWnd, GWLP_WNDPROC, (LONG_PTR)(LPVOID)ProgressMPDlgProc );
	SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)pusr );
}

void SubclassProgressCP( HWND hWnd, UserInfo *pusr )
{
	if( OriginalProgressProc == NULL ) OriginalProgressProc = (WNDPROC)(LPVOID)GetWindowLongPtr( hWnd, GWLP_WNDPROC );
	SetWindowLongPtr( hWnd, GWLP_WNDPROC, (LONG_PTR)(LPVOID)ProgressCPDlgProc );
	SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)pusr );
}

void SubclassProgressWeight( HWND hWnd, UserInfo *pusr )
{
	if( OriginalProgressProc == NULL ) OriginalProgressProc = (WNDPROC)(LPVOID)GetWindowLongPtr( hWnd, GWLP_WNDPROC );
	SetWindowLongPtr( hWnd, GWLP_WNDPROC, (LONG_PTR)(LPVOID)ProgressWeightDlgProc );
	SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)pusr );
}

void SubclassProgressTargetHP( HWND hWnd, UserInfo *pusr )
{
	if( OriginalProgressProc == NULL ) OriginalProgressProc = (WNDPROC)(LPVOID)GetWindowLongPtr( hWnd, GWLP_WNDPROC );
	SetWindowLongPtr( hWnd, GWLP_WNDPROC, (LONG_PTR)(LPVOID)ProgressTargetHPDlgProc );
	SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)pusr );
}

LRESULT CALLBACK ProgressHPDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT ret = CallWindowProc( OriginalProgressProc, hWnd, uMsg, wParam, lParam );
	if( uMsg == WM_PAINT )
	{
		UserInfo *pusr = (UserInfo *)GetWindowLongPtr( hWnd, GWLP_USERDATA );
		if( !pusr ) return ret;
		// update HP numbers
		HDC hdc;
		RECT rc;
		TCHAR text[64] = {0};
		wsprintf( text, TEXT("%d / %d"), (int)pusr->curHp, (int)pusr->maxHp );
		hdc = GetDC( hWnd );
		GetClientRect( hWnd, &rc );
		SetBkMode( hdc, TRANSPARENT );
		HFONT hFntOld = SelectFont( hdc, GetStockFont( ANSI_VAR_FONT ) );
		DrawText( hdc, text, lstrlen(text), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
		SelectFont( hdc, hFntOld );
		ReleaseDC( hWnd, hdc );
	}
	return ret;
}

LRESULT CALLBACK ProgressMPDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT ret = CallWindowProc( OriginalProgressProc, hWnd, uMsg, wParam, lParam );
	if( uMsg == WM_PAINT )
	{
		UserInfo *pusr = (UserInfo *)GetWindowLongPtr( hWnd, GWLP_USERDATA );
		if( !pusr ) return ret;
		// update MP numbers
		HDC hdc;
		RECT rc;
		TCHAR text[64] = {0};
		wsprintf( text, TEXT("%d / %d"), (int)pusr->curMp, (int)pusr->maxMp );
		hdc = GetDC( hWnd );
		GetClientRect( hWnd, &rc );
		SetBkMode( hdc, TRANSPARENT );
		HFONT hFntOld = SelectFont( hdc, GetStockFont( ANSI_VAR_FONT ) );
		DrawText( hdc, text, lstrlen(text), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
		SelectFont( hdc, hFntOld );
		ReleaseDC( hWnd, hdc );
	}
	return ret;
}

LRESULT CALLBACK ProgressCPDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT ret = CallWindowProc( OriginalProgressProc, hWnd, uMsg, wParam, lParam );
	if( uMsg == WM_PAINT )
	{
		UserInfo *pusr = (UserInfo *)GetWindowLongPtr( hWnd, GWLP_USERDATA );
		if( !pusr ) return ret;
		// update CP numbers
		HDC hdc;
		RECT rc;
		TCHAR text[64] = {0};
		wsprintf( text, TEXT("%d / %d"), (int)pusr->curCp, (int)pusr->maxCp );
		hdc = GetDC( hWnd );
		GetClientRect( hWnd, &rc );
		SetBkMode( hdc, TRANSPARENT );
		HFONT hFntOld = SelectFont( hdc, GetStockFont( ANSI_VAR_FONT ) );
		DrawText( hdc, text, lstrlen(text), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
		SelectFont( hdc, hFntOld );
		ReleaseDC( hWnd, hdc );
	}
	return ret;
}

LRESULT CALLBACK ProgressWeightDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT ret = CallWindowProc( OriginalProgressProc, hWnd, uMsg, wParam, lParam );
	if( uMsg == WM_PAINT )
	{
		UserInfo *pusr = (UserInfo *)GetWindowLongPtr( hWnd, GWLP_USERDATA );
		if( !pusr ) return ret;
		// update load
		HDC hdc;
		RECT rc;
		TCHAR text[64] = {0};
		wsprintf( text, TEXT("%d / %d"), (int)pusr->curLoad, (int)pusr->maxLoad );
		hdc = GetDC( hWnd );
		GetClientRect( hWnd, &rc );
		SetBkMode( hdc, TRANSPARENT );
		HFONT hFntOld = SelectFont( hdc, GetStockFont( ANSI_VAR_FONT ) );
		DrawText( hdc, text, lstrlen(text), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
		SelectFont( hdc, hFntOld );
		ReleaseDC( hWnd, hdc );
	}
	return ret;
}

LRESULT CALLBACK ProgressTargetHPDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT ret = CallWindowProc( OriginalProgressProc, hWnd, uMsg, wParam, lParam );
	if( uMsg == WM_PAINT )
	{
		UserInfo *pusr = (UserInfo *)GetWindowLongPtr( hWnd, GWLP_USERDATA );
		if( !pusr ) return ret;
		// update HP numbers
		HDC hdc;
		RECT rc;
		TCHAR text[64] = {0};
		wsprintf( text, TEXT("%d / %d"), (int)pusr->targetCurHp, (int)pusr->targetMaxHp );
		hdc = GetDC( hWnd );
		GetClientRect( hWnd, &rc );
		SetBkMode( hdc, TRANSPARENT );
		HFONT hFntOld = SelectFont( hdc, GetStockFont( ANSI_VAR_FONT ) );
		DrawText( hdc, text, lstrlen(text), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
		SelectFont( hdc, hFntOld );
		ReleaseDC( hWnd, hdc );
	}
	return ret;
}
