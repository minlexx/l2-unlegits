#ifndef H_SCRIPT_ENGINE
#define H_SCRIPT_ENGINE

#include "lua/lua.hpp"

class ScriptEngine
{
protected:
	static ScriptEngine *s_instance;
	static int           s_refCount;
public:
	static ScriptEngine *getInstance();
	static void          freeInstance();
protected:
	ScriptEngine();
	~ScriptEngine();

public:
	void init();
	void deinit();

public:
	void getLastFileName( char *out );

public:
	void startScriptThread( const char *filename = NULL );
	bool stopScriptThread( DWORD dwTimeoutMsec = 5000, bool bTerminate = true ); // true if stop OK
	int  isScriptThreadRunning() const;
	int  isScriptThreadPaused() const;
	bool pauseScriptThread( bool bPause = true );

public: // handlers
	void set_onChat_handler( const char *funcName );
	void call_onChat( unsigned int senderObjectId, int channelId, const wchar_t *message, const wchar_t *from );

protected:
	static DWORD WINAPI scriptEngineThread( LPVOID lpvClass );

protected:
	lua_State *L;
	HANDLE     hThread;
	char       script_fileName[256];
	bool       should_stop;
	bool       _isPaused;
	// handlers
	bool       m_onChat_enabled;
	char       m_onChat_functionName[128];

friend int sys_should_exit( lua_State *L );
};

#endif
