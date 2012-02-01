#ifndef H_NPC_HTML_MESSAGE_DLG
#define H_NPC_HTML_MESSAGE_DLG

class NpcHtmlMessageDlg
{
public:
	NpcHtmlMessageDlg( void *pvL2Client );
	~NpcHtmlMessageDlg();

public:
	bool createDialog( HWND hWndParent );
	void destroyDialog();
	void showWindow( bool bShow );
	void displayNPCHTML( const TCHAR *html, L2Npc *pNpc,
		unsigned int itemID = 0, unsigned int questID = 0 );

protected:
	HWND          m_hDlg;
	HWND          m_hWndParent;
	TCHAR        *m_html;
	unsigned int  m_itemID;
	unsigned int  m_questID;
	L2Npc        *m_npc;
	void         *m_pvL2Client;
	// child windows
	HWND          m_hWndHtml;
	HWND          m_hWndCombo;
	HWND          m_hWndBypassText;

protected:
	static INT_PTR CALLBACK NpcHtmlMessageDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	static void wnd_initHtml( HWND hDlg );
	static void wnd_onCancel( HWND hDlg );
	static void wnd_onOK( HWND hDlg );
	static void wnd_onCombo1SelChange( HWND hDlg );
	static void wnd_onDestroy( HWND hDlg );
};

#endif
