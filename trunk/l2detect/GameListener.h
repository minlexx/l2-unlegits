#ifndef FAKEGAMELISTENER_H_
#define FAKEGAMELISTENER_H_

struct PlayServerInfo
{
	unsigned char      ip[4];
	unsigned short int port;
};

// signals to thread
#define FLGS_STOP       1

class GameListener
{
	friend void FGS_Thread_freeInfo( struct FLGT_INFO *pinfo );
	friend DWORD WINAPI FGS_Thread( LPVOID lpParam );
public:
	GameListener();
	virtual ~GameListener();
public:
	bool         start();
	void         signal( unsigned int code )                { this->m_signal = code; }
	void         signalStop()                               { this->signal( FLGS_STOP ); }
	bool         waitStopped( unsigned int timeoutMsec );
	unsigned int getLastSignal() const                      { return this->m_signal; }
	void         setLastSignal( unsigned int code )         { this->m_signal = code; }
	void         terminateThread();
public:
	bool         isRunning() const;
public:
	void         setPlayServerInfo( struct PlayServerInfo *pinfo );
	void         getPlayServerInfo( struct PlayServerInfo *pinfo ) const;
protected:
	void         _initNull();
protected:
	HANDLE                 m_hThread;
	unsigned int           m_signal;
	struct PlayServerInfo  m_sinfo;
	// thread resources
	SOCKET                 m_listen_socket;
};

#endif /*FAKEGAMELISTENER_H_*/
