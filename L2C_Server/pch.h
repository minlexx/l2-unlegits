#pragma once

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#define _WIN32_IE 0x0600

// CRT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <stdarg.h>
#include <assert.h>
// STL
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <bitset>

// Win32 API
#include <winsock2.h>
#include <windows.h>
#include <windowsx.h>
#include <wchar.h>
#include <tchar.h>
#include <process.h>
#include <commctrl.h>
#include <dbghelp.h>
#include <richedit.h>

// extern libs
#include <L2Packets.h>
#include <mysqlclient/mysql.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/blowfish.h>
#include <openssl/sha.h>
