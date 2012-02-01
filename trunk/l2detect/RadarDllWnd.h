#ifndef H_RADAR_WND
#define H_RADAR_WND

extern HINSTANCE g_radardll_hinst;
extern HWND      g_radardll_hwnd;
extern DWORD     g_radardll_thread_id;
extern DWORD     g_l2_thread_id;

LRESULT CALLBACK RadarDllWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL RadarDllWindowRegClass( HINSTANCE hInst );
HWND RadarDllWindowCreate();

DWORD WINAPI RadarDllWindowThread( LPVOID lpParam );
void RadarDllWindowStart( HINSTANCE hInst );

#define WMMY_WNDSHOWTOGGLE        (WM_USER+100)
#define WMMY_UPDATE_CHARS_LIST    (WM_USER+101)
#define WMMY_SET_USER_COORDS      (WM_USER+102) // unused
#define WMMY_UPDATE_NPCS_LIST     (WM_USER+103)
#define WMMY_TRAYMESSAGE          (WM_USER+104)
#define WMMY_GAMECLIENT_NOTIFY    (WM_USER+105)
#define WMMY_LOGINCLIENT_NOTIFY   (WM_USER+106)
#define WMMY_NOTIFY_TARGET        (WM_USER+107)
#define WMMY_UPDATE_CHECKS        (WM_USER+108)
#define WMMY_SAVE_WINDOW_SIZE     (WM_USER+109)

// new way of lists updating
#define WMMY_ADD_OBJECT           (WM_USER+110) // msg
#define WMMY_DEL_OBJECT           (WM_USER+111) // msg
#define WMMY_UPDATE_OBJECT        (WM_USER+112) // msg
// wParams for these messages
#define MSG_PC     1
#define MSG_NPC    2
// lParams will represent objectId

#define WMMY_FORCE_UPDATE_LIST    (WM_USER+113) // msg
#define FORCE_UPDATE_CHARS_LIST 1 // wparam
#define FORCE_UPDATE_NPCS_LIST  2 // wparam

void RadarWnd_NotifyTarget( unsigned int oid_from, unsigned int oid_to, bool selected = true );

// new lists update way
void RadarWnd_AddChar( unsigned int objectId );
void RadarWnd_UpdChar( unsigned int objectId );
void RadarWnd_DelChar( unsigned int objectId );

void RadarWnd_AddNpc( unsigned int objectId );
void RadarWnd_UpdNpc( unsigned int objectId );
void RadarWnd_DelNpc( unsigned int objectId );

void RadarWnd_ForceUpdateCharsList();
void RadarWnd_ForceUpdateNpcsList();

void RadarWnd_UpdateChecksState();

#endif /* H_RADAR_WND */
