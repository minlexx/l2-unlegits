#ifndef H_BOTCFGDLG
#define H_BOTCFGDLG

void BotCfgDlg_Run( HWND hWndParent, L2Client *pcl );

// children tabs create funcs
// self
HWND BotCfgDlg_Self_Create( HWND hWndParent, BotConfig *pcfg );
HWND BotCfgDlg_SelfHeal_Create( HWND hWndParent, BotConfig *pcfg );
// party
HWND BotCfgDlg_Party_Create( HWND hWndParent, BotConfig *pcfg );

#endif
