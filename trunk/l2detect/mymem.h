#ifndef H_MYMEM
#define H_MYMEM

void MyMem_Init();
void MyMem_Free();

// Memory tracking allocation

//inline void *operator new( size_t size ); // non-debug version

void *operator new( size_t nSize, const char *lpszFileName, int nLine );

#define DEBUG_NEW new( __FILE__, __LINE__ )


void operator delete( void *p );
void operator delete( void *p, size_t size );

void operator delete( void* p, const char *lpszFileName, int nLine );


//void *__cdecl operator new[]( size_t );
//#if _MSC_VER >= 1210
//void* __cdecl operator new[](size_t nSize, LPCSTR lpszFileName, int nLine);
//void __cdecl operator delete[](void* p, LPCSTR lpszFileName, int nLine);
//void __cdecl operator delete[](void *);
//#endif

#endif
