#ifndef _H_REMOTESERVERINFO
#define _H_REMOTESERVERINFO

class RemoteServerInfo
{
public:
	RemoteServerInfo();
	RemoteServerInfo( const RemoteServerInfo& other );
	RemoteServerInfo &operator=( const RemoteServerInfo& other );
	virtual ~RemoteServerInfo();
public:
	LPCTSTR            desc() const;
	const char        *ip() const;
	unsigned short     port() const { return m_port; }
public:
	void               setDesc( LPCTSTR desc );
	void               setIP( const char *ip );
	void               setPort( unsigned short int port ) { m_port = port; }
protected:
	TCHAR              m_desc[128];
	char               m_ip[16];
	unsigned short int m_port;
protected:
	TCHAR              str_desc[32];
	char               str_ip[16];
};

#endif /* _H_REMOTESERVERINFO */
