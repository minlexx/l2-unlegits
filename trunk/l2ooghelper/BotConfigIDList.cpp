#include "stdafx.h"

bool BotCfg_IsInList( const char *id, const char *list )
{
	if( !id || !list ) return false;
	// olny one id in list, which is equal to what we need?
	if( _stricmp( id, list ) == 0 ) return true;
	// make copies
	char *idc = _strdup( id );
	char *listc = _strdup( list );
	// lowercase!
	_strlwr( idc );
	_strlwr( listc );
	//
	bool ret = false;
	char test_id[256];
	// id is in the beginning
	sprintf( test_id, "%s;", idc );
	if( strstr( listc, idc ) == listc ) ret = true;
	// id in the end
	if( !ret )
	{
		sprintf( test_id, "%;s", idc );
		if( strstr( listc, idc ) ) ret = true;
	}
	// in the middle?
	if( !ret )
	{
		sprintf( test_id, "%;s;", idc );
		if( strstr( listc, idc ) ) ret = true;
	}
	//
	free( idc );
	free( listc );
	return ret;
}
