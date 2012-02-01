#ifndef H_UTILS
#define H_UTILS

int freadline( FILE *f, char *line, size_t maxCount );

bool MyInitCommonControlsEx();

template< class T >
bool in_array( const T& var, T* arr, size_t array_size )
{
	if( !arr || (array_size<1) ) return false;
	size_t i = 0;
	for( i=0; i<array_size; i++ )
		if( var == arr[i] ) return true;
	return false;
}

// detect clan war state depending on current Lineage version
void utils_detectClanWar( unsigned int relation, bool *isEnemy, bool *is2side );

#endif
