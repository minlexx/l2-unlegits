#pragma once

extern HINSTANCE           gl_hThisInstance;
extern HINSTANCE           gl_hOriginalDll;

void D3D9Proxy_Initialize();
void D3D9Proxy_Uninitialize();
void D3D9Proxy_LoadL2Detect();