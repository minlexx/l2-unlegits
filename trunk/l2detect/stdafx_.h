#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

//#ifndef VC_EXTRALEAN
//#define VC_EXTRALEAN // Исключите редко используемые компоненты из заголовков Windows
//#endif

#include "targetver.h"

//#include <winsock2.h>
//#include <windows.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS // некоторые конструкторы CString будут явными

// отключает функцию скрытия некоторых общих и часто пропускаемых предупреждений MFC
#define _AFX_ALL_WARNINGS

#include <afxwin.h>  // основные и стандартные компоненты MFC
#include <afxext.h>  // расширения MFC


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h> // поддержка MFC для типовых элементов управления Internet Explorer 4
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>   // поддержка MFC для типовых элементов управления Windows
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <winsock2.h>

#include <stdarg.h>
#include <process.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/blowfish.h>
#include "utils.h"





#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


