#ifndef _H_TARGET_VER
#define _H_TARGET_VER

/** \file targetver.h
* Defines version of Windows headers to use. Only under Win32.\n
* Targets WinXP.
*/

// target WinXP

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0501
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#endif /* _H_TARGET_VER */
