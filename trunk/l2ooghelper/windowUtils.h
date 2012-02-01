#ifndef H_WINDOW_UTILS
#define H_WINDOW_UTILS

void WULV_SetExtStyle( HWND hwndLV, bool fullRowSelect, bool gridLines );
void WULV_InsertColumn( HWND hwndLV, LPTSTR colName, int width, int idx, int fmt = LVCFMT_LEFT );
int  WULV_InsertItem( HWND hwndLV, LPTSTR itemText, int iItem = 0, LPARAM lParam = 0 );
int  WULV_InsertItem2( HWND hwndLV, LPTSTR itemText, int iItem, int iImage, LPARAM lParam );
void WULV_SetItem( HWND hwndLV, LPTSTR itemText, int iItem, int iSubItem );
LPARAM WULV_GetItemLPARAM( HWND hwndLV, int iItem );
void WULV_GetItemText( HWND hwndLV, int iItem, int iSubItem, LPTSTR out, int cchTextMax );
int WULV_FindItemByString( HWND hwndLV, int start_iItem, LPCTSTR string, bool exactMatch = false );
int WULV_GetCurSelItem( HWND hwndLV );

void WUTab_InsertItem( HWND hwndTab, LPTSTR text, int idx );

HTREEITEM WUTV_InsertItem( HWND hwndTV, HTREEITEM hParent, HTREEITEM hInsertAfter, LPTSTR text, LPARAM lParam,
	bool hasChildren, bool expanded, bool bold );

BOOL WU_SetDlgItemInt64( HWND hDlg, int nIDDlgItem, unsigned long long int value, BOOL bSigned );
unsigned long long int WU_GetDlgItemInt64( HWND hDlg, int nIDDlgItem, BOOL bSigned );

#endif
