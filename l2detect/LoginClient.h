#ifndef FAKELOGIN_LOGINCLIENT_H_
#define FAKELOGIN_LOGINCLIENT_H_

// client state
#define LCST_OFFLINE       0 // not connected to LS :)
#define LCST_CONNECTED     1 // just connected to LS
#define LCST_AUTHED_GG     2 // passed GG auth
#define LCST_AUTHED_LOGIN  3 // authorized
#define LCST_AUTH_ERROR    4 // auth error

// window notification wParam
#define LCN_STATECHANGE 1 // lParam will hold new state

class LoginClient
{
public:
	LoginClient();
	virtual ~LoginClient();
public:
	bool ProcessClient( SOCKET scl );
	bool getSessionKeys( unsigned char *sKey1, unsigned char *sKey2 );
	bool getPlayServerAddress( unsigned char *ip, unsigned short int *port );
public:
	void setNotifyWindow( HWND notifyHwnd, UINT notifyMsg );
	void postNotify( WPARAM wParam, LPARAM lParam );
public:
	int getState() { return state; }
protected:
	int state;
protected:
	unsigned char      sessionKey1[8];
	unsigned char      sessionKey2[8];
	unsigned char      gsIP[4];
	unsigned short int gsPort;
protected:
	// notify window
	HWND hWndNotify;
	UINT uMsgNotify;
};

extern LoginClient *g_login_client;

#endif /*FAKELOGIN_LOGINCLIENT_H_*/
