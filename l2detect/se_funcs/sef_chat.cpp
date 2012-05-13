#include "stdafx.h"
#include "../Logger.h"
#include "../lua/lua.hpp"
#include "../ScriptEngine.h"
#include "../GameClient.h"
extern class GameClient *g_game_client;
#include "../PacketInjector.h"

int l2c_say( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs == 0 ) return 0;
	if( !g_game_client ) return 0;
	size_t len = 0;
	double dchannel = lua_tonumber( L, 1 );
	unsigned int ichannel = (unsigned int)dchannel;
	const char *mes = luaL_checklstring( L, 2, &len );
	wchar_t *text = (wchar_t *)malloc( len*2 + 2 );
	if( text )
	{
		memset( text, 0, len*2+2 );
		MultiByteToWideChar( CP_ACP, 0, mes, -1, text, len );
		PGen_Say2C( ichannel, text, NULL );
		free( text );
	}
	return 0;
}

int l2c_sayPm( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 2 ) return 0;
	if( !g_game_client ) return 0;
	size_t lenmes = 0;
	size_t lento = 0;
	const char *mes = luaL_checklstring( L, 1, &lenmes );
	const char *to = luaL_checklstring( L, 2, &lento );
	wchar_t *text = (wchar_t *)malloc( lenmes*2 + 2 );
	if( text )
	{
		wchar_t tto[64] = {0};
		memset( text, 0, lenmes*2+2 );
		memset( tto, 0, sizeof(tto) );
		MultiByteToWideChar( CP_ACP, 0, mes, -1, text, lenmes );
		MultiByteToWideChar( CP_ACP, 0, to, -1, tto, 63 );
		tto[63] = 0;
		PGen_Say2C( 2, text, tto ); // public final static int TELL = 2;
		free( text );
	}
	return 0;
}

// l2c_npcDlg( string bypassActionName )
int l2c_npcDlg( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	if( !g_game_client ) return 0;
	const char *abypass = lua_tolstring( L, 1, NULL );
	if( !abypass ) return 0;
	wchar_t bypass[256] = {0};
	MultiByteToWideChar( CP_ACP, 0, abypass, -1, bypass, 255 );
	if( bypass[0] == 0 ) return 0;
	//
	unsigned int last_npc_html_oid = 0;
	wchar_t *last_html = NULL;
	last_html = g_game_client->ai.get_last_NPC_HTML( &last_npc_html_oid );
	if( !last_html ) return 0;
	//
	wchar_t strtofind[512] = {0};
	swprintf( strtofind, 511, L">%s</a>", bypass );
	//log_error_np( LOG_OK, "NPCDLG: to find: [%S]\n", strtofind );
	wchar_t *ps = wcsstr( last_html, strtofind );
	if( !ps )
	{
		char areq[256] = {0};
		WideCharToMultiByte( CP_ACP, 0, bypass, -1, areq, 255, NULL, NULL );
		log_error( LOG_ERROR, "l2c_npcDlg(): not found requested NPCDLG [%s]\n", areq );
		return 0;
	}
	while( (ps > last_html) && ((*ps) != L'<') ) ps--;
	if( ps == last_html )
	{
		log_error( LOG_ERROR, "l2c_npcDlg(): not found bypass link to requested NPCDLG\n" );
		return 0;
	}
	//log_error_np( LOG_OK, "NPCDLG: found: [%S]\n", ps );
	wchar_t bypassStr[256];
	wchar_t *pbypassStr = bypassStr;
	(*pbypassStr) = 0;
	// now find first occurence of "
	while( (*ps) != L'\"' ) ps++;
	ps++;
	// now copy till second "
	while( (*ps) != L'\"' )
	{
		(*pbypassStr) = (*ps);
		ps++;
		pbypassStr++;
		(*pbypassStr) = 0;
	}
	log_error_np( LOG_OK, "l2c_npcDlg(): got bypassStr [%S]\n", bypassStr );
	if( wcsstr( bypassStr, L"bypass -h " ) == bypassStr )
	{
		PGen_RequestBypassToserver( bypassStr + 10 /*wcslen( L"bypass -h " )*/ );
	}
	else if( wcsstr( bypassStr, L"link " ) == bypassStr )
	{
		PGen_RequestLinkHtml( bypassStr + 5 /*wcslen( L"link " )*/ );
	}
	else
	{
		log_error( LOG_WARNING, "l2c_npcDlg(): bypassStr must start with \"bypass -h \"!\n" );
	}
	//
	return 0;
}

// l2c_npcDlgClear()
int l2c_npcDlgClear( lua_State *L )
{
	UNREFERENCED_PARAMETER( L );
	if( !g_game_client ) return 0;
	g_game_client->ai.clear_last_NPC_HTML();
	return 0;
}

// bool l2c_npcDlgExists()
int l2c_npcDlgExists( lua_State *L )
{
	if( !g_game_client )
	{
		lua_pushboolean( L, 0 );
		return 0;
	}
	wchar_t *html = g_game_client->ai.get_last_NPC_HTML( NULL );
	lua_pushboolean( L, html ? 1 : 0 );
	return 1;
}


// l2c_RequestBypassToServer( string bypass )
int l2c_RequestBypassToServer( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	if( !g_game_client ) return 0;
	const char *abypass = lua_tolstring( L, 1, NULL );
	if( !abypass ) return 0;
	wchar_t wsz[512];
	MultiByteToWideChar( CP_ACP, 0, abypass, -1, wsz, 511 );
	PGen_RequestBypassToserver( wsz );
	return 0;
}


// l2c_RequestLinkHtml( string link )
int l2c_RequestLinkHtml( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs < 1 ) return 0;
	if( !g_game_client ) return 0;
	const char *abypass = lua_tolstring( L, 1, NULL );
	if( !abypass ) return 0;
	wchar_t wsz[512];
	MultiByteToWideChar( CP_ACP, 0, abypass, -1, wsz, 511 );
	PGen_RequestLinkHtml( wsz );
	return 0;
}
