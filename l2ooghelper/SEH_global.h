#ifndef H_SEH_GLOBAL
#define H_SEH_GLOBAL

#ifdef USE_SEH
#ifdef _MSC_VER // only under MS Visual C++

#ifdef _DEBUG
bool SEH_loadMap();
#endif

LONG WINAPI GlobalUnhandledExceptionFilter( LPEXCEPTION_POINTERS e );

LONG WINAPI ScriptEngineThreadExceptonHandler( LPEXCEPTION_POINTERS e );

LONG WINAPI L2ClientThreadExceptonHandler( LPEXCEPTION_POINTERS e );

#endif
#endif

#endif
