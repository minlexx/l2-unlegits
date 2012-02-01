#include "stdafx.h"
#include "L2Login_RequestAuthLogin.h"

L2Login_RequestAuthLogin::L2Login_RequestAuthLogin()
{
	_initNull();
}

L2Login_RequestAuthLogin::L2Login_RequestAuthLogin( const unsigned char *bytes, unsigned int length )
{
	_initNull();
	setBytes( bytes, length );
}


// RSA_pubKeyMod must point to 128-bytes length buffer
bool L2Login_RequestAuthLogin::create( const char *szL2Login, const char *szL2Password,
	unsigned int ggAuthResponse, const unsigned char *RSA_pubKeyMod )
{
	// assert
	if( !szL2Login || !szL2Password ) return false;
	// vars
	size_t        l2l_len = strlen( szL2Login );
	size_t        l2p_len = strlen( szL2Password );
	char          l2login[L2_LOGIN_MAXLEN];
	char          l2password[L2_PASSWORD_MAXLEN];
	unsigned char block128[128];
	int           i = 0;
	const int     l2l_offset = 94;
	const int     l2p_offset = 108;
	// memsets
	memset( l2login,    0, sizeof(l2login)    );
	memset( l2password, 0, sizeof(l2password) );
	memset( block128,   0, sizeof(block128)   );
	// check strings length
	if( (l2l_len<1) || (l2l_len>L2_LOGIN_MAXLEN) )
	{
		printf( "L2Login_RequestAuthLogin::create(): L2 Login length must be [1..%d] chars\n",
			L2_LOGIN_MAXLEN );
		return false;
	}
	if( (l2p_len<1) || (l2p_len>L2_PASSWORD_MAXLEN) )
	{
		printf( "L2Login_RequestAuthLogin::create(): L2 Password length must be [1..%d] chars\n",
			L2_PASSWORD_MAXLEN );
		return false;
	}
	// copy strings
	for( i=0; i<(int)l2l_len; i++ ) l2login[i]    = szL2Login[i];
	for( i=0; i<(int)l2p_len; i++ ) l2password[i] = szL2Password[i];
	// copy login/pass to 128-byte block
	memcpy( (void *)(block128 + l2l_offset), l2login,    L2_LOGIN_MAXLEN );
	memcpy( (void *)(block128 + l2p_offset), l2password, L2_PASSWORD_MAXLEN );
	// RSA encode block...
	if( !this->block_encode_RSA( block128, sizeof(block128), RSA_pubKeyMod ) ) return false;
	// construct packet
	this->writeReset();
	this->setPacketType( 0x00 );                    // RequestAuthLogin code
	this->writeBytes( block128, sizeof(block128) ); // block with login/password
	this->writeUInt( ggAuthResponse );              // from GGAuthResponse
	for( i=0; i<16; i++ ) this->writeUChar( 0x00 ); // 16 NULLs
	this->writeUChar( 0x08 );
	for( i=0; i<10; i++ ) this->writeUChar( 0x00 ); // some 11 bytes
	// now packet must be padded to 8-byte border
	// next caller must add checksum, and so on...
	return true;
}

// for internal use only
// encode block with login/password
// RSA_pubKeyMod must point to 128-bytes length buffer
bool L2Login_RequestAuthLogin::block_encode_RSA( unsigned char *block , unsigned int blockSize,
		const unsigned char *RSA_pubKeyMod )
{
	if( !block || (blockSize<1) || !RSA_pubKeyMod ) return false;
	bool retVal = true;
	unsigned char blockE[128];
	memset( blockE, 0, sizeof(blockE) );
	// allocate & initialize RSA struct
	RSA *rsa = RSA_new();
	// set exponent and n
	rsa->e = BN_new();
	BN_dec2bn( &(rsa->e), "65537" ); // exponent is constant, 65537
	rsa->n = BN_bin2bn( RSA_pubKeyMod, 128, NULL ); // n is taken from unscrambled RSA public key modulus from Init packet
	//printf( "RSA_size: %d\n", RSA_size( rsa ) );
	int res = RSA_public_encrypt( 128, block, blockE, rsa, RSA_NO_PADDING );
	if( res == -1 )
	{
		retVal = false;
		fprintf( stderr, "RSA_public_encrypt res: %d\n", res );
		ERR_print_errors_fp( stderr );
	}
	// store result...
	memcpy( block, blockE, 128 );
	// free RSA struct
	RSA_free( rsa );
	rsa = NULL;
	return retVal;
}

bool L2Login_RequestAuthLogin::block_decode_RSA( unsigned char *block , unsigned int blockSize,
		const unsigned char *RSA_pubKeyMod )
{
	if( !block || (blockSize<1) || !RSA_pubKeyMod ) return false;
	bool retVal = true;
	unsigned char blockE[128];
	memset( blockE, 0, sizeof(blockE) );
	// allocate & initialize RSA struct
	RSA *rsa = RSA_new();
	// set exponent and n
	rsa->e = BN_new();
	BN_dec2bn( &(rsa->e), "65537" ); // exponent is constant, 65537
	rsa->n = BN_bin2bn( RSA_pubKeyMod, 128, NULL ); // n is taken from unscrambled RSA public key modulus from Init packet
	int res = RSA_private_decrypt( 128, block, blockE, rsa, RSA_NO_PADDING );
	if( res == -1 )
	{
		retVal = false;
		fprintf( stderr, "RSA_private_decrypt res: %d\n", res );
		ERR_print_errors_fp( stderr );
	}
	// store result...
	memcpy( block, blockE, 128 );
	// free RSA struct
	RSA_free( rsa );
	rsa = NULL;
	return retVal;
}

bool L2Login_RequestAuthLogin::parse( L2_VERSION ver /*= L2_VERSION_T1*/ )
{
	UNREFERENCED_PARAMETER(ver);
	fprintf( stderr, "L2Login_RequestAuthLogin::parse() not implemented!\n" );
	return false;
}
