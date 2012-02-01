#pragma once

void Hook_connect( bool write_hook );

int __stdcall call_real_connect( int s, void *a, int l );

bool Hook_validateIntercept_connect();
void Hook_dbg_displayInterceptionState();
