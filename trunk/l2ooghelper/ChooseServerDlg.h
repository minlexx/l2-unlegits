#ifndef H_CHOOSESERVER_DLG
#define H_CHOOSESERVER_DLG

// returns: index of selected server in array 'servers': [0 .. (nCount-1)]
//
// -1 on cancel
int ChooseServer( HWND hWndParent, L2GameServerInfo *servers, int nCount );

#endif
