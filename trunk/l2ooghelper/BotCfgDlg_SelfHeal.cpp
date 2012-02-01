#include "stdafx.h"
#include "Resource.h"
#include "windowUtils.h"
#include "logger.h"
#include "L2Client.h"
#include "IL2Client.h"
#include "BotCfgDlg.h"
#include "Dlg_FindInDB.h"
#include "DBLayer.h"

extern HINSTANCE g_hInst;

INT_PTR CALLBACK BotCfg_SelfHeal_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

HWND BotCfgDlg_SelfHeal_Create( HWND hWndParent, BotConfig *pcfg )
{
	return CreateDialogParam( g_hInst, MAKEINTRESOURCE(IDD_BOTCFG_SELFHEAL),
		hWndParent, BotCfg_SelfHeal_DlgProc, (LPARAM)pcfg );
}

void BotCfg_SelfHeal_initLV( HWND hDlg );
void BotCfg_SelfHeal_init( HWND hDlg, BotConfig *cfg );
void BotCfg_SelfHeal_apply( HWND hDlg, BotConfig *cfg );

void BotCfg_SelfHeal_fill_LV( HWND hDlg, BotConfig *cfg );
void BotCfg_SelfHeal_apply_LV( HWND hDlg, BotConfig *cfg );

void BotCfg_SelfHeal_OnAddHealCond( HWND hDlg, BotConfig *cfg );
void BotCfg_SelfHeal_OnEditHealCond( HWND hDlg, BotConfig *cfg );
void BotCfg_SelfHeal_OnDelHealCond( HWND hDlg, BotConfig *cfg );

INT_PTR CALLBACK BotCfg_SelfHeal_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			BotConfig *cfg = (BotConfig *)lParam;
			SetWindowLongPtrW( hDlg, GWLP_USERDATA, (LONG_PTR)cfg );
			BotCfg_SelfHeal_initLV( hDlg );
			BotCfg_SelfHeal_init( hDlg, cfg );
		} break;
	case WM_COMMAND:
		{
			BotConfig *cfg = (BotConfig *)GetWindowLongPtrW( hDlg, GWLP_USERDATA );
			int ctrlID = LOWORD(wParam);
			switch( ctrlID )
			{
			case IDC_APPLY: BotCfg_SelfHeal_apply( hDlg, cfg ); break;
			case IDC_LOAD: BotCfg_SelfHeal_init( hDlg, cfg ); break;
			case IDC_B_ADD: BotCfg_SelfHeal_OnAddHealCond( hDlg, cfg ); break;
			case IDC_B_EDIT: BotCfg_SelfHeal_OnEditHealCond( hDlg, cfg ); break;
			case IDC_B_DEL: BotCfg_SelfHeal_OnDelHealCond( hDlg, cfg ); break;
			case IDC_COMBO2: BotCfg_SelfHeal_fill_LV( hDlg, cfg ); break;
			default: return FALSE; break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

void BotCfg_SelfHeal_init( HWND hDlg, BotConfig *cfg )
{
	int sit_hp = cfg->getValInt( "self_heal_sit_hp" );
	int sit_mp = cfg->getValInt( "self_heal_sit_mp" );
	int stand_hp = cfg->getValInt( "self_heal_stand_hp" );
	int stand_mp = cfg->getValInt( "self_heal_stand_mp" );
	int sit_hp_enable = cfg->getValInt( "self_heal_sit_hp_enable" );
	int sit_mp_enable = cfg->getValInt( "self_heal_sit_mp_enable" );
	int stand_hp_enable = cfg->getValInt( "self_heal_stand_hp_enable" );
	int stand_mp_enable = cfg->getValInt( "self_heal_stand_mp_enable" );
	//
	SetDlgItemInt( hDlg, IDC_E_SITHP, sit_hp, TRUE );
	SetDlgItemInt( hDlg, IDC_E_SITMP, sit_mp, TRUE );
	SetDlgItemInt( hDlg, IDC_E_STANDHP, stand_hp, TRUE );
	SetDlgItemInt( hDlg, IDC_E_STANDMP, stand_mp, TRUE );
	CheckDlgButton( hDlg, IDC_C_SITHP, sit_hp_enable );
	CheckDlgButton( hDlg, IDC_C_SITMP, sit_mp_enable );
	CheckDlgButton( hDlg, IDC_C_STANDHP, stand_hp_enable );
	CheckDlgButton( hDlg, IDC_C_STANDMP, stand_mp_enable );
	//
	BotCfg_SelfHeal_fill_LV( hDlg, cfg );
}

void BotCfg_SelfHeal_apply( HWND hDlg, BotConfig *cfg )
{
	//log_error( LOG_OK, "BotCfg_SelfHeal_apply\n" );
	int sit_hp = GetDlgItemInt( hDlg, IDC_E_SITHP, NULL, TRUE );
	int sit_mp = GetDlgItemInt( hDlg, IDC_E_SITMP, NULL, TRUE );
	int stand_hp = GetDlgItemInt( hDlg, IDC_E_STANDHP, NULL, TRUE );
	int stand_mp = GetDlgItemInt( hDlg, IDC_E_STANDMP, NULL, TRUE );
	int sit_hp_enable = IsDlgButtonChecked( hDlg, IDC_C_SITHP );
	int sit_mp_enable = IsDlgButtonChecked( hDlg, IDC_C_SITMP );
	int stand_hp_enable = IsDlgButtonChecked( hDlg, IDC_C_STANDHP );
	int stand_mp_enable = IsDlgButtonChecked( hDlg, IDC_C_STANDMP );
	//
	cfg->setValInt( "self_heal_sit_hp", sit_hp );
	cfg->setValInt( "self_heal_sit_mp", sit_mp );
	cfg->setValInt( "self_heal_stand_hp", stand_hp );
	cfg->setValInt( "self_heal_stand_mp", stand_mp );
	cfg->setValInt( "self_heal_sit_hp_enable", sit_hp_enable );
	cfg->setValInt( "self_heal_sit_mp_enable", sit_mp_enable );
	cfg->setValInt( "self_heal_stand_hp_enable", stand_hp_enable );
	cfg->setValInt( "self_heal_stand_mp_enable", stand_mp_enable );
	//
	BotCfg_SelfHeal_apply_LV( hDlg, cfg );
}

void BotCfg_SelfHeal_initLV( HWND hDlg )
{
	HWND hwndCB = GetDlgItem( hDlg, IDC_COMBO2 );
	ComboBox_ResetContent( hwndCB );
	ComboBox_AddString( hwndCB, TEXT("Отжор банками") );
	ComboBox_AddString( hwndCB, TEXT("СамоХил скиллами") );
	ComboBox_SetCurSel( hwndCB, 0 );
	HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
	WULV_InsertColumn( hwndLV, TEXT("Действие"), 100, 0 );
	WULV_InsertColumn( hwndLV, TEXT("HP<>"), 50, 1 );
	WULV_InsertColumn( hwndLV, TEXT("HP%"), 40, 2 );
	WULV_InsertColumn( hwndLV, TEXT("MP<>"), 50, 3 );
	WULV_InsertColumn( hwndLV, TEXT("MP%"), 40, 4 );
	WULV_InsertColumn( hwndLV, TEXT("CP<>"), 50, 5 );
	WULV_InsertColumn( hwndLV, TEXT("CP%"), 40, 6 );
	WULV_InsertColumn( hwndLV, TEXT("delay,ms"), 60, 7 );
}

void BotCfg_SelfHeal_fill_LV( HWND hDlg, BotConfig *cfg )
{
	HWND hwndCB = GetDlgItem( hDlg, IDC_COMBO2 );
	HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
	int cur_sel = ComboBox_GetCurSel( hwndCB );
	ListView_DeleteAllItems( hwndLV );
	int i;
	char valname[256];
	char valname_base[256];
	int num_cond = 0;
	if( cur_sel == 0 ) // items
	{
		strcpy( valname_base, "self_heal_use_item" );
		num_cond = cfg->getValInt( "self_heal_use_item_nConds" );
	}
	else if( cur_sel == 1 ) // skills
	{
		strcpy( valname_base, "self_heal_use_skill" );
		num_cond = cfg->getValInt( "self_heal_use_skill_nConds" );
	}
	else return;
	for( i=0; i<num_cond; i++ )
	{
		sprintf( valname, "%s_[%d]_id", valname_base, i );
		int id = cfg->getValInt( valname );
		sprintf( valname, "%s_[%d]_hp_cond", valname_base, i );
		const char *hpcond = cfg->getValStr( valname );
		sprintf( valname, "%s_[%d]_mp_cond", valname_base, i );
		const char *mpcond = cfg->getValStr( valname );
		sprintf( valname, "%s_[%d]_cp_cond", valname_base, i );
		const char *cpcond = cfg->getValStr( valname );
		sprintf( valname, "%s_[%d]_hp", valname_base, i );
		int hpp = cfg->getValInt( valname );
		sprintf( valname, "%s_[%d]_mp", valname_base, i );
		int mpp = cfg->getValInt( valname );
		sprintf( valname, "%s_[%d]_cp", valname_base, i );
		int cpp = cfg->getValInt( valname );
		sprintf( valname, "%s_[%d]_delayms", valname_base, i );
		int delayms = cfg->getValInt( valname );
		// item/skill name
		TCHAR text[256] = {0};
		if( cur_sel == 0 ) DBLayer_getItemName( id, text, 255 );
		if( cur_sel == 1 ) DBLayer_getSkillName( id, text, 255 );
		int inserted_iid = WULV_InsertItem( hwndLV, text, 0, (LPARAM)id );
		// hp cond
		_tcscpy( text, _T("=") );
		if( strcmp( hpcond, "lt" ) == 0 ) _tcscpy( text, _T("<") );
		else if( strcmp( hpcond, "le" ) == 0 ) _tcscpy( text, _T("<=") );
		else if( strcmp( hpcond, "gt" ) == 0 ) _tcscpy( text, _T(">") );
		else if( strcmp( hpcond, "ge" ) == 0 ) _tcscpy( text, _T(">=") );
		WULV_SetItem( hwndLV, text, inserted_iid, 1 );
		// hp
		wsprintf( text, TEXT("%d"), hpp );
		WULV_SetItem( hwndLV, text, inserted_iid, 2 );
		// mp cond
		_tcscpy( text, _T("=") );
		if( strcmp( mpcond, "lt" ) == 0 ) _tcscpy( text, _T("<") );
		else if( strcmp( mpcond, "le" ) == 0 ) _tcscpy( text, _T("<=") );
		else if( strcmp( mpcond, "gt" ) == 0 ) _tcscpy( text, _T(">") );
		else if( strcmp( mpcond, "ge" ) == 0 ) _tcscpy( text, _T(">=") );
		WULV_SetItem( hwndLV, text, inserted_iid, 3 );
		// mp
		wsprintf( text, TEXT("%d"), mpp );
		WULV_SetItem( hwndLV, text, inserted_iid, 4 );
		// cp cond
		_tcscpy( text, _T("=") );
		if( strcmp( cpcond, "lt" ) == 0 ) _tcscpy( text, _T("<") );
		else if( strcmp( cpcond, "le" ) == 0 ) _tcscpy( text, _T("<=") );
		else if( strcmp( cpcond, "gt" ) == 0 ) _tcscpy( text, _T(">") );
		else if( strcmp( cpcond, "ge" ) == 0 ) _tcscpy( text, _T(">=") );
		WULV_SetItem( hwndLV, text, inserted_iid, 5 );
		// cp
		wsprintf( text, TEXT("%d"), cpp );
		WULV_SetItem( hwndLV, text, inserted_iid, 6 );
		// delay, ms
		wsprintf( text, TEXT("%d"), delayms );
		WULV_SetItem( hwndLV, text, inserted_iid, 7 );
		//
	}
}

void BotCfg_SelfHeal_apply_LV( HWND hDlg, BotConfig *cfg )
{
	HWND hwndCB = GetDlgItem( hDlg, IDC_COMBO2 );
	HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
	int cur_sel_cb = ComboBox_GetCurSel( hwndCB );
	int i;
	char valname[256];
	char valname_base[256];
	int num_cond = ListView_GetItemCount( hwndLV );
	if( cur_sel_cb == 0 ) // items
	{
		strcpy( valname_base, "self_heal_use_item" );
		cfg->setValInt( "self_heal_use_item_nConds", num_cond );
	}
	else if( cur_sel_cb == 1 ) // skills
	{
		strcpy( valname_base, "self_heal_use_skill" );
		cfg->setValInt( "self_heal_use_skill_nConds", num_cond );
	}
	else return;
	int iItem = -1;
	i = 0;
	while( (iItem = ListView_GetNextItem( hwndLV, iItem, LVNI_ALL )) != -1 )
	{
		TCHAR text[256] = {0};
		int ival = 0;
		char strval[32] = {0};
		// id
		WULV_GetItemText( hwndLV, iItem, 0, text, 255 );
		int id = (int)WULV_GetItemLPARAM( hwndLV, iItem );
		//log_error( LOG_OK, "BotCfg_SelfHeal_apply_LV[%d] %S id %u\n", iItem, text, id );
		sprintf( valname, "%s_[%d]_id", valname_base, i );
		cfg->setValInt( valname, id );
		// hp_cond
		WULV_GetItemText( hwndLV, iItem, 1, text, 255 );
		strcpy( strval, "eq" );
		if( _tcscmp( text, _T("<") ) == 0 ) strcpy( strval, "lt" );
		else if( _tcscmp( text, _T("<=") ) == 0 ) strcpy( strval, "le" );
		else if( _tcscmp( text, _T(">") ) == 0 ) strcpy( strval, "gt" );
		else if( _tcscmp( text, _T(">=") ) == 0 ) strcpy( strval, "ge" );
		sprintf( valname, "%s_[%d]_hp_cond", valname_base, i );
		cfg->setValStr( valname, strval );
		// hp
		WULV_GetItemText( hwndLV, iItem, 2, text, 255 );
		_stscanf( text, _T("%d"), &ival );
		sprintf( valname, "%s_[%d]_hp", valname_base, i );
		cfg->setValInt( valname, ival );
		// mp_cond
		WULV_GetItemText( hwndLV, iItem, 3, text, 255 );
		strcpy( strval, "eq" );
		if( _tcscmp( text, _T("<") ) == 0 ) strcpy( strval, "lt" );
		else if( _tcscmp( text, _T("<=") ) == 0 ) strcpy( strval, "le" );
		else if( _tcscmp( text, _T(">") ) == 0 ) strcpy( strval, "gt" );
		else if( _tcscmp( text, _T(">=") ) == 0 ) strcpy( strval, "ge" );
		sprintf( valname, "%s_[%d]_mp_cond", valname_base, i );
		cfg->setValStr( valname, strval );
		// mp
		WULV_GetItemText( hwndLV, iItem, 4, text, 255 );
		_stscanf( text, _T("%d"), &ival );
		sprintf( valname, "%s_[%d]_mp", valname_base, i );
		cfg->setValInt( valname, ival );
		// cp_cond
		WULV_GetItemText( hwndLV, iItem, 5, text, 255 );
		strcpy( strval, "eq" );
		if( _tcscmp( text, _T("<") ) == 0 ) strcpy( strval, "lt" );
		else if( _tcscmp( text, _T("<=") ) == 0 ) strcpy( strval, "le" );
		else if( _tcscmp( text, _T(">") ) == 0 ) strcpy( strval, "gt" );
		else if( _tcscmp( text, _T(">=") ) == 0 ) strcpy( strval, "ge" );
		sprintf( valname, "%s_[%d]_cp_cond", valname_base, i );
		cfg->setValStr( valname, strval );
		// cp
		WULV_GetItemText( hwndLV, iItem, 6, text, 255 );
		_stscanf( text, _T("%d"), &ival );
		sprintf( valname, "%s_[%d]_cp", valname_base, i );
		cfg->setValInt( valname, ival );
		// delay, ms
		WULV_GetItemText( hwndLV, iItem, 7, text, 255 );
		_stscanf( text, _T("%d"), &ival );
		sprintf( valname, "%s_[%d]_delayms", valname_base, i );
		cfg->setValInt( valname, ival );
		//
		i++;
	}
}

class self_heal_cond
{
public:
	bool used;
	int type; // 0 - item; 1 - skill
	int id; // skill or item ID
	int hp;
	int hp_cond; // < <= = > >= ||| in cfg: 'lt' 'le' 'eq' 'gt' 'ge'
	int mp;
	int mp_cond;
	int cp;
	int cp_cond;
	int delayms;
};

// < <= = > >= ||| in cfg: 'lt' 'le' 'eq' 'gt' 'ge'
void getCondTextByNum( int n, char *outHR, char *outASM )
{
	if( outHR ) outHR[0] = 0;
	if( outASM ) outASM[0] = 0;
	switch( n )
	{
	case 0:
		{
			if( outHR ) strcpy( outHR, "<" );
			if( outASM ) strcpy( outASM, "lt" );
		} break;
	case 1:
		{
			if( outHR ) strcpy( outHR, "<=" );
			if( outASM ) strcpy( outASM, "le" );
		} break;
	case 2:
		{
			if( outHR ) strcpy( outHR, "=" );
			if( outASM ) strcpy( outASM, "eq" );
		} break;
	case 3:
		{
			if( outHR ) strcpy( outHR, ">" );
			if( outASM ) strcpy( outASM, "gt" );
		} break;
	case 4:
		{
			if( outHR ) strcpy( outHR, ">=" );
			if( outASM ) strcpy( outASM, "ge" );
		} break;
	}
}

bool BotCfg_SelfHeal_CondDlgRun( HWND hWndParent, self_heal_cond *st );

void BotCfg_SelfHeal_OnAddHealCond( HWND hDlg, BotConfig *cfg )
{
	HWND hwndCB = GetDlgItem( hDlg, IDC_COMBO2 );
	HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
	int cur_sel = ComboBox_GetCurSel( hwndCB );
	self_heal_cond st;
	st.type = cur_sel;
	st.used = false;
	st.delayms = 0;
	st.id = 0;
	st.hp = st.mp = st.cp = 0;
	st.hp_cond = st.mp_cond = st.cp_cond = 0;
	if( BotCfg_SelfHeal_CondDlgRun( hDlg, &st ) )
	{
		char name[256] = {0};
		wchar_t namew[256] = {0};
		switch( st.type )
		{
		case 0: DBLayer_getItemName( st.id, namew, 255 ); break;
		case 1: DBLayer_getSkillName( st.id, namew, 255 ); break;
		}
		// name
		int added_idx = WULV_InsertItem( hwndLV, namew, 999, st.id );
		// hp cond
		getCondTextByNum( st.hp_cond, name, NULL );
		MultiByteToWideChar( CP_ACP, 0, name, -1, namew, 255 );
		WULV_SetItem( hwndLV, namew, added_idx, 1 );
		wsprintfW( namew, L"%d", st.hp );
		WULV_SetItem( hwndLV, namew, added_idx, 2 );
		// mp cond
		getCondTextByNum( st.mp_cond, name, NULL );
		MultiByteToWideChar( CP_ACP, 0, name, -1, namew, 255 );
		WULV_SetItem( hwndLV, namew, added_idx, 3 );
		wsprintfW( namew, L"%d", st.mp );
		WULV_SetItem( hwndLV, namew, added_idx, 4 );
		// cp cond
		getCondTextByNum( st.cp_cond, name, NULL );
		MultiByteToWideChar( CP_ACP, 0, name, -1, namew, 255 );
		WULV_SetItem( hwndLV, namew, added_idx, 5 );
		wsprintfW( namew, L"%d", st.cp );
		WULV_SetItem( hwndLV, namew, added_idx, 6 );
		// delay ms
		wsprintfW( namew, L"%d", st.delayms );
		WULV_SetItem( hwndLV, namew, added_idx, 7 );
	}
	BotCfg_SelfHeal_apply_LV( hDlg, cfg );
	//BotCfg_SelfHeal_fill_LV( hDlg, cfg ); // update LV
}

void BotCfg_SelfHeal_OnEditHealCond( HWND hDlg, BotConfig *cfg )
{
	HWND hwndCB = GetDlgItem( hDlg, IDC_COMBO2 );
	HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
	int cur_sel_cb = ComboBox_GetCurSel( hwndCB );
	int cur_sel_lv = ListView_GetNextItem( hwndLV, -1, LVNI_SELECTED );
	if( cur_sel_lv < 0 ) return;
	self_heal_cond st;
	st.type = cur_sel_cb;
	// fill struct with existing info
	TCHAR text[256];
	// id
	st.id = (int)WULV_GetItemLPARAM( hwndLV, cur_sel_lv );
	// hp_cond
	WULV_GetItemText( hwndLV, cur_sel_lv, 1, text, 255 );
	st.hp_cond = 2; // eq
	if( _tcscmp( text, _T("<") ) == 0 ) st.hp_cond = 0;
	else if( _tcscmp( text, _T("<=") ) == 0 ) st.hp_cond = 1;
	else if( _tcscmp( text, _T(">") ) == 0 ) st.hp_cond = 3;
	else if( _tcscmp( text, _T(">=") ) == 0 ) st.hp_cond = 4;
	// hp
	WULV_GetItemText( hwndLV, cur_sel_lv, 2, text, 255 );
	_stscanf( text, _T("%d"), &(st.hp) );
	// mp_cond
	WULV_GetItemText( hwndLV, cur_sel_lv, 3, text, 255 );
	st.mp_cond = 2; // eq
	if( _tcscmp( text, _T("<") ) == 0 ) st.mp_cond = 0;
	else if( _tcscmp( text, _T("<=") ) == 0 ) st.mp_cond = 1;
	else if( _tcscmp( text, _T(">") ) == 0 ) st.mp_cond = 3;
	else if( _tcscmp( text, _T(">=") ) == 0 ) st.mp_cond = 4;
	// mp
	WULV_GetItemText( hwndLV, cur_sel_lv, 4, text, 255 );
	_stscanf( text, _T("%d"), &(st.mp) );
	// cp_cond
	WULV_GetItemText( hwndLV, cur_sel_lv, 5, text, 255 );
	st.cp_cond = 2; // eq
	if( _tcscmp( text, _T("<") ) == 0 ) st.cp_cond = 0;
	else if( _tcscmp( text, _T("<=") ) == 0 ) st.cp_cond = 1;
	else if( _tcscmp( text, _T(">") ) == 0 ) st.cp_cond = 3;
	else if( _tcscmp( text, _T(">=") ) == 0 ) st.cp_cond = 4;
	// cp
	WULV_GetItemText( hwndLV, cur_sel_lv, 6, text, 255 );
	_stscanf( text, _T("%d"), &(st.cp) );
	// delayms
	WULV_GetItemText( hwndLV, cur_sel_lv, 7, text, 255 );
	_stscanf( text, _T("%d"), &(st.delayms) );
	// run dialog
	if( BotCfg_SelfHeal_CondDlgRun( hDlg, &st ) )
	{
		// get name from DB
		char name[256] = {0};
		wchar_t namew[256] = {0};
		switch( st.type )
		{
		case 0: DBLayer_getItemName( st.id, namew, 255 ); break;
		case 1: DBLayer_getSkillName( st.id, namew, 255 ); break;
		}
		// del existing item, then insert new with new values :)
		ListView_DeleteItem( hwndLV, cur_sel_lv );
		// name
		int added_idx = WULV_InsertItem( hwndLV, namew, 999, st.id );
		// hp cond
		getCondTextByNum( st.hp_cond, name, NULL );
		MultiByteToWideChar( CP_ACP, 0, name, -1, namew, 255 );
		WULV_SetItem( hwndLV, namew, added_idx, 1 );
		wsprintfW( namew, L"%d", st.hp );
		WULV_SetItem( hwndLV, namew, added_idx, 2 );
		// mp cond
		getCondTextByNum( st.mp_cond, name, NULL );
		MultiByteToWideChar( CP_ACP, 0, name, -1, namew, 255 );
		WULV_SetItem( hwndLV, namew, added_idx, 3 );
		wsprintfW( namew, L"%d", st.mp );
		WULV_SetItem( hwndLV, namew, added_idx, 4 );
		// cp cond
		getCondTextByNum( st.cp_cond, name, NULL );
		MultiByteToWideChar( CP_ACP, 0, name, -1, namew, 255 );
		WULV_SetItem( hwndLV, namew, added_idx, 5 );
		wsprintfW( namew, L"%d", st.cp );
		WULV_SetItem( hwndLV, namew, added_idx, 6 );
		// delay ms
		wsprintfW( namew, L"%d", st.delayms );
		WULV_SetItem( hwndLV, namew, added_idx, 7 );
	}
	//
	BotCfg_SelfHeal_apply_LV( hDlg, cfg );
	//BotCfg_SelfHeal_fill_LV( hDlg, cfg ); // update LV
}

void BotCfg_SelfHeal_OnDelHealCond( HWND hDlg, BotConfig *cfg )
{
	HWND hwndLV = GetDlgItem( hDlg, IDC_LIST1 );
	int cur_sel_lv = ListView_GetNextItem( hwndLV, -1, LVNI_SELECTED );
	ListView_DeleteItem( hwndLV, cur_sel_lv );
	BotCfg_SelfHeal_apply_LV( hDlg, cfg );
	BotCfg_SelfHeal_fill_LV( hDlg, cfg ); // update LV
}

INT_PTR CALLBACK BotCfg_SelfHeal_CondDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

bool BotCfg_SelfHeal_CondDlgRun( HWND hWndParent, self_heal_cond *st )
{
	INT_PTR ret = DialogBoxParam( g_hInst, MAKEINTRESOURCE(IDD_BOTCFG_SELFHEAL_COND), hWndParent,
		BotCfg_SelfHeal_CondDlgProc, (LPARAM)st );
	if( ret == IDOK ) return true;
	return false;
}

INT_PTR CALLBACK BotCfg_SelfHeal_CondDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			self_heal_cond *st = (self_heal_cond *)lParam;
			SetWindowLongPtrW( hDlg, GWLP_USERDATA, (LONG_PTR)st );
			// comboboxes
			HWND hwndCB = GetDlgItem( hDlg, IDC_CB_HP );
			ComboBox_ResetContent( hwndCB );
			ComboBox_AddString( hwndCB, TEXT("<") );
			ComboBox_AddString( hwndCB, TEXT("<=") );
			ComboBox_AddString( hwndCB, TEXT("=") );
			ComboBox_AddString( hwndCB, TEXT(">") );
			ComboBox_AddString( hwndCB, TEXT(">=") );
			ComboBox_SetCurSel( hwndCB, st->hp_cond );
			hwndCB = GetDlgItem( hDlg, IDC_CB_MP );
			ComboBox_ResetContent( hwndCB );
			ComboBox_AddString( hwndCB, TEXT("<") );
			ComboBox_AddString( hwndCB, TEXT("<=") );
			ComboBox_AddString( hwndCB, TEXT("=") );
			ComboBox_AddString( hwndCB, TEXT(">") );
			ComboBox_AddString( hwndCB, TEXT(">=") );
			ComboBox_SetCurSel( hwndCB, st->mp_cond );
			hwndCB = GetDlgItem( hDlg, IDC_CB_CP );
			ComboBox_ResetContent( hwndCB );
			ComboBox_AddString( hwndCB, TEXT("<") );
			ComboBox_AddString( hwndCB, TEXT("<=") );
			ComboBox_AddString( hwndCB, TEXT("=") );
			ComboBox_AddString( hwndCB, TEXT(">") );
			ComboBox_AddString( hwndCB, TEXT(">=") );
			ComboBox_SetCurSel( hwndCB, st->cp_cond );
			// item/skill id
			SetDlgItemInt( hDlg, IDC_E_ITEMID, st->id, TRUE );
			if( st->id > 0 )
			{
				wchar_t skillName[256] = {0};
				DBLayer_getSkillName( (unsigned int)st->id, skillName, 256 );
				SetDlgItemTextW( hDlg, IDC_E_ITEM, skillName );
			}
			// hp, mp, cp values
			SetDlgItemInt( hDlg, IDC_E_HP, st->hp, TRUE );
			SetDlgItemInt( hDlg, IDC_E_MP, st->mp, TRUE );
			SetDlgItemInt( hDlg, IDC_E_CP, st->cp, TRUE );
			// delay ms
			SetDlgItemInt( hDlg, IDC_E_DELAYMS, st->delayms, TRUE );
			//
			return TRUE;
		} break;
	case WM_COMMAND:
		{
			self_heal_cond *st = (self_heal_cond *)GetWindowLongPtrW( hDlg, GWLP_USERDATA );
			switch( LOWORD( wParam ) )
			{
			case IDOK:
				{
					// get all fields from dialog
					// hp,mp,cp cond & val
					HWND hwndCB = GetDlgItem( hDlg, IDC_CB_HP );
					st->hp_cond = ComboBox_GetCurSel( hwndCB );
					st->hp = GetDlgItemInt( hDlg, IDC_E_HP, NULL, TRUE );
					hwndCB = GetDlgItem( hDlg, IDC_CB_MP );
					st->mp_cond = ComboBox_GetCurSel( hwndCB );
					st->mp = GetDlgItemInt( hDlg, IDC_E_MP, NULL, TRUE );
					hwndCB = GetDlgItem( hDlg, IDC_CB_CP );
					st->cp_cond = ComboBox_GetCurSel( hwndCB );
					st->cp = GetDlgItemInt( hDlg, IDC_E_CP, NULL, TRUE );
					// skill id
					st->id = GetDlgItemInt( hDlg, IDC_E_ITEMID, NULL, TRUE );
					// delay ms
					st->delayms = GetDlgItemInt( hDlg, IDC_E_DELAYMS, NULL, TRUE );
					//
					st->used = true;
					EndDialog( hDlg, IDOK );
					return TRUE;
				} break;
			case IDCANCEL:
				{
					st->used = false;
					EndDialog( hDlg, IDCANCEL );
					return TRUE;
				} break;
			case IDC_B_FIND:
				{
					Dlg_FindInDB *dlg = NULL;
					switch( st->type )
					{
					case 0: dlg = new Dlg_FindInDB( Dlg_FindInDB::MODE_ITEM ); break;
					case 1: dlg = new Dlg_FindInDB( Dlg_FindInDB::MODE_SKILL ); break;
					}
					if( !dlg ) return FALSE;
					if( dlg->runDialog( hDlg ) )
					{
						SetDlgItemTextW( hDlg, IDC_E_ITEM, dlg->itemNameW );
						SetDlgItemInt( hDlg, IDC_E_ITEMID, dlg->itemID, FALSE );
					}
					delete dlg;
				} break;
			}
			return FALSE;
		} break;
	}
	return FALSE;
}
