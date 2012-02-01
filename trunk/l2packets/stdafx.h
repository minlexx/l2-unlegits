#ifndef H_STDAFX
#define H_STDAFX

/** \file stdafx.h
* precompiled header
*/

// detect OS
#if defined(_WIN32) || defined(WIN32) || defined(WINVER) || defined(_WIN32_WINNT)
#undef L2PACKETS_WINDOWS
#define L2PACKETS_WINDOWS
#else
#undef L2PACKETS_LINUX
#define L2PACKETS_LINUX
#endif

// before including windows and CRT headers, we must
// kill warnings for Visual Studio
#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE // vc2005
#define _CRT_SECURE_NO_WARNINGS // vc2008
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <wchar.h>

#if defined(_WIN32) || defined(WIN32) || defined(WINVER) || defined(_WIN32_WINNT)
#define WIN32_LEAN_AND_MEAN // for windows headers
#include "targetver.h"
#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#else // LINUX
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/blowfish.h>

#endif
