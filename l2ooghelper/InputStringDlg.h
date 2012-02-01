#ifndef H_DLG_INPUTSTRING
#define H_DLG_INPUTSTRING

class InputStringDlg
{
public:
	InputStringDlg();
	InputStringDlg( LPCTSTR ptitle, LPCTSTR pquestion, LPCTSTR ptext = NULL );
	~InputStringDlg();
	bool run( HWND hWndParent );
public:
	TCHAR title[256];
	TCHAR question[256];
	TCHAR text[256];
protected:
	static INT_PTR CALLBACK DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

#endif
