#pragma once

class LoginConfig;
#include "LoginClient.h"
#include "FailedLoginAttempt.h"
#include "BanInfo.h"

// warning C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#pragma warning( disable: 4290 )

class LoginController
{
public:
	static const int MAX_RSA_KEYS = 10;
	static const int MAX_BLOWFISH_KEYS = 20;
	static const int RSA_NUM_BITS = 1024;
	static const int RSA_PUBLIC_EXPONENT = 65537;

	enum AuthLoginResult
	{
		INVALID_PASSWORD,
		ACCOUNT_BANNED,
		ALREADY_ON_LS,
		ALREADY_ON_GS,
		AUTH_SUCCESS
	};

	static const int LOGIN_TIMEOUT = 60000; // msec

public:
	class HackingException: public std::exception
	{
	public:
		HackingException( int login_attempts ): std::exception(), m_attempts(login_attempts) {}
		inline int getConnects() const { return m_attempts; }
	protected:
		int m_attempts;
	};

public:
	class LoginClientOnGS
	{
	public:
		LoginClientOnGS( const char *account, unsigned char *sessionKey1, unsigned char *sessionKey2 );
		void getSessionKeys( unsigned char *sessionKey1, unsigned char *sessionKey2 );
		const char *getAccount() const { return m_account; }
	protected:
		char m_account[128];
		unsigned char m_sessionKey1[8];
		unsigned char m_sessionKey2[8];
	};

public:
	typedef std::list<LoginClient *>                  LoginClientList;
	typedef std::list<LoginClientOnGS *>              LoginClientOnGSList;
	typedef std::map<std::string, FailedLoginAttempt> HackProtection;
	typedef std::map<std::string, BanInfo>            BannedIpList;

public:
	LoginController( LoginServer *pServer );
	~LoginController();

public:
	bool init();
	void cleanup();
	RSA *getRandomRSAKeyPair();
	const unsigned char *getRandomBlowfishKey();

public:
	bool addLoginClient( LoginClient *cl );
	bool removeClient( LoginClient *cl );
	void closeAllClients( bool force );

public:
	AuthLoginResult tryAuthLogin( const char *account, const char *password, LoginClient *client ) throw(HackingException);
	void addBanForAddress( const char *ip, time_t seconds );
	void removeBanForAddress( const char *ip );
	bool isIpBanned( const char *ip );
	bool getSessionKeysForAccount( const char *account, unsigned char *player_loginKey, unsigned char *player_playKey );
	void addClientOnGS( const char *account, unsigned char *player_loginKey, unsigned char *player_playKey );
	void removeAuthedGSClient( const char *account );
	void setAccountAccessLevel( const wchar_t *account, int level );

protected:
	bool isLoginValid( const char *user, const char *password, LoginClient *client );

protected:
	void generateRSAKeyPairs();
	void freeRSAKeyPairs();
	void generateBFKeys();
	void freeBFKeys();
	void loadBannedIps();

	const LoginConfig *getConfig() const;

	void Lock();
	void Unlock();

protected:
	FailedLoginAttempt *getFailedLoginAttemptForAddress( std::string& address );
	void removeHackProtectionForAddress( std::string& address );

protected:
	LoginServer                *m_loginServer;
	RSA                        *m_rsa_keyPairs[MAX_RSA_KEYS];
	unsigned char               m_bf_keys[MAX_BLOWFISH_KEYS][16];
	LoginClientList             m_clients; // currently connected login server clients
	LoginClientOnGSList         m_clientsOnGS; // authorized nd went on game server clients
	CRITICAL_SECTION            m_lock;
	HackProtection              m_hackProtection;
	BannedIpList                m_bannedIps;
};
