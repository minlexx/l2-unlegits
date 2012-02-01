#ifndef H_RADARDLLWND_CFG
#define H_RADARDLLWND_CFG

struct RADARWNDCFG
{
	struct st_ui
	{
		struct st_colors
		{
			COLORREF bgColor;          // = RGB(0,0,0);
			COLORREF circleColor;      // = RGB(127,128,128);
			COLORREF npcColor;         // = RGB(200,200,0);
			COLORREF mobColor;         // = RGB(128,0,0);
			COLORREF pcColor;          // = RGB(75,75,255);
			COLORREF pcColorDead;      // = RGB(75,75,75);
			COLORREF pcColorWar;       // = RGB(255,85,85);
			COLORREF pcColorWarDead;   // = RGB(155,65,65);
			COLORREF pcColorParty;     // = RGB(60,255,60);
			COLORREF pcColorPartyDead; // = RGB(0,200,0);
		} colors;
	} ui;
};

void RadarWndCfg_InitDefault( RADARWNDCFG *pCfg );

void RadarWndCfg_LoadConfigFromFile( RADARWNDCFG *pCfg, const char *fileName );
void RadarWndCfg_SaveConfigToFile( RADARWNDCFG *pCfg, const char *fileName );

#endif