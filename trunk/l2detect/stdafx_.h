#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

//#ifndef VC_EXTRALEAN
//#define VC_EXTRALEAN // ��������� ����� ������������ ���������� �� ���������� Windows
//#endif

#include "targetver.h"

//#include <winsock2.h>
//#include <windows.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS // ��������� ������������ CString ����� ������

// ��������� ������� ������� ��������� ����� � ����� ������������ �������������� MFC
#define _AFX_ALL_WARNINGS

#include <afxwin.h>  // �������� � ����������� ���������� MFC
#include <afxext.h>  // ���������� MFC


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h> // ��������� MFC ��� ������� ��������� ���������� Internet Explorer 4
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>   // ��������� MFC ��� ������� ��������� ���������� Windows
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


