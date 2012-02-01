#include "pch.h"
#include "StatsSet.h"
#include "utils/Debugging.h"
#include "utils/Exception.h"
#include "l2c_utils.h"

StatsSet::StatsSet()
{
	m_map.clear();
}

StatsSet::~StatsSet()
{
	m_map.clear();
}

StatsSet::StatsSet( const StatsSet& other )
{
	this->operator=( other );
}

const StatsSet& StatsSet::operator=( const StatsSet& other )
{
	if( this == &other ) return (*this);
	m_map = other.m_map;
	return (*this);
}

bool StatsSet::getInt( const char *name, int *val )
{
	std::wstring value;
	std::string sname( name );
	std::map<std::string, std::wstring>::const_iterator iter = m_map.find( sname );
	if( iter == m_map.end() )
	{
		throw Exception( "StatsSet: trying to get non-existent Int var [%s] with no default value!", name );
		//return false; // warning C4702: unreachable code
	}
	value = iter->second;
	int i = 0;
	int r = swscanf( value.c_str(), L"%d", &i );
	if( r == 1 )
	{
		(*val) = i;
		return true;
	}
	throw Exception( "StatsSet.getInt: failed to scanf %%d from [%s]=[%S]", name, value.c_str() );
}

bool StatsSet::getInt( const char *name, int *val, int defVal )
{
	std::wstring ws;
	std::wstring ws_def(L"0");
	getWString( name, ws, ws_def );
	int v = 0;
	int r = swscanf( ws.c_str(), L"%d", &v );
	if( r <= 0 ) (*val) = defVal; // 0 tokens were read by swscanf or EOL reached unexpectedly
	else (*val) = v;
	return true;
}

bool StatsSet::getUInt( const char *name, unsigned int *val )
{
	std::wstring value;
	std::string sname( name );
	std::map<std::string, std::wstring>::const_iterator iter = m_map.find( sname );
	if( iter == m_map.end() )
	{
		throw Exception( "StatsSet: trying to get non-existent UInt var [%s] with no default value!", name );
		//return false; // warning C4702: unreachable code
	}
	value = iter->second;
	unsigned int ui = 0;
	int r = swscanf( value.c_str(), L"%u", &ui );
	if( r == 1 )
	{
		(*val) = ui;
		return true;
	}
	throw Exception( "StatsSet.getInt: failed to scanf %%u from [%s]=[%S]", name, value.c_str() );
}

bool StatsSet::getUInt( const char *name, unsigned int *val, unsigned int defVal )
{
	std::wstring ws;
	std::wstring ws_def(L"0");
	getWString( name, ws, ws_def );
	unsigned int v = 0;
	int r = swscanf( ws.c_str(), L"%u", &v );
	if( r <= 0 ) (*val) = defVal; // 0 tokens were read by swscanf or EOL reached unexpectedly
	else (*val) = v;
	return true;
}

bool StatsSet::getInt64( const char *name, long long int *val )
{
	std::wstring value;
	std::string sname( name );
	std::map<std::string, std::wstring>::const_iterator iter = m_map.find( sname );
	if( iter == m_map.end() )
	{
		throw Exception( "StatsSet: trying to get non-existent Int64 var [%s] with no default value!", name );
		//return false; // warning C4702: unreachable code
	}
	value = iter->second;
	long long int i64 = 0;
	int r = swscanf( value.c_str(), L"%I64d", &i64 );
	if( r == 1 )
	{
		(*val) = i64;
		return true;
	}
	throw Exception( "StatsSet.getInt: failed to scanf %%I64d from [%s]=[%S]", name, value.c_str() );
}

bool StatsSet::getInt64( const char *name, long long int *val, long long int defVal )
{
	std::wstring ws;
	std::wstring ws_def(L"0");
	getWString( name, ws, ws_def );
	long long int v = 0;
	int r = swscanf( ws.c_str(), L"%I64d", &v );
	if( r <= 0 ) (*val) = defVal; // 0 tokens were read by swscanf or EOL reached unexpectedly
	else (*val) = v;
	return true;
}

bool StatsSet::getUInt64( const char *name, unsigned long long int *val )
{
	std::wstring value;
	std::string sname( name );
	std::map<std::string, std::wstring>::const_iterator iter = m_map.find( sname );
	if( iter == m_map.end() )
	{
		throw Exception( "StatsSet: trying to get non-existent UInt64 var [%s] with no default value!", name );
		//return false; // warning C4702: unreachable code
	}
	value = iter->second;
	unsigned long long int ui64 = 0;
	int r = swscanf( value.c_str(), L"%I64u", &ui64 );
	if( r == 1 )
	{
		(*val) = ui64;
		return true;
	}
	throw Exception( "StatsSet.getInt: failed to scanf %%I64u from [%s]=[%S]", name, value.c_str() );
}

bool StatsSet::getUInt64( const char *name, unsigned long long int *val, unsigned long long int defVal )
{
	std::wstring ws;
	std::wstring ws_def(L"0");
	getWString( name, ws, ws_def );
	unsigned long long int v = 0;
	int r = swscanf( ws.c_str(), L"%I64u", &v );
	if( r <= 0 ) (*val) = defVal; // 0 tokens were read by swscanf or EOL reached unexpectedly
	else (*val) = v;
	return true;
}

bool StatsSet::getDouble( const char *name, double *val )
{
	std::wstring value;
	std::string sname( name );
	std::map<std::string, std::wstring>::const_iterator iter = m_map.find( sname );
	if( iter == m_map.end() )
	{
		throw Exception( "StatsSet: trying to get non-existent Double var [%s] with no default value!", name );
		//return false; // warning C4702: unreachable code
	}
	value = iter->second;
	double lf = 0;
	int r = swscanf( value.c_str(), L"%lf", &lf );
	if( r == 1 )
	{
		(*val) = lf;
		return true;
	}
	throw Exception( "StatsSet.getInt: failed to scanf %%lf from [%s]=[%S]", name, value.c_str() );
}

bool StatsSet::getDouble( const char *name, double *val, double defVal )
{
	std::wstring ws;
	std::wstring ws_def(L"0.0");
	getWString( name, ws, ws_def );
	double v = 0.0;
	int r = swscanf( ws.c_str(), L"%lf", &v );
	if( r <= 0 ) (*val) = defVal; // 0 tokens were read by swscanf or EOL reached unexpectedly
	else (*val) = v;
	return true;
}

bool StatsSet::getString( const char *name, std::string& val )
{
	std::wstring ws;
	std::string sname( name );
	std::map<std::string, std::wstring>::const_iterator iter = m_map.find( sname );
	if( iter == m_map.end() )
	{
		throw Exception( "StatsSet: trying to get non-existent String var [%s] with no default value!", name );
		//return false; // warning C4702: unreachable code
	}
	ws = iter->second;
	const wchar_t *cws = ws.c_str(); // get wchar_t *
	char *c_tmp = (char *)malloc( ws.size() + 16 ); // allocate space for Unicode ->  ANSI convert
	if( c_tmp )
	{
		l2c_unicode_to_ansi( cws, c_tmp, ws.size()+1 );
		c_tmp[ws.size()] = 0;
		val.assign( c_tmp, ws.size() );
		free( c_tmp );
	}
	else val.assign( "malloc() failed" );
	return true;
}

bool StatsSet::getString( const char *name, std::string& val, std::string& defVal )
{
	std::wstring ws;
	std::wstring ws_def( L"" );
	getWString( name, ws, ws_def ); // always returns true
	const wchar_t *cws = ws.c_str();
	char *c_tmp = (char *)malloc( ws.size() + 16 );
	if( c_tmp )
	{
		l2c_unicode_to_ansi( cws, c_tmp, ws.size()+1 );
		c_tmp[ws.size()] = 0;
		val.assign( c_tmp, ws.size() );
		free( c_tmp );
	}
	else val = defVal;
	return true;
}

bool StatsSet::getWString( const char *name, std::wstring& val )
{
	std::string sname( name );
	std::map<std::string, std::wstring>::const_iterator iter = m_map.find( sname );
	if( iter == m_map.end() )
	{
		throw Exception( "StatsSet: trying to get non-existent WString var [%s] with no default value!", name );
		//return false; // warning C4702: unreachable code
	}
	val = iter->second;
	return true;
}

bool StatsSet::getWString( const char *name, std::wstring& val, std::wstring& defVal )
{
	std::string sname( name );
	std::map<std::string, std::wstring>::const_iterator iter = m_map.find( sname );
	if( iter == m_map.end() )
	{
		val = defVal;
		return true;
	}
	val = iter->second;
	return true;
}

bool StatsSet::getBool( const char *name, bool *val )
{
	std::wstring value;
	std::string sname( name );
	std::map<std::string, std::wstring>::const_iterator iter = m_map.find( sname );
	if( iter == m_map.end() )
	{
		throw Exception( "StatsSet: trying to get non-existent Bool var [%s] with no default value!", name );
		//return false; // warning C4702: unreachable code
	}
	value = iter->second;
	int i = 0;
	swscanf( value.c_str(), L"%d", &i );
	(*val) = (bool)(i != 0);
	return true;
}

bool StatsSet::getBool( const char *name, bool *val, bool defVal )
{
	std::wstring value;
	std::string sname( name );
	std::map<std::string, std::wstring>::const_iterator iter = m_map.find( sname );
	if( iter == m_map.end() )
	{
		(*val) = defVal;
		return true;
	}
	value = iter->second;
	int i = 0;
	swscanf( value.c_str(), L"%d", &i );
	(*val) = (bool)(i != 0);
	return true; // defVal funcs never return false and never throw exceptions
}



bool StatsSet::setInt( const char *name, int val )
{
	std::string sname( name );
	wchar_t wval[32];
	swprintf( wval, 31, L"%d", val );
	std::wstring v( wval );
	m_map[name] = v;
	return true;
}

bool StatsSet::setUInt( const char *name, unsigned int val )
{
	std::string sname( name );
	wchar_t wval[32];
	swprintf( wval, 31, L"%u", val );
	std::wstring v( wval );
	m_map[name] = v;
	return true;
}

bool StatsSet::setInt64( const char *name, long long int val )
{
	std::string sname( name );
	wchar_t wval[32];
	swprintf( wval, 31, L"%I64d", val );
	std::wstring v( wval );
	m_map[name] = v;
	return true;
}

bool StatsSet::setUInt64( const char *name, unsigned long long int val )
{
	std::string sname( name );
	wchar_t wval[32];
	swprintf( wval, 31, L"%I64u", val );
	std::wstring v( wval );
	m_map[name] = v;
	return true;
}

bool StatsSet::setDouble( const char *name, double val )
{
	std::string sname( name );
	wchar_t wval[32];
	swprintf( wval, 31, L"%0.20f", val );
	std::wstring v( wval );
	m_map[name] = v;
	return true;
}

bool StatsSet::setString( const char *name, std::string& val )
{
	std::string sname( name );
	wchar_t *wval = (wchar_t *)malloc( (val.size()+16) * sizeof(wchar_t) );
	if( wval )
	{
		swprintf( wval, val.size()+1, L"%S", val.c_str() );
		std::wstring v( wval );
		m_map[sname] = v;
		free( wval );
		return true;
	}
	return false;
}

bool StatsSet::setString(  const char *name, const char *val )
{
	std::string sval( val );
	return setString( name, sval );
}

bool StatsSet::setWString( const char *name, const wchar_t *val )
{
	std::wstring sval( val );
	return setWString( name, sval );
}

bool StatsSet::setWString( const char *name, std::wstring& val )
{
	std::string sname( name );
	m_map[sname] = val;
	return true;
}

bool StatsSet::setBool( const char *name, bool val )
{
	return setInt( name, (int)val );
}
