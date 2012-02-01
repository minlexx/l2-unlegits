#ifndef H_TRADEP2PDLG
#define H_TRADEP2PDLG

#include "BuySellList.h"

class TradeP2PDlg
{
public:
	TradeP2PDlg( void *pvL2Client );
	~TradeP2PDlg();

public:
	BOOL createDialog( HWND hWndParent );
	void destroyDialog();
	void showWindow( bool bShow = true );
	const wchar_t *getPartnerName() const;

public:
	void clearContents();
	void tradeStart( const TradeItemsList *ownInv, const wchar_t *partnerName );
	void tradeOwnAdd( const TradeItem& it );
	void tradeOtherAdd( const TradeItem& it );
	void tradeDone();

protected:
	HWND            m_hDlg;
	HWND            m_hWndParent;
	void           *m_pvL2Client;
	TradeItemsList *m_ownInv;
	TradeItemsList *m_youGive;
	TradeItemsList *m_youRecv;
	// child lists (do not call DestroyWindow for these)
	HWND            m_hWndYourInv;
	HWND            m_hWndYouGive;
	HWND            m_hWndYouRecv;
	HWND            m_hWndStaticOtherPlayerName;
	wchar_t         m_partnerName[256];

protected: // Dlg proceure and event handlers
	static INT_PTR CALLBACK TradeP2PDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	static void wnd_fill_LV_from_list( HWND hWndLV, TradeItemsList *list );
	static void wnd_updateLists( TradeP2PDlg *pcls );
	static void wnd_OnOK( TradeP2PDlg *pcls );
	static void wnd_OnCancel( TradeP2PDlg *pcls );
	static void wnd_OnInvDblClick( TradeP2PDlg *pcls, int iItem );
};

#endif
