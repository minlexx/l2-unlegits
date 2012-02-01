#include "stdafx.h"
#include "Logger.h"
#include "ConfigIni.h"
#include "GameClient.h"
extern class CConfig     g_cfg;
extern class GameClient *g_game_client;

int freadline( FILE *f, char *line, size_t maxCount )
{
	if( !f ) return -1;
	int nRead = 0;
	line[0] = 0;
	fgets( line, maxCount, f );
	if( line[0] == 0 ) return nRead;
	size_t ll = strlen( line );
	if( ll > 0 )
	{
		if( (line[ll-1] == '\r') || (line[ll-1] == '\n') ) line[ll-1] = 0;
		if( ll > 1 )
		{
			if( (line[ll-2] == '\r') || (line[ll-2] == '\n') ) line[ll-1] = 0;
		}
	}
	nRead = (int)strlen( line );
	return nRead;
}


bool MyInitCommonControlsEx()
{
	//InitCommonControls();
	// InitCommonControlsEx() требуетс€ дл€ Windows XP, если манифест
	// приложени€ использует ComCtl32.dll версии 6 или более поздней версии дл€ включени€
	// стилей отображени€. ¬ противном случае будет возникать сбой при создании любого окна.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ¬ыберите этот параметр дл€ включени€ всех общих классов управлени€, которые необходимо использовать
	// в вашем приложении.
	InitCtrls.dwICC = ICC_WIN95_CLASSES; // | ICC_STANDARD_CLASSES;
	InitCtrls.dwICC = ICC_ANIMATE_CLASS | ICC_BAR_CLASSES | ICC_COOL_CLASSES | ICC_DATE_CLASSES | ICC_HOTKEY_CLASS | 
		ICC_INTERNET_CLASSES | ICC_LINK_CLASS | ICC_LISTVIEW_CLASSES | ICC_NATIVEFNTCTL_CLASS | 
		ICC_PAGESCROLLER_CLASS | ICC_PROGRESS_CLASS | ICC_STANDARD_CLASSES | ICC_TAB_CLASSES | 
		ICC_TREEVIEW_CLASSES | ICC_UPDOWN_CLASS | ICC_USEREX_CLASSES | ICC_WIN95_CLASSES;
	if( InitCommonControlsEx( &InitCtrls ) )
		log_error( LOG_OK, "InitCommonControlsEx(): OK\n" );
	else
	{
		DWORD e = GetLastError();
		ErrorLogger_LogLastError( "InitCommonControlsEx()", e );
	}
	return true;
}


void utils_detectClanWar( unsigned int relation, bool *isEnemy, bool *is2side )
{
	L2_VERSION ver = (L2_VERSION)g_cfg.L2_version;
	unsigned int urel = g_game_client->ai.usr.relation;
	// relation constants before Gracia Final
	unsigned int RELATION_ENEMY      = 0x01000; // true when red icon, doesn't matter with blue
	unsigned int RELATION_ALLY       = 0x00800; // blue siege icon, cannot have if red
	unsigned int RELATION_MUTUAL_WAR = 0x08000;
	unsigned int RELATION_1SIDED_WAR = 0x10000;
	// gracia final & epilogue constants changed
	if( ver >= L2_VERSION_T23 )
	{
		RELATION_MUTUAL_WAR = 0x04000;
		RELATION_1SIDED_WAR = 0x08000;
		//_relation = _activeChar.isClanLeader() ? 0x40 : 0;
        //if (_activeChar.getSiegeState() == 1) _relation |= 0x180;
        //if (_activeChar.getSiegeState() == 2) _relation |= 0x80;
	}
	if( (relation & RELATION_1SIDED_WAR ) ||
		(relation & RELATION_MUTUAL_WAR ) )
	{
		(*isEnemy) = true;
		if( relation & RELATION_MUTUAL_WAR ) (*is2side) = true;
	}
	// test: siege enemies detection
	if( ((urel & RELATION_ENEMY) && (relation & RELATION_ALLY)) ||
		((urel & RELATION_ALLY) && (relation & RELATION_ENEMY)) )
	{
		(*isEnemy) = true;
		(*is2side) = true;
	}
}
