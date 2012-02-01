#include "stdafx.h"
#include "BotConfig.h"

char botconfig_dummy_str_buffer[2];

void BotConfigValue::clear()
{
	m_name[0] = 0; // clear name :)
	clearVal();
}

void BotConfigValue::clearVal()
{
	if( (type == CFG_TYPE_STR) && str ) free( str );
	type = CFG_TYPE_NONE;
	str = NULL;
	i = 0;
	d = 0.0;
}

void BotConfigValue::setStr( const char *stringValue )
{
	clearVal();
	str = NULL;
	if( stringValue ) str = _strdup( stringValue );
	type = CFG_TYPE_STR;
}

void BotConfigValue::setInt( int iVal )
{
	clearVal();
	i = iVal;
	type = CFG_TYPE_INT;
}
void BotConfigValue::setFlt( double dVal )
{
	clearVal();
	d = dVal;
	type = CFG_TYPE_FLT;
}

// always returns non-NULL value
const char *BotConfigValue::getStr() const
{
	if( (type != CFG_TYPE_STR) )
	{
		botconfig_dummy_str_buffer[0] = botconfig_dummy_str_buffer[1] = 0;
		return (const char *)botconfig_dummy_str_buffer;
	}
	if( !str )
	{
		botconfig_dummy_str_buffer[0] = botconfig_dummy_str_buffer[1] = 0;
		return (const char *)botconfig_dummy_str_buffer;
	}
	return str;
}

BotConfigValue::BotConfigValue( const BotConfigValue& other )
{
	type = CFG_TYPE_NONE;
	this->operator=( other );
}

BotConfigValue& BotConfigValue::operator=( const BotConfigValue& other )
{
	this->d = other.d;
	this->i = other.i;
	this->str = NULL; if( other.str ) this->str = _strdup( other.str );
	this->type = other.type;
	this->m_name[0] = 0; if( other.m_name[0] ) strcpy( this->m_name, other.m_name );
	return (*this);
}

void BotConfigValue::setName( const char *valName )
{
	memset( m_name, 0, sizeof(m_name) );
	strncpy( m_name, valName, sizeof(m_name)-1 );
}


BotConfig::BotConfig()
{
	removeAllValues();
}

BotConfig::~BotConfig()
{
	removeAllValues();
}

BotConfig::BotConfig( const BotConfig& other )
{
	this->operator=( other );
}

BotConfig& BotConfig::operator=( const BotConfig& other )
{
	removeAllValues();
	int i;
	for( i=0; i<MAX_VALUES; i++ ) this->value[i] = other.value[i];
	return (*this);
}

void BotConfig::removeAllValues()
{
	int i;
	for( i=0; i<BotConfig::MAX_VALUES; i++ ) value[i].clear();
}

int BotConfig::findValueIndex( const char *valName ) const
{
	if( !valName ) return -1;
	if( valName[0] == 0 ) return -1;
	int i = 0;
	for( i=0; i<MAX_VALUES; i++ )
	{
		if( strcmp( valName, value[i].name() ) == 0 ) return i;
	}
	return -1;
}

int  BotConfig::findFreeIndex() const
{
	int i;
	for( i=0; i<MAX_VALUES; i++ ) if( value[i].getType() == CFG_TYPE_NONE ) return i;
	return -1;
}

bool BotConfig::addValue( BotConfigValue& val )
{
	int idx = findFreeIndex();
	if( idx == -1 )
	{
		MessageBox( NULL, TEXT("Cannot add var to config: overflow?"), TEXT("BotConfig"), MB_ICONWARNING );
		return false;
	}
	value[idx] = val;
	return true;
}

CFG_VALTYPE BotConfig::getValType( const char *valName ) const
{
	int idx = findValueIndex( valName );
	if( idx == -1 ) return CFG_TYPE_NONE;
	return value[idx].getType();
}

int BotConfig::getValInt( const char *valName ) const
{
	int idx = findValueIndex( valName );
	if( idx == -1 ) return 0;
	return value[idx].getInt();
}

double BotConfig::getValFlt( const char *valName ) const
{
	int idx = findValueIndex( valName );
	if( idx == -1 ) return 0.0;
	return value[idx].getFlt();
}

const char *BotConfig::getValStr( const char *valName ) const
{
	int idx = findValueIndex( valName );
	if( idx == -1 )
	{
		botconfig_dummy_str_buffer[0] = botconfig_dummy_str_buffer[1] = 0;
		return botconfig_dummy_str_buffer;
	}
	return value[idx].getStr();
}

BotConfigValue BotConfig::getVal( const char *valName ) const
{
	BotConfigValue val;
	int idx = findValueIndex( valName );
	if( idx == -1 ) return val;
	val = value[idx];
	return val;
}

const BotConfigValue& BotConfig::getValByIdx( int idx ) const
{
	if( (idx<0) || (idx>=MAX_VALUES) )
	{
		return value[0];
	}
	return value[idx];
}

void BotConfig::setValue( const char *valName, BotConfigValue &val )
{
	int idx = findValueIndex( valName );
	if( idx == -1 ) addValue( val );
	else value[idx] = val;
}

void BotConfig::setValInt( const char *valName, int i )
{
	BotConfigValue val; val.setName( valName ); val.setInt( i );
	setValue( valName, val );
}

void BotConfig::setValFlt( const char *valName, double d )
{
	BotConfigValue val; val.setName( valName ); val.setFlt( d );
	setValue( valName, val );
}

void BotConfig::setValStr( const char *valName, const char *str )
{
	BotConfigValue val; val.setName( valName ); val.setStr( str );
	setValue( valName, val );
}

bool BotConfig::valExists( const char *valName ) const
{
	int idx = findValueIndex( valName );
	return ( idx != -1 );
}

bool BotConfig::readConfig( const char *fileName, bool clearExistingConfig /*= true*/ )
{
	FILE *f = fopen( fileName, "rt" );
	if( !f ) return false;
	if( clearExistingConfig ) removeAllValues();
	char line[2048];
	while( !feof( f ) )
	{
		memset( line, 0, sizeof(line) );
		fgets( line, sizeof(line)-1, f );
		if( line[0] == 0 ) continue;
		// pass comments
		if( (line[0] == '#') || ((line[0] == '/') && (line[1] == '/') ) ) continue;
		//
		size_t ll = strlen( line );
		if( (line[ll-1] == '\r') || (line[ll-1] == '\n') ) line[ll-1] = 0;
		if( (line[ll-2] == '\r') || (line[ll-2] == '\n') ) line[ll-2] = 0;
		//
		char *valVal = strchr( line, '=' );
		if( !valVal ) continue;
		(*valVal) = 0;
		valVal++;
		char valType = line[0];
		char *valName = line+1;
		switch( valType )
		{
		case 'I':
			{
				int i = 0;
				sscanf( valVal, "%d", &i );
				BotConfigValue val;
				val.setInt( i );
				val.setName( valName );
				setValue( valName, val );
			} break;
		case 'F':
			{
				double d = 0;
				sscanf( valVal, "%f", &d );
				BotConfigValue val;
				val.setFlt( d );
				val.setName( valName );
				setValue( valName, val );
			} break;
		case 'S':
			{
				BotConfigValue val;
				val.setStr( valVal );
				val.setName( valName );
				setValue( valName, val );
			} break;
		}
	}
	fclose( f );
	return true;
}

bool BotConfig::saveConfig( const char *fileName ) const
{
	FILE *f = fopen( fileName, "wt" );
	if( !f ) return false;
	int i;
	for( i=0; i<MAX_VALUES; i++ )
	{
		if( value[i].getType() == CFG_TYPE_NONE ) continue;
		switch( value[i].getType() )
		{
		case CFG_TYPE_INT:
			{
				fprintf( f, "I%s=%d\n", value[i].name(), value[i].getInt() );
			} break;
		case CFG_TYPE_FLT:
			{
				fprintf( f, "F%s=%0.6f\n", value[i].name(), value[i].getFlt() );
			} break;
		case CFG_TYPE_STR:
			{
				fprintf( f, "S%s=%s\n", value[i].name(), value[i].getStr() );
			} break;
		}
	}
	fclose( f );
	return true;
}

bool BotConfig::readConfig( const wchar_t *fileName, bool clearExistingConfig /*= true*/ )
{
	char aFileName[256] = {0};
	WideCharToMultiByte( CP_ACP, 0, fileName, -1, aFileName, 255, NULL, NULL );
	aFileName[255] = 0;
	return readConfig( aFileName, clearExistingConfig );
}

bool BotConfig::saveConfig( const wchar_t *fileName ) const
{
	char aFileName[256] = {0};
	WideCharToMultiByte( CP_ACP, 0, fileName, -1, aFileName, 255, NULL, NULL );
	aFileName[255] = 0;
	return saveConfig( aFileName );
}
