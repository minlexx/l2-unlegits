#ifndef H_ACCOUNT
#define H_ACCOUNT

class Account
{
public:
	Account();
	Account( const Account& other );
	Account& operator=( const Account& other );
	~Account();
public:
	void clear();
public:
	bool loadFromFile( const wchar_t *fileName );
	bool saveToFile( const wchar_t *fileName );
protected:
	unsigned int account_version;
public:
	L2_VERSION getL2Version() const;
public:
	wchar_t login[32];
	wchar_t pass[32];
	char loginServerIP[32];
	int loginServerPort;
	int gameserverID;
	int gameserverSelectManual;
	int charSelectManual;
	wchar_t charSelectName[32];
	wchar_t configFileName[512];
	unsigned int protocolVersion;
	int serverVersion; // 0 - t1, 1 - t1.5, 2 - t2, 3 - t2.2, 4 - t2.3
	int useDefaultProtocolVersion;
};

#endif
