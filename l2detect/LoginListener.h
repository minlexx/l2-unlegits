#ifndef FAKELOGINLISTENER_H_
#define FAKELOGINLISTENER_H_

// signals to thread
#define FLLS_STOP       1

class LoginListener
{
	friend void FLS_Thread_freeInfo( struct FLLT_INFO *pinfo );
protected:
	static DWORD WINAPI FLS_Thread( LPVOID lpParam );
public:
	LoginListener();
	virtual ~LoginListener();
public:
	bool         start();
	void         signal( unsigned int code )              { this->m_signal = code; }
	void         signalStop()                             { this->signal( FLLS_STOP ); }
	bool         waitStopped( unsigned int timeoutMsec );
	unsigned int getLastSignal() const                    { return this->m_signal; }
	void         setLastSignal( unsigned int code )       { this->m_signal = code; }
	void         terminateThread();
public:
	bool         isRunning() const;
protected:
	void         _initNull();
protected:
	HANDLE          m_hThread;
	unsigned int    m_signal;
	// thread resources
	SOCKET          m_listen_socket;
};

#endif /*FAKELOGINLISTENER_H_*/
