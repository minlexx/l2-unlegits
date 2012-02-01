#include "pch.h"
#include "Utils.h"

bool Utils_isValidCharName( const wchar_t *name )
{
	if( !name ) return false;
	size_t nl = wcslen( name );
	if( nl < 3 ) return false;
	size_t i = 0;
	for( i=0; i<nl; i++ )
	{
		wchar_t c = name[i];
		if( !iswalnum( c ) ) return false;
	}
	return true;
}
