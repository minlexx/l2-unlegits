#include "pch.h"
#include "l2c_utils.h"
#include "l2c_exception.h"
#include <mysql.h>

L2C_Exception::L2C_Exception()
{
	m_what = NULL;
}

L2C_Exception::L2C_Exception( const char *_Format, ... )
{
	va_list _ArgList;
	va_start( _ArgList, _Format );
	int n = _vscprintf( _Format, _ArgList );
	if( n >= 1 )
	{
		m_what = (char *)malloc( n+16 );
		if( m_what )
		{
			va_start( _ArgList, _Format );
			vsprintf( m_what, _Format, _ArgList );
		}
	}
}

L2C_Exception::~L2C_Exception()
{
	if( m_what )
	{
		free( m_what );
		m_what = NULL;
	}
}

const char *L2C_Exception::what() const
{
	return m_what;
}

MysqlException::MysqlException( void *pvMysql )
{
	m_what = NULL;
	MYSQL *mys = (MYSQL *)pvMysql;
	if( mys )
	{
		const char *myserr = mysql_error( mys );
		if( myserr )
			m_what = _strdup( myserr );
	}
}

