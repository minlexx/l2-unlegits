#include "stdafx.h"
#include "logger.h"
#include "IL2Client.h"
#include "SE_funcs.h"
#include "ScriptEngine.h"

#ifdef USE_SEH
#include "SEH_global.h"
#endif

ScriptEngine::ScriptEngine()
{
	L = NULL;
	m_pvL2Client = NULL;
	hThread = NULL;
	memset( script_fileName, 0, sizeof(script_fileName) );
	should_stop = false;
	_isPaused = false;
	// handlers
	m_onChat_enabled = false;
	m_onChat_functionName[0] = 0;
}

ScriptEngine::~ScriptEngine()
{
	deinit();
}

void ScriptEngine::init()
{
	if( !L )
	{
		L = luaL_newstate(); // open Lua virtual machine
		if( L )
		{
			luaL_openlibs( L ); // load the Lua libraries: math, io, ..
			SE_funcs_register( L ); // register main functions (l2h_*)
		}
		else log_error( LOG_ERROR, "ScriptEngine::init(): luaL_newstate() returned NULL!\n" );
	}
}

void ScriptEngine::deinit()
{
	if( L )
	{
		lua_close( L ); // Destroys all objects in the given Lua state
		//(calling the corresponding garbage-collection metamethods, if any)
		//and frees all dynamic memory used by this state
		L = NULL;
		if( hThread ) CloseHandle( hThread );
		hThread = NULL;
		should_stop = false;
		_isPaused = false;
		// handlers
		m_onChat_enabled = false;
		m_onChat_functionName[0] = 0;
	}
}

void ScriptEngine::setL2Client( void *pvL2Client )
{
	init();
	if( L )
	{
		m_pvL2Client = pvL2Client;
		SE_register_client_funcs( L, pvL2Client, (void *)this ); // register client functions (l2c_*)
	}
}

void ScriptEngine::startScript( const char *filename )
{
	// run a Lua scrip here
	int r = luaL_loadfile( L, filename );
	if( r == 0 )
	{
		log_error( LOG_DEBUG, "SE: file [%s] loaded.\n", filename );
		// Call main...
		r = lua_pcall( L, 0, LUA_MULTRET, 0 );
		if( r == 0 ) log_error( LOG_DEBUG, "SE: run OK\n" );
		else
		{
			log_error( LOG_ERROR, "SE: file [%s] run error %d\n", filename, r );
			switch( r )
			{
			case LUA_ERRRUN: log_error( LOG_ERROR, "SE: LUA_ERRRUN\n" ); break;
			case LUA_ERRMEM: log_error( LOG_ERROR, "SE: LUA_ERRMEM\n" ); break;
			case LUA_ERRERR: log_error( LOG_ERROR, "SE: LUA_ERRERR\n" ); break;
			}
			int nArgs = lua_gettop( L );
			log_error( LOG_ERROR, "SE: nArgs = %d\n", nArgs );
		}
	}
	else log_error( LOG_ERROR, "SE: file [%s] load error %d\n", filename, r );
}

void ScriptEngine::getLastFileName( char *out )
{
	if( !out ) return;
	out[0] = 0;
	if( script_fileName[0] != 0 ) strcpy( out, script_fileName );
}

void ScriptEngine::startScriptThread( const char *filename /*= NULL */ )
{
	if( hThread ) return;
	DWORD dwThreadID = 0;
	if( filename ) strcpy( script_fileName, filename );
	hThread = (HANDLE)_beginthreadex( NULL, 0,
		(unsigned int (__stdcall *)(void *))scriptEngineThread, (void *)this,
		0, (unsigned int *)&dwThreadID );
}

bool ScriptEngine::stopScriptThread( DWORD dwTimeoutMsec, bool bTerminate )
{
	if( !hThread ) return true;
	this->should_stop = true; // "signal" to stop )
	DWORD wait_res = WaitForSingleObject( hThread, dwTimeoutMsec );
	if( wait_res != WAIT_OBJECT_0 ) // wait failed :(
	{
		if( !bTerminate ) return false; // do not do anything, just ret false
		TerminateThread( hThread, 0 );
		log_error( LOG_ERROR, "ScriptEngine::stopScriptThread(): script thread wait failed, TERMINATED!\n" );
		this->deinit();
		this->set_onChat_handler( NULL ); // disable onChat() handler
		this->init();
		if( this->m_pvL2Client ) this->setL2Client( this->m_pvL2Client );
		if( L ) log_error( LOG_WARNING, "ScriptEngine::stopScriptThread(): re-initialized ScriptEngine, bugs possible ><\n" );
	}
	if( hThread ) CloseHandle( hThread ); // cleanup
	hThread = NULL;
	this->should_stop = false; // no more need to stop...
	return true;
}

int ScriptEngine::isScriptThreadRunning() const
{
	if( hThread == NULL ) return 0;
	return 1;
}

int ScriptEngine::isScriptThreadPaused() const
{
	if( _isPaused ) return 1;
	return 0;
}

bool ScriptEngine::pauseScriptThread( bool bPause /*= true*/ )
{
	if( !isScriptThreadRunning() ) return false;
	int prev_suspend_count = 0;
	if( bPause )
	{
		prev_suspend_count = (int)SuspendThread( hThread );
		if( prev_suspend_count > 0 )
		{
			log_error( LOG_WARNING, "ScriptEngine::pauseScriptThread(): PAUSE: thread was already suspended, resuming again\n" );
			ResumeThread( hThread );
		}
		_isPaused = true;
		return true;
	}
	else
	{
		prev_suspend_count = ResumeThread( hThread );
		if( prev_suspend_count > 1 )
		{
			log_error( LOG_WARNING, "ScriptEngine::pauseScriptThread(): UNPAUSE: possible that thread is still suspended!\n" );
		}
		_isPaused = false;
		return true;
	}
}

DWORD WINAPI ScriptEngine::scriptEngineThread( LPVOID lpvClass )
{
	if( !lpvClass ) return 0;
	ScriptEngine *pcls = (ScriptEngine *)lpvClass;

#ifdef USE_SEH
#ifdef _MSC_VER
	__try { // start exception handler
#endif
#endif

	// run a Lua script here
	int r = luaL_loadfile( pcls->L, pcls->script_fileName );
	if( r == 0 )
	{
		log_error( LOG_DEBUG, "SE: file [%s] loaded.\n", pcls->script_fileName );
		// Call main...
#ifndef USE_SEH
		try
		{
#endif
			r = lua_pcall( pcls->L, 0, LUA_MULTRET, 0 );
#ifndef USE_SEH
		}
		catch(...)
		{
			log_error( LOG_ERROR, "SE: catched C++ exception after lua_pcall()\n" );
			ErrorLogger_FlushLogFile();
		}
#endif
		if( r == 0 ) log_error( LOG_DEBUG, "SE: run OK\n" );
		else
		{
			log_error( LOG_ERROR, "SE: file [%s] lua_pcall() error %d\n", pcls->script_fileName, r );
			switch( r )
			{
			case LUA_ERRRUN:    log_error( LOG_ERROR, "SE: LUA_ERRRUN\n" ); break;
			case LUA_ERRMEM:    log_error( LOG_ERROR, "SE: LUA_ERRMEM\n" ); break;
			case LUA_ERRSYNTAX: log_error( LOG_ERROR, "SE: LUA_ERRSYNTAX\n" ); break;
			case LUA_ERRERR:    log_error( LOG_ERROR, "SE: LUA_ERRERR\n" ); break;
			}
			int nArgs = lua_gettop( pcls->L );
			if( nArgs >= 1 )
			{
				if( lua_isstring( pcls->L, 1 ) )
				{
					log_error( LOG_ERROR, "SE: [%s]\n", lua_tolstring( pcls->L, 1, NULL ) );
					//lua_pop( pcls->L, 1 ); // really?
				}
			}
			lua_settop( pcls->L, 0 ); // clear stack
		}
	}
	else
	{
		log_error( LOG_ERROR, "SE: file [%s] luaL_loadfile() error %d\n", pcls->script_fileName, r );
		switch( r )
		{
		case LUA_ERRRUN:    log_error( LOG_ERROR, "SE: LUA_ERRRUN\n" ); break;
		case LUA_ERRMEM:    log_error( LOG_ERROR, "SE: LUA_ERRMEM\n" ); break;
		case LUA_ERRSYNTAX: log_error( LOG_ERROR, "SE: LUA_ERRSYNTAX\n" ); break;
		case LUA_ERRERR:    log_error( LOG_ERROR, "SE: LUA_ERRERR\n" ); break;
		case LUA_ERRFILE:   log_error( LOG_ERROR, "SE: LUA_ERRFILE\n" ); break;
		}
		int nArgs = lua_gettop( pcls->L );
		if( nArgs >= 1 )
		{
			if( lua_isstring( pcls->L, 1 ) )
			{
				log_error( LOG_ERROR, "SE: [%s]\n", lua_tolstring( pcls->L, 1, NULL ) );
				//lua_pop( pcls->L, 1 ); // really?
			}
		}
		lua_settop( pcls->L, 0 ); // clear stack
	}
	pcls->set_onChat_handler( NULL ); // disable onChat() handler
	if( pcls->hThread ) CloseHandle( pcls->hThread ); // cleanup
	pcls->hThread = NULL; // mark thread as not running
	pcls->should_stop = false;
	log_error( LOG_DEBUG, "SE: scriptEngineThread() stopped [%s]\n", pcls->script_fileName );

#ifdef USE_SEH
#ifdef _MSC_VER // exception handler
	}
	__except( ScriptEngineThreadExceptonHandler( GetExceptionInformation() ) )
	{
		log_error( LOG_ERROR, "Exception in ScriptEngine thread... possible small memory leak\n" );
		ErrorLogger_FlushLogFile();
		pcls->hThread = NULL;
		pcls->should_stop = false;
	}
#endif
#endif

	return 1;
}

// http://www.lua.ru/doc/3.7.html
// Эквивалентыный код в Lua:
//            a = f('how', t.x, 14)
//            
//То же в C:
//     lua_getfield(L, LUA_GLOBALSINDEX, 'f'); /* function to be called */
//     lua_pushstring(L, 'how');                        /* 1st argument */
//     lua_getfield(L, LUA_GLOBALSINDEX, 't');   /* table to be indexed */
//     lua_getfield(L, -1, 'x');        /* push result of t.x (2nd arg) */
//     lua_remove(L, -2);                  /* remove 't' from the stack */
//     lua_pushinteger(L, 14);                          /* 3rd argument */
//     lua_call(L, 3, 1);     /* call 'f' with 3 arguments and 1 result */
//     lua_setfield(L, LUA_GLOBALSINDEX, 'a');        /* set global 'a' */
//    
//Заметьте что код сбалансирован: в его конце стек возвращается в исходныю конфигурацию. Это считается хорошей практикой программирования.

void ScriptEngine::call_onChat( unsigned int senderObjectId, int channelId, const wchar_t *message, const wchar_t *from )
{
	if( !m_onChat_enabled ) return;
	if( !message ) return;
	if( !isScriptThreadRunning() ) return;
	if( isScriptThreadPaused() ) return;
	//
	pauseScriptThread( true ); // pause script thread
	//
	// convert strings from Unicode to ANSI
	char message_ansi[1024];
	char from_ansi[128];
	message_ansi[0] = from_ansi[0] = 0;
	WideCharToMultiByte( CP_ACP, 0, message, -1, message_ansi, sizeof(message_ansi)-1, NULL, NULL );
	if( from ) WideCharToMultiByte( CP_ACP, 0, from, -1, from_ansi, sizeof(from_ansi)-1, NULL, NULL );
	message_ansi[sizeof(message_ansi)-1] = 0;
	from_ansi[sizeof(from_ansi)-1] = 0;
	//
	lua_getfield( L, LUA_GLOBALSINDEX, m_onChat_functionName ); // function to be called
	lua_pushinteger( L, senderObjectId );          // param 1 - sender object ID
	lua_pushinteger( L, channelId );               // param 2 - channel ID
	lua_pushstring( L, message_ansi );             // param 3 - chat message
	lua_pushstring( L, from_ansi );                // param 4 - sender name (only for PM)
	//  call onChat with 4 arguments and 0 return values:  onChat( senderObjectId, channelId, message, from )
	int ret = lua_pcall( L, 4, 0, 0 );
	if( ret != 0 )
	{
		log_error( LOG_ERROR, "SE: call_onChat() [%s] error %d\n", script_fileName, ret );
		switch( ret )
		{
		case LUA_ERRRUN:    log_error( LOG_ERROR, "SE: LUA_ERRRUN\n" ); break;
		case LUA_ERRMEM:    log_error( LOG_ERROR, "SE: LUA_ERRMEM\n" ); break;
		case LUA_ERRSYNTAX: log_error( LOG_ERROR, "SE: LUA_ERRSYNTAX\n" ); break;
		case LUA_ERRERR:    log_error( LOG_ERROR, "SE: LUA_ERRERR\n" ); break;
		case LUA_ERRFILE:   log_error( LOG_ERROR, "SE: LUA_ERRFILE\n" ); break;
		}
		int nArgs = lua_gettop( L );
		if( nArgs >= 1 )
		{
			if( lua_isstring( L, 1 ) )
				log_error( LOG_ERROR, "SE: [%s]\n", lua_tolstring( L, 1, NULL ) );
		}
		lua_settop( L, 0 ); // clear stack
	}
	else log_error( LOG_DEBUG, "SE: call_onChat() [%s] OK\n", script_fileName );
	//
	pauseScriptThread( false ); // resume script thread
	//
}

void ScriptEngine::set_onChat_handler( const char *funcName )
{
	if( funcName == NULL ) // disable
	{
		m_onChat_enabled = false;
		m_onChat_functionName[0] = 0;
		return;
	}
	m_onChat_enabled = true;
	strncpy( m_onChat_functionName, funcName, sizeof(m_onChat_functionName)-1 );
	m_onChat_functionName[sizeof(m_onChat_functionName)-1] = 0;
}
