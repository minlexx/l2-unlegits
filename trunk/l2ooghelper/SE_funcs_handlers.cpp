#include "stdafx.h"
#include "Logger.h"
#include "SE_funcs.h"
#include "ScriptEngine.h"

// sys_register_onChat( function_name )
// ������������ ������� function_name ��� ���������� ���-���������
// ���������� ������ ����� �������� Lua
// function onChat( senderObjectID, chatChannelID, chatText, senderName )
int sys_register_onChat( lua_State *L )
{
	int typ = lua_type( L, lua_upvalueindex( 1 ) );
	if( typ != LUA_TLIGHTUSERDATA )
	{
		log_error( LOG_ERROR, "sys_register_onChat(): no light user data!\n" );
		return 0;
	}
	// ������� ��������� �� ScriptEngine ��� light user data �������
	// (��������������� ��� ����������� ������� � SE_regScriptEngineFunc)
	ScriptEngine *se = (ScriptEngine *)lua_touserdata( L, lua_upvalueindex( 1 ) );
	if( se == NULL )
	{
		log_error( LOG_ERROR, "sys_register_onChat(): ptr to ScriptEngine == NULL!\n" );
		return 0;
	}
	// ������� ������ ������� Lua ������� sys_register_onChat ��� ������
	const char *funcName = lua_tolstring( L, 1, NULL );
	// funcName ����� ���� NULL, ���� ������ �������� �� ��� �������. ����� ���������� ���������
	// �������� ����������
	se->set_onChat_handler( funcName );
	return 0;
}
