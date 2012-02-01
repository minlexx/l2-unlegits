#ifndef H_GLOBAL_MAP_IMAGES
#define H_GLOBAL_MAP_IMAGES

class GlobalMapImages
{
public:
	static const int GEO_X_MIN = 16;
	static const int GEO_Y_MIN = 10;
	static const int GEO_X_MAX = 26;
	static const int GEO_Y_MAX = 25;
	static const int GEO_W = 10;
	static const int GEO_H = 15;
	static const int GEO_BOX_L2_SIZE = 32760; // originally was 33333
public:
	GlobalMapImages();
	~GlobalMapImages();
public:
	void setMapsDir( const char *dir );
public:
	bool preloadSquare( int geo_x, int geo_y );
	void preloadAllSquares();
	bool unloadSquare( int geo_x, int geo_y );
	void unloadAllSquares();
	bool isSquareLoaded( int geo_x, int geo_y ) const;
	HDC  getSquareHDC( int geo_x, int geo_y ) const;
	void getSquareWH( int geo_x, int geo_y, int& w, int &h ) const;
public:
	static void GetGeoSquareStartL2Coords( int geo_x, int geo_y, int *l2_x, int *l2_y );
	static void GetGeoMapSquareXYByL2Coords( int l2_x, int l2_y, int *geo_x, int *geo_y );
public:
	int getLoadedCount() const { return m_nLoadedCount; }
protected:
	void Lock() { EnterCriticalSection( &m_cs ); }
	void UnLock() { LeaveCriticalSection( &m_cs ); }
protected:
	static bool isValidGeoXY( int geo_x, int geo_y );
	static void geo_XY_to_array_index( int& geo_x, int& geo_y ) { geo_x -= GEO_X_MIN; geo_y -= GEO_Y_MIN; }
protected:
	CRITICAL_SECTION m_cs;
	int m_nLoadedCount;
	HDC m_geo_hdc[GEO_W][GEO_H];
	int m_width[GEO_W][GEO_H];
	int m_height[GEO_W][GEO_H];
	char m_mapsDir[256];
};

#endif
