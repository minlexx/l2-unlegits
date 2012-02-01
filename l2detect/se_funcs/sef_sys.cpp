#include "stdafx.h"
#include "../Logger.h"
#include "../lua/lua.hpp"
#include "../ScriptEngine.h"

// "system" functions
// вызывается скриптом, проверяет, должен ли скрипт остановиться
int sys_should_exit( lua_State *L )
{
	ScriptEngine *se = ScriptEngine::getInstance();
	if( !se )
	{
		log_error( LOG_ERROR, "sys_should_exit(): ScriptEngine ptr == NULL!\n" );
		lua_pushboolean( L, 1 );
		return 1;
	}
	if( se->should_stop ) lua_pushboolean( L, 1 );
	else lua_pushboolean( L, 0 );
	return 1; // возвращает одно значение
}

// sys_register_onChat( function_name )
// регистрирует функцию function_name как обработчик чат-сообщений
// обработчик должен иметь прототип Lua
// function onChat( senderObjectID, chatChannelID, chatText, senderName )
int sys_register_onChat( lua_State *L )
{
	ScriptEngine *se = ScriptEngine::getInstance();
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


// выводит в лог (и также в консоль) список значений, разделеных запятыми
int l2h_print( lua_State *L )
{
	int nArgs = lua_gettop( L );
	if( nArgs > 0 )
	{
		int i;
		for( i=1; i<=nArgs; i++ )
		{
			int typ = lua_type( L, i );
			switch( typ )
			{
			case LUA_TNIL: log_error_np( LOG_OK, "NULL" ); break;
			case LUA_TNUMBER:
				{
					double d = lua_tonumber( L, i );
					double floor_d = floor( d * 100 ); // 10012.00
					__int64 int_d = (__int64)floor_d; // 10012
					__int64 eds_cnt = int_d % 100; // 12
					if( eds_cnt ) log_error_np( LOG_OK, "%0.2f", d ); // original double
					else log_error_np( LOG_OK, "%I64d", int_d/100 ); // rounded integer
				} break;
			case LUA_TBOOLEAN:
				{
					lua_toboolean( L, i ) ? log_error_np( LOG_OK, "true" ) : log_error_np( LOG_OK, "false" );
				} break;
			case LUA_TSTRING: log_error_np( LOG_OK, "%s", lua_tostring( L, i ) ); break;
			case LUA_TTABLE: { log_error_np( LOG_OK, "(table)" ); /*lua_pop( L, i );*/ } break;
			case LUA_TFUNCTION: { log_error_np( LOG_OK, "(function)" ); /*lua_pop( L, i );*/ } break;
			default: lua_pop( L, i ); break;
			}
		}
	}
	return 0;
}

int l2h_console_enable( lua_State *L )
{
	int n = lua_gettop( L );    // number of arguments
	if( n > 0 )
	{
		if( lua_type( L, 1 ) == LUA_TBOOLEAN )
		{
			int enable = lua_toboolean( L, 1 );
			ErrorLogger_EnableLoggingToConsole( enable ? true : false );
		}
	}
	return 0;
}

int l2h_delay( lua_State *L )
{
	int n = lua_gettop( L );    // number of arguments
	//log_error_np( LOG_OK, "delay_lua: %d args\n", n );
	if( n >= 1 )
	{
		if( lua_type( L, 1 ) == LUA_TNUMBER )
		{
			int msec = (int)lua_tonumber( L, 1 );
			//log_error_np( LOG_OK, "delay_lua( %d );\n", msec );
			Sleep( (DWORD)msec );
		}
	}
	return 0;
}

int l2h_soundAlert( lua_State *L )
{
	int nArgs = lua_gettop( L );
	TCHAR fileName[256] = {0};
	if( nArgs == 0 )
	{
		GetWindowsDirectory( fileName, 255 );
		_tcscat( fileName, _T("\\Media\\ringin.wav") );
	}
	else
	{
		const char *astr = lua_tolstring( L, 1, NULL );
		MultiByteToWideChar( CP_ACP, 0, astr, -1, fileName, 255 );
	}
	PlaySound( fileName, NULL, SND_FILENAME|SND_ASYNC ); 
	return 0;
}

int l2h_time( lua_State *L )
{
	unsigned int secs = (unsigned int)time( NULL );
	lua_pushnumber( L, secs );
	return 1;
}

int l2h_timeMsec( lua_State *L )
{
	unsigned int msecs = GetTickCount();
	lua_pushnumber( L, msecs );
	return 1;
}
