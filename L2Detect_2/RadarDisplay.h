#pragma once

class RadarDisplay
{
protected:
	RadarDisplay();
	~RadarDisplay();
protected:
	static RadarDisplay *s_instance;
	static int s_refCount;
public:
	static RadarDisplay *getInstance();
	static void freeInstance();

public: // handlers
	void onD3D_CreateDevice( HWND hWndFocus, IDirect3DDevice9 *pdevice );
	void onD3D_Release( IDirect3DDevice9 *pdevice );
	void onD3D_Present( IDirect3DDevice9 *pdevice );

public:
	void toggleShowRadar();
	void toggleShowCur();

protected:
	void preload_resources( IDirect3DDevice9 *pdevice );
	static bool preload_surface_from_file( IDirect3DDevice9 *pdevice, IDirect3DSurface9 **ppSur,
		const wchar_t *fileName,
		UINT width, UINT height, D3DXIMAGE_FILEFORMAT fmt );
	static bool preload_texture_from_file( IDirect3DDevice9 *pdevice, IDirect3DTexture9 **ppTex, const wchar_t *fileName );

protected:
	// render window
	HWND                    m_hWndL2;
	// show options
	bool                    m_ui_show_radar;
	bool                    m_ui_show_cur;
	// Direct3D resources
	bool                    m_res_loaded;
	IDirect3DTexture9      *m_radarTex;
	ID3DXSprite            *m_spr;
};
