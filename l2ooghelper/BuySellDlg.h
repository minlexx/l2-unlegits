#ifndef H_BUYSELLDLG
#define H_BUYSELLDLG

#include "BuySellList.h"

class BuySellDlg
{
public:
	static const int TYPE_BUYLIST = 0;
	static const int TYPE_SELLLIST = 1;
	static const int TYPE_PRIVATESTOREBUYLIST = 2;
	static const int TYPE_PRIVATESTORESELLLIST = 3;
public:
	BuySellDlg( HWND hWndNotify, UINT uMessage, int type, TradeItemsList *list );
	~BuySellDlg();
	void run();
protected:
	HWND            m_hWndNotify;
	UINT            m_uNotifyMesasage;
	int             m_type;
	TradeItemsList *m_list_src;
	TradeItemsList *m_list_result;
protected:
	static DWORD WINAPI BuySellDlg_Thread( LPVOID lpParam );
	static INT_PTR CALLBACK BuySell_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	static void OnInitDialog( HWND hDlg, LPARAM lParam );
	static void OnDestroy( HWND hDlg );
	static void OnCommand( HWND hDlg, WPARAM wParam, LPARAM lParam );
	static void OnNotify( HWND hDlg, WPARAM wParam, LPARAM lParam );
protected:
	static void FillListViews( HWND hDlg, BuySellDlg *pcls );
};

#endif
