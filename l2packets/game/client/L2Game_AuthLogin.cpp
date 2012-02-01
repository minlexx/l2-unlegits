#include "stdafx.h"
#include "L2Game_AuthLogin.h"

/* interlude pcode is 08
   hellbound pcode is 2b
   L2J:
	protected void readImpl()
	{
		_loginName = readS().toLowerCase();
		_playKey2 = readD();
		_playKey1 = readD();
		_loginKey1 = readD();
		_loginKey2 = readD();
	}
*/

L2Game_AuthLogin::L2Game_AuthLogin()
{
	this->_initNull();
}

L2Game_AuthLogin::L2Game_AuthLogin( const unsigned char *bytes, unsigned int length )
{
	this->_initNull();
	this->setBytes( bytes, length );
}

// session keys must point to 8-byte arrays
bool L2Game_AuthLogin::create( char *loginName,
			unsigned char *sessionKey1,
			unsigned char *sessionKey2 )
{
	if( !loginName || !sessionKey1 || !sessionKey2 ) return false;
	wchar_t loginU[32];
	memset( loginU, 0, sizeof(loginU) );
#ifdef L2PACKETS_WINDOWS
	_snwprintf( loginU, 32, L"%S", loginName );
#endif
#ifdef L2PACKETS_LINUX
	swprintf( loginU, 32, L"%ls", loginName );
#endif
	loginU[31] = 0;
	return this->create( loginU, sessionKey1, sessionKey2 );
}

bool L2Game_AuthLogin::create( wchar_t *loginName,
			unsigned char *sessionKey1,
			unsigned char *sessionKey2 )
{
	if( !loginName || !sessionKey1 || !sessionKey2 ) return false;
	this->writeReset();
	// write packet code
	//this->writeUChar( 0x08 ); // interlude
	this->writeUChar( 0x2b ); // hellbound
	this->writeUnicodeString( loginName );
	this->writeBytes( sessionKey2+4, 4 );
	this->writeBytes( sessionKey2,   4 );
	this->writeBytes( sessionKey1,   8 );
	this->writeUInt( 1 );
	return true;
}

bool L2Game_AuthLogin::read_login( wchar_t *loginOut )
{
	if( !loginOut ) return false;
	this->readReset();
	this->readUChar();
	loginOut[0] = 0;
	wchar_t *login = this->readUnicodeString();
	if( !login ) return false;
	wcscpy( loginOut, login );
	free( login );
	return true;
}

bool L2Game_AuthLogin::read_login( char *loginOut )
{
	if( !loginOut ) return false;
	loginOut[0] = 0;
	wchar_t loginU[32] = {0};
	if( this->read_login(loginU) )
	{
		if( loginU[0] == 0 ) return true;
#ifdef L2PACKETS_WINDOWS
		sprintf( loginOut, "%S", loginU );
#else
#ifdef L2PACKETS_LINUX
		sprintf( loginOut, "%ls", loginU );
#else
#error Unknown OS? O_o
#endif
#endif
		return true;
	}
	return false;
}
