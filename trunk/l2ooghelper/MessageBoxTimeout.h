#ifndef H_MESSAGEBOX_TIMEOUT
#define H_MESSAGEBOX_TIMEOUT

class MessageBoxTimeout
{
public:
	static const int TYPE_PARTY = 0;
	static const int TYPE_CLAN = 1;
	static const int TYPE_FRIEND = 2;
	static const int TYPE_ALLY = 3;
	static const int TYPE_TRADEREQUEST = 4;
	static const int TYPE_CONFIRMDLG = 10;
public:
	MessageBoxTimeout( HWND hWndNotify, UINT uMessage, int type, LPTSTR szQuestiion,
		int timeoutSec = 0 );
	~MessageBoxTimeout();
	void run( const wchar_t *curCharName );
	void runConfirmDlg( const wchar_t *curCharName,
		unsigned int requestId, unsigned int sm_ID,
		void *pvActionL2Client );
protected:
	HWND          m_hWndNotify;
	UINT          m_uNotifyMesasage;
	int           m_type;
	TCHAR         m_question[8192];
	int           m_timeout;
	wchar_t       m_curChar[256];
	unsigned int  m_requestId;
	unsigned int  m_sm_ID;
	void         *m_pvL2Client;
protected:
	static DWORD WINAPI MBTimeout_Thread( LPVOID lpParam );
	static INT_PTR CALLBACK MBTimeout_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	static void OnInitDialog( HWND hDlg, LPARAM lParam );
	static void OnDestroy( HWND hDlg );
	static void OnCommand( HWND hDlg, WPARAM wParam, LPARAM lParam );
	static void OnTimer( HWND hDlg, WPARAM wParam );
};

#endif
