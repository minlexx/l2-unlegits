#include "pch.h"
#include "l2c_utils.h"
#include "l2c_db.h"


MysqlQuery::MysqlQuery()
{
	q = NULL;
	result = NULL;
	num_rows = num_fields = 0;
	strcpy( NULL_RET, "NULL" );
}

MysqlQuery::MysqlQuery( const wchar_t *_Format, ... )
{
	q = NULL;
	result = NULL;
	num_rows = num_fields = 0;
	strcpy( NULL_RET, "NULL" );
	if( _Format )
	{
		va_list arglist;
		va_start( arglist, _Format );
		int n = _vscwprintf( _Format, arglist );
		if( n >= 1 )
		{
			q = (wchar_t *)malloc( 2*n + 16 );
			if( q )
			{
				va_start( arglist, _Format );
				vswprintf( q, n+2, _Format, arglist );
			}
		}
	}
}

MysqlQuery::~MysqlQuery()
{
	clear();
}

void MysqlQuery::clear()
{
	if( q ) free( q );
	q = NULL;
	if( result ) mysql_free_result( result );
	result = NULL;
	num_rows = num_fields = 0;
	row = NULL;
}

int MysqlQuery::getNumRows() const
{
	return num_rows;
}

int MysqlQuery::getNumFields() const
{
	return num_fields;
}

void MysqlQuery::create( const wchar_t *_Format, ... )
{
	clear();
	va_list _ArgList;
	va_start( _ArgList, _Format );
	int l = _vscwprintf( _Format, _ArgList ); // number of chars will be printed
	if( l <= 1 ) return;
	q = (wchar_t *)malloc( l*2 + 2 ); // number of bytes needed
	if( !q ) return;
	va_start( _ArgList, _Format );
	vswprintf( q, l+1, _Format, _ArgList );
}

bool MysqlQuery::getNextRow()
{
	if( !result ) return false;
	row = mysql_fetch_row( result );
	if( row ) return true;
	return false;
}

char *MysqlQuery::getFieldStr( int idx )
{
	if( !row || !result ) return NULL_RET;
	char *ret = NULL;
	if( idx>=0 && idx<=(num_fields-1) ) ret = row[idx];
	if( !ret ) return NULL_RET; // row[idx] can still be NULL if MySQL table specs ALLOW NULL
	return ret;
}

char *MysqlQuery::getFieldStr( const wchar_t *field_name )
{
	if( !field_name ) return NULL_RET;
	size_t flen = wcslen( field_name );
	char *aname = (char *)malloc( flen+1 );
	if( !aname ) return NULL_RET;
	l2c_unicode_to_ansi( field_name, aname, flen );
	char *ret = getFieldStr( aname );
	free( aname );
	aname = NULL;
	return ret;
}

char *MysqlQuery::getFieldStr( const char *field_name )
{
	if( !field_name ) return NULL_RET;
	if( !result || !row ) return NULL_RET;
	if( num_fields < 1 ) return NULL_RET;
	MYSQL_FIELD *fields = mysql_fetch_fields( result );
	if( !fields ) return NULL_RET;
	int i;
	int idx = -1;
	for( i=0; i<num_fields; i++ )
	{
		if( _stricmp( fields[i].name, field_name ) == 0 )
		{
			idx = i;
			break;
		}
	}
	if( idx == -1 ) return NULL_RET;
	if( row[idx] == NULL ) return NULL_RET; // row[idx] can still be NULL if MySQL table specs ALLOW NULL
	return row[idx];
}

int MysqlQuery::getFieldInt( int idx )
{
	int i = 0;
	char *sret = getFieldStr( idx );
	//if( sret == NULL_RET ) return false;
	int r = sscanf( sret, "%d", &i );
	if( r != 1 ) throw L2C_Exception( "MysqlQuery::getFieldInt(): cannot convert [%s] to int", sret );
	return i;
}

unsigned int MysqlQuery::getFieldUInt( int idx )
{
	unsigned int i = 0;
	char *sret = getFieldStr( idx );
	//if( sret == NULL_RET ) return false;
	int r = sscanf( sret, "%u", &i );
	if( r != 1 ) throw L2C_Exception( "MysqlQuery::getFieldUInt(): cannot convert [%s] to unsigned int", sret );
	return i;
}

long long int MysqlQuery::getFieldInt64( int idx )
{
	long long int i = 0;
	char *sret = getFieldStr( idx );
	//if( sret == NULL_RET ) return false;
	int r = sscanf( sret, "%I64d", &i );
	if( r != 1 ) throw L2C_Exception( "MysqlQuery::getFieldInt64(): cannot convert [%s] to int64", sret );
	return true;
}

unsigned long long int MysqlQuery::getFieldUInt64( int idx )
{
	unsigned long long int i = 0;
	char *sret = getFieldStr( idx );
	//if( sret == NULL_RET ) return false;
	int r = sscanf( sret, "%I64u", &i );
	if( r != 1 ) throw L2C_Exception( "MysqlQuery::getFieldUInt64(): cannot convert [%s] to unsigned int64", sret );
	return i;
}

double MysqlQuery::getFieldDouble( int idx )
{
	double i = 0.0;
	char *sret = getFieldStr( idx );
	//if( sret == NULL_RET ) return false;
	int r = sscanf( sret, "%lf", &i );
	if( r != 1 ) throw L2C_Exception( "MysqlQuery::getFieldDouble(): cannot convert [%s] to double", sret );
	return i;
}

bool MysqlQuery::getFieldBool( int idx )
{
	bool i = false;
	char *sret = getFieldStr( idx );
	if( sret == NULL_RET ) return false;
	if( _stricmp( sret, "true" ) == 0 ) { i = true; return true; }
	if( _stricmp( sret, "false" ) == 0 ) { i = false; return true; }
	int ii = 0;
	int r = sscanf( sret, "%d", &ii );
	if( r != 1 ) throw L2C_Exception( "MysqlQuery::getFieldBool(): cannot convert [%s] to bool", sret );
	i = (ii != 0);
	return i;
}


int MysqlQuery::getFieldInt( const char *field_name )
{
	int i = 0;
	char *sret = getFieldStr( field_name );
	//if( sret == NULL_RET ) return false;
	int r = sscanf( sret, "%d", &i );
	if( r != 1 ) throw L2C_Exception( "MysqlQuery::getFieldInt(): cannot convert [%s] to int", sret );
	return i;
}

unsigned int MysqlQuery::getFieldUInt( const char *field_name )
{
	unsigned int i = 0;
	char *sret = getFieldStr( field_name );
	//if( sret == NULL_RET ) return false;
	int r = sscanf( sret, "%u", &i );
	if( r != 1 ) throw L2C_Exception( "MysqlQuery::getFieldUInt(): cannot convert [%s] to unsigned int", sret );
	return i;
}

long long int MysqlQuery::getFieldInt64( const char *field_name )
{
	long long int i = 0;
	char *sret = getFieldStr( field_name );
	//if( sret == NULL_RET ) return false;
	int r = sscanf( sret, "%I64d", &i );
	if( r != 1 ) throw L2C_Exception( "MysqlQuery::getFieldInt64(): cannot convert [%s] to int64", sret );
	return i;
}

unsigned long long int MysqlQuery::getFieldUInt64( const char *field_name )
{
	unsigned long long int i = 0;
	char *sret = getFieldStr( field_name );
	//if( sret == NULL_RET ) return false;
	int r = sscanf( sret, "%I64u", &i );
	if( r != 1 ) throw L2C_Exception( "MysqlQuery::getFieldUInt64(): cannot convert [%s] to unsigned int64", sret );
	return i;
}

double MysqlQuery::getFieldDouble( const char *field_name )
{
	double i = 0.0;
	char *sret = getFieldStr( field_name );
	//if( sret == NULL_RET ) return false;
	int r = sscanf( sret, "%lf", &i );
	if( r != 1 ) throw L2C_Exception( "MysqlQuery::getFieldDouble(): cannot convert [%s] to double", sret );
	return i;
}

bool MysqlQuery::getFieldBool( const char *field_name )
{
	bool i = false;
	char *sret = getFieldStr( field_name );
	if( sret == NULL_RET ) return false;
	if( _stricmp( sret, "true" ) == 0 ) { i = true; return true; }
	if( _stricmp( sret, "false" ) == 0 ) { i = false; return true; }
	int ii = 0;
	int r = sscanf( sret, "%d", &ii );
	if( r != 1 ) throw L2C_Exception( "MysqlQuery::getFieldBool(): cannot convert [%s] to bool", sret );
	i = (ii != 0);
	return i;
}


// get field content as unicode strings

void MysqlQuery::getFieldStrW( int idx, wchar_t *out, size_t maxCount )
{
	char *ret = getFieldStr( idx );
	l2c_ansi_to_unicode( ret, out, maxCount );
}

void MysqlQuery::getFieldStrW( const char *field_name, wchar_t *out, size_t maxCount )
{
	char *ret = getFieldStr( field_name );
	l2c_ansi_to_unicode( ret, out, maxCount );
}

void MysqlQuery::getFieldStrW( const wchar_t *field_name, wchar_t *out, size_t maxCount )
{
	char *ret = getFieldStr( field_name );
	l2c_ansi_to_unicode( ret, out, maxCount );
}





MysqlConnection::MysqlConnection()
{
	mysql_init( &m_mysql );
	m_connected = false;
	m_errstr[0] = 0;
	return;
}

MysqlConnection::~MysqlConnection()
{
	close();
}

char *MysqlConnection::escapeStringMalloc( const char *in )
{
	if( !in ) return NULL;
	size_t ll = strlen( in );
	char *out = (char *)malloc( ll*2 + 2 );
	if( !out ) return NULL;
	if( m_connected )
		mysql_real_escape_string( &m_mysql, out, in, ll );
	else
		mysql_escape_string( out, in, ll );
	return out;
}

bool MysqlConnection::connect( const wchar_t *server, const wchar_t *user, const wchar_t *pass, const wchar_t *db )
{
	if( m_connected ) return false;
	// set option
	my_bool bVal = 1;
	int oret = mysql_options( &m_mysql, MYSQL_OPT_RECONNECT, (const char *)&bVal );
	if( oret != 0 ) return false;
	// convert strings
	char aserver[256] = {0};
	char auser[256] = {0};
	char apass[256] = {0};
	char adb[256] = {0};
	l2c_unicode_to_ansi( server, aserver, 255 );
	l2c_unicode_to_ansi( user, auser, 255 );
	l2c_unicode_to_ansi( pass, apass, 255 );
	l2c_unicode_to_ansi( db, adb, 255 );
	// attempt connect
	MYSQL *ret = mysql_real_connect( &m_mysql, aserver, auser, apass, adb, 3306, NULL,
		/*CLIENT_FOUND_ROWS |*/ CLIENT_INTERACTIVE | CLIENT_MULTI_STATEMENTS );
	if( ret == NULL || ret != &m_mysql ) return false;
	m_connected = true;
	return true;
}

bool MysqlConnection::close()
{
	if( !m_connected ) return false;
	mysql_close( &m_mysql );
	m_connected = false;
	return true;
}

const wchar_t *MysqlConnection::getErrorStr()
{
	char *aerr = _strdup( mysql_error( &m_mysql ) );
	wcscpy( m_errstr, L"<none>" );
	if( aerr ) l2c_ansi_to_unicode( aerr, m_errstr, 255 );
	return (const wchar_t *)m_errstr;
}

bool MysqlConnection::isConnected()
{
	if( m_connected )
	{
		int ret = mysql_ping( &m_mysql );
		if( ret != 0 )
		{
			close();
			return false;
		}
		return true;
	}
	return false;
}

bool MysqlConnection::executeQuery( MysqlQuery& q )
{
	if( !m_connected ) return false;
	if( !q.q ) return false;
	size_t qlen = wcslen( q.q );
	char *a_query = (char *)malloc( qlen + 1 );
	if( !a_query ) return false;
	l2c_unicode_to_ansi( q.q, a_query, qlen+1 );
	int q_res = mysql_query( &m_mysql, a_query );
	free( a_query );
	a_query = NULL;
	if( q_res == 0 )
	{
		// no error
		q.result = mysql_store_result( &m_mysql );
		if( q.result )
		{
			q.num_fields = (int)mysql_num_fields( q.result );
			q.num_rows = (int)mysql_num_rows( q.result );
		}
		else
		{
			q.num_fields = mysql_field_count( &m_mysql );
			// query was not a SELECT
			if( q.num_fields == 0 )
				q.num_rows = (int)mysql_affected_rows( &m_mysql );
		}
		// no error
		return true;
	}
	// error
	return false;
}









// memory for static variable
bool MysqlConnectionManager::m_wasLibraryInit = false;

MysqlConnectionManager::MysqlConnectionManager()
{
	m_initialized = false;
	m_server[0] = m_user[0] = m_pass[0] = m_db[0] = 0;
	m_maxConnections = m_numActiveConnections = 0;
	m_connections = NULL;
}

MysqlConnectionManager::~MysqlConnectionManager()
{
	closeAllConnections();
}

bool MysqlConnectionManager::initialize( int maxConnections, const wchar_t *server, const wchar_t *user,
		const wchar_t *pass, const wchar_t *db )
{
	if( m_initialized ) return true;
	// init lock
	InitializeCriticalSectionAndSpinCount( &m_lock, 10 );
	Lock();
	// init mysql library?
	if( !MysqlConnectionManager::m_wasLibraryInit )
	{
		mysql_server_init( 0, NULL, NULL );
		MysqlConnectionManager::m_wasLibraryInit = true;
	}
	// try allocate
	m_connections = (MysqlConnection **)malloc( maxConnections * sizeof(MysqlConnection *) );
	if( !m_connections )
	{
		Unlock();
		return false;
	}
	m_connbusy = (bool *)malloc( maxConnections * sizeof(bool) );
	if( !m_connbusy )
	{
		free( m_connections );
		m_connections = NULL;
		Unlock();
		return false;
	}
	// init nulls
	int i;
	for( i=0; i<maxConnections; i++ )
	{
		m_connections[i] = NULL;
		m_connbusy[i] = false;
	}
	// copy data
	wcscpy( m_server, server );
	wcscpy( m_user, user );
	m_pass[0] = 0;
	if( pass ) wcscpy( m_pass, pass );
	wcscpy( m_db, db );
	m_maxConnections = maxConnections;
	//
	m_initialized = true;
	Unlock();
	return true;
}

void MysqlConnectionManager::closeAllConnections()
{
	if( !m_initialized ) return;
	Lock();
	int i;
	for( i=0; i<m_maxConnections; i++ )
	{
		if( m_connections[i] )
		{
			m_numActiveConnections--;
			m_connections[i]->close();
			delete m_connections[i];
			m_connections[i] = NULL;
			m_connbusy[i] = false;
		}
	}
	free( m_connections );
	m_connections = NULL;
	free( m_connbusy );
	m_connbusy = NULL;
	m_initialized = false;
	Unlock();
	DeleteCriticalSection( &m_lock );
}

MysqlConnection *MysqlConnectionManager::getConnection()
{
	MysqlConnection *ret = NULL;
	int i = 0;
	Lock();
	for( i=0; i<m_maxConnections; i++ )
	{
		if( !m_connections[i] )
		{
			m_connections[i] = new MysqlConnection();
			m_connections[i]->connect( m_server, m_user, m_pass, m_db );
			m_connbusy[i] = true;
			ret = m_connections[i];
			m_numActiveConnections++;
			break;
		}
		if( m_connections[i] && !m_connbusy[i] )
		{
			if( !m_connections[i]->isConnected() )
				m_connections[i]->connect( m_server, m_user, m_pass, m_db );
			m_connbusy[i] = true;
			ret = m_connections[i];
			m_numActiveConnections++;
			break;
		}
	}
	Unlock();
	return ret;
}

bool MysqlConnectionManager::releaseConnection( MysqlConnection *con )
{
	if( !con ) return false;
	int i = 0;
	bool ret = false;
	Lock();
	for( i=0; i<m_maxConnections; i++ )
	{
		if( m_connections[i] == con )
		{
			m_connbusy[i] = false;
			// close connections with index > 0
			if( i > 0 ) m_connections[i]->close();
			// at least 1 conn with idx 0 will be active all the time
			m_numActiveConnections--;
			ret = true;
			break;
		}
	}
	Unlock();
	return ret;
}

int MysqlConnectionManager::getCountActiveConnections() const
{
	return m_numActiveConnections;
}

int MysqlConnectionManager::getCountMaxConnections() const
{
	return m_maxConnections;
}

void MysqlConnectionManager::Lock()
{
	EnterCriticalSection( &m_lock );
}

void MysqlConnectionManager::Unlock()
{
	LeaveCriticalSection( &m_lock );
}


