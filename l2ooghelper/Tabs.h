#ifndef H_TABS
#define H_TABS

#include "Account.h"

// init
void Tabs_Init();
// add/remove tabs
void Tabs_AddAccount( Account *pAccount );
void Tabs_DeleteTab( int tab_id );
// window messages handlers
void Tabs_Resize( int w, int h );
void Tabs_OnPaint();
void Tabs_OnSelChanging( LPNMHDR lpnmhdr );
void Tabs_OnSelChange( LPNMHDR lpnmhdr );

#endif
