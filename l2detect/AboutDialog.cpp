#include "stdafx.h"
#include "Resource.h"
#include "AboutDialog.h"

// EXTERNALS
extern HINSTANCE      g_radardll_hinst;

INT_PTR CALLBACK AboutDialogProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	lParam;
	switch( uMsg )
	{
	case WM_INITDIALOG: /*ConfigDlg_OnInitDialog( hDlg );*/ break;
	case WM_COMMAND:
		{
			switch( LOWORD( wParam ) )
			{
			case IDOK: EndDialog( hDlg, TRUE ); break;
			case IDCANCEL: EndDialog( hDlg, IDCANCEL ); break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

void AboutDialogStart( HWND hWndParent )
{
	DialogBoxParam( g_radardll_hinst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWndParent, AboutDialogProc, 0 );
}
