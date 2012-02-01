#ifndef H_ACCOUNT_DLG
#define H_ACCOUNT_DLG

#include "Account.h"

class AccountDlg
{
public:
	AccountDlg() {}
	~AccountDlg() {}
public:
	void setAccount( const class Account& acc ) { account = acc; }
	Account getAccount() const { return account; }
public:
	bool run( HWND hWndParent );
protected:
	static INT_PTR CALLBACK AccountDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	Account account;
};

#endif
