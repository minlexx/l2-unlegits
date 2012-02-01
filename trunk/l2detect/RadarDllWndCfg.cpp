#include "stdafx.h"
#include "RadarDllWndCfg.h"

void RadarWndCfg_InitDefault( RADARWNDCFG *pCfg )
{
	memset( pCfg, 0, sizeof(RADARWNDCFG) );
	pCfg->ui.colors.bgColor          = RGB(0,0,0);
	pCfg->ui.colors.circleColor      = RGB(128,128,128);
	pCfg->ui.colors.npcColor         = RGB(200,200,0);
	pCfg->ui.colors.mobColor         = RGB(128,0,0);
	pCfg->ui.colors.pcColor          = RGB(75,75,255);
	pCfg->ui.colors.pcColorDead      = RGB(75,75,75);
	pCfg->ui.colors.pcColorWar       = RGB(255,85,85);
	pCfg->ui.colors.pcColorWarDead   = RGB(155,65,65);
	pCfg->ui.colors.pcColorParty     = RGB(60,255,60);
	pCfg->ui.colors.pcColorPartyDead = RGB(0,200,0);
}

void RadarWndCfg_LoadConfigFromFile( RADARWNDCFG *pCfg, const char *fileName )
{
	if( !pCfg ) return;
	char *useFN = (char *)fileName;
	if( !useFN ) useFN = "L2Detect_colors.ini";
	FILE *f = fopen( useFN, "rb" );
	if( !f ) return;
	fread( pCfg, 1, sizeof(RADARWNDCFG), f );
	fclose( f );
}

void RadarWndCfg_SaveConfigToFile( RADARWNDCFG *pCfg, const char *fileName )
{
	if( !pCfg ) return;
	char *useFN = (char *)fileName;
	if( !useFN ) useFN = "L2Detect_colors.ini";
	FILE *f = fopen( useFN, "wb" );
	if( !f ) return;
	//
	fwrite( pCfg, 1, sizeof(RADARWNDCFG), f );
	//
	fclose( f );
}

