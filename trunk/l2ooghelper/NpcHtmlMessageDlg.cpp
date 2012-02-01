#include "stdafx.h"
#include "Resource.h"
#include "Logger.h"
#include "NpcHtmlMessageDlg.h"
#include "L2Client.h"

extern HINSTANCE g_hInst;

NpcHtmlMessageDlg::NpcHtmlMessageDlg( void *pvL2Client )
{
	m_hDlg = m_hWndParent = NULL;
	m_html = NULL;
	m_itemID = m_questID = 0;
	m_npc = NULL;
	m_pvL2Client = pvL2Client;
	// childs
	m_hWndHtml = m_hWndCombo = m_hWndBypassText = NULL;
}

NpcHtmlMessageDlg::~NpcHtmlMessageDlg()
{
	destroyDialog();
	if( m_html )
	{
		free( m_html );
		m_html = NULL;
	}
}

bool NpcHtmlMessageDlg::createDialog( HWND hWndParent )
{
	m_hWndParent = hWndParent;
	m_hDlg = CreateDialogParam( g_hInst, MAKEINTRESOURCE(IDD_NPCHTML), hWndParent,
		NpcHtmlMessageDlgProc, (LPARAM)this );
	if( m_hDlg ) return true;
	// error
	m_hWndParent = NULL;
	return false;
}

void NpcHtmlMessageDlg::destroyDialog()
{
	if( m_hDlg )
	{
		DestroyWindow( m_hDlg );
		m_hDlg = NULL;
	}
}

void NpcHtmlMessageDlg::showWindow( bool bShow )
{
	if( m_hDlg )
	{
		if( bShow ) ShowWindow( m_hDlg, SW_SHOWNORMAL );
		else ShowWindow( m_hDlg, SW_HIDE );
	}
}

void NpcHtmlMessageDlg::displayNPCHTML( const TCHAR *html, L2Npc *pNpc,
		unsigned int itemID, unsigned int questID )
{
	if( !html ) return;
	//
	m_itemID = itemID;
	m_questID = questID;
	m_npc = pNpc;
	if( m_html ) free( m_html );
	m_html = _tcsdup( html );
	//
	NpcHtmlMessageDlg::wnd_initHtml( m_hDlg );
	showWindow( true );
}



class NpcHtmlLink
{
public:
	NpcHtmlLink() { text[0] = 0; bypass[0] = 0; next = NULL; }
	~NpcHtmlLink() { if( next ) delete next; }
public:
	wchar_t text[128];
	wchar_t bypass[128];
	NpcHtmlLink *next;
};

class NpcHtmlInfo
{
public:
	NpcHtmlInfo()
	{
		html = NULL;
		head = NULL;
	}
	~NpcHtmlInfo()
	{
		if( html ) free( html );
		html = NULL;
		if( head ) delete head;
		head = NULL;
	}
public:
	wchar_t *html;
	NpcHtmlLink *head;
};

void process_bypass( NpcHtmlInfo *info, wchar_t *tagHeader, wchar_t *tagTextStart )
{
	// a action="bypass -h npc_268437990_Quest 1106_teleport_to_fantasy_isle" msg="811;Fantasy Island"
	//log_error( LOG_OK, "[%S] [%S]\n", tagHeader, tagTextStart );
	wchar_t bypass[128]  = {0};
	wchar_t *pbypass = bypass;
	wchar_t text[128] = {0};
	wchar_t *ptext = text;
	wchar_t *w = wcschr( tagHeader, L'\"' );
	if( !w ) return;
	w++;
	while( (*w) != L'\"' ) { (*pbypass) = (*w); pbypass++, w++; }
	(*pbypass) = 0;
	w = tagTextStart;
	while( (*w) != L'<' ) { (*ptext) = (*w); ptext++, w++; }
	(*ptext) = 0;
	//
	NpcHtmlLink *adding = new NpcHtmlLink();
	wcscpy( adding->bypass, bypass );
	wcscpy( adding->text, text );
	adding->next = NULL;
	//
	if( !info->head ) info->head = adding;
	else
	{
		NpcHtmlLink *cur = info->head;
		while( cur->next ) cur = cur->next;
		cur->next = adding;
	}
}

NpcHtmlInfo *parse_NpcHtml( wchar_t *html )
{
	if( !html ) return NULL;
	NpcHtmlInfo *info = new NpcHtmlInfo();
	//
	info->html = (wchar_t *)malloc( 20480 );
	if( !info->html ) { delete info; return NULL; }
	info->html[0] = 0;
	//
	wchar_t *phtml = html;
	wchar_t *pinfohtml = info->html;
	wchar_t curChar = phtml[0];
	wchar_t tagHeader[256] = {0};
	wchar_t *ptagHeader = tagHeader;
	bool in_tag = false;
	//
	const int maxCharsInLine = 40;
	int curCharsInLine = 0;
	bool break_next_space = false;
	//
	while( curChar )
	{
		curChar = (*phtml);
		if( curChar == L'<' )
		{
			in_tag = true;
			phtml++;
			tagHeader[0] = (*phtml);
			ptagHeader = tagHeader;
			ptagHeader++;
			phtml++;
		}
		else if( curChar == L'>' )
		{
			(*ptagHeader) = 0;
			in_tag = false;
			phtml++;
			// parse passed tag
			if( (_wcsicmp( tagHeader, L"br" ) == 0) || (_wcsicmp( tagHeader, L"br1" ) == 0) )
			{
				(*pinfohtml) = L'\r';
				pinfohtml++;
				(*pinfohtml) = L'\n';
				pinfohtml++;
				(*pinfohtml) = 0;
				curCharsInLine = 0;
				break_next_space = false;
			}
			else if( (wcsstr( tagHeader, L"a " ) == tagHeader) || (wcsstr( tagHeader, L"A " ) == tagHeader) )
			{
				process_bypass( info, tagHeader, phtml );
			}
		}
		else if( (curChar == L'\r') || (curChar == L'\n') || (curChar == L' ') || (curChar == L'\t') )
		{
			if( break_next_space ) // insert line break
			{
				(*pinfohtml) = L'\r';
				pinfohtml++;
				(*pinfohtml) = L'\n';
				pinfohtml++;
				(*pinfohtml) = 0;
				curCharsInLine = 0; // new line started
				break_next_space = false; // we should not break at next whitespace )
			}
			// append space instead of \r, \n, \t
			else if( curChar == L' ' )
			{
				if( in_tag )
				{
					(*ptagHeader) = L' ';
					ptagHeader++;
					(*ptagHeader) = 0;
				}
				else
				{
					(*pinfohtml) = L' ';
					pinfohtml++;
				}
			}
			phtml++;
		}
		else
		{
			if( !in_tag )
			{
				if( curChar != L'\n' && curChar != L'\r' )
				{
					(*pinfohtml) = curChar;
					pinfohtml++;
					(*pinfohtml) = 0;
					//
					curCharsInLine++;
					if( curCharsInLine >= maxCharsInLine ) break_next_space = true;
				}
			}
			else
			{
				(*ptagHeader) = curChar;
				ptagHeader++;
				(*ptagHeader) = 0;
			}
			phtml++;
		}
	}
	//
	return info;
}

struct NpcHtmlDlgSt
{
	NpcHtmlMessageDlg *cls;
	NpcHtmlInfo *htmlInfo;
};

INT_PTR CALLBACK NpcHtmlMessageDlg::NpcHtmlMessageDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			// get ptr to class
			NpcHtmlMessageDlg *cls = (NpcHtmlMessageDlg *)lParam;
			// save all info as window user data
			NpcHtmlDlgSt *st = (NpcHtmlDlgSt *)malloc( sizeof(NpcHtmlDlgSt) );
			st->cls = cls; // pointer to class
			st->htmlInfo = NULL; // no html and links at this time
			SetWindowLongPtr( hDlg, GWLP_USERDATA, (LONG_PTR)st );
			// get children
			cls->m_hWndHtml = GetDlgItem( hDlg, IDC_EDIT1 );
			cls->m_hWndCombo = GetDlgItem( hDlg, IDC_COMBO1 );
			cls->m_hWndBypassText = GetDlgItem( hDlg, IDC_EDIT2 );
		} break;
	case WM_COMMAND:
		{
			switch( LOWORD( wParam ) )
			{
			case IDCANCEL: NpcHtmlMessageDlg::wnd_onCancel( hDlg ); break;
			case IDOK: NpcHtmlMessageDlg::wnd_onOK( hDlg ); break;
			case IDC_COMBO1:
				{
					int code = HIWORD(wParam);
					if( code == CBN_SELCHANGE ) NpcHtmlMessageDlg::wnd_onCombo1SelChange( hDlg );
				} break;
			}
			return FALSE; // message was not processed
		} break;
	case WM_DESTROY: NpcHtmlMessageDlg::wnd_onDestroy( hDlg ); break;
	case WM_KEYDOWN:
		{
			log_error( LOG_ERROR, "NpcHtmlMessageDlgProc(): WM_KEYDOWN\n" );
		} break;
	default: return FALSE; break;
	}
	return TRUE; // message was processed
}

void NpcHtmlMessageDlg::wnd_initHtml( HWND hDlg )
{
	NpcHtmlDlgSt *st = (NpcHtmlDlgSt *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	if( !st ) return;

	// set dlg title
	if( st->cls->m_npc )
	{
		TCHAR title[512] = {0};
		wsprintf( title, TEXT("%s (%s)"), st->cls->m_npc->charName, st->cls->m_npc->charTitle );
		SetWindowText( hDlg, title );
	}
	else SetWindowText( hDlg, TEXT("NpcHtmlMessage") );

	// clear prev. html
	SetWindowText( st->cls->m_hWndHtml, TEXT("no text") );
	if( st->htmlInfo )
	{
		delete st->htmlInfo;
		st->htmlInfo = NULL;
	}

	// parse and set html
	NpcHtmlInfo *htmlInfo = parse_NpcHtml( st->cls->m_html );
	st->htmlInfo = htmlInfo; // pointer to html info...
	if( st->htmlInfo )
		SetDlgItemText( hDlg, IDC_EDIT1, st->htmlInfo->html );

	// init combobox
	ComboBox_ResetContent( st->cls->m_hWndCombo );
	if( st->htmlInfo )
	{
		NpcHtmlLink *cur = st->htmlInfo->head;
		while( cur )
		{
			ComboBox_AddString( st->cls->m_hWndCombo, cur->text );
			cur = cur->next;
		}
	}

	// init bypass text
	SetWindowText( st->cls->m_hWndBypassText, TEXT("") );
}

void NpcHtmlMessageDlg::wnd_onCancel( HWND hDlg )
{
	ShowWindow( hDlg, SW_HIDE );
}

void NpcHtmlMessageDlg::wnd_onOK( HWND hDlg )
{
	NpcHtmlDlgSt *st = (NpcHtmlDlgSt *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	if( !st ) return;
	// save selection and post message to notify window
	int cur_sel = ComboBox_GetCurSel( st->cls->m_hWndCombo );
	if( cur_sel == -1 ) return;
	//
	ShowWindow( hDlg, SW_HIDE );
	//
	int nPassed = 0;
	NpcHtmlLink *cur = st->htmlInfo->head;
	while( cur )
	{
		if( nPassed == cur_sel )
		{
			if( cur->bypass )
			{
				// send request bypass to server
				class L2Client *pcl = (L2Client *)st->cls->m_pvL2Client;
				if( wcsstr( cur->bypass, L"bypass -h " ) )
				{
					// сам текст "bypass -h " не отсылается
					pcl->send_RequestBypassToServer( cur->bypass + 10 /*wcslen( L"bypass -h " )*/ );
				}
				else if( wcsstr( cur->bypass, L"link " ) )
				{
					// сам текст "link " не отсылается
					pcl->send_RequestLinkHtml( cur->bypass + 5 /*wcslen( L"link " )*/ );
				}
				else
				{
					log_error( LOG_ERROR, "Possibly wrong bypass in HTML from server!\n[%S]\n", cur->bypass );
				}
			}
			break;
		}
		else
		{
			cur = cur->next;
			nPassed++;
		}
	}
}

void NpcHtmlMessageDlg::wnd_onCombo1SelChange( HWND hDlg )
{
	NpcHtmlDlgSt *st = (NpcHtmlDlgSt *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	if( !st ) return;
	int cur_sel = ComboBox_GetCurSel( st->cls->m_hWndCombo );
	if( cur_sel == -1 ) return;
	//
	if( !st->htmlInfo ) return;
	int nPassed = 0;
	NpcHtmlLink *cur = st->htmlInfo->head;
	while( cur )
	{
		if( nPassed == cur_sel )
		{
			SetWindowText( st->cls->m_hWndBypassText, cur->bypass );
			break;
		}
		else
		{
			cur = cur->next;
			nPassed++;
		}
	}
}

void NpcHtmlMessageDlg::wnd_onDestroy( HWND hDlg )
{
	NpcHtmlDlgSt *st = (NpcHtmlDlgSt *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	if( !st ) return;
	delete st->htmlInfo;
	st->htmlInfo = NULL;
	free( st );
	SetWindowLongPtr( hDlg, GWLP_USERDATA, 0 );
}
