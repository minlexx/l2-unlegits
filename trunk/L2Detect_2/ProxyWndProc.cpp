#include "pch.h"
#include "RadarDisplay.h"

WNDPROC g_l2OriginalWndProc = (WNDPROC)NULL;

LRESULT CALLBACK ProxyWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

void InstallProxyWndProc( HWND hWndL2 )
{
	if( g_l2OriginalWndProc == NULL )
		g_l2OriginalWndProc = (WNDPROC)(void *)GetWindowLongPtr( hWndL2, GWLP_WNDPROC );
	else
		log_error( LOG_WARNING, "InstallProxyWndProc(): g_l2OriginalWndProc already is not NULL (called twice? HOW???)\n" );
	// check if already intercepted
	if( (void *)GetWindowLongPtr( hWndL2, GWLP_WNDPROC ) == (void *)ProxyWndProc )
		log_error( LOG_WARNING, "InstallProxyWndProc(): wndproc already intercepted (called twice? HOW???)\n" );
	else // intercept!
	{
		SetWindowLongPtr( hWndL2, GWLP_WNDPROC, (LONG_PTR)(void *)ProxyWndProc );
		if( (void *)GetWindowLongPtr( hWndL2, GWLP_WNDPROC ) == (void *)ProxyWndProc )
			log_error( LOG_OK, "InstallProxyWndProc(): Window procedure intercepted, new wndproc = 0x%08X\n",
				(unsigned int)ProxyWndProc );
		else
			log_error( LOG_ERROR, "InstallProxyWndProc(): Window procedure interception failed!\n" );
	}
}

LRESULT CALLBACK ProxyWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_KEYUP:
		{
			int vkey = (int)wParam;
			switch( vkey )
			{
			case VK_PAUSE:
				RadarDisplay::getInstance()->toggleShowRadar();
				return 0;
				break;
			case 'Q':
				RadarDisplay::getInstance()->toggleShowCur();
				return 0;
				break;
			}
		} break;
	//case WM_LBUTTONUP:
		//{
			//log_error( LOG_DEBUG, "WM_LBUTTONUP ( %d, %d )\n", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
		//} break;
	}
	return CallWindowProc( g_l2OriginalWndProc, hWnd, uMsg, wParam, lParam );
}
