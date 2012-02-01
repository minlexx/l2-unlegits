#ifndef H_SCRIPT_ENGINE
#define H_SCRIPT_ENGINE

#include "lib/lua/lua.hpp"

class ScriptEngine
{
public:
	ScriptEngine();
	~ScriptEngine();

public:
	void init();
	void deinit();
	void setL2Client( void *pvL2Client );
	void startScript( const char *filename );

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
	void      *m_pvL2Client;
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
