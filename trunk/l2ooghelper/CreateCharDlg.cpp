#include "stdafx.h"
#include "Resource.h"
#include "windowUtils.h"
#include "Logger.h"
#include "CreateCharDlg.h"

extern HINSTANCE g_hInst;

INT_PTR CALLBACK CreateCharDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

struct CREATECHARDLGSTRUCT
{
	CreateCharDialogResult      *pResult;
	L2Game_NewCharacterTemplate *templates;
	int                          templatesCount;
};

void CreateCharDialog( HWND hWndParent,
					  L2Game_NewCharacterTemplate *templates,
					  int templatesCount,
					  CreateCharDialogResult *pResult )
{
	//
	pResult->createCharName[0] = 0;
	pResult->createTemplateIndex = -1;
	pResult->createHairStyle = -1;
	pResult->createHairColor = -1;
	pResult->createFace = -1;
	pResult->createGender = -1;
	//
	CREATECHARDLGSTRUCT st;
	st.pResult        = pResult;
	st.templates      = templates;
	st.templatesCount = templatesCount;
	//
	INT_PTR ret = DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_CREATECHAR), hWndParent,
		CreateCharDlgProc, (LPARAM)&st );
	//
	if( ret == IDCANCEL )
	{
		pResult->createCharName[0] = 0;
		pResult->createTemplateIndex = -1;
	}
}

INT_PTR CALLBACK CreateCharDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			SetWindowLongPtr( hDlg, GWLP_USERDATA, (LONG_PTR)lParam );
			CREATECHARDLGSTRUCT *st = (CREATECHARDLGSTRUCT *)lParam;
			// insert races/classes
			HWND hwndCB = GetDlgItem( hDlg, IDC_CB_CLASS );
			ComboBox_ResetContent( hwndCB );
			int i;
			for( i=0; i<st->templatesCount; i++ )
			{
				wchar_t wstr[128] = {0};
				wsprintfW( wstr, L"%S (%S)",
					L2Data_getRace( st->templates[i].race ),
					L2Data_getClass( st->templates[i].classID ) );
				ComboBox_AddString( hwndCB, wstr );
			}
			ComboBox_SetCurSel( hwndCB, 0 );
			// insert hair styles
			hwndCB = GetDlgItem( hDlg, IDC_CB_HSTYLE );
			ComboBox_ResetContent( hwndCB );
			ComboBox_AddString( hwndCB, TEXT("A") );
			ComboBox_AddString( hwndCB, TEXT("B") );
			ComboBox_AddString( hwndCB, TEXT("C") );
			ComboBox_AddString( hwndCB, TEXT("D") );
			ComboBox_AddString( hwndCB, TEXT("E") );
			ComboBox_AddString( hwndCB, TEXT("F (только телки)") );
			ComboBox_AddString( hwndCB, TEXT("G (только телки)") );
			ComboBox_SetCurSel( hwndCB, 0 );
			// insert hair colors
			hwndCB = GetDlgItem( hDlg, IDC_CB_HCOLOR );
			ComboBox_ResetContent( hwndCB );
			ComboBox_AddString( hwndCB, TEXT("A") );
			ComboBox_AddString( hwndCB, TEXT("B") );
			ComboBox_AddString( hwndCB, TEXT("C") );
			ComboBox_AddString( hwndCB, TEXT("D (кроме Kamael)") );
			ComboBox_SetCurSel( hwndCB, 0 );
			// insert faces
			hwndCB = GetDlgItem( hDlg, IDC_CB_FSTYLE );
			ComboBox_ResetContent( hwndCB );
			ComboBox_AddString( hwndCB, TEXT("A") );
			ComboBox_AddString( hwndCB, TEXT("B") );
			ComboBox_AddString( hwndCB, TEXT("C") );
			ComboBox_SetCurSel( hwndCB, 0 );
			// insert genders
			hwndCB = GetDlgItem( hDlg, IDC_CB_GENDER );
			ComboBox_ResetContent( hwndCB );
			ComboBox_AddString( hwndCB, TEXT("Mужыг") );
			ComboBox_AddString( hwndCB, TEXT("Телка") );
			ComboBox_SetCurSel( hwndCB, 0 );
		} break;
	case WM_COMMAND:
		{
			CREATECHARDLGSTRUCT *st = (CREATECHARDLGSTRUCT *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
			switch( LOWORD(wParam) )
			{
			case IDCANCEL: EndDialog( hDlg, IDCANCEL ); break;
			case IDOK:
				{
					// get char name
					GetDlgItemTextW( hDlg, IDC_EDIT1, st->pResult->createCharName, 255 );
					st->pResult->createCharName[255] = 0;
					// get race & class, template index
					HWND hwndCB = GetDlgItem( hDlg, IDC_CB_CLASS );
					st->pResult->createTemplateIndex = ComboBox_GetCurSel( hwndCB );
					// get hair style
					hwndCB = GetDlgItem( hDlg, IDC_CB_HSTYLE );
					st->pResult->createHairStyle = ComboBox_GetCurSel( hwndCB );
					// get hair color
					hwndCB = GetDlgItem( hDlg, IDC_CB_HCOLOR );
					st->pResult->createHairColor = ComboBox_GetCurSel( hwndCB );
					// get face
					hwndCB = GetDlgItem( hDlg, IDC_CB_FSTYLE );
					st->pResult->createFace = ComboBox_GetCurSel( hwndCB );
					// get gender
					hwndCB = GetDlgItem( hDlg, IDC_CB_GENDER );
					st->pResult->createGender = ComboBox_GetCurSel( hwndCB );
					//
					EndDialog( hDlg, IDOK );
				} break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}
