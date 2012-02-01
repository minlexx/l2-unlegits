#include "stdafx.h"
#include "Resource.h"
#include "logger.h"
#include "GlobalMapImages.h"
#include "PngJpegImage.h"

GlobalMapImages::GlobalMapImages()
{
	InitializeCriticalSection( &m_cs );
	EnterCriticalSection( &m_cs );
	int i, j;
	for( i=0; i<GEO_W; i++ )
	{
		for( j=0; j<GEO_H; j++ )
		{
			m_geo_hdc[i][j] = NULL;
			m_width[i][j] = m_height[i][j] = 0;
		}
	}
	m_nLoadedCount = 0;
	m_mapsDir[0] = 0;
	LeaveCriticalSection( &m_cs );
}

GlobalMapImages::~GlobalMapImages()
{
	unloadAllSquares();
	DeleteCriticalSection( &m_cs );
}

void GlobalMapImages::setMapsDir( const char *dir )
{
	strcpy( m_mapsDir, dir );
}

bool GlobalMapImages::isValidGeoXY( int geo_x, int geo_y )
{
	if( (geo_x>=GEO_X_MIN) && (geo_x<=GEO_X_MAX) &&
		(geo_y>=GEO_Y_MIN) && (geo_y<=GEO_Y_MAX) ) return true;
	return false;
}

bool GlobalMapImages::preloadSquare( int geo_x, int geo_y )
{
	Lock();
	if( !isValidGeoXY( geo_x, geo_y ) ) { UnLock(); return false; }
	if( isSquareLoaded( geo_x, geo_y ) ) { UnLock(); return false; }
	// check maps dir
	if( m_mapsDir[0] == 0 )
	{
		char aCurDir[256] = {0};
		GetCurrentDirectoryA( 255, aCurDir );
		strcat( aCurDir, "\\maps" );
		strcpy( m_mapsDir, aCurDir );
	}
	// first try to load image
	char filename[256];
	sprintf( filename, "%s\\%d_%d.jpg", m_mapsDir, geo_x, geo_y );
	PngJpegImage *img = new PngJpegImage();
	if( !img->readFile_JPEG( filename ) )
	{
		// try another file name
		sprintf( filename, "%s\\%d_%d_1.jpg", m_mapsDir, geo_x, geo_y );
		if( !img->readFile_JPEG( filename ) )
		{
			log_error( LOG_ERROR, "GlobalMapImages::preloadSquare( %d, %d ): could not readFile_JPEG( %s ): %u\n",
				geo_x, geo_y, filename, GetLastError() );
			UnLock();
			return false;
		}
		// here loaded XX_XX_1.jpg
	}
	// image size
	int w = 0, h = 0;
	img->getParams( &w, &h, NULL, NULL );
	//
	HWND hWnd = GetDesktopWindow();
	HDC hWindowDC = GetDC( hWnd );
	HDC hdc = CreateCompatibleDC( hWindowDC );
	if( !hdc )
	{
		log_error( LOG_ERROR, "GlobalMapImages::preloadSquare( %d, %d ): could not CreateCompatibleDC(): %u\n",
			geo_x, geo_y, GetLastError() );
		delete img;
		UnLock();
		return false;
	}
	HBITMAP hbm = CreateCompatibleBitmap( hWindowDC, w, h );
	ReleaseDC( hWnd, hWindowDC ); // now we don't need desktop window DC
	if( hbm == NULL )
	{
		log_error( LOG_ERROR, "GlobalMapImages::preloadSquare( %d, %d ): could not CreateCompatibleBitmap(): %u\n",
			geo_x, geo_y, GetLastError() );
		delete img;
		DeleteDC( hdc );
		UnLock();
		return false;
	}
	SelectObject( hdc, (HGDIOBJ)hbm );
	img->displayOn( hdc, 0, 0 ); // print image to HDC
	delete img;
	SetStretchBltMode( hdc, HALFTONE );
	geo_XY_to_array_index( geo_x, geo_y );
	m_geo_hdc[ geo_x ][ geo_y ] = hdc; // save loaded HDC in array
	m_width[ geo_x ][ geo_y ] = w;
	m_height[ geo_x ][ geo_y ] = h;
	m_nLoadedCount++; // inc loaded count
	UnLock();
	//log_error_np( LOG_OK, "[GlobalMap] loaded %d_%d\n", geo_x+GEO_X_MIN, geo_y+GEO_Y_MIN );
	return true;
}

bool GlobalMapImages::unloadSquare( int geo_x, int geo_y )
{
	Lock();
	if( !isSquareLoaded( geo_x, geo_y ) ) { UnLock(); return false; }
	geo_XY_to_array_index( geo_x, geo_y );
	// first get current DC bitmap
	HBITMAP hbm = (HBITMAP)GetCurrentObject( m_geo_hdc[geo_x][geo_y], OBJ_BITMAP );
	// then delete DC
	DeleteDC( m_geo_hdc[geo_x][geo_y] );
	m_geo_hdc[geo_x][geo_y] = NULL; // mark as not loaded
	m_width[ geo_x ][ geo_y ] = m_height[ geo_x ][ geo_y ] = 0;
	// then delete bitmap
	if( hbm ) DeleteObject( (HGDIOBJ)hbm );
	m_nLoadedCount--; // dec loaded count
	UnLock();
	return true;
}

void GlobalMapImages::preloadAllSquares()
{
	Lock();
	int i, j;
	for( i=GEO_X_MIN; i<=GEO_X_MAX; i++ )
	{
		for( j=GEO_Y_MIN; j<=GEO_Y_MAX; j++ )
		{
			preloadSquare( i, j );
		}
	}
	UnLock();
}

void GlobalMapImages::unloadAllSquares()
{
	Lock();
	int i, j;
	for( i=GEO_X_MIN; i<=GEO_X_MAX; i++ )
	{
		for( j=GEO_Y_MIN; j<=GEO_Y_MAX; j++ )
		{
			unloadSquare( i, j );
		}
	}
	UnLock();
}

bool GlobalMapImages::isSquareLoaded( int geo_x, int geo_y ) const
{
	if( !isValidGeoXY( geo_x, geo_y ) ) return false;
	geo_XY_to_array_index( geo_x, geo_y );
	if( m_geo_hdc[ geo_x ][ geo_y ] != NULL ) return true;
	return false;
}

HDC  GlobalMapImages::getSquareHDC( int geo_x, int geo_y ) const
{
	if( !isValidGeoXY( geo_x, geo_y ) ) return NULL;
	geo_XY_to_array_index( geo_x, geo_y );
	return m_geo_hdc[ geo_x ][ geo_y ];
}

void GlobalMapImages::getSquareWH( int geo_x, int geo_y, int& w, int &h ) const
{
	if( !isValidGeoXY( geo_x, geo_y ) ) return;
	geo_XY_to_array_index( geo_x, geo_y );
	w = m_width[ geo_x ][ geo_y ];
	h = m_height[ geo_x ][ geo_y ];
}


/*
protected:
	CRITICAL_SECTION m_cs;
	int m_nLoadedCount;
	HDC m_geo_hdc[GEO_W][GEO_H];
};
*/

void GlobalMapImages::GetGeoSquareStartL2Coords( int geo_x, int geo_y, int *l2_x, int *l2_y )
{
	const int geo_square_x_0 = 20;
	const int geo_square_y_0 = 18;
	if( (geo_x<16) || (geo_x>26) || (geo_y<10) || (geo_y>25) ) return;
	(*l2_x) = (geo_x - geo_square_x_0) * GlobalMapImages::GEO_BOX_L2_SIZE;
	(*l2_y) = (geo_y - geo_square_y_0) * GlobalMapImages::GEO_BOX_L2_SIZE;
}

void GlobalMapImages::GetGeoMapSquareXYByL2Coords( int l2_x, int l2_y, int *geo_x, int *geo_y )
{
	const int geo_square_x_0 = 20;
	const int geo_square_y_0 = 18;
	int nSquaresFrom_0_x = l2_x / GlobalMapImages::GEO_BOX_L2_SIZE;
	int nSquaresFrom_0_y = l2_y / GlobalMapImages::GEO_BOX_L2_SIZE;
	int geo_square_x = geo_square_x_0 + nSquaresFrom_0_x;
	int geo_square_y = geo_square_y_0 + nSquaresFrom_0_y;
	// fix negative offset coords
	if( l2_x < 0 ) geo_square_x--;
	if( l2_y < 0 ) geo_square_y--;
	//
	if( (geo_square_x < 16) || (geo_square_x > 26) || (geo_square_y < 10) || (geo_square_y > 25) )
	{
		log_error_np( LOG_ERROR, "[GlobalMap]: GetGeoMapSquareXYByL2Coords(): error overflow!\n" );
		return;
	}
	if( geo_x ) (*geo_x) = geo_square_x;
	if( geo_y ) (*geo_y) = geo_square_y;
}

