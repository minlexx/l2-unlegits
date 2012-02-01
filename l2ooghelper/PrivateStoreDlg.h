#ifndef H_PRIVATESTOREDLG
#define H_PRIVATESTOREDLG

#include "BuySellList.h"

class PrivateStoreManageDlg
{
public:
	static const int TYPE_BUY = 0;
	static const int TYPE_SELL = 1;
	static const int TYPE_CRAFT = 2;
	static const int TYPE_COMMONCRAFT = 3;
public:
	PrivateStoreManageDlg( HWND hWndNotify, UINT uMessage, int type, TradeItemsList *list1, TradeItemsList *list2 );
	~PrivateStoreManageDlg();
	void run();
protected:
	HWND            m_hWndNotify;
	UINT            m_uNotifyMesasage;
	int             m_type;
	TradeItemsList *m_list1;
	TradeItemsList *m_list2;
protected:
	static DWORD WINAPI Dlg_Thread( LPVOID lpParam );
	static INT_PTR CALLBACK Dlg_Proc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	static void OnInitDialog( HWND hDlg, LPARAM lParam );
	static void OnDestroy( HWND hDlg );
	static void OnCommand( HWND hDlg, WPARAM wParam, LPARAM lParam );
	static void OnNotify( HWND hDlg, WPARAM wParam, LPARAM lParam );
protected:
	static void FillListViews( HWND hDlg, PrivateStoreManageDlg *pcls );
};

#endif
