#include "stdafx.h"
#include "InputStringDlg.h"
#include "Resource.h"

extern HINSTANCE g_hInst;

InputStringDlg::InputStringDlg()
{
	title[0] = 0;
	question[0] = 0;
	text[0] = 0;
}

InputStringDlg::InputStringDlg( LPCTSTR ptitle, LPCTSTR pquestion, LPCTSTR ptext /*= NULL*/ )
{
	title[0] = 0;
	question[0] = 0;
	text[0] = 0;
	_tcscpy( title, ptitle );
	_tcscpy( question, pquestion );
	if( ptext ) _tcscpy( text, ptext );
}

InputStringDlg::~InputStringDlg()
{
	title[0] = 0;
	question[0] = 0;
	text[0] = 0;
}

bool InputStringDlg::run( HWND hWndParent )
{
	INT_PTR ret = DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_INPUTSTRING), hWndParent, DlgProc,
		(LPARAM)this );
	if( ret == IDCANCEL ) return false;
	return true;
}

INT_PTR CALLBACK InputStringDlg::DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			InputStringDlg *pcls = (InputStringDlg *)lParam;
			SetWindowLongPtr( hDlg, GWLP_USERDATA, (LONG_PTR)pcls );
			SetWindowText( hDlg, pcls->title );
			SetDlgItemText( hDlg, IDC_QUESTION, pcls->question );
			SetDlgItemText( hDlg, IDC_EDIT1, pcls->text );
		} break;
	case WM_COMMAND:
		{
			InputStringDlg *pcls = (InputStringDlg *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
			switch( LOWORD( wParam ) )
			{
			case IDCANCEL:
				{
					pcls->text[0] = 0;
					EndDialog( hDlg, IDCANCEL );
				} break;
			case IDOK:
				{
					GetDlgItemText( hDlg, IDC_EDIT1, pcls->text, 255 );
					EndDialog( hDlg, IDOK );
				} break;
			default: return FALSE; break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

/*public:
	TCHAR title[256];
	TCHAR question[256];
	TCHAR text[256];
};*/
