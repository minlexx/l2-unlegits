#include "stdafx.h"
#include "Resource.h"
#include "IL2Client.h"
#include "MapWnd.h"
#include "GlobalMapImages.h"
#include "logger.h"
#include "DBLayer.h"

extern GlobalMapImages g_mapImg;

WNDPROC OriginalMapStaticProc = NULL;

LRESULT CALLBACK NewMapStaticProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

//const int MapWindowBrushes = 1;
const int MapWindowPens = 8;
const int MapWindowFonts = 1;

struct MapWindowStruct
{
	IL2Client *pcl; // L2Client interface
	HDC hdcc; // compatible DC
	HBITMAP hbm; // compatible bitmap
	// gdi
	HPEN pens[MapWindowPens];
	HFONT fonts[MapWindowFonts];
	// map image
	HDC hdcImage; // NULL if not loaded
	int geo_x;
	int geo_y;
	int geo_img_w;
	int geo_img_h;
};

void Map_struct_create( MapWindowStruct *st )
{
	HPEN hpenMob   = CreatePen( PS_SOLID, 1, RGB(255,0,0) );
	HPEN hpenParty = CreatePen( PS_SOLID, 1, RGB(0,255,0) );
	HPEN hpenPc    = CreatePen( PS_SOLID, 1, RGB(0,0,255) );
	HPEN hpenNpc   = CreatePen( PS_SOLID, 1, RGB(255,255,0) );
	HPEN hpenMoveVector = CreatePen( PS_SOLID, 1, RGB(60,60,60) );
	HPEN hpenItem  = CreatePen( PS_SOLID, 1, RGB(255,128,64) );
	HPEN hpenTarget = CreatePen( PS_SOLID, 1, RGB(255,0,255) );
	HPEN hpenDeadMob = CreatePen( PS_SOLID, 1, RGB(128,128,128) );
	//
	st->pens[0] = hpenMob;
	st->pens[1] = hpenParty;
	st->pens[2] = hpenPc;
	st->pens[3] = hpenNpc;
	st->pens[4] = hpenMoveVector;
	st->pens[5] = hpenItem;
	st->pens[6] = hpenTarget;
	st->pens[7] = hpenDeadMob;
	//
	//st->fonts[0] = CreateFont( 11, 5, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
	st->fonts[0] = CreateFont( 14, 5, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		/*DEFAULT_QUALITY,*/ ANTIALIASED_QUALITY,
		FF_DONTCARE, TEXT("Arial") );
	// map image
	st->hdcImage = NULL;
	st->geo_x = 0;
	st->geo_y = 0;
	st->geo_img_w = 0;
	st->geo_img_h = 0;
}

void Map_struct_delete( MapWindowStruct *st )
{
	int i;
	for( i=0; i<MapWindowPens; i++ ) DeleteObject( st->pens[i] );
	for( i=0; i<MapWindowFonts; i++ ) DeleteObject( st->fonts[i] );
	DeleteDC( st->hdcc );
	DeleteObject( (HGDIOBJ)st->hbm );
	// map image
	st->hdcImage = NULL;
	// free all struct mem
	HeapFree( GetProcessHeap(), 0, st );
}

void Map_struct_check_geo_square_and_preload_image( MapWindowStruct *st, int usr_x, int usr_y );

void SubclassMapWindowProc( HWND hWnd, IL2Client *pcl )
{
	if( OriginalMapStaticProc == NULL )
	{
		OriginalMapStaticProc = (WNDPROC)(LPVOID)GetWindowLongPtr( hWnd, GWLP_WNDPROC );
		//log_error( LOG_OK, "SubclassMapWindowProc: saved old, userdata: 0x%08X\n", GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
	}
	//
	RECT rc;
	GetClientRect( hWnd, &rc );
	//
	MapWindowStruct *st = (MapWindowStruct *)HeapAlloc( GetProcessHeap(), 0, sizeof(MapWindowStruct) );
	st->pcl = pcl;
	st->hdcc = NULL;
	st->hbm = NULL;
	Map_struct_create( st );
	//
	SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)st );
	SetWindowLongPtr( hWnd, GWLP_WNDPROC, (LONG_PTR)(LPVOID)NewMapStaticProc );
}

void NewMapStatic_OnPaint( HWND hWnd );
void NewMapStatic_OnMouseMove( HWND hWnd, WPARAM wParam, LPARAM lParam );
void NewMapStatic_OnLButtonUp( HWND hWnd, WPARAM wParam, LPARAM lParam );
void NewMapStatic_ReloadImage( HWND hWnd );

LRESULT CALLBACK NewMapStaticProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT ret = 0;
	switch( uMsg )
	{
	case WM_PAINT: NewMapStatic_OnPaint( hWnd ); break;
	case WM_LBUTTONDOWN: NewMapStatic_OnLButtonUp( hWnd, wParam, lParam ); break;
	case WM_DESTROY:
		{
			MapWindowStruct *st = (MapWindowStruct *)GetWindowLongPtr( hWnd, GWLP_USERDATA );
			Map_struct_delete( st );
			ret = CallWindowProc( OriginalMapStaticProc, hWnd, uMsg, wParam, lParam );
		} break;
	case WM_USER: NewMapStatic_ReloadImage( hWnd ); break;
	default: ret = CallWindowProc( OriginalMapStaticProc, hWnd, uMsg, wParam, lParam ); break;
	}
	return ret;
}

void NewMapStatic_OnPaint( HWND hWnd )
{
	//log_error_np( LOG_OK, "WM_PAINT\n" );
	//
	RECT rc;
	GetClientRect( hWnd, &rc );
	int w = rc.right - rc.left;
	int h = rc.bottom - rc.top;
	int cx = w/2;
	int cy = h/2;
	int min_size = h;
	if( w < min_size ) min_size = w;
	MapWindowStruct *st = (MapWindowStruct *)GetWindowLongPtr( hWnd, GWLP_USERDATA );
	// init hdc?
	if( st->hdcc == NULL )
	{
		HDC hdc = GetDC( hWnd ); // get window hdc
		st->hdcc = CreateCompatibleDC( hdc );
		st->hbm = CreateCompatibleBitmap( hdc, w, h );
		SelectObject( st->hdcc, (HGDIOBJ)st->hbm );
		SelectObject( st->hdcc, GetStockObject( WHITE_PEN ) );
		SelectObject( st->hdcc, GetStockObject( BLACK_BRUSH ) );
		SelectObject( st->hdcc, st->fonts[0] );
		SetStretchBltMode( st->hdcc, HALFTONE ); // TODO: check StretchBlt mode
		// release window hdc
		ReleaseDC( hWnd, hdc );
		//log_error( LOG_OK, "NewMapStaticProc(): init %d x %d\n", w, h );
		st->hdcImage = NULL;
	}
	// init
	const int map_text_h = 13;
	const double max_range = 20000.0; // максимальный ДИАМЕТР (а НЕ радиус) обзора
	// scale - к-во координат Л2, "влезающих" в окно. масштаб меняется юзером
	double scale = max_range * (double)st->pcl->get_MapScale() / 100.0;
	// scale_k во сколько раз Л2 расстояния больше расстояний на окне
	double scale_k = scale / (double)min_size;
	unsigned int nDisplayed = 0;
	unsigned int i;
	int map_dx = 0, map_dy = 0, map_dz = 0;
	UserInfo *pusr = st->pcl->get_UserInfo();
	int usr_x = pusr->x;
	int usr_y = pusr->y;
	int usr_z = pusr->z;
	int usr_xDst = pusr->xDst;
	int usr_yDst = pusr->yDst;
	int screen_dx = 0, screen_dy = 0;
	int draw_x = 0, draw_y = 0;
	int sq_w = 4;
	// map show text params
	st_MapShowParams map_show;
	st->pcl->getMapShowParams( &map_show );

	//
	//  all drawing below
	//
	// save old gdi objects
	//HBRUSH oldBrush = (HBRUSH)SelectObject( st->hdcc, GetStockObject( NULL_BRUSH ) );
	//HPEN oldPen = (HPEN)SelectObject( st->hdcc, GetStockObject( WHITE_PEN ) );
	//HFONT oldFont = (HFONT)SelectObject( st->hdcc, GetStockObject( ANSI_VAR_FONT ) );
	//
	SelectObject( st->hdcc, GetStockObject( BLACK_PEN ) );
	SelectObject( st->hdcc, GetStockObject( BLACK_BRUSH ) );
	SetBkMode( st->hdcc, TRANSPARENT );
	SetTextColor( st->hdcc, RGB(255,255,255) );

	// map background:
	Rectangle( st->hdcc, 0, 0, w, h ); // black rect
	// if map show enabled, show
	if( map_show.realmap ) // real map
	{
		Map_struct_check_geo_square_and_preload_image( st, usr_x, usr_y );
		if( st->hdcImage && (st->geo_x>0) && (st->geo_y>0) )
		{
			// preloaded into surface, BitBlt/StretchBlt
			// scale - к-во координат Л2, "влезающих" в окно. масштаб меняется юзером
			// 33333 - сколько Л2 пикселей влезает по ширине в картинку геодаты
			// geoimg_scale - во сколько раз Л2 координата больше чем координата на гео картинке
			double geoimg_scale = ((double)GlobalMapImages::GEO_BOX_L2_SIZE) / ((double)st->geo_img_w);
			// screen_geoimg_w - ширина (и высота) участка, "вырезаемого" для масштабирования и отображения в окно
			//    из картинки квадрата геодаты (длина видимого участка местности в окне при текущем масштабе отображения).
			//    в координатах на картинке гео квадрата
			int screen_geoimg_w = (int)( (scale / geoimg_scale) + 0.5 );
			int geo_square_l2_x = 0; // координаты левого верхнего угла геодаты в Л2 мире
			int geo_square_l2_y = 0;
			GlobalMapImages::GetGeoSquareStartL2Coords( st->geo_x, st->geo_y, &geo_square_l2_x, &geo_square_l2_y );
			// смещение координат юзера относительно левого верхнего угла квадрата геодаты в Л2 координатах
			int geo_square_l2_user_dx = usr_x - geo_square_l2_x;
			int geo_square_l2_user_dy = usr_y - geo_square_l2_y;
			int geoimg_offset_x = (int)( ((double)geo_square_l2_user_dx - scale/2 ) / geoimg_scale + 0.5 );
			int geoimg_offset_y = (int)( ((double)geo_square_l2_user_dy - scale/2 ) / geoimg_scale + 0.5 );
			//
			StretchBlt( st->hdcc,
				0, 0, min_size, min_size,
				st->hdcImage, 
				geoimg_offset_x, geoimg_offset_y, screen_geoimg_w, screen_geoimg_w,
				SRCCOPY );
		}
	}

	//
	// display all npcs
	//
	unsigned int count = 0;
	NpcArray *pnpcs = st->pcl->get_WorldNpcs();
	if( pnpcs->getCount() > 0 )
	{
		SelectObject( st->hdcc, st->pens[3] );
		nDisplayed = 0;
		pnpcs->Lock();
		count = pnpcs->getCount();
		for( i=0; i<NpcArray::NPCA_MAX_NPCS; i++ )
		{
			if( pnpcs->npcs_array[i]->isUnused() ) continue;
			nDisplayed++;
			//
			map_dx = pnpcs->npcs_array[i]->x - usr_x;
			map_dy = pnpcs->npcs_array[i]->y - usr_y;
			map_dz = pnpcs->npcs_array[i]->z - usr_z;
			if( abs( map_dz ) >= map_show.zrange )
			{
				log_error( LOG_DEBUG, "Denied display npc [%S] because map_dz is %d (zrange %d)\n",
					pnpcs->npcs_array[i]->charName, map_dz, map_show.zrange );
				continue; // not fit in zrange
			}
			//
			screen_dx = (int)( (double)map_dx / scale_k );
			screen_dy = (int)( (double)map_dy / scale_k );
			//
			SelectObject( st->hdcc, st->pens[3] ); // npc pen
			if( pnpcs->npcs_array[i]->isAttackable ) SelectObject( st->hdcc, st->pens[0] ); // mob pen
			if( pnpcs->npcs_array[i]->isAlikeDead ) SelectObject( st->hdcc, st->pens[7] ); // dead pen
			if( pnpcs->npcs_array[i]->objectID == pusr->targetObjectID ) SelectObject( st->hdcc, st->pens[6] ); // target pen
			draw_x = cx + screen_dx;
			draw_y = cy + screen_dy;
			Rectangle( st->hdcc, draw_x - 1, draw_y - 1, draw_x + sq_w, draw_y + sq_w );
			// vector?
			if( map_show.vectors )
			{
				if( pnpcs->npcs_array[i]->isMoving() )
				{
					int map_vector_dx = pnpcs->npcs_array[i]->xDst - pnpcs->npcs_array[i]->x;
					int map_vector_dy = pnpcs->npcs_array[i]->yDst - pnpcs->npcs_array[i]->y;
					int screen_vector_dx = (int)( (double)map_vector_dx / scale_k );
					int screen_vector_dy = (int)( (double)map_vector_dy / scale_k );
					SelectObject( st->hdcc, st->pens[4] );
					MoveToEx( st->hdcc, draw_x+1, draw_y+1, NULL );
					LineTo( st->hdcc, draw_x+1+screen_vector_dx, draw_y+1+screen_vector_dy );
				}
			}
			// npc name?
			bool show_text = false;
			if( (pnpcs->npcs_array[i]->isAttackable) && map_show.mobs ) show_text = true;
			if( (!pnpcs->npcs_array[i]->isAttackable) && map_show.npcs ) show_text = true;
			if( show_text )
			{
				wchar_t *text = pnpcs->npcs_array[i]->charName;
				//TextOutW( st->hdcc, draw_x, draw_y, text, wcslen( text ) );
				rc.left = draw_x - 100;
				rc.right = draw_x + 100;
				rc.bottom = draw_y;
				rc.top = rc.bottom - map_text_h;
				DrawText( st->hdcc, text, wcslen(text), &rc, DT_CENTER | DT_VCENTER );
			}
			//
			if( nDisplayed >= count ) break;
		}
		pnpcs->Unlock();
	}
	//
	// display all items
	//
	GIArray *pitems = st->pcl->get_WorldItems();
	if( pitems->getCount() > 0 )
	{
		nDisplayed = 0;
		SelectObject( st->hdcc, st->pens[5] );
		pitems->Lock();
		count = pitems->getCount();
		for( i=0; i<GIArray::GA_MAX_ITEMS; i++ )
		{
			if( pitems->gi_array[i]->isUnused() ) continue;
			nDisplayed++;
			//
			map_dx = pitems->gi_array[i]->x - usr_x;
			map_dy = pitems->gi_array[i]->y - usr_y;
			map_dz = pitems->gi_array[i]->z - usr_z;
			if( abs( map_dz ) >= map_show.zrange ) continue; // not fit in zrange
			//
			screen_dx = (int)( (double)map_dx / scale_k );
			screen_dy = (int)( (double)map_dy / scale_k );
			//
			draw_x = cx + screen_dx;
			draw_y = cy + screen_dy;
			Rectangle( st->hdcc, draw_x - 1, draw_y - 1, draw_x + sq_w, draw_y + sq_w );
			// show name?
			if( map_show.items )
			{
				wchar_t text[256] = {0};
				// itemName is no longer part og GroundItem
				//if( pitems->gi_array[i]->itemName[0] )
				//	MultiByteToWideChar( CP_ACP, 0, pitems->gi_array[i]->itemName, -1, text, 255 );
				// use cached DB layer instead
				DBLayer_getItemName( pitems->gi_array[i]->itemID, text, 255 );
				if( text[0] )
				{
					rc.left = draw_x - 100;
					rc.right = draw_x + 100;
					rc.bottom = draw_y;
					rc.top = rc.bottom - map_text_h;
					DrawText( st->hdcc, text, wcslen(text), &rc, DT_CENTER | DT_VCENTER );
				}
			}
			//
			if( nDisplayed >= count ) break;
		}
		pitems->Unlock();
	}
	//
	//
	// display all players
	//
	UserParty *pparty = st->pcl->get_Party();
	CharArray *pchars = st->pcl->get_WorldChars();
	if( pchars->GetCount() > 0 )
	{
		nDisplayed = 0;
		SelectObject( st->hdcc, st->pens[2] ); // pc
		pchars->Lock();
		count = pchars->GetCount();
		for( i=0; i<CharArray::CHARARRAY_MAX_CHARS; i++ )
		{
			if( pchars->chars_array[i]->isUnused() ) continue;
			nDisplayed++;
			//
			map_dx = pchars->chars_array[i]->x - usr_x;
			map_dy = pchars->chars_array[i]->y - usr_y;
			map_dz = pchars->chars_array[i]->z - usr_z;
			if( abs( map_dz ) >= map_show.zrange ) continue; // not fit in zrange
			//
			screen_dx = (int)( (double)map_dx / scale_k );
			screen_dy = (int)( (double)map_dy / scale_k );
			//
			draw_x = cx + screen_dx;
			draw_y = cy + screen_dy;
			SelectObject( st->hdcc, st->pens[2] ); // always select pc pen beore draw
			if( pparty->isInParty( pchars->chars_array[i]->objectID, NULL ) ) SelectObject( st->hdcc, st->pens[1] ); // party
			if( pchars->chars_array[i]->objectID == pusr->targetObjectID ) SelectObject( st->hdcc, st->pens[6] ); // target
			Rectangle( st->hdcc, draw_x - 1, draw_y - 1, draw_x + sq_w, draw_y + sq_w );
			// vector?
			if( map_show.vectors )
			{
				if( pchars->chars_array[i]->isMoving() )
				{
					int map_vector_dx = pchars->chars_array[i]->xDst - pchars->chars_array[i]->x;
					int map_vector_dy = pchars->chars_array[i]->yDst - pchars->chars_array[i]->y;
					int screen_vector_dx = (int)( (double)map_vector_dx / scale_k );
					int screen_vector_dy = (int)( (double)map_vector_dy / scale_k );
					SelectObject( st->hdcc, st->pens[4] );
					MoveToEx( st->hdcc, draw_x+1, draw_y+1, NULL );
					LineTo( st->hdcc, draw_x+1+screen_vector_dx, draw_y+1+screen_vector_dy );
				}
			}
			// show name?
			if( map_show.pcs || (pchars->chars_array[i]->privateStoreType > 0) )
			{
				wchar_t *text = pchars->chars_array[i]->charName;
				//TextOutW( st->hdcc, draw_x, draw_y, text, wcslen( text ) );
				rc.left = draw_x - 100;
				rc.right = draw_x + 100;
				rc.bottom = draw_y;
				rc.top = rc.bottom - map_text_h;
				if( map_show.pcs ) DrawText( st->hdcc, text, wcslen(text), &rc, DT_CENTER | DT_VCENTER );
				if( pchars->chars_array[i]->privateStoreType > 0 )
				{
					wchar_t *storemsg = NULL;
					COLORREF textColor = RGB(255,255,255);
					switch( pchars->chars_array[i]->privateStoreType )
					{
					case L2Player::PRIVATE_STORE_BUY:
						{
							if( map_show.buyshop )
							{
								storemsg = pchars->chars_array[i]->privateStoreMsgBuy;
								textColor = RGB(255,255,151);
							}
						} break;
					case L2Player::PRIVATE_STORE_SELL:
						{
							if( map_show.sellshop )
							{
								storemsg = pchars->chars_array[i]->privateStoreMsgSell;
								textColor = RGB(255,108,255);
							}
						} break;
					case L2Player::PRIVATE_STORE_CRAFT:
						{
							if( map_show.craftshop )
							{
								storemsg = pchars->chars_array[i]->privateStoreMsgRecipe;
								textColor = RGB(250,204,46);
							}
						} break;
					case L2Player::PRIVATE_STORE_PACKAGESALE:
						{
							if( map_show.sellshop )
							{
								storemsg = pchars->chars_array[i]->privateStoreMsgSell;
								textColor = RGB(253,80,43);
							}
						} break;
					}
					if( storemsg )
					{
						rc.top -= (map_text_h+1);
						rc.bottom -= (map_text_h+1);
						SetTextColor( st->hdcc, textColor );
						DrawText( st->hdcc, storemsg, wcslen(storemsg), &rc, DT_CENTER | DT_VCENTER );
						SetTextColor( st->hdcc, RGB(255,255,255) );
					}
				} // privateStoreType > 0
			} // show_pcs
			//
			if( nDisplayed >= count ) break;
		}
		pchars->Unlock();
	}
	//
	// display self in center
	//
	SelectObject( st->hdcc, GetStockObject( WHITE_PEN ) );
	Rectangle( st->hdcc, cx-1, cy-1, cx+sq_w, cy+sq_w );
	// user name?
	if( map_show.pcs || (pusr->privateStoreType > 0) )
	{
		wchar_t *text = pusr->charName;
		rc.left = cx - 100;
		rc.right = cx + 100;
		rc.bottom = cy;
		rc.top = rc.bottom - map_text_h;
		if( map_show.pcs ) DrawText( st->hdcc, text, wcslen(text), &rc, DT_CENTER | DT_VCENTER );
		// self shop message?
		if( pusr->privateStoreType > 0 )
		{
			wchar_t *storemsg = NULL;
			COLORREF textColor = RGB(255,255,255);
			switch( pusr->privateStoreType )
			{
			case L2Player::PRIVATE_STORE_BUY:
				{
					if( map_show.buyshop )
					{
						storemsg = pusr->privateStoreMsgBuy;
						textColor = RGB(255,255,151);
					}
				} break;
			case L2Player::PRIVATE_STORE_SELL:
				{
					if( map_show.sellshop )
					{
						storemsg = pusr->privateStoreMsgSell;
						textColor = RGB(255,108,255);
					}
				} break;
			case L2Player::PRIVATE_STORE_CRAFT:
				{
					if( map_show.craftshop )
					{
						storemsg = pusr->privateStoreMsgRecipe;
						textColor = RGB(250,204,46);
					}
				} break;
			case L2Player::PRIVATE_STORE_PACKAGESALE:
				{
					if( map_show.sellshop )
					{
						storemsg = pusr->privateStoreMsgSell;
						textColor = RGB(253,80,43);
					}
				} break;
			}
			if( storemsg )
			{
				rc.top -= (map_text_h+1);
				rc.bottom -= (map_text_h+1);
				SetTextColor( st->hdcc, textColor );
				DrawText( st->hdcc, storemsg, wcslen(storemsg), &rc, DT_CENTER | DT_VCENTER );
				SetTextColor( st->hdcc, RGB(255,255,255) );
			}
		} // privateStoreType > 0
	} // show_pcs
	// user move vector?
	if( map_show.vectors )
	{
		if( pusr->isMoving() )
		{
			int map_vector_dx = usr_xDst - usr_x;
			int map_vector_dy = usr_yDst - usr_y;
			int screen_vector_dx = (int)( (double)map_vector_dx / scale_k );
			int screen_vector_dy = (int)( (double)map_vector_dy / scale_k );
			SelectObject( st->hdcc, st->pens[4] );
			MoveToEx( st->hdcc, cx+1, cy+1, NULL );
			LineTo( st->hdcc, cx+1+screen_vector_dx, cy+1+screen_vector_dy );
		}
	}
	// select old GDI objects
	//SelectObject( st->hdcc, oldPen );
	//SelectObject( st->hdcc, oldBrush );
	// copy surface
	//PAINTSTRUCT ps;
	//HDC hdc = BeginPaint( hWnd, &ps );
	HDC hdc = GetDC( hWnd );
	BitBlt( hdc, 0, 0, w, h, st->hdcc, 0, 0, SRCCOPY );
	//EndPaint( hWnd, &ps );
	ReleaseDC( hWnd, hdc );
	ValidateRect( hWnd, NULL );
}

bool Map_isInTargetRect( int mx, int my, int object_x, int object_y )
{
	const int d = 40;
	int l = mx - d/4;
	int r = mx + d;
	int t = my - d/4;
	int b = my + d;
	if( (object_x>=l) && (object_x<=r) && (object_y>=t) && (object_y<=b) ) return true;
	return false;
}

void NewMapStatic_OnLButtonUp( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(wParam);
	//log_error_np( LOG_OK, "WM_LBUTTONUP\n" );
	// message params
	int mx = GET_X_LPARAM(lParam);
	int my = GET_Y_LPARAM(lParam);
	// window dimensions
	RECT rc;
	GetClientRect( hWnd, &rc );
	int w = rc.right - rc.left;
	int h = rc.bottom - rc.top;
	int cx = w/2;
	int cy = h/2;
	int min_size = h;
	if( w < min_size ) min_size = w;
	MapWindowStruct *st = (MapWindowStruct *)GetWindowLongPtr( hWnd, GWLP_USERDATA );
	// init default scale state
	const double max_range = 20000.0;
	double scale = max_range * (double)st->pcl->get_MapScale() / 100.0;
	double scale_k = scale / (double)min_size;
	unsigned int nPassed = 0;
	unsigned int i = 0;
	unsigned int count;
	int map_dx = 0, map_dy = 0; // map_dz = 0;
	UserInfo *pusr = st->pcl->get_UserInfo();
	int usr_x = pusr->x;
	int usr_y = pusr->y;
	int usr_z = pusr->z;
	//int usr_xDst = pusr->xDst;
	//int usr_yDst = pusr->yDst;
	int screen_dx = 0, screen_dy = 0;
	//int draw_x = 0, draw_y = 0;
	//int sq_w = 4;
	// map show text params
	st_MapShowParams map_show;
	st->pcl->getMapShowParams( &map_show );
	//
	// user clicked mouse in coords mx, my relative to 0,0 of window client area
	// but we know only user's L2 map coords
	// to get real L2 map coords of user click, we must calc click delta from user coords
	//
	screen_dx = mx - cx;  // if mx > cx, user moves east
	screen_dy = my - cy;  // if my > cy, user moves south
	map_dx = (int)( screen_dx * scale_k );
	map_dy = (int)( screen_dy * scale_k );
	int map_x = usr_x + map_dx;
	int map_y = usr_y + map_dy;
	//log_error( LOG_OK, "m(%d,%d) screen(%d,%d) map(%d,%d)\n", mx, my, screen_dx, screen_dy, map_dx, map_dy );

	// maybe target self?
	if( (screen_dx>-5) && (screen_dx<5) && (screen_dy>-5) && (screen_dy<5) )
	{
		st->pcl->game_Action( pusr->objectID );
		return;
	}

	// first try to target players
	nPassed = 0;
	CharArray *ca = st->pcl->get_WorldChars();
	count = ca->GetCount();
	ca->Lock();
	for( i=0; i<count; i++ )
	{
		if( ca->chars_array[i]->isUnused() ) continue;
		nPassed++;
		// Z-range limit (if this char is not be displayed on map, it cannot be targeted)
		if( abs( ca->chars_array[i]->z - usr_z ) > map_show.zrange ) continue;
		// hit target coords?
		if( Map_isInTargetRect( map_x, map_y, ca->chars_array[i]->x, ca->chars_array[i]->y ) )
		{
			ca->Unlock();
			st->pcl->game_Action( ca->chars_array[i]->objectID );
			return;
		}
		if( nPassed >= count ) break;
	}
	ca->Unlock();
	ca = NULL;
	// if we here, PC not targeted

	// try to target NPC / mob
	nPassed = 0;
	NpcArray *na = st->pcl->get_WorldNpcs();
	count = na->getCount();
	na->Lock();
	for( i=0; i<count; i++ )
	{
		if( na->npcs_array[i]->isUnused() ) continue;
		nPassed++;
		// Z-range limit (if this npc is not be displayed on map, it cannot be targeted)
		if( abs( na->npcs_array[i]->z - usr_z ) > map_show.zrange ) continue;
		// hit target coords?
		if( Map_isInTargetRect( map_x, map_y, na->npcs_array[i]->x, na->npcs_array[i]->y ) )
		{
			na->Unlock();
			st->pcl->game_Action( na->npcs_array[i]->objectID );
			return;
		}
		if( nPassed >= count ) break;
	}
	na->Unlock();
	na = NULL;
	// if we here, no PC/NPC/Mob targeted :(

	// maybe try to pick up item?
	nPassed = 0;
	GIArray *ga = st->pcl->get_WorldItems();
	count = ga->getCount();
	ga->Lock();
	for( i=0; i<count; i++ )
	{
		if( ga->gi_array[i]->isUnused() ) continue;
		nPassed++;
		// Z-range limit (if this char is not be displayed on map, it cannot be targeted)
		if( abs( ga->gi_array[i]->z - usr_z ) > map_show.zrange ) continue;
		// hit target coords?
		if( Map_isInTargetRect( map_x, map_y, ga->gi_array[i]->x, ga->gi_array[i]->y ) )
		{
			ga->Unlock();
			st->pcl->game_Action( ga->gi_array[i]->objectID );
			return;
		}
		if( nPassed >= count ) break;
	}
	ga->Unlock();
	ga = NULL;

	// nothing targeted, move ? :)
	st->pcl->game_MoveToDelta( map_dx, map_dy );
}

void Map_struct_check_geo_square_and_preload_image( MapWindowStruct *st, int usr_x, int usr_y )
{
	int cur_geo_x = 0, cur_geo_y = 0;
	GlobalMapImages::GetGeoMapSquareXYByL2Coords( usr_x, usr_y, &cur_geo_x, &cur_geo_y );
	if( (st->geo_x != cur_geo_x) || (st->geo_y != cur_geo_y) )
	{
		//
		log_error_np( LOG_OK, "Geo: square changed: %d,%d -> %d,%d\n",
			st->geo_x, st->geo_y, cur_geo_x, cur_geo_y );
		// geodata square changed! change coords
		st->geo_x = cur_geo_x;
		st->geo_y = cur_geo_y;
		// NULL current vars
		st->hdcImage = NULL;
		st->geo_img_w = 0;
		st->geo_img_h = 0;
		// load new
		g_mapImg.preloadSquare( cur_geo_x, cur_geo_y );
		if( g_mapImg.isSquareLoaded( cur_geo_x, cur_geo_y ) )
		{
			st->hdcImage = g_mapImg.getSquareHDC( cur_geo_x, cur_geo_y );
			g_mapImg.getSquareWH( cur_geo_x, cur_geo_y, st->geo_img_w, st->geo_img_h );
			log_error_np( LOG_OK, "Geo: preloaded geo square [%d_%d] img size [%d x %d]\n",
				cur_geo_x, cur_geo_y, st->geo_img_w, st->geo_img_h );
		}
		else
		{
			log_error( LOG_WARNING, "Geo: cannot preload geo square [%d_%d]\n", cur_geo_x, cur_geo_y );
			/*char aCurDir[256];
			GetCurrentDirectoryA( 200, aCurDir );
			log_error( LOG_WARNING, "Geo: curDir [%s]\n", aCurDir );*/
		}
	}
}

void NewMapStatic_ReloadImage( HWND hWnd )
{
	MapWindowStruct *st = (MapWindowStruct *)GetWindowLongPtr( hWnd, GWLP_USERDATA );
	// reload
	log_error_np( LOG_OK, "MapWindow: reload square: %d,%d\n", st->geo_x, st->geo_y );
	// NULL current vars
	st->hdcImage = NULL;
	st->geo_img_w = 0;
	st->geo_img_h = 0;
	// load new
	g_mapImg.preloadSquare( st->geo_x, st->geo_y );
	if( g_mapImg.isSquareLoaded( st->geo_x, st->geo_y ) )
	{
		st->hdcImage = g_mapImg.getSquareHDC( st->geo_x, st->geo_y );
		g_mapImg.getSquareWH( st->geo_x, st->geo_y, st->geo_img_w, st->geo_img_h );
		log_error_np( LOG_OK, "MapWindow: reloaded geo square [%d_%d] img size [%d x %d]\n",
			st->geo_x, st->geo_y, st->geo_img_w, st->geo_img_h );
	}
	else
	{
		log_error( LOG_WARNING, "MapWindow: cannot reload geo square [%d_%d]\n", st->geo_x, st->geo_y );
	}
}
