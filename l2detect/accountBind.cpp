#include "stdafx.h"
#include "accountBind.h"

#ifndef _ACCOUNT_BIND
int isValidAccount( const char *account_name )
{
	account_name;
	return 1;
}
#else

//#define ACC_ALLOWED "mincxclock"
//#define ACC_ALL_LEN 10

int isValidAccount( const char *account_name )
{
	int ret = strcmp( account_name, "Max10209080" );
	if( ret == 0 ) return 1;
	ret = strcmp( account_name, "10209080" );
	if( ret == 0 ) return 1;
	ret = strcmp( account_name, "MaxMen228" );
	if( ret == 0 ) return 1;

	ret = strcmp( account_name, "MaxMen609" );
	if( ret == 0 ) return 1;
	ret = strcmp( account_name, "Shmell228" );
	if( ret == 0 ) return 1;
	ret = strcmp( account_name, "buffer" );
	if( ret == 0 ) return 1;

	ret = strcmp( account_name, "Shmel" );
	if( ret == 0 ) return 1;

	return 0;
}

/*int isValidAccount_xor( const char *account_name )
{
	int ret = 0; // think not equal
	char *accAllowed = (char *)malloc( ACC_ALL_LEN*10 );
	int offset_table[ACC_ALL_LEN] = { 5, 1, 3, 2, 4 };
	int char_idx[ACC_ALL_LEN] = { 0, 0, 0, 0, 0 };
	int i;
	for( i=0; i<ACC_ALL_LEN*10; i++ )
	{
		accAllowed[i] = (rand() % 256) & 0xFF;
	}
	for( i=0; i<ACC_ALL_LEN; i++ )
	{
		char_idx[i] = i*10 + offset_table[i];
		accAllowed[ char_idx[i] ] =
	}

	// strlen
	int ll = 0;
	char *paccount_name = account_name;
	while( (*paccount_name) ) { ll++; paccount_name++; }

	if( ll = ACC_ALL_LEN )
	{
		// compare only if lengths equal
		// think equal
		ret = 1;
		for( i=0; i<ACC_ALL_LEN; i++ )
		{
			char acc_char = account_name[i];
			char valid_acc_char = accAllowed[ char_idx[i] ];
			if( acc_char != valid_acc_char ) ret = 0;
		}
	}

	free( accAllowed );
	return ret;
}*/

#endif