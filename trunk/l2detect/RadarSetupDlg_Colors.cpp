#include "stdafx.h"
#include "Resource.h"
#include "Logger.h"
#include "GameClient.h"
#include "RadarDllWndCfg.h"

extern HINSTANCE g_radardll_hinst;
extern class GameClient *g_game_client; // in main.cpp

// radar window UI config
extern RADARWNDCFG g_rwCfg; // in RadarDllWndProc.cpp

INT_PTR CALLBACK Colors_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

HWND RadarSetupDlg_Colors_Create( HWND hWndParent )
{
	return CreateDialogParam( g_radardll_hinst, MAKEINTRESOURCE(IDD_SR_COLORS),
		hWndParent, Colors_DlgProc, 0 );
}

// =========================================================

void Colors_Init( HWND hDlg );
void Colors_OnDestroy( HWND hDlg );
void Colors_OnApply( HWND hDlg );
void Colors_OnPaint( HWND hDlg );
void Colors_OnSelColor( HWND hDlg, int idx );


INT_PTR CALLBACK Colors_DlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	lParam;
	switch( uMsg )
	{
	case WM_INITDIALOG: Colors_Init( hDlg ); break;
	case WM_DESTROY: { Colors_OnDestroy( hDlg ); return FALSE; } break;
	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			case IDC_APPLY: Colors_OnApply( hDlg ); break;
				// colors
			case IDC_B_SELNPCCOLOR: Colors_OnSelColor( hDlg, 0 ); break;
			case IDC_B_SELMOBCOLOR: Colors_OnSelColor( hDlg, 1 ); break;
			case IDC_B_SELPLAYERCOLOR: Colors_OnSelColor( hDlg, 2 ); break;
			case IDC_B_SELDEADPLAYERCOLOR: Colors_OnSelColor( hDlg, 3 ); break;
			case IDC_B_SELPARTYPLAYERCOLOR: Colors_OnSelColor( hDlg, 4 ); break;
			case IDC_B_SELDEADPARTYPLAYERCOLOR: Colors_OnSelColor( hDlg, 5 ); break;
			case IDC_B_SELENEMYPLAYERCOLOR: Colors_OnSelColor( hDlg, 6 ); break;
			case IDC_B_SELDEADENEMYPLAYERCOLOR: Colors_OnSelColor( hDlg, 7 ); break;
			}
		} break;
	case WM_PAINT: Colors_OnPaint( hDlg ); break;
	default: return FALSE; break;
	}
	return TRUE;
}

void Colors_Init( HWND hDlg )
{
	hDlg;
	//log_error( LOG_OK, "Colors_Init\n" );
}


void Colors_OnDestroy( HWND hDlg )
{
	hDlg;
	//log_error( LOG_OK, "Colors_OnDestroy\n" );
}

void Colors_OnApply( HWND hDlg )
{
	hDlg;
	//log_error( LOG_OK, "Colors_OnApply\n" );
	RadarWndCfg_SaveConfigToFile( &g_rwCfg, NULL );
}

void Colors_OnPaint( HWND hDlg )
{
	PAINTSTRUCT ps;
	HDC hdc;
	hdc = BeginPaint( hDlg, &ps );
	HPEN hPenPrev = (HPEN)SelectObject( hdc, GetStockObject( DC_PEN ) );
	HBRUSH hBrushPrev = (HBRUSH)SelectObject( hdc, GetStockObject( DC_BRUSH ) );
	//
	SetDCPenColor( hdc, RGB(0,0,0) );
	int i;
	for( i=0; i<8; i++ )
	{
		switch( i )
		{
		case 0: SetDCBrushColor( hdc, g_rwCfg.ui.colors.npcColor ); break;
		case 1: SetDCBrushColor( hdc, g_rwCfg.ui.colors.mobColor ); break;
		case 2: SetDCBrushColor( hdc, g_rwCfg.ui.colors.pcColor ); break;
		case 3: SetDCBrushColor( hdc, g_rwCfg.ui.colors.pcColorDead ); break;
		case 4: SetDCBrushColor( hdc, g_rwCfg.ui.colors.pcColorParty ); break;
		case 5: SetDCBrushColor( hdc, g_rwCfg.ui.colors.pcColorPartyDead ); break;
		case 6: SetDCBrushColor( hdc, g_rwCfg.ui.colors.pcColorWar ); break;
		case 7: SetDCBrushColor( hdc, g_rwCfg.ui.colors.pcColorWarDead ); break;
		}
		Rectangle( hdc, 240, 5 + i*21, 260, 5 + i*21+15 );
	}
	//
	SelectObject( hdc, (HGDIOBJ)hBrushPrev );
	SelectObject( hdc, (HGDIOBJ)hPenPrev );
	EndPaint( hDlg, &ps );
}

COLORREF Colors_MyChooseColor( HWND hDlg, COLORREF in )
{
	COLORREF init = in;
	COLORREF custColors[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
	CHOOSECOLOR cc;
	memset( &cc, 0, sizeof(cc) );
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hDlg;
	cc.rgbResult = in;
	cc.lpCustColors = custColors;
	cc.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	if( ChooseColor( &cc ) ) init = cc.rgbResult;
	return init;
}

void Colors_OnSelColor( HWND hDlg, int idx )
{
	switch( idx )
	{
	case 0: g_rwCfg.ui.colors.npcColor = Colors_MyChooseColor( hDlg, g_rwCfg.ui.colors.npcColor ); break;
	case 1: g_rwCfg.ui.colors.mobColor = Colors_MyChooseColor( hDlg, g_rwCfg.ui.colors.mobColor ); break;
	case 2: g_rwCfg.ui.colors.pcColor = Colors_MyChooseColor( hDlg, g_rwCfg.ui.colors.pcColor ); break;
	case 3: g_rwCfg.ui.colors.pcColorDead = Colors_MyChooseColor( hDlg, g_rwCfg.ui.colors.pcColorDead ); break;
	case 4: g_rwCfg.ui.colors.pcColorParty = Colors_MyChooseColor( hDlg, g_rwCfg.ui.colors.pcColorParty ); break;
	case 5: g_rwCfg.ui.colors.pcColorPartyDead = Colors_MyChooseColor( hDlg, g_rwCfg.ui.colors.pcColorPartyDead ); break;
	case 6: g_rwCfg.ui.colors.pcColorWar = Colors_MyChooseColor( hDlg, g_rwCfg.ui.colors.pcColorWar ); break;
	case 7: g_rwCfg.ui.colors.pcColorWarDead = Colors_MyChooseColor( hDlg, g_rwCfg.ui.colors.pcColorWarDead ); break;
	}
	RedrawWindow( hDlg, NULL, NULL, RDW_INVALIDATE );
}

