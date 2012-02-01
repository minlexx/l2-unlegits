#ifndef L2GAME_AUTHLOGIN_H_
#define L2GAME_AUTHLOGIN_H_

#include "../L2GamePacket.h"

class L2Game_AuthLogin : public L2GamePacket
{
public:
	L2Game_AuthLogin();
	L2Game_AuthLogin( const unsigned char *bytes, unsigned int length );
public:
	// session keys must point to 8-byte arrays
	bool create( char *loginName,
			unsigned char *sessionKey1,
			unsigned char *sessionKey2 );
	// unicode version of upper function
	// session keys must point to 8-byte arrays
	bool create( wchar_t *loginName,
			unsigned char *sessionKey1,
			unsigned char *sessionKey2 );
public:
	bool read_login( char *loginOut );
	bool read_login( wchar_t *loginOut );
	//bool read_session_keys( unsigned char *skey1, unsigned char *skey2 );
};

#endif /*L2GAME_AUTHLOGIN_H_*/
