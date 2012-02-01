#include "StdAfx.h"
#include "Resource.h"
#include "Logger.h"
#include "MainWindow.h"
#include "L2Client.h"
#include "DBlayer.h"
// tab dialogs
#include "UserInfoTabDlg.h"
#include "UserInvTabDlg.h"
#include "UserSkillsTabDlg.h"
#include "UserBuffTabDlg.h"
#include "UserPartyTabDlg.h"
#include "MapItemsTabDlg.h"
// bot config dialog
#include "BotCfgDlg.h"
#include "BuySellDlg.h" // needed to process reply from this dialog
#include "PrivateStoreDlg.h" // needed to process reply from this dialog
// subclass windows
#include "ProgressHP.h"
#include "MapWnd.h"
#include "GlobalMapImages.h"

extern HINSTANCE g_hInst;
extern HWND g_hWnd;
extern GlobalMapImages g_mapImg;


WNDPROC OldEditProc = NULL; // старая процедура обработки сообщений Edit'a
// а вот наша новая - будет перехватывать нажатие Enter в строке эдита штоп посылать чат на Enter а не тока кнопкой
LRESULT CALLBACK NewEditProc( HWND hWndEdit, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( uMsg == WM_KEYUP ) // отпустили кнопку
	{
		if( wParam == 0x0D ) // код Enter'a
		{
			// пошлем нашему родителю сообщение якобы юзер нажал кнопку "Сказать"
			HWND hWndParent = GetParent( hWndEdit );
			PostMessage( hWndParent, WM_COMMAND, MAKEWPARAM(IDC_B_SAY,BN_CLICKED), 0 );
		}
	}
	// вызовем оригинальную процедуру обработки сообщений эдита
	LRESULT ret = CallWindowProc( OldEditProc, hWndEdit, uMsg, wParam, lParam );
	return ret;
}


L2Client::L2Client()
{
	// interface
	pInterface = new IL2Client( this );
	// ai
	m_ai = new L2ClientAI( pInterface );
	// non-modal dialogs
	tradep2pdlg = new TradeP2PDlg( this );
	npcHtmlDlg = new NpcHtmlMessageDlg( this );

	// lock
	InitializeCriticalSection( &cs_sendPacket );
	// map checks
	mapShowParams.init();
	//
	clear();
	scripter.init();
	scripter.setL2Client( this );
	sys_scripter.init();
	sys_scripter.setL2Client( this );
	bot_enabled = 0;
	bot_combat_enabled = 0;
	strcpy( sys_main_ai_script_filename, ".\\sys_scripts\\sys_ai_general.lua" );
	botipc_index = -1;
}

L2Client::~L2Client()
{
	// free resources
	if( hWnd ) DestroyWindow( hWnd );
	if( sock != INVALID_SOCKET )
	{
		L2PNet_shutdown( sock );
		L2PNet_closesocket( sock );
	}
	clear();
	scripter.deinit();
	DeleteCriticalSection( &cs_sendPacket );
	// ai
	if( m_ai ) delete m_ai;
	m_ai = NULL;
	// interface
	if( pInterface ) delete pInterface;
	pInterface = NULL;
	// non-modal dialogs
	if( tradep2pdlg ) delete tradep2pdlg;
	tradep2pdlg = NULL;
	if( npcHtmlDlg ) delete npcHtmlDlg;
	npcHtmlDlg = NULL;
	botipc_index = -1;
}

void L2Client::clear()
{
	bUsed = false;
	hWnd = NULL;
	hWndTabChat = NULL;
	hWndTabInfo = NULL;
	hWndMap = NULL;
	tab_id = -1;
	state = STATE_OFFLINE;
	sock = INVALID_SOCKET;
	hThread = NULL;
	dwThreadId = 0;
	botipc_index = -1;
	//
	account.clear();
	botcfg.removeAllValues();
	clearUserAndWorldState();
	// map checks
	mapShowParams.init();
}

void L2Client::clearUserAndWorldState()
{
	state = STATE_OFFLINE;
	bot_enabled = bot_combat_enabled = 0;
	// game state vars
	game_XOR_enabled = false;
	game_recvTimeoutMsec = 5000;
	game_sendTimeoutMsec = 5000;
	// user state vars
	usr.setUnused();
	inv.clear();
	skills.clear();
	etcStatus.clear();
	buffs.clear();
	party.clear();
	// world state
	world_tree.DeleteAll();
	world_chars.DeleteAll();
	world_npcs.DeleteAll();
	world_ground_items.DeleteAll();
	world_clans.DeleteAll();
	// non modal dialogs
	if( tradep2pdlg ) tradep2pdlg->clearContents();
	if( npcHtmlDlg ) npcHtmlDlg->showWindow( false );
}

IL2Client *L2Client::getInterface()
{
	if( !this->pInterface ) { this->pInterface = new IL2Client( this ); }
	return pInterface;
}

void L2Client::setState( L2Client::STATE newState )
{
	state = newState;
	char state_name[32] = {0};
	switch( state )
	{
	case STATE_OFFLINE:          strcpy( state_name, "OFFLINE" ); break;
	case STATE_CONNECTING_LOGIN: strcpy( state_name, "CONNECTING_LOGIN" ); break;
	case STATE_CONNECTED_LOGIN:  strcpy( state_name, "CONNECTED_LOGIN" ); break;
	case STATE_AUTHED_LOGIN:     strcpy( state_name, "AUTHED_LOGIN" ); break;
	case STATE_CONNECTING_GAME:  strcpy( state_name, "CONNECTING_GAME" ); break;
	case STATE_CONNECTED_GAME:   strcpy( state_name, "CONNECTED_GAME" ); break;
	case STATE_AUTHED_GAME:      strcpy( state_name, "AUTHED_GAME" ); break;
	case STATE_IN_GAME:          strcpy( state_name, "IN_GAME" ); break;
	}
	log_error( LOG_DEBUG, "L2Client[%d]: STATE CHANGE TO [%s]\n", tab_id, state_name );
}

HWND L2Client::createWindow( HWND hWndParent )
{
	hWnd = CreateDialogParam( g_hInst, MAKEINTRESOURCE(IDD_TAB), hWndParent,
		L2ClientDlgProc, (LPARAM)this );
	if( hWnd )
	{
		ShowWindow( hWnd, SW_HIDE );
		RECT r;
		GetClientRect( hWndParent, &r );
		MoveWindow( hWnd, 1, 25, r.right - r.left - 3, r.bottom - r.top-25, TRUE );
		// create non modal child dialogs
		if( tradep2pdlg ) // трейд
		{
			tradep2pdlg->createDialog( hWnd );
			tradep2pdlg->showWindow( false );
		}
		if( npcHtmlDlg ) // нпц хтмл
		{
			npcHtmlDlg->createDialog( hWnd );
			npcHtmlDlg->showWindow( false );
		}
		// load initial bot config, if set
		loadDefaultBotConfig();
	}
	return hWnd;
}

void L2Client::showWindow()
{
	if( !hWnd ) return;
	ShowWindow( hWnd, SW_SHOWNORMAL );
}

void L2Client::hideWindow()
{
	if( !hWnd ) return;
	ShowWindow( hWnd, SW_HIDE );
}

void L2Client::updateWindow()
{
	if( !hWnd ) return;
	//ShowWindow( hWnd, SW_SHOW );
	InvalidateRect( hWnd, NULL, TRUE );
	UpdateWindow( hWnd );
}

void L2Client::closeTab()
{
	if( !hWnd ) return;
	// disconnect force before close tab? :)
	disconnectClient( true );
	// stop scripts
	if( scripter.isScriptThreadRunning() ) scripter.stopScriptThread( 1000 );
	if( sys_scripter.isScriptThreadRunning() ) sys_scripter.stopScriptThread( 1000 );
	// destroy chils dialogs
	if( tradep2pdlg ) tradep2pdlg->destroyDialog();
	if( npcHtmlDlg ) npcHtmlDlg->destroyDialog();
	// destroy window
	DestroyWindow( hWnd );
	PostMessage( g_hWnd, WMMY_DELETETAB, 0, (LPARAM)tab_id );
	clear();
}

void L2Client::postUpdateUI( UIUPDATECODE updateCode, LPARAM lParam )
{
	PostMessage( this->hWnd, WMMY_UI_UPDATE, (WPARAM)updateCode, lParam );
}

int L2Client::getScaleSliderPos()
{
	int iPos = (int)SendMessage( hWndSliderMapScale, TBM_GETPOS, 0, 0 );
	return iPos;
}

void L2Client::postUpdateMap()
{
	InvalidateRect( hWndMap, NULL, FALSE );
	PostMessage( hWndMap, WM_PAINT, 0, 0 );
}

void L2Client::getMapShowParams( st_MapShowParams *p )
{
	p->mobs = mapShowParams.mobs;
	p->npcs = mapShowParams.npcs;
	p->pcs = mapShowParams.pcs;
	p->items = mapShowParams.items;
	p->vectors = mapShowParams.vectors;
	p->fightrange = mapShowParams.fightrange;
	p->zrange = mapShowParams.zrange;
	p->realmap = mapShowParams.realmap;
	p->sellshop = mapShowParams.sellshop;
	p->craftshop = mapShowParams.craftshop;
	p->buyshop = mapShowParams.buyshop;
}

// loads initial bot config, if config file is set
void L2Client::loadDefaultBotConfig()
{
	if( account.configFileName[0] != 0 )
	{
		wchar_t cfn[256] = {0};
		wsprintfW( cfn, L".\\configs\\%s", account.configFileName );
		log_error( LOG_DEBUG, "L2Client::loadDefaultBotConfig(): loading default bot config from file [%S]\n", cfn );
		bool loaded = botcfg.readConfig( cfn, true );
		if( !loaded )
		{
			log_error( LOG_ERROR, "Failed to load default bot config from file [%S]!\n", cfn );
			addChatToTabFormat( CHAT_SYS, L"Failed to load default bot config from file [%s]!\n", cfn );
		}
		else
		{
			log_error( LOG_DEBUG, "Loaded default bot config from file [%S]\n", cfn );
			addChatToTabFormat( CHAT_SYS, L"Loaded default bot config from file [%s]\n", cfn );
		}
	}
	else log_error( LOG_DEBUG, "L2Client::loadDefaultBotConfig(): default cfg file not set\n" );
}


void L2Client::ensureRemoveSelfFromBotIPC()
{
	if( botipc_index >= 0 )
	{
		bool remove_ok = BotIPC::getInstance()->delBot( botipc_index );
		if( remove_ok )
			log_error( LOG_DEBUG, "L2Client: Removed self from Bot IPC (idx %d).\n", botipc_index );
		else
			log_error( LOG_WARNING, "L2Client: Failed to remove self from bot IPC (idx %d)!\n", botipc_index );
		botipc_index = -1; // mark not registered
	}
}


void L2Client::addChatToTab( CHAT_TAB_ID chatTabId, wchar_t *message, wchar_t *from )
{
	if( !from )	addChatStringToTab_internal( chatTabId, chatTabId, message );
	else
	{
		wchar_t *mes = (wchar_t *)malloc( wcslen( message )*2 + 2 + wcslen( from )*2 + 2 + 16 );
		if( mes )
		{
			wsprintfW( mes, L"%s: %s", from, message );
			addChatStringToTab_internal( chatTabId, chatTabId, mes );
			free( mes );
		}
	}
}

void L2Client::addChatToTabFormat( CHAT_TAB_ID chatTabId, wchar_t *_Format, ... )
{
	va_list _ArgList;
	va_start( _ArgList, _Format );
	wchar_t message[256];
	_vsnwprintf( message, 256, _Format, _ArgList );
	message[255] = 0;
	addChatStringToTab_internal( chatTabId, chatTabId, message );
}

void L2Client::addChatAnnouncement( wchar_t *message )
{
	addChatStringToTab_internal( CHAT_SYS, CHAT_SYS, message );
	addChatStringToTab_internal( CHAT_ALL, CHAT_SYS, message );
}

// chatTabId - sets the tab to put chat text to
// origTabId - sets only chat text color
void L2Client::addChatStringToTab_internal( CHAT_TAB_ID chatTabId, CHAT_TAB_ID origTabId, wchar_t *message, bool scroll )
{
#ifdef CHAT_USE_RICHEDIT
	static wchar_t nl[2] = { L'\r', L'\0' }; // newline line-break
	// move selection after last char
	CHARRANGE cr;
	cr.cpMin = -1;
	cr.cpMax = -1;
	/*LRESULT lres =*/ SendMessage( hWndTabChat_item[ chatTabId ], EM_EXSETSEL, 0, (LPARAM)&cr );
	//log_error( LOG_OK, "CharRange: %d..%d   (LRESULT %d)\n", cr.cpMin, cr.cpMax, lres );
	// set color
	CHARFORMAT2 cf2;
	memset( &cf2, 0, sizeof(cf2) );
	cf2.cbSize      = sizeof(cf2);
	cf2.dwMask      = CFM_BACKCOLOR | CFM_COLOR;
	cf2.crBackColor = RGB(0,0,0);
	cf2.crTextColor = addChatStringToTab_internal_getChatTabColor( origTabId );
	cf2.dwEffects   = 0; //CFE_LINK | CFE_AUTOCOLOR | CFE_AUTOBACKCOLOR;
	SendMessage( hWndTabChat_item[ chatTabId ], EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf2 );
	// add string
	SendMessage( hWndTabChat_item[ chatTabId ], EM_REPLACESEL, FALSE, (LPARAM)message );
	// add line-break
	SendMessage( hWndTabChat_item[ chatTabId ], EM_REPLACESEL, FALSE, (LPARAM)nl );
#else // using listbox
	ListBox_AddString( hWndTabChat_item[ chatTabId ], message );
#endif
	if( scroll ) PostMessage( hWndTabChat_item[ chatTabId ], WM_VSCROLL, SB_BOTTOM, 0 );
	//
	// shout, trade, pm, party, clan, ally, hero chats got to tab "All" also
	if( addChatStringToTab_internal_shouldAlsoAddToAll( chatTabId ) )
		addChatStringToTab_internal( CHAT_ALL, origTabId, message, scroll );
}

// return true, если чат-сообщение в заданный канал нужно продублировать во вкладку "All"
bool L2Client::addChatStringToTab_internal_shouldAlsoAddToAll( CHAT_TAB_ID chatTabId )
{
	// shout, trade, pm, party, clan, ally, hero chats go to tab "All" also
	if( chatTabId == CHAT_SHOUT   ||  chatTabId == CHAT_TRADE ||
		chatTabId == CHAT_WHISPER || chatTabId == CHAT_PARTY  ||
		chatTabId == CHAT_CLAN    || chatTabId == CHAT_ALLY   || chatTabId == CHAT_HERO )
		return true;
	return false;
}

COLORREF L2Client::addChatStringToTab_internal_getChatTabColor( CHAT_TAB_ID chatTabId )
{
	COLORREF crTextColor = RGB(255,255,255); // CHAT_ALL, CHAT_DMG
	switch( chatTabId )
	{
	case CHAT_SYS:     crTextColor = RGB(200,255,255); break;
	case CHAT_TRADE:   crTextColor = RGB(255,128,192); break;
	case CHAT_SHOUT:   crTextColor = RGB(255,128,64);  break;
	case CHAT_PARTY:   crTextColor = RGB(0,255,64);    break;
	case CHAT_CLAN:    crTextColor = RGB(74,74,255);   break;
	case CHAT_ALLY:    crTextColor = RGB(128,255,166); break;
	case CHAT_HERO:    crTextColor = RGB(0,128,255);   break;
	case CHAT_WHISPER: crTextColor = RGB(255,0,255);   break;
	}
	return crTextColor;
}


void L2Client::onButtonSystem()
{
	POINT pt;
	GetCursorPos( &pt );
	HMENU hMenu = CreatePopupMenu();
	UINT uFlags;
	//
	uFlags = MF_STRING;
	if( state != STATE_OFFLINE ) uFlags |= MF_GRAYED;
	AppendMenu( hMenu, uFlags, 1, TEXT("Login") );
	//
	uFlags = MF_STRING;
	if( state != STATE_IN_GAME ) uFlags |= MF_GRAYED;
	AppendMenu( hMenu, uFlags, 3, TEXT("Logout") );
	//
	uFlags = MF_STRING;
	if( state == STATE_OFFLINE ) uFlags |= MF_GRAYED;
	AppendMenu( hMenu, uFlags, 2, TEXT("Disconnect") );
	//
	uFlags = MF_STRING;
	if( state == STATE_OFFLINE ) uFlags |= MF_GRAYED;
	AppendMenu( hMenu, uFlags, 4, TEXT("Disconnect FORCED") );
	//
	int cmd = TrackPopupMenu( hMenu,
		TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD,
		pt.x, pt.y, 0, hWnd, NULL );
	DestroyMenu( hMenu );
	switch( cmd )
	{
	case 1: startClient(); break;
	case 2: disconnectClient(); break;
	case 3: logoutClient(); break;
	case 4: disconnectClient( true ); break;
	}
}

void L2Client::onButtonConfig()
{
	BotCfgDlg_Run( hWnd, this );
}

void L2Client::onButtonScript()
{
	POINT pt;
	GetCursorPos( &pt );
	HMENU hMenu = CreatePopupMenu();
	UINT uFlags;
	char last_filename[256] = {0};
	this->scripter.getLastFileName( last_filename );
	bool isRunning = false;
	if( this->scripter.isScriptThreadRunning() ) isRunning = true;
	//
	// create array with script file names
	//
	std::map<int, std::string> script_files;
	int file_index = 1;
	char curDir[256] = {0};
	char scripts_Dir[256] = {0};
	GetCurrentDirectoryA( 250, curDir );
	wsprintfA( scripts_Dir, "%s\\scripts\\*.lua*", curDir );
	WIN32_FIND_DATAA fdata;
	HANDLE hFind = FindFirstFileA( scripts_Dir, &fdata );
	HMENU hMenuScriptFiles = CreateMenu();
	const int max_script_files = 256; // not more than 256 files
	if( hFind != INVALID_HANDLE_VALUE )
	{
		std::string stds;
		uFlags = MF_STRING;
		if( isRunning ) uFlags |= MF_GRAYED;
		AppendMenuA( hMenuScriptFiles, uFlags, 1000 + file_index, fdata.cFileName );
		stds.assign( fdata.cFileName );
		script_files[file_index] = stds;
		file_index++;
		while( FindNextFileA( hFind, &fdata ) && (file_index <= max_script_files) )
		{
			AppendMenuA( hMenuScriptFiles, uFlags, 1000 + file_index, fdata.cFileName );
			stds.assign( fdata.cFileName );
			script_files[file_index] = stds;
			file_index++;
		}
		FindClose( hFind );
		hFind = INVALID_HANDLE_VALUE;
	}
	// were any files found?
	if( file_index == 0 ) // no
		AppendMenu( hMenuScriptFiles, MF_STRING | MF_GRAYED, 1000, TEXT("<No files>") );
	//
	//
	uFlags = MF_POPUP; // Sub Menu
	if( isRunning ) uFlags |= MF_GRAYED;
	AppendMenu( hMenu, uFlags, (UINT_PTR)hMenuScriptFiles, TEXT("Запустить скрипт") );
	//
	uFlags = MF_STRING;
	if( !isRunning ) uFlags |= MF_GRAYED;
	if( this->scripter.isScriptThreadPaused() ) uFlags |= MF_GRAYED;
	AppendMenu( hMenu, uFlags, 2, TEXT("Остановить скрипт") );
	//
	uFlags = MF_STRING;
	if( !this->scripter.isScriptThreadPaused() && isRunning )
		AppendMenu( hMenu, uFlags, 3, TEXT("Приостановить скрипт") );
	else if( this->scripter.isScriptThreadPaused() && isRunning )
		AppendMenu( hMenu, uFlags, 4, TEXT("Продолжить скрипт") );
	else
	{
		uFlags |= MF_GRAYED;
		AppendMenu( hMenu, uFlags, 4, TEXT("Приостановить/Продолжить скрипт") );
	}
	//
	uFlags = MF_STRING | MF_GRAYED;
	if( last_filename[0] && !isRunning ) uFlags = MF_STRING;
	AppendMenu( hMenu, uFlags, 5, TEXT("Запустить прошлый скрипт") );
	// run menu
	int cmd = TrackPopupMenu( hMenu,
		TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD,
		pt.x, pt.y, 0, hWnd, NULL );
	DestroyMenu( hMenu );
	// check command
	if( (cmd >= 1000) && (cmd <= 1000+max_script_files) )
	{
		if( cmd == 1000 ) return; // no files found!
		file_index = cmd - 1000;
		std::string fn(".\\scripts\\");
		fn.append( script_files[file_index] );
		log_error( LOG_DEBUG, "onButtonScript(): file [%s]\n", fn.c_str() );
		scripter.startScriptThread( fn.c_str() );
	}
	switch( cmd )
	{
	//case 1: // open script...
	//	{
	//		wchar_t curDir[256] = {0};
	//		wchar_t newDir[256] = {0};
	//		GetCurrentDirectoryW( 200, curDir );
	//		wsprintfW( newDir, L"%s\\scripts", curDir );
	//		SetCurrentDirectoryW( newDir );
	//		//
	//		wchar_t fileName[256] = {0};
	//		OPENFILENAMEW ofn;
	//		memset( &ofn, 0, sizeof(ofn) );
	//		ofn.lStructSize = sizeof(ofn);
	//		ofn.hwndOwner = hWnd;
	//		ofn.hInstance = g_hInst;
	//		ofn.lpstrFilter = TEXT("Lua files\0*.lua\0All files\0*.*\0\0");
	//		ofn.lpstrFile = fileName;
	//		ofn.nMaxFile = 255;
	//		ofn.lpstrTitle = TEXT("Откуда читать?");
	//		ofn.lpstrInitialDir = newDir;
	//		ofn.Flags = OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	//		if( GetOpenFileNameW( &ofn ) )
	//		{
	//			char fileNameA[256] = {0};
	//			WideCharToMultiByte( CP_ACP, 0, fileName, -1, fileNameA, 255, NULL, NULL );
	//			scripter.startScriptThread( fileNameA );
	//		}
	//		SetCurrentDirectoryW( curDir );
	//	} break;
	case 2: // stop
		{
			this->scripter.stopScriptThread( 10000, true );
		} break;
	case 3: // pause
		{
			if( !this->scripter.pauseScriptThread( true ) )
				log_error( LOG_WARNING, "Warning! cannot pause script...\n" );
		} break;
	case 4: // resume
		{
			if( !this->scripter.pauseScriptThread( false ) )
				log_error( LOG_WARNING, "Warning! cannot resume script...\n" );
		} break;
	case 5: // run prev. script file
		{
			this->scripter.startScriptThread( NULL );
		} break;
	}
}

void L2Client::onButtonActionTarget()
{
	if( usr.targetObjectID != 0 ) send_Action( usr.targetObjectID );
}

void L2Client::onButtonAttackTarget()
{
	if( usr.targetObjectID != 0 ) send_AttackRequest( usr.targetObjectID );
}

void L2Client::onButtonCancelTarget()
{
	if( usr.targetObjectID != 0 ) send_RequestTargetCanceld();
}

void L2Client::onButtonSitStand()
{
	send_RequestActionUse( 0 );
}

void L2Client::onButtonRunWalk()
{
	send_RequestActionUse( 1 );
}

void L2Client::onButtonSay()
{
	TCHAR chat[512] = {0};
	TCHAR tell_to[128] = {0};
	TCHAR *ptell_to = NULL;
	GetDlgItemText( hWnd, IDC_E_CHAT, chat, 511 );
	//
	int tab_sel = TabCtrl_GetCurSel( this->hWndTabChat );
	unsigned int l2chat = L2_CHAT_MESSAGE::ALL;
	//log_error( LOG_OK, "Say to tab %d\n", tab_sel );
	if( tab_sel == CHAT_DMG ) return;
	if( state != STATE_IN_GAME ) return;
	// clear input line
	SetDlgItemText( hWnd, IDC_E_CHAT, TEXT("") );
	// is it command?
	if( chat[0] == L'/' )
	{
		if( handleSayCommand( chat ) ) return;
	}
	// determine l2chat type
	switch( tab_sel )
	{
	case CHAT_ALL: l2chat = L2_CHAT_MESSAGE::ALL; break;
	case CHAT_TRADE: l2chat = L2_CHAT_MESSAGE::TRADE; break;
	case CHAT_SHOUT: l2chat = L2_CHAT_MESSAGE::SHOUT; break;
	case CHAT_PARTY: l2chat = L2_CHAT_MESSAGE::PARTY; break;
	case CHAT_CLAN: l2chat = L2_CHAT_MESSAGE::CLAN; break;
	case CHAT_ALLY: l2chat = L2_CHAT_MESSAGE::ALLIANCE; break;
	case CHAT_HERO: l2chat = L2_CHAT_MESSAGE::HERO_VOICE; break;
	case CHAT_WHISPER:
		{
			l2chat = L2_CHAT_MESSAGE::TELL;
			if( chat[0] == L'\"' )
			{
				int i = 1;
				while( chat[i] != L' ' ) { tell_to[i-1] = chat[i]; i++; }
				tell_to[i-1] = 0;
				ptell_to = tell_to;
				TCHAR temp[512];
				lstrcpy( temp, chat );
				lstrcpy( chat, temp + i + 1 );
			}
		} break;
	}
	// send chat
	send_Say2( l2chat, chat, ptell_to );
}

// returns true if command was handled
bool L2Client::handleSayCommand( LPWSTR text )
{
	if( !text ) return false;
	if( text[0] != L'/' ) return false;
	// use admin command?
	if( (text[0] == L'/') && (text[1] == L'/') )
	{
		if( wcslen( text ) > 3 )
		{
			send_SendBypassBuildCmd( text );
			return true;
		}
	}
	if( wcsstr( text, L"/invite" ) == text )
	{
		wchar_t *target = text + wcslen( L"/invite " );
		send_RequestJoinParty( target );
		return true;
	}
	if( wcsstr( text, L"/target" ) == text )
	{
		pInterface->game_TargetByName( text + wcslen( L"/target " ) );
		return true;
	}
	// user commands
	if( wcsstr( text, L"/loc" ) == text )
	{
		send_RequestUserCommand( L2Game_RequestUserCommand::CMD_LOC );
		return true;
	}
	if( wcsstr( text, L"/unstuck" ) == text )
	{
		send_RequestUserCommand( L2Game_RequestUserCommand::CMD_UNSTUCK );
		return true;
	}
	if( wcsstr( text, L"/time" ) == text )
	{
		send_RequestUserCommand( L2Game_RequestUserCommand::CMD_TIME );
		return true;
	}
	if( wcsstr( text, L"/gmlist" ) == text )
	{
		send_RequestGMList();
		return true;
	}
	// TODO: other user commands
	return false; // unhandled
}

void L2Client::onButtonActions()
{
	POINT pt;
	GetCursorPos( &pt );
	HMENU hMenu = CreatePopupMenu();
	//
	HMENU hMenuResurrect = CreatePopupMenu();
	AppendMenu( hMenuResurrect, MF_STRING, 10, TEXT("Встать в деревню") );
	AppendMenu( hMenuResurrect, MF_STRING, 11, TEXT("Встать в КланХолл") );
	AppendMenu( hMenuResurrect, MF_STRING, 12, TEXT("Встать в замок") );
	AppendMenu( hMenuResurrect, MF_STRING, 13, TEXT("Встать в форт") );
	AppendMenu( hMenuResurrect, MF_STRING, 14, TEXT("Встать к осадному флагу") );
	AppendMenu( hMenuResurrect, MF_STRING, 15, TEXT("Рес на месте (для ГМ или во время фестиваля)") );
	AppendMenu( hMenu, MF_POPUP, (UINT_PTR)hMenuResurrect, TEXT("После смерти ->") );
	//
	HMENU hMenuTarget = CreatePopupMenu();
	AppendMenu( hMenuTarget, MF_STRING, 20, TEXT("Торговать") );
	AppendMenu( hMenuTarget, MF_STRING, 21, TEXT("Рекомендовать") );
	AppendMenu( hMenuTarget, MF_STRING, 22, TEXT("Пригласить в партию") );
	AppendMenu( hMenu, MF_POPUP, (UINT_PTR)hMenuTarget, TEXT("Цель ->") );
	//
	HMENU hMenuStore = CreatePopupMenu();
	AppendMenu( hMenuStore, MF_STRING, 30, TEXT("Сесть на продажу") );
	AppendMenu( hMenuStore, MF_STRING, 31, TEXT("Сесть на скупку") );
	AppendMenu( hMenuStore, MF_STRING, 32, TEXT("Сесть на крафт") );
	AppendMenu( hMenuStore, MF_STRING, 33, TEXT("Сесть на коммон крафт") );
	AppendMenu( hMenuStore, MF_STRING, 34, TEXT("Сесть на продажу (package sale)") );
	AppendMenu( hMenu, MF_POPUP, (UINT_PTR)hMenuStore, TEXT("Личный магазин ->") );
	//
	// run menu
	int cmd = TrackPopupMenu( hMenu,
		TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD,
		pt.x, pt.y, 0, hWnd, NULL );
	DestroyMenu( hMenu );
	// command act
	switch( cmd )
	{
	// resurrect commands
	case 10: send_RequestRestartPoint( 0 ); break;
	case 11: send_RequestRestartPoint( 1 ); break; // ch
	case 12: send_RequestRestartPoint( 2 ); break; // castle
	case 13: send_RequestRestartPoint( 3 ); break; // fort
	case 14: send_RequestRestartPoint( 4 ); break; // siege HQ
	case 15: send_RequestRestartPoint( 5 ); break; // Fixed/festival
	// target commands
	case 20: send_TradeRequest(); break;
	case 21: send_RequestEvaluate(); break; // recommend
	case 22: // invite
		{
			// read config - item distrubution
			unsigned int itemDistribution = botcfg.getValInt( "party_loot" );
			// valudate target: must exist and be player
			if( usr.targetObjectID > 0 )
			{
				WorldObjectTreeNode wotNode;
				if( world_tree.GetInfoByObjectID( usr.targetObjectID, &wotNode ) )
				{
					if( wotNode.getObjectType() == L2OT_PC )
					{
						L2Player *pCha = world_chars.chars_array[wotNode.getArrayIdx()];
						send_RequestJoinParty( pCha->charName, itemDistribution );
					}
				}
			}
		} break;
	// private store commands
	case 30: send_RequestActionUse( 10 ); break; // RequestActionUse - private store sell
	case 31: send_RequestActionUse( 28 ); break; // RequestActionUse - private store buy
	case 32: send_RequestActionUse( 37 ); break; // RequestActionUse - private dwarven craft 37
	case 33: break; // RequestActionUse - private common craft 37 or 51? validate;
	case 34: send_RequestActionUse( 61 ); break; // RequestActionUse - private store sell package
	}
}

#ifdef _DEBUG
void L2Client::onButtonDebug()
{
	POINT pt;
	GetCursorPos( &pt );
	HMENU hMenu = CreatePopupMenu();
	UINT uFlags;
	uFlags = MF_STRING;
	//
	AppendMenu( hMenu, uFlags, 1, TEXT("Dump chars") );
	AppendMenu( hMenu, uFlags, 2, TEXT("Dump NPCs") );
	AppendMenu( hMenu, uFlags, 3, TEXT("Dump Ground Items") );
	AppendMenu( hMenu, uFlags, 4, TEXT("Dump Clans") );
	AppendMenu( hMenu, MF_SEPARATOR, 0, NULL );
	AppendMenu( hMenu, uFlags, 10, TEXT("Global map: loaded images status") );
	AppendMenu( hMenu, uFlags, 11, TEXT("Global map: preload all") );
	AppendMenu( hMenu, uFlags, 12, TEXT("Global map: unload all") );
	AppendMenu( hMenu, uFlags, 13, TEXT("Global map: current square info") );
	AppendMenu( hMenu, uFlags, 14, TEXT("Global map: RELOAD current geo square (dangeorus!)") );
	AppendMenu( hMenu, MF_SEPARATOR, 0, NULL );
	AppendMenu( hMenu, uFlags, 20, TEXT("BotCfg: dump all config to console") );
	AppendMenu( hMenu, MF_SEPARATOR, 0, NULL );
	AppendMenu( hMenu, uFlags, 30, TEXT("DBLayer: dump status to console") );
	AppendMenu( hMenu, MF_SEPARATOR, 0, NULL );
	AppendMenu( hMenu, uFlags, 50, TEXT("BotIPC: dump all info to console") );
	AppendMenu( hMenu, uFlags, 51, TEXT("BotIPC: dump my info to console") );
	//
	int cmd = TrackPopupMenu( hMenu,
		TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD,
		pt.x, pt.y, 0, hWnd, NULL );
	DestroyMenu( hMenu );
	switch( cmd )
	{
	case 1: this->world_chars.DisplayToConsole(); break;
	case 2: this->world_npcs.DisplayToConsole(); break;
	case 3: this->world_ground_items.DisplayToConsole(); break;
	case 4: this->world_clans.DisplayConsole(); break;
	case 10:
		{
			int lc = g_mapImg.getLoadedCount();
			log_error_np( LOG_OK, "[GlobalMap] loaded %d / 150 images\n", lc );
		} break;
	case 11:
		{
			g_mapImg.preloadAllSquares();
			int lc = g_mapImg.getLoadedCount();
			log_error_np( LOG_OK, "[GlobalMap] loaded %d / 150 images\n", lc );
		} break;
	case 12:
		{
			g_mapImg.unloadAllSquares();
			int lc = g_mapImg.getLoadedCount();
			log_error_np( LOG_OK, "[GlobalMap] loaded %d / 150 images\n", lc );
		} break;
	case 13:
		{
			int geo_x, geo_y, st_l2_x, st_l2_y;
			g_mapImg.GetGeoMapSquareXYByL2Coords( usr.x, usr.y, &geo_x, &geo_y );
			g_mapImg.GetGeoSquareStartL2Coords( geo_x, geo_y, &st_l2_x, &st_l2_y );
			log_error_np( LOG_OK, "[GlobalMap] User is at: %d,%d,%d. Reported geo square: %d_%d\n",
				usr.x, usr.y, usr.z, geo_x, geo_y );
			log_error_np( LOG_OK, "[GlobalMap] Geo square start coords: %d, %d\n", st_l2_x, st_l2_y );
			if( g_mapImg.isSquareLoaded( geo_x, geo_y ) ) log_error_np( LOG_OK, "[GlobalMap] current square is loaded\n" );
			else log_error_np( LOG_OK, "[GlobalMap] current square is NOT loaded\n" );
		} break;
	case 14:
		{
			int geo_x, geo_y;
			g_mapImg.GetGeoMapSquareXYByL2Coords( usr.x, usr.y, &geo_x, &geo_y );
			log_error_np( LOG_OK, "[GlobalMap] Reloading geo square: %d_%d... \n", geo_x, geo_y );
			g_mapImg.unloadSquare( geo_x, geo_y );
			g_mapImg.preloadSquare( geo_x, geo_y );
			log_error_np( LOG_OK, "[GlobalMap] Reloaded geo square: %d_%d.\n", geo_x, geo_y );
			SendMessage( this->hWndMap, WM_USER, 0, 0 ); // tell map window to reload image too
		} break;
	case 20:
		{
			BotConfigValue val;
			int i = 0;
			log_error_np( LOG_OK, "BotCfg: ====== DUMPING ======\n" );
			for( i=0; i<BotConfig::MAX_VALUES; i++ )
			{
				val = botcfg.getValByIdx( i );
				if( val.getType() == CFG_TYPE_NONE ) continue;
				log_error_np( LOG_OK, "BotCfg: #%4d %s ", i, val.name() );
				switch( val.getType() )
				{
				case CFG_TYPE_INT: log_error_np( LOG_OK, " INT: %d\n", val.getInt() ); break;
				case CFG_TYPE_FLT: log_error_np( LOG_OK, " FLT: %0.6f\n", val.getFlt() ); break;
				case CFG_TYPE_STR: log_error_np( LOG_OK, " STR: \"%s\"\n", val.getStr() ); break;
				}
			}
			log_error_np( LOG_OK, "BotCfg: ====== DUMP END ======\n" );
		} break;
	case 30:
		{
			DBLayer_printStats();
		} break;
	case 50:
		{
			BotIPC::getInstance()->dumpToLog();
		} break;
	case 51:
		{
			log_error( LOG_DEBUG, "BotIPC: my index is %d\n", botipc_index );
			if( botipc_index >= 0 )
			{
				BOT_INFO bi;
				BotIPC::getInstance()->getBotInfo( botipc_index, &bi );
				log_error( LOG_DEBUG, "BotIPC: my slave, master status: %d, %d\n", (int)bi.isSlave, (int)bi.isMaster );
				log_error( LOG_DEBUG, "BotIPC: my follow status: %d, %u\n", (int)bi.followEnable, bi.oidFollow );
				log_error( LOG_DEBUG, "BotIPC: my npc follow chat status: %d (%u) [%S]\n", (int)bi.followNpcChatEnable, bi.npcChatOid, bi.npcChatStr );
			}
		} break;
	}
}
#endif

INT_PTR CALLBACK L2Client::L2ClientDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG: L2Client::OnInitDialog( hDlg, lParam ); break;
	case WM_DESTROY: L2Client::OnDestroy( hDlg ); break;
	case WM_COMMAND: L2Client::OnCommand( hDlg, wParam, lParam ); break;
	case WM_NOTIFY: L2Client::OnNotify( hDlg, wParam, lParam ); break;
	case WM_PAINT: L2Client::OnPaint( hDlg, wParam, lParam ); break;
	case WMMY_UI_UPDATE: L2Client::OnMyUiUpdate( hDlg, wParam, lParam ); break;
	case WMMY_UI_MESSAGEBOXTIMEOUTREPLY: L2Client::OnMyMessageBoxTimeoutReply( hDlg, wParam, lParam ); break;
	case WMMY_UI_BUYSELLDLGREPLY: L2Client::OnMyBuySellDlgReply( hDlg, wParam, lParam ); break;
	case WMMY_UI_PRIVATESTOREDLGREPLY: L2Client::OnMyPrivateStoreManageDlgReply( hDlg, wParam, lParam ); break;
	default: return FALSE; break;
	}
	return TRUE;
}

void L2Client::OnInitDialog( HWND hDlg, LPARAM lParam )
{
	SetWindowLongPtr( hDlg, GWLP_USERDATA, (LONG_PTR)lParam );
	class L2Client *pcls = (class L2Client *)lParam;
	// child windows
	pcls->hWndMap              = GetDlgItem( hDlg, IDC_MAPAREA );
	pcls->hWndTabChat          = GetDlgItem( hDlg, IDC_TAB_CHAT );
	pcls->hWndTabInfo          = GetDlgItem( hDlg, IDC_TAB_INFO );
	pcls->hWndProgressHP       = GetDlgItem( hDlg, IDC_PR_HP );
	pcls->hWndProgressMP       = GetDlgItem( hDlg, IDC_PR_MP );
	pcls->hWndProgressCP       = GetDlgItem( hDlg, IDC_PR_CP );
	pcls->hWndProgressWeight   = GetDlgItem( hDlg, IDC_PR_WEIGHT );
	pcls->hWndProgressTargetHP = GetDlgItem( hDlg, IDC_PR_TARGETHP );
	pcls->hWndSliderMapScale   = GetDlgItem( hDlg, IDC_SLI_SCALE );
	pcls->hWndProgressCasting  = GetDlgItem( hDlg, IDC_PR_CASTING );
	pcls->hWndProgressExp      = GetDlgItem( hDlg, IDC_PR_EXP );
	// font (used only for chat list-boxes)
#ifndef CHAT_USE_RICHEDIT // создать шрифт для чата (только если используется ListBox а не RichEdit)
	HFONT hFontChat = CreateFont( -9, 0, 0, 0, FW_NORMAL, 0, 0, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY, FF_DONTCARE, TEXT("Tahoma") );
#endif
	//
	int i = 0;
	TCHAR text[256] = {0};
	TCITEM tci;
	RECT rc;
	HWND hw;
	//
	// init chats tab
	//
	TabCtrl_SetItemSize( pcls->hWndTabChat, 30, 20 );
	GetClientRect( pcls->hWndTabChat, &rc );
	for( i=0; i<NUM_CHAT_TABS; i++ )
	{
		tci.mask = TCIF_PARAM | TCIF_TEXT;
		tci.pszText = text;
		switch( i )
		{
		case CHAT_SYS: lstrcpy( text, TEXT("Sys") ); break;
		case CHAT_DMG: lstrcpy( text, TEXT("dmg") ); break;
		case CHAT_ALL: lstrcpy( text, TEXT("All") ); break;
		case CHAT_TRADE: lstrcpy( text, TEXT("+") ); break;
		case CHAT_SHOUT: lstrcpy( text, TEXT("!") ); break;
		case CHAT_PARTY: lstrcpy( text, TEXT("#") ); break;
		case CHAT_CLAN: lstrcpy( text, TEXT("@") ); break;
		case CHAT_ALLY: lstrcpy( text, TEXT("$") ); break;
		case CHAT_HERO: lstrcpy( text, TEXT("%") ); break;
		case CHAT_WHISPER: lstrcpy( text, TEXT("pm") ); break;
		}
		TabCtrl_InsertItem( pcls->hWndTabChat, i, &tci );
#ifdef CHAT_USE_RICHEDIT // создает RichEdit окно с чатом (вывод чата)
		pcls->hWndTabChat_item[i] = CreateWindowEx( WS_EX_CLIENTEDGE,
			RICHEDIT_CLASS, TEXT("LB_RichChat"),
			WS_CHILD | /*WS_BORDER |*/ WS_VSCROLL | WS_HSCROLL |
			// richedit styles
				ES_LEFT | ES_NOHIDESEL | ES_READONLY |
				//ES_AUTOHSCROLL | ES_AUTOVSCROLL | // ?
				ES_MULTILINE | ES_DISABLENOSCROLL,
			1, 24,
			rc.right - rc.left - 5, // width
			rc.bottom - rc.top - 30, // height
			pcls->hWndTabChat, NULL, g_hInst, NULL );
		SendMessage( pcls->hWndTabChat_item[i], WM_SETTEXT, 0, (LPARAM)TEXT("") );
		SendMessage( pcls->hWndTabChat_item[i], EM_SETTEXTMODE, TM_RICHTEXT, 0 );
		SendMessage( pcls->hWndTabChat_item[i], EM_SETBKGNDCOLOR, FALSE, RGB(0,0,0) );
		//SendMessage( pcls->hWndTabChat_item[i], EM_AUTOURLDETECT, TRUE, 0 );
		// setup default char format for control
		CHARFORMAT2 cf2;
		memset( &cf2, 0, sizeof(cf2) );
		cf2.cbSize = sizeof(cf2);
		cf2.dwMask = CFM_BACKCOLOR | CFM_COLOR | CFM_LINK | CFM_FACE | CFM_SIZE | CFM_WEIGHT;
		cf2.crBackColor = RGB(0,0,0);
		cf2.crTextColor = RGB(255,255,255);
		cf2.dwEffects = 0; //CFE_LINK | CFE_AUTOCOLOR | CFE_AUTOBACKCOLOR;
		_tcscpy( cf2.szFaceName, _T("Tahoma") );
		cf2.yHeight = 150;
		cf2.wWeight = FW_NORMAL;
		SendMessage( pcls->hWndTabChat_item[i], EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf2 );
		SendMessage( pcls->hWndTabChat_item[i], EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf2 );
#else
		pcls->hWndTabChat_item[i] = CreateWindowEx( WS_EX_CLIENTEDGE,
			TEXT("LISTBOX"), TEXT("LB_Chat"),
			WS_CHILD | /*WS_BORDER |*/ WS_VSCROLL | WS_HSCROLL,
			1, 24, rc.right - rc.left - 5, rc.bottom - rc.top - 26,
			pcls->hWndTabChat, NULL, g_hInst, NULL );
		SendMessage( pcls->hWndTabChat_item[i] , WM_SETFONT, (WPARAM)hFontChat, TRUE );
#endif // CHAT_USE_RICHEDIT
	}
	pcls->curTabChat = 0;
	ShowWindow( pcls->hWndTabChat_item[0], SW_SHOW );
	//
	// init info tabs
	//
	TabCtrl_SetItemSize( pcls->hWndTabInfo, 38, 20 );
	GetClientRect( pcls->hWndTabInfo, &rc );
	for( i=0; i<NUM_INFO_TABS; i++ )
	{
		hw = NULL;
		tci.mask = TCIF_PARAM | TCIF_TEXT;
		tci.pszText = text;
		switch( i )
		{
		case INFO_USER:
			{
				lstrcpy( text, TEXT("Инфо") );
				hw = UserInfoTabDlg_Create( pcls->hWndTabInfo );
			} break;
		case INFO_INV:
			{
				lstrcpy( text, TEXT("Инв.") );
				hw = UserInvTabDlg_Create( pcls->hWndTabInfo, pcls->getInterface() );
			} break;
		case INFO_SKILLS:
			{
				lstrcpy( text, TEXT("Скил.") );
				hw = UserSkillsTabDlg_Create( pcls->hWndTabInfo, pcls->getInterface() );
			} break;
		case INFO_BUFFS:
			{
				lstrcpy( text, TEXT("Баф") );
				hw = UserBuffTabDlg_Create( pcls->hWndTabInfo, pcls->getInterface() );
			} break;
		case INFO_PARTY:
			{
				lstrcpy( text, TEXT("Пати") );
				hw = UserPartyTabDlg_Create( pcls->hWndTabInfo, pcls->getInterface() );
			} break;
		case INFO_MAP:
			{
				lstrcpy( text, TEXT("Мир") );
				hw = MapItemsTabDlg_Create( pcls->hWndTabInfo, pcls->getInterface() );
			} break;
		case INFO_OTHER:
			{
				lstrcpy( text, TEXT("...") );
			} break;
		case INFO_STATS:
			{
				lstrcpy( text, TEXT("Stats") );
			} break;
		}
		TabCtrl_InsertItem( pcls->hWndTabInfo, i, &tci );
		pcls->hWndTabInfo_item[i] = hw;
		MoveWindow( hw, 1, 24, rc.right - rc.left - 5, rc.bottom - rc.top - 27, TRUE );
		//SendMessage( pcls->hWndTabChat_item[i] , WM_SETFONT, (WPARAM)hFontChat, TRUE );
	}
	pcls->curTabChat = 0;
	ShowWindow( pcls->hWndTabInfo_item[0], SW_SHOW );
	// setup map scale slider
	SendMessage( pcls->hWndSliderMapScale, TBM_SETRANGE, TRUE, (LPARAM)MAKELONG( 0, 100 ) );
	SendMessage( pcls->hWndSliderMapScale, TBM_SETPOS, TRUE, 50 );
	// init map checks
	SetDlgItemInt( hDlg, IDC_E_MAPSHOWDZ, pcls->mapShowParams.zrange, TRUE );
	CheckDlgButton( hDlg, IDC_C_MAPSHOWMOBS, pcls->mapShowParams.mobs );
	CheckDlgButton( hDlg, IDC_C_MAPSHOWNPCS, pcls->mapShowParams.npcs );
	CheckDlgButton( hDlg, IDC_C_MAPSHOWPCS, pcls->mapShowParams.pcs );
	CheckDlgButton( hDlg, IDC_C_MAPSHOWITEMS, pcls->mapShowParams.items );
	CheckDlgButton( hDlg, IDC_C_MAPSHOWVECTORS, pcls->mapShowParams.vectors );
	CheckDlgButton( hDlg, IDC_C_MAPSHOWZONE, pcls->mapShowParams.fightrange );
	CheckDlgButton( hDlg, IDC_C_MAPSHOWMAP, pcls->mapShowParams.realmap );
	CheckDlgButton( hDlg, IDC_C_MAPSHOWSELL, pcls->mapShowParams.sellshop );
	CheckDlgButton( hDlg, IDC_C_MAPSHOWBUY, pcls->mapShowParams.buyshop );
	CheckDlgButton( hDlg, IDC_C_MAPSHOWCRAFT, pcls->mapShowParams.craftshop );
	// setup progressbars limits
	SendMessage( pcls->hWndProgressHP,       PBM_SETRANGE, 0, MAKELPARAM(0,100) );
	SendMessage( pcls->hWndProgressMP,       PBM_SETRANGE, 0, MAKELPARAM(0,100) );
	SendMessage( pcls->hWndProgressCP,       PBM_SETRANGE, 0, MAKELPARAM(0,100) );
	SendMessage( pcls->hWndProgressWeight,   PBM_SETRANGE, 0, MAKELPARAM(0,100) );
	SendMessage( pcls->hWndProgressTargetHP, PBM_SETRANGE, 0, MAKELPARAM(0,100) );
	SendMessage( pcls->hWndProgressCasting,  PBM_SETRANGE, 0, MAKELPARAM(0,100) );
	ShowWindow( pcls->hWndProgressCasting, SW_HIDE );
	SendMessage( pcls->hWndProgressExp,      PBM_SETRANGE, 0, MAKELPARAM(0,100) );
	// subclass progress
	SubclassProgressHP( pcls->hWndProgressHP, &(pcls->usr) );
	SubclassProgressMP( pcls->hWndProgressMP, &(pcls->usr) );
	SubclassProgressCP( pcls->hWndProgressCP, &(pcls->usr) );
	SubclassProgressWeight( pcls->hWndProgressWeight, &(pcls->usr) );
	SubclassProgressTargetHP( pcls->hWndProgressTargetHP, &(pcls->usr) );
	// subclass map
	SubclassMapWindowProc( pcls->hWndMap, pcls->getInterface() );
	//
	// заменим процедуру обработки сообщений эдита чата на нашу
	//
	HWND hEditChat = GetDlgItem( hDlg, IDC_E_CHAT ); // get edit window handle
	if( OldEditProc == NULL )
	{
		OldEditProc = (WNDPROC)GetWindowLongPtr( hEditChat, GWLP_WNDPROC ); // запомним старую процедуру обработки сообщений
	}
	else
	{
		WNDPROC New_OldEditProc = (WNDPROC)GetWindowLongPtr( hEditChat, GWLP_WNDPROC );
		if( New_OldEditProc != OldEditProc )
			log_error( LOG_ERROR, "Edit window default wndProc changed!!!\n" );
	}
	SetWindowLongPtr( hEditChat, GWLP_WNDPROC, (LONG_PTR)NewEditProc ); // установим новую
}

void L2Client::OnDestroy( HWND hDlg )
{
	UNREFERENCED_PARAMETER(hDlg);
	log_error( LOG_DEBUG, "L2Client::OnDestroy\n" );
}

void L2Client::OnCommand( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam);
	class L2Client *pcls = (class L2Client *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	int updateCode = HIWORD(wParam);
	int ctrlID = LOWORD(wParam);
	switch( ctrlID )
	{
	case IDC_B_CLOSETAB: pcls->closeTab(); break;
	case IDC_B_SYSTEM: pcls->onButtonSystem(); break;
	case IDC_B_CONFIG: pcls->onButtonConfig(); break;
	case IDC_B_SCRIPT: pcls->onButtonScript(); break;
	case IDC_B_ACTIONTARGET: pcls->onButtonActionTarget(); break;
	case IDC_B_ATTACKTARGET: pcls->onButtonAttackTarget(); break;
	case IDC_B_CANCELTARGET: pcls->onButtonCancelTarget(); break;
	case IDC_B_SITSTAND: pcls->onButtonSitStand(); break;
	case IDC_B_RUNWALK: pcls->onButtonRunWalk(); break;
	case IDC_B_SAY: pcls->onButtonSay(); break;
#ifdef _DEBUG
	case IDC_B_DEBUG: pcls->onButtonDebug(); break;
#endif
	case IDC_B_ACTIONS: pcls->onButtonActions(); break;
		// map checks handlers
	case IDC_E_MAPSHOWDZ:
		{
			if( updateCode == EN_UPDATE )
			{
				//log_error( LOG_OK, "EN_UPDATE\n" );
				pcls->mapShowParams.zrange = GetDlgItemInt( hDlg, IDC_E_MAPSHOWDZ, NULL, TRUE );
				if( pcls->mapShowParams.zrange < 0 ) pcls->mapShowParams.zrange = 0;
			}
		} break;
	case IDC_C_MAPSHOWMOBS:    pcls->mapShowParams.mobs       = IsDlgButtonChecked( hDlg, IDC_C_MAPSHOWMOBS ); break;
	case IDC_C_MAPSHOWNPCS:    pcls->mapShowParams.npcs       = IsDlgButtonChecked( hDlg, IDC_C_MAPSHOWNPCS ); break;
	case IDC_C_MAPSHOWPCS:     pcls->mapShowParams.pcs        = IsDlgButtonChecked( hDlg, IDC_C_MAPSHOWPCS ); break;
	case IDC_C_MAPSHOWITEMS:   pcls->mapShowParams.items      = IsDlgButtonChecked( hDlg, IDC_C_MAPSHOWITEMS ); break;
	case IDC_C_MAPSHOWVECTORS: pcls->mapShowParams.vectors    = IsDlgButtonChecked( hDlg, IDC_C_MAPSHOWVECTORS ); break;
	case IDC_C_MAPSHOWZONE:    pcls->mapShowParams.fightrange = IsDlgButtonChecked( hDlg, IDC_C_MAPSHOWZONE ); break;
	case IDC_C_MAPSHOWMAP:     pcls->mapShowParams.realmap    = IsDlgButtonChecked( hDlg, IDC_C_MAPSHOWMAP ); break;
	case IDC_C_MAPSHOWSELL:    pcls->mapShowParams.sellshop   = IsDlgButtonChecked( hDlg, IDC_C_MAPSHOWSELL ); break;
	case IDC_C_MAPSHOWBUY:     pcls->mapShowParams.buyshop    = IsDlgButtonChecked( hDlg, IDC_C_MAPSHOWBUY ); break;
	case IDC_C_MAPSHOWCRAFT:   pcls->mapShowParams.craftshop  = IsDlgButtonChecked( hDlg, IDC_C_MAPSHOWCRAFT ); break;
		// botting enable/disable, combat enable/disable
	case IDC_C_BOTENABLE:
		{
			if( updateCode == BN_CLICKED )
			{
				int bEnable = IsDlgButtonChecked( hDlg, IDC_C_BOTENABLE );
				if( bEnable == BST_CHECKED ) pcls->bottingEnable( true );
				else pcls->bottingEnable( false );
			}
		} break;
	case IDC_C_BOTCOMBATENABLE:
		{
			if( updateCode == BN_CLICKED )
			{
				int bEnable = IsDlgButtonChecked( hDlg, IDC_C_BOTCOMBATENABLE );
				if( bEnable == BST_CHECKED ) pcls->bottingCombatEnable( true );
				else pcls->bottingCombatEnable( false );
			}
		} break;
	}
}

void L2Client::OnNotify( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(wParam);
	class L2Client *pcls = (class L2Client *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	LPNMHDR phdr = (LPNMHDR)lParam;

	if( phdr->hwndFrom == pcls->hWndTabChat )
	{
		int cur_sel = TabCtrl_GetCurSel( pcls->hWndTabChat );
		switch( phdr->code )
		{
		case TCN_SELCHANGING:
			{
				//log_error( LOG_DEBUG, "L2Client[%d]: chat tab selchanging\n", pcls->tab_id );
				ShowWindow( pcls->hWndTabChat_item[cur_sel], SW_HIDE );
			} break;
		case TCN_SELCHANGE:
			{
				//log_error( LOG_DEBUG, "L2Client[%d]: chat tab selchange\n", pcls->tab_id );
				ShowWindow( pcls->hWndTabChat_item[cur_sel], SW_SHOW );
				pcls->curTabChat = cur_sel;
			} break;
		}
	}

	if( phdr->hwndFrom == pcls->hWndTabInfo )
	{
		int cur_sel = TabCtrl_GetCurSel( pcls->hWndTabInfo );
		switch( phdr->code )
		{
		case TCN_SELCHANGING:
			{
				//log_error( LOG_DEBUG, "L2Client[%d]: chat tab selchanging\n", pcls->tab_id );
				ShowWindow( pcls->hWndTabInfo_item[cur_sel], SW_HIDE );
			} break;
		case TCN_SELCHANGE:
			{
				//log_error( LOG_DEBUG, "L2Client[%d]: chat tab selchange\n", pcls->tab_id );
				ShowWindow( pcls->hWndTabInfo_item[cur_sel], SW_SHOW );
				pcls->curTabInfo = cur_sel;
			} break;
		}
	}

	if( phdr->code == EN_LINK )
	{
		log_error_np( LOG_OK, "EN_LINK\n" );
	}
}

void L2Client::OnPaint( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	DefWindowProc( hDlg, WM_PAINT, wParam, lParam );
}

void L2Client::OnMyUiUpdate( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam);
	class L2Client *pcls = (class L2Client *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	L2Client::UIUPDATECODE updateCode = (L2Client::UIUPDATECODE)wParam;
	switch( updateCode )
	{
	case L2Client::UPDATE_USER: pcls->OnUiUpdateUser( hDlg ); break;
	case L2Client::UPDATE_INV: pcls->OnUiUpdateInv( hDlg ); break;
	case L2Client::UPDATE_SKILLS: pcls->OnUiUpdateSkills( hDlg ); break;
	case L2Client::UPDATE_BUFFS: pcls->OnUiUpdateBuffs( hDlg ); break;
	case L2Client::UPDATE_MAP_PLAYERS: pcls->OnUiUpdateMapTab( hDlg, 0 ); break;
	case L2Client::UPDATE_MAP_MOBS: pcls->OnUiUpdateMapTab( hDlg, 1 ); break;
	case L2Client::UPDATE_MAP_NPCS: pcls->OnUiUpdateMapTab( hDlg, 2 ); break;
	case L2Client::UPDATE_MAP_ITEMS: pcls->OnUiUpdateMapTab( hDlg, 3 ); break;
	case L2Client::UPDATE_PARTY: pcls->OnUiUpdateParty( hDlg ); break;
	case L2Client::UPDATE_USER_TARGET: pcls->OnUiUpdateTarget( hDlg ); break;
	case L2Client::UPDATE_USER_CASTING: pcls->OnUiUpdateCasting( hDlg ); break;
	case L2Client::UPDATE_SKILL_COOLTIME: pcls->OnUiUpdateSkillCoolTimes(); break;
	case L2Client::UPDATE_BUFF_DURATION: pcls->OnUiUpdateBuffsDuration(); break;
	case L2Client::UPDATE_PARTY_BUFFSDURATION: pcls->OnUiUpdatePartyBuffsDuration(); break;
	}
}

void L2Client::OnMyMessageBoxTimeoutReply( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	log_error( LOG_DEBUG, "L2Client::OnMyMessageBoxTimeoutReply wParam %u, lParam %d\n", wParam, lParam );
	class L2Client *pcls = (class L2Client *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	switch( (unsigned int)lParam )
	{
	case MessageBoxTimeout::TYPE_PARTY:
		{
			pcls->send_RequestAnswerJoinParty( (unsigned int)wParam );
		} break;
	case MessageBoxTimeout::TYPE_TRADEREQUEST:
		{
			pcls->send_AnswerTradeRequest( wParam == TRUE );
		} break;
	default:
		{
			log_error( LOG_ERROR, "L2Client::OnMyMessageBoxTimeoutReply(): unknown reply wParam %u, lParam %d\n",
				wParam, lParam );
		} break;
	}
}


// Message sent as:
// PostMessage( pcls->m_hWndNotify, pcls->m_uNotifyMesasage, (WPARAM)(pcls->m_type), (LPARAM)(pcls->m_list_result) );
void L2Client::OnMyBuySellDlgReply( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( hDlg );
	class L2Client *pcls = (class L2Client *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	int reply_type = (int)wParam;
	TradeItemsList *list = (TradeItemsList *)lParam;
	if( !list ) return;
	//
	switch( reply_type )
	{
	case BuySellDlg::TYPE_BUYLIST:
		{
			//log_error( LOG_DEBUG, "BuyList Dialog reply (list id %u)\n", list->listID );
			pcls->send_RequestBuyItem( list );
		} break;
	case BuySellDlg::TYPE_SELLLIST:
		{
			//log_error( LOG_DEBUG, "SellList Dialog reply (list id %u)\n", list->listID );
			pcls->send_RequestSellItem( list );
		} break;
	case BuySellDlg::TYPE_PRIVATESTOREBUYLIST:
		{
			//log_error( LOG_OK, "BuyList (pivate) Dialog reply (oid %u)\n", list->listID );
			/*int i;
			for( i=0; i<list->itemCount; i++ )
			{
				char itemName[256] = {0};
				L2Data_DB_GetItemNamePicByID( list->item[i].itemID, itemName, NULL );
				log_error( LOG_OK, "%2d: %s count %d\n", i, itemName, list->item[i].count );
			}*/
			pcls->send_RequestPrivateStoreSell( list );
		} break;
	case BuySellDlg::TYPE_PRIVATESTORESELLLIST:
		{
			//log_error( LOG_OK, "SellList (pivate) Dialog reply (oid %u)\n", list->listID );
			pcls->send_RequestPrivateStoreBuy( list );
		} break;
	}
	//
	delete list;
	list = NULL;
}

void L2Client::OnMyPrivateStoreManageDlgReply( HWND hDlg, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( hDlg );
	class L2Client *pcls = (class L2Client *)GetWindowLongPtr( hDlg, GWLP_USERDATA );
	int reply_type = (int)wParam;
	TradeItemsList *list = (TradeItemsList *)lParam;
	//log_error( LOG_OK, "L2Client::OnMyPrivateStoreManageDlgReply(): reply_type = %d, list = 0x%p\n",
	//	reply_type, list );
	if( !list )
	{
		// cancel trade
		switch( reply_type )
		{
		case PrivateStoreManageDlg::TYPE_BUY: pcls->send_RequestPrivateStoreQuitBuy(); break;
		case PrivateStoreManageDlg::TYPE_SELL: pcls->send_RequestPrivateStoreQuitSell(); break;
		case PrivateStoreManageDlg::TYPE_CRAFT: pcls->send_RequestRecipeShopManageQuit(); break;
		case PrivateStoreManageDlg::TYPE_COMMONCRAFT: pcls->send_RequestRecipeShopManageQuit(); break;
		}
		return;
	}
	//
	switch( reply_type )
	{
	case PrivateStoreManageDlg::TYPE_BUY: pcls->send_SetPrivateStoreListBuy( list ); break;
	case PrivateStoreManageDlg::TYPE_SELL: pcls->send_SetPrivateStoreListSell( list ); break;
	case PrivateStoreManageDlg::TYPE_CRAFT: pcls->send_RequestRecipeShopListSet( list ); break;
	case PrivateStoreManageDlg::TYPE_COMMONCRAFT: pcls->send_RequestRecipeShopListSet( list ); break;
	}
	delete list;
	list = NULL;
}

void L2Client::OnUiUpdateUser( HWND hDlg )
{
	//log_error( LOG_DEBUG, "L2Client::OnUiUpdateUser()\n" );
	TCHAR text[256] = {0};
	// user name, level, class
	wsprintf( text, TEXT("%s Lv %d %S"), usr.charName, usr.level, L2Data_getClass( usr.classID ) );
	SetDlgItemTextW( hDlg, IDC_ST_CHAR_NAME, text );
	// sit/stand, run/walk type
	if( usr.isSitting ) lstrcpy( text, TEXT("Sit, ") );
	else lstrcpy( text, TEXT("Stand, ") );
	if( usr.isRunning ) lstrcat( text, TEXT("Run") );
	else lstrcat( text, TEXT("Walk") );
	SetDlgItemTextW( hDlg, IDC_ST_WAITTYPE, text );
	// progress
	int ipercent;
	// HP
	ipercent = 0;
	if( usr.maxHp > 0 ) ipercent = (int)(100.0 * usr.curHp / usr.maxHp);
	SendMessage( hWndProgressHP, PBM_SETPOS, ipercent, 0 );
	// MP
	ipercent = 0;
	if( usr.maxMp > 0 ) ipercent = (int)(100.0 * usr.curMp / usr.maxMp);
	SendMessage( hWndProgressMP, PBM_SETPOS, ipercent, 0 );
	// CP
	ipercent = 0;
	if( usr.maxCp > 0 ) ipercent = (int)(100.0 * usr.curCp / usr.maxCp);
	SendMessage( hWndProgressCP, PBM_SETPOS, ipercent, 0 );
	// WEIGHT
	ipercent = 0;
	if( usr.maxLoad > 0 ) ipercent = (int)(100.0 * usr.curLoad / usr.maxLoad);
	SendMessage( hWndProgressWeight, PBM_SETPOS, ipercent, 0 );
	// exp
	double dpercent = L2Experience::getExpPercent( usr.experience );
	ipercent = (int)dpercent;
	SendMessage( hWndProgressExp, PBM_SETPOS, ipercent, 0 );
	swprintf( text, 255, TEXT("%0.2f%%"), dpercent );
	SetDlgItemText( hDlg, IDC_ST_EXP, text );
	// update UserInfo dialog
	PostMessage( hWndTabInfo_item[ INFO_USER ], WM_USER, 0, (LPARAM)&usr );
}

void L2Client::OnUiUpdateInv( HWND hDlg )
{
	UNREFERENCED_PARAMETER(hDlg);
	//log_error( LOG_DEBUG, "L2Client::OnUiUpdateInv()\n" );
	PostMessage( hWndTabInfo_item[ INFO_INV ], WM_USER, 0, (LPARAM)&inv );
}

void L2Client::OnUiUpdateSkills( HWND hDlg )
{
	UNREFERENCED_PARAMETER(hDlg);
	//log_error( LOG_DEBUG, "L2Client::OnUiUpdateSkills()\n" );
	PostMessage( hWndTabInfo_item[ INFO_SKILLS ], WM_USER, 0, (LPARAM)&skills );
}

void L2Client::OnUiUpdateBuffs( HWND hDlg )
{
	UNREFERENCED_PARAMETER(hDlg);
	//log_error( LOG_DEBUG, "L2Client::OnUiUpdateBuffs()\n" );
	PostMessage( hWndTabInfo_item[ INFO_BUFFS ], WM_USER, 0, (LPARAM)&buffs );
}

void L2Client::OnUiUpdateMapTab( HWND hDlg, int tabNo )
{
	UNREFERENCED_PARAMETER(hDlg);
	//log_error( LOG_DEBUG, "L2Client::OnUiUpdateMapTab()\n" );
	LPARAM lParam = (LPARAM)(this->getInterface());
	PostMessage( hWndTabInfo_item[ INFO_MAP ], WM_USER, (WPARAM)tabNo, lParam );
}

void L2Client::OnUiUpdateParty( HWND hDlg )
{
	UNREFERENCED_PARAMETER(hDlg);
	//log_error( LOG_DEBUG, "L2Client::OnUiUpdateParty()\n" );
	PostMessage( hWndTabInfo_item[ INFO_PARTY ], WM_USER, 0, (LPARAM)&party );
}

void L2Client::OnUiUpdateTarget( HWND hDlg )
{
	log_error( LOG_DEBUG, "L2Client::OnUiUpdateTarget()\n" );
	if( usr.targetObjectID != 0 )
	{
		if( usr.targetObjectID == usr.objectID )
		{
			// self on target
			usr.targetCurHp = usr.curHp;
			usr.targetMaxHp = usr.maxHp;
			SetDlgItemTextW( hDlg, IDC_ST_TARGETNAME, usr.charName );
		}
		else // someone from L2 world on target: pc, npc or mob. find it
		{
			WorldObjectTreeNode wotNode;
			if( world_tree.GetInfoByObjectID( usr.targetObjectID, &wotNode ) )
			{
				TCHAR *text = NULL;
				int idx = wotNode.getArrayIdx();
				switch( wotNode.getObjectType() )
				{
				case L2OT_NPC: text = world_npcs.npcs_array[ idx ]->charName; break;
				case L2OT_PC:  text = world_chars.chars_array[ idx ]->charName; break;
				}
				if( text ) SetDlgItemTextW( hDlg, IDC_ST_TARGETNAME, text );
				else SetDlgItemTextW( hDlg, IDC_ST_TARGETNAME, L"<?>" );
			}
		}
	}
	else SetDlgItemText( hDlg, IDC_ST_TARGETNAME, TEXT("-") );
	// progress
	int ipercent;
	// target HP
	ipercent = 0;
	if( usr.targetMaxHp > 0 ) ipercent = (int)(100.0 * usr.targetCurHp / usr.targetMaxHp);
	//SendMessage( hWndProgressTargetHP, PBM_SETPOS, ipercent, 0 );
	PostMessage( hWndProgressTargetHP, PBM_SETPOS, ipercent, 0 );
}

void L2Client::OnUiUpdateCasting( HWND hDlg )
{
	UNREFERENCED_PARAMETER(hDlg);
	if( skills.isCasting() )
	{
		int ipercent = 0;
		if( skills.castMsecStart > 0 )
			ipercent = (int)( 100.0 * (double)skills.castMsecLeft / (double)skills.castMsecStart );
		ShowWindow( hWndProgressCasting, SW_SHOW );
		PostMessage( hWndProgressCasting, PBM_SETPOS, ipercent, 0 );
	}
	else ShowWindow( hWndProgressCasting, SW_HIDE );
}

void L2Client::OnUiUpdateSkillCoolTimes()
{
	PostMessage( hWndTabInfo_item[ INFO_SKILLS ], (WM_USER+1), 0, (LPARAM)&skills );
}

void L2Client::OnUiUpdateBuffsDuration()
{
	PostMessage( hWndTabInfo_item[ INFO_BUFFS ], (WM_USER+1), 0, (LPARAM)&buffs );
}

void L2Client::OnUiUpdatePartyBuffsDuration()
{
	PostMessage( hWndTabInfo_item[ INFO_PARTY ], (WM_USER+1), 0, (LPARAM)&party );
}




void L2Client::bottingEnable( bool bEnable )
{
	if( bEnable ) // start script
	{
		if( this->sys_scripter.isScriptThreadRunning() )
		{
			MessageBox( this->hWnd, TEXT("Sys AI script is already running!"),
				TEXT("Cannot start sys script"), MB_ICONWARNING );
			return;
		}
		this->sys_scripter.startScriptThread( sys_main_ai_script_filename );
		bot_enabled = true;
		return;
	}
	// else stop
	if( !this->sys_scripter.isScriptThreadRunning() )
	{
		MessageBox( this->hWnd, TEXT("Sys AI script is not running!"),
			TEXT("Cannot stop sys script"), MB_ICONWARNING );
		return;
	}
	if( !this->sys_scripter.stopScriptThread( 5000, false ) )
	{
		log_error( LOG_DEBUG, "L2Client::bottingEnable(): sys script stop failed!\n" );
		int yes = MessageBox( hWnd,
			TEXT("Скрипт не отвечает на запрос остановки.\nВы хотите прибить его силой?"),
			TEXT("Остановка: Скрипт не отвечает"), MB_ICONWARNING | MB_YESNO );
		if( yes == IDYES )
		{
			this->sys_scripter.stopScriptThread( 1000, true );
			bot_enabled = false;
			log_error( LOG_DEBUG, "L2Client::bottingEnable(): sys script FORCED terminated\n" );
		}
	}
	else // stopped ok
	{
		bot_enabled = false;
		log_error( LOG_DEBUG, "L2Client::bottingEnable(): sys script stopped OK\n" );
	}
}

void L2Client::bottingCombatEnable( bool bEnable )
{
	bot_combat_enabled = bEnable;
}

L2_VERSION L2Client::getL2Version() const
{
	return this->account.getL2Version();
}
