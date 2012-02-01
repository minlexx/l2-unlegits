#include "stdafx.h"
#include "Resource.h"

extern HINSTANCE g_radardll_hinst;

int g_dlgpk_vkey = 0;
HWND g_dlgpk_hdlg = NULL;
WNDPROC g_dlgpk_oldproc = NULL;

LRESULT CALLBACK DlgPressKey_StaticDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( uMsg == WM_KEYUP )
	{
		g_dlgpk_vkey = (int)wParam;
		PostMessage( g_dlgpk_hdlg, WM_CLOSE, 0, 0 );
		return 0;
	}
	return CallWindowProc( g_dlgpk_oldproc, hWnd, uMsg, wParam, lParam );
}

INT_PTR CALLBACK DlgPressKey_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	HWND hwndSt = NULL; wParam; lParam;
	switch( uMsg )
	{
	case WM_INITDIALOG:
		g_dlgpk_vkey = 0;
		g_dlgpk_hdlg = hDlg;
		hwndSt = GetDlgItem( hDlg, IDC_APPLY );
		g_dlgpk_oldproc = (WNDPROC)(void *)(LONG_PTR)GetWindowLongPtr( hwndSt, GWLP_WNDPROC );
		SetWindowLongPtr( hwndSt, GWLP_WNDPROC, (LONG_PTR)(void *)DlgPressKey_StaticDlgProc );
		break;
	case WM_CLOSE:
		if( g_dlgpk_vkey == 0 )EndDialog( hDlg, IDCANCEL );
		else EndDialog( hDlg, IDOK );
		break;
	default: return FALSE; break;
	}
	return TRUE;
}

int DlgPressKey_InputPressKey( HWND hWndParent )
{
	g_dlgpk_vkey = 0;
	INT_PTR ret = DialogBoxParam( g_radardll_hinst, MAKEINTRESOURCE(IDD_PRESSKEY),
		hWndParent, DlgPressKey_DlgProc, 0 );
	if( ret == IDCANCEL ) return 0;
	return g_dlgpk_vkey;
}
