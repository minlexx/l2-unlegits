#ifndef H_CHOOSECHAR_DLG
#define H_CHOOSECHAR_DLG

// returns: index of selected char in array 'chars': [0 .. (nCount-1)]
//
// -1 on cancel
int ChooseChar( HWND hWndParent, L2Game_CharSelectionInfoBlock *chars, int nCount );

#endif
