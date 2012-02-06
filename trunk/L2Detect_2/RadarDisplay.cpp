#include "pch.h"
#include "RadarDisplay.h"
#include "l2d.h"

RadarDisplay *RadarDisplay::s_instance = NULL;
int RadarDisplay::s_refCount = 0;

RadarDisplay *RadarDisplay::getInstance()
{
	if( RadarDisplay::s_instance == NULL )
	{
		RadarDisplay::s_instance = new RadarDisplay();
		RadarDisplay::s_refCount++;
	}
	return RadarDisplay::s_instance;
}

void RadarDisplay::freeInstance()
{
	if( RadarDisplay::s_instance )
	{
		RadarDisplay::s_refCount--;
		if( RadarDisplay::s_refCount == 0 )
		{
			delete RadarDisplay::s_instance;
			RadarDisplay::s_instance = NULL;
		}
	}
}

RadarDisplay::RadarDisplay()
{
	m_hWndL2 = NULL;
	// display opts
	m_ui_show_radar = m_ui_show_cur = false;
	// d3d9 resources
	m_res_loaded = false;
	m_radarTex = NULL;
	m_spr = NULL;
}

RadarDisplay::~RadarDisplay()
{
}

void RadarDisplay::toggleShowRadar()
{
	m_ui_show_radar = !m_ui_show_radar;
}

void RadarDisplay::toggleShowCur()
{
	m_ui_show_cur = !m_ui_show_cur;
}

void RadarDisplay::preload_resources( IDirect3DDevice9 *pdevice )
{
	// main radar texture
	if( !m_radarTex )
	{
		if( preload_texture_from_file( pdevice, &m_radarTex, L"transp_r.png" ) )
			log_error( LOG_OK, "RadarDisplay::preload_resources(): Loaded texture transp_r.png OK\n" );
	}
	if( !m_spr )
	{
		if( D3DXCreateSprite( pdevice, &m_spr ) != S_OK )
			log_error( LOG_ERROR, "D3DXCreateSprite() FAILED!\n" );
	}
	m_res_loaded = true;
}

void RadarDisplay::onD3D_Release( IDirect3DDevice9 *pdevice )
{
	UNREFERENCED_PARAMETER(pdevice);
	// release our resources
	if( m_spr )
	{
		m_spr->Release();
		m_spr = NULL;
	}
	if( m_radarTex )
	{
		m_radarTex->Release();
		m_radarTex = NULL;
	}
	m_res_loaded = false;
}

void RadarDisplay::onD3D_CreateDevice( HWND hWndFocus, IDirect3DDevice9 *pdevice )
{
	if( hWndFocus )
	{
		m_hWndL2 = hWndFocus;
		WINDOWINFO wi;
		memset( &wi, 0, sizeof(wi) );
		wi.cbSize = sizeof(WINDOWINFO);
		GetWindowInfo( m_hWndL2, &wi );
		LONG_PTR ptrWndProc = GetWindowLongPtr( m_hWndL2, GWLP_WNDPROC );
		TCHAR className[256] = {0};
		GetClassName( m_hWndL2, className, 255 );
		//
		log_error( LOG_DEBUG, "RadarDisplay::onD3D_CreateDevice(): got Lineage client window,\n"
			"     handle 0x%08X [class %S]\n"
			"     client rect (%d,%d)-(%d,%d) [%d x %d]; WndProc 0x%08X\n",
			(unsigned int)m_hWndL2, className,
			wi.rcClient.left, wi.rcClient.top, wi.rcClient.right, wi.rcClient.right,
			wi.rcClient.right - wi.rcClient.left, wi.rcClient.bottom - wi.rcClient.top,
			ptrWndProc );
		// hook wndproc
		InstallProxyWndProc( m_hWndL2 );
	}
	log_error( LOG_DEBUG, "RadarDisplay::onD3D_CreateDevice(): try to preload resources..\n" );
	preload_resources( pdevice );
	log_error( LOG_DEBUG, "RadarDisplay::onD3D_CreateDevice(): end.\n" );
}

void RadarDisplay::onD3D_Present( IDirect3DDevice9 *pdevice )
{
	if( m_res_loaded ) preload_resources( pdevice );
	m_spr->Begin( D3DXSPRITE_ALPHABLEND );
	// follow mouse
	if( m_ui_show_cur )
	{
		POINT pt;
		GetCursorPos( &pt );
		RECT rc;
		GetWindowRect( m_hWndL2, &rc );
		//
		D3DRECT rec;
		rec.x1 = pt.x - rc.left;
		rec.y1 = pt.y - rc.top;
		rec.x2 = rec.x1 + 50;
		rec.y2 = rec.y1 + 50;
		pdevice->Clear( 1, &rec, D3DCLEAR_TARGET, D3DCOLOR_ARGB(128,255,255,0), 0, 0 );
	}
	if( m_ui_show_radar)
	{
		RECT rc;
		GetClientRect( m_hWndL2, &rc );
		D3DXVECTOR3 pos;
		pos.x = (float)(rc.right - 173);
		pos.y = 0;
		pos.z = 0;
		m_spr->Draw( m_radarTex, NULL, NULL, &pos, 0xFFFFFFFF );
	}
	m_spr->End();
}


bool RadarDisplay::preload_surface_from_file( IDirect3DDevice9 *pdevice, IDirect3DSurface9 **ppSur,
	const wchar_t *fileName,
	UINT width, UINT height, D3DXIMAGE_FILEFORMAT fmt )
{
	HRESULT hr = NO_ERROR;
	hr = pdevice->CreateOffscreenPlainSurface( width, height, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, ppSur, NULL );
	//hr = pdevice->CreateOffscreenPlainSurface( width, height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, ppSur, NULL );
	if( SUCCEEDED( hr ) )
	{
		D3DXIMAGE_INFO imageInfo;
		memset( &imageInfo, 0, sizeof(imageInfo) );
		imageInfo.Width = width;
		imageInfo.Height = height;
		imageInfo.Depth = 24;
		imageInfo.ImageFileFormat = fmt;
		//imageInfo.Format = D3DFMT_X8R8G8B8;
		imageInfo.Format = D3DFMT_A8R8G8B8;
		imageInfo.MipLevels = 1;
		imageInfo.ResourceType = D3DRTYPE_SURFACE;
		// ColorKey 
		// [in] D3DCOLOR value to replace with transparent black, or 0 to disable the colorkey.
		// This is always a 32-bit ARGB color, independent of the source image format.
		// Alpha is significant and should usually be set to FF for opaque color keys Thus,
		// for opaque black, the value would be equal to 0xFF000000. 
		hr = D3DXLoadSurfaceFromFile( *ppSur, NULL, NULL,
			fileName, NULL,
			D3DX_FILTER_NONE, // D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER,
			D3DCOLOR_ARGB(0,0,0,0), &imageInfo );
		if( FAILED( hr ) )
			log_error( LOG_ERROR, "RadarDisplay::preload_surface_from_file(): Failed to load [%ux%u] [%S]! Error: %d\n",
				width, height, fileName, hr );
	}
	else
		log_error( LOG_ERROR, "RadarDisplay::preload_surface_from_file(): Failed to create surface! Error: %d (0x%08X)\n", hr, hr );
	return (hr == NO_ERROR);
}

bool RadarDisplay::preload_texture_from_file( IDirect3DDevice9 *pdevice, IDirect3DTexture9 **ppTex, const wchar_t *fileName )
{
	if( D3DXCreateTextureFromFileEx( pdevice, fileName, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2,
			D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, ppTex ) == S_OK )
		return true;
	log_error( LOG_ERROR, "RadarDisplay::preload_surface_from_file(): Failed to create texture!\n" );
	return false;
}
