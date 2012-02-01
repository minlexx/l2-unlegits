#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#define _WIN32_IE 0x0600
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

// C++ RTL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <errno.h>

// WinAPI
#include <windows.h>
#include <windowsx.h>
#include <wchar.h>
#include <process.h>
#include <commctrl.h>
#include <mmsystem.h>
#include <ddraw.h>

// STL
#include <vector>
#include <map>
#include <list>
#include <string>

// logger
#include "logger/Logger.h"

// local Direct3D 9 Interface
#include <d3d9.h>
#include <d3dx9.h>
#include "d3d/myIDirect3D9.h"
#include "d3d/myIDirect3DDevice9.h"
