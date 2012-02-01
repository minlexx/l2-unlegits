#pragma once

extern HINSTANCE           gl_hThisInstance;
extern myIDirect3DDevice9* gl_pmyIDirect3DDevice9;
extern myIDirect3D9*       gl_pmyIDirect3D9;
extern HINSTANCE           gl_hOriginalDll;
extern HINSTANCE           gl_hThisInstance;

void L2D_Initialize();
void L2D_Uninitialize();
void L2D_ThreadStart();

// wndproc hook
void InstallProxyWndProc( HWND hWndL2 );
