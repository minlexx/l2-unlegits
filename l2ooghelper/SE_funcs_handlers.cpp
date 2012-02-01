#include "stdafx.h"
#include "Logger.h"
#include "SE_funcs.h"
#include "ScriptEngine.h"

// sys_register_onChat( function_name )
// регистрирует функцию function_name как обработчик чат-сообщений
// обработчик должен иметь прототип Lua
// function onChat( senderObjectID, chatChannelID, chatText, senderName )
int sys_register_onChat( lua_State *L )
{
	int typ = lua_type( L, lua_upvalueindex( 1 ) );
	if( typ != LUA_TLIGHTUSERDATA )
	{
		log_error( LOG_ERROR, "sys_register_onChat(): no light user data!\n" );
		return 0;
	}
	// получим указатель на ScriptEngine как light user data функции
	// (устанавливается при регистрации функции в SE_regScriptEngineFunc)
	ScriptEngine *se = (ScriptEngine *)lua_touserdata( L, lua_upvalueindex( 1 ) );
	if( se == NULL )
	{
		log_error( LOG_ERROR, "sys_register_onChat(): ptr to ScriptEngine == NULL!\n" );
		return 0;
	}
	// получим первый арумент Lua функции sys_register_onChat как строку
	const char *funcName = lua_tolstring( L, 1, NULL );
	// funcName может быть NULL, если первый аргумент не был строкой. тогда обработчик отменится
	// зарегаем обработчик
	se->set_onChat_handler( funcName );
	return 0;
}
