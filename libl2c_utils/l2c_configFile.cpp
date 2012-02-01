#include "pch.h"
#include "l2c_utils.h"
#include "l2c_configFile.h"

L2C_ConfigFile::L2C_ConfigFile()
{
	int i;
	for( i=0; i<MAX_SETTINGS; i++ ) m_varName[i] = m_varValue[i] = NULL;
	m_nVars = 0;
}

L2C_ConfigFile::~L2C_ConfigFile()
{
	clear();
}

void L2C_ConfigFile::clear()
{
	int i;
	for( i=0; i<MAX_SETTINGS; i++ )
	{
		if( m_varName[i] ) free( m_varName[i] );
		if( m_varValue[i] ) free( m_varValue[i] );
		m_varName[i] = m_varValue[i] = NULL;
	}
	m_nVars = 0;
}

int L2C_ConfigFile::getVarsCount() const
{
	return m_nVars;
}

bool L2C_ConfigFile::load( const wchar_t *filename )
{
	FILE *f = _wfopen( filename, L"rt" );
	if( !f ) return false;
	//
	wchar_t line[1024] = {0};
	int nVarsAdded = 0;
	while( !feof( f ) )
	{
		line[0] = 0;
		fgetws( line, 1023, f );
		if( line[0] == 0 ) continue;
		if( line[0] == L'#' ) continue;
		// process line	
		wchar_t varName[256] = {0};
		wchar_t varValue[256] = {0};
		wchar_t *pVarName = varName;
		wchar_t *pVarValue = varValue;
		wchar_t *pLine = line;
		// copy var name until ' ' or '='
		int nCharsCopied = 0;
		while( (nCharsCopied < 255) && ((*pLine) != 0) && ((*pLine) != L' ') && ((*pLine) != L'=') )
		{
			(*pVarName) = (*pLine);
			pVarName++;
			pLine++;
			nCharsCopied++;
		}
		(*pVarName) = 0;
		// find '=' in line (or end of line)
		while( (*pLine) && ((*pLine) != L'=') ) pLine++;
		// line end? (no '=' in line)
		if( (*pLine) == 0 ) continue;
		// if we here, we found '='; move pointer once
		pLine++;
		// line end? no parameter
		if( (*pLine) == 0 ) continue;
		// pass all spaces in line
		while( (*pLine) == L' ' ) pLine++;
		// check for line end? (no var value)
		if( (*pLine) )
		{
			// copy until space or EOL
			nCharsCopied = 0;
			while( (nCharsCopied < 255) && (*pLine) && ((*pLine) != L' ') && ((*pLine) != L'\r') &&
				((*pLine) != L'\n') && ((*pLine) != L'\t') )
			{
				(*pVarValue) = (*pLine);
				pVarValue++;
				pLine++;
				nCharsCopied++;
			}
			(*pVarValue) = 0;
		}
		// add new var
		m_varName[nVarsAdded] = _wcsdup( varName );
		if( varValue[0] > 0 )
			m_varValue[nVarsAdded] = _wcsdup( varValue );
		else
			m_varValue[nVarsAdded] = NULL;
		// inc
		nVarsAdded++;
		if( nVarsAdded >= L2C_ConfigFile::MAX_SETTINGS ) break; // overflow...
	}
	//
	fclose( f );
	m_nVars = nVarsAdded;
	return true;
}

const wchar_t *L2C_ConfigFile::getValueStrW( const wchar_t *varName )
{
	if( !varName ) return NULL;
	int i;
	for( i=0; i<m_nVars; i++ )
	{
		if( m_varName[i] )
			if( _wcsicmp( m_varName[i], varName ) == 0 )
				return m_varValue[i];
	}
	return NULL;
}

void L2C_ConfigFile::getValueStrA( const wchar_t *varName, char *out, size_t maxCount )
{
	if( !varName || maxCount<1 ) return;
	out[0] = 0;
	const wchar_t *varValue = getValueStrW( varName );
	if( varValue ) l2c_unicode_to_ansi( varValue, out, maxCount );
}

int L2C_ConfigFile::getValueInt( const wchar_t *varName )
{
	const wchar_t *varValue = getValueStrW( varName );
	if( !varValue ) return 0;
	int i = 0;
	swscanf( varValue, L"%d", &i );
	return i;
}

long long int L2C_ConfigFile::getValueInt64( const wchar_t *varName )
{
	const wchar_t *varValue = getValueStrW( varName );
	if( !varValue ) return 0;
	long long int i = 0;
	swscanf( varValue, L"%I64d", &i );
	return i;
}

bool L2C_ConfigFile::getValueBool( const wchar_t *varName )
{
	const wchar_t *varValue = getValueStrW( varName );
	if( !varValue ) return false;
	if( _wcsicmp( varValue, L"true" ) == 0 ) return true;
	//if( _wcsicmp( varValue, L"false" ) == 0 ) return false;
	//if( _wcsicmp( varValue, L"0" ) == 0 ) return false;
	return false;
}

double L2C_ConfigFile::getValueDouble( const wchar_t *varName )
{
	const wchar_t *varValue = getValueStrW( varName );
	if( !varValue ) return 0.0;
	double d = 0.0;
	swscanf( varValue, L"%lf", &d );
	return d;
}


const wchar_t *L2C_ConfigFile::getValueStrW( const wchar_t *varName, const wchar_t *defVal )
{
	if( !varName ) return defVal;
	int i;
	for( i=0; i<m_nVars; i++ )
	{
		if( m_varName[i] )
			if( _wcsicmp( m_varName[i], varName ) == 0 )
				return m_varValue[i];
	}
	return defVal;
}

void L2C_ConfigFile::getValueStrA( const wchar_t *varName, char *out, size_t maxCount, const char *defVal )
{
	if( !varName || maxCount<1 ) return;
	out[0] = 0;
	const wchar_t *varValue = getValueStrW( varName );
	if( varValue )
	{
		l2c_unicode_to_ansi( varValue, out, maxCount );
	}
	else
	{
		if( defVal )
		{
			strncpy( out, defVal, maxCount );
			out[maxCount-1] = 0;
		}
	}
}

int L2C_ConfigFile::getValueInt( const wchar_t *varName, int defVal )
{
	const wchar_t *varValue = getValueStrW( varName );
	if( !varValue ) return defVal;
	int i = defVal;
	swscanf( varValue, L"%d", &i );
	return i;
}

long long int L2C_ConfigFile::getValueInt64( const wchar_t *varName, long long int defVal )
{
	const wchar_t *varValue = getValueStrW( varName );
	if( !varValue ) return defVal;
	long long int i = defVal;
	swscanf( varValue, L"%I64d", &i );
	return i;
}

bool L2C_ConfigFile::getValueBool( const wchar_t *varName, bool defVal )
{
	const wchar_t *varValue = getValueStrW( varName );
	if( !varValue ) return defVal;
	if( _wcsicmp( varValue, L"true" ) == 0 ) return true;
	return false;
}

double L2C_ConfigFile::getValueDouble( const wchar_t *varName, double defVal )
{
	const wchar_t *varValue = getValueStrW( varName );
	if( !varValue ) return defVal;
	double d = defVal;
	swscanf( varValue, L"%lf", &d );
	return d;
}
