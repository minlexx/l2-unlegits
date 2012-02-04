#include "stdafx.h"
#include "Resource.h"

INT_PTR CALLBACK ChooseDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_CLOSE: EndDialog( hDlg, IDCANCEL ); break;
	case WM_COMMAND:
		{
			switch( LOWORD( wParam ) )
			{
			case IDC_INGAME: EndDialog( hDlg, IDYES ); break;
			case IDC_OUTGAME: EndDialog( hDlg, IDNO ); break;
			case IDC_B_TEST: EndDialog( hDlg, IDRETRY ); break;
			}
		} break;
	default: return FALSE; break;
	}
	lParam = 0;
	return TRUE;
}

INT_PTR ChooseMode( HINSTANCE hInst )
{
	INT_PTR ret = DialogBoxParam( hInst, MAKEINTRESOURCE(IDD_START), NULL, ChooseDlgProc, 0 );
	return ret;
}
