#include "pch.h"
#include "Log.h"
#include "Exception.h"

Exception::Exception()
{
	m_what = NULL;
	m_trace = new DebugStackTrace();
	Debug::getInstance()->createStackTrace( (*m_trace) );
}

Exception::Exception( const char *_Format, ... )
{
	m_what = NULL;
	m_trace = new DebugStackTrace();
	Debug::getInstance()->createStackTrace( (*m_trace) );
	//
	va_list _ArgPtr;
	va_start( _ArgPtr, _Format );
	// _vscprintf(): Returns the number of characters in the formatted string using a pointer to a list of arguments.
	size_t what_len = _vscprintf( _Format, _ArgPtr );
	m_what = (char *)malloc( what_len + 16 );
	if( m_what )
	{
		va_start( _ArgPtr, _Format );
		vsprintf( m_what, _Format, _ArgPtr );
	}
}

Exception::Exception( const Exception& other )
{
	m_what = NULL;
	m_trace = new DebugStackTrace();
	Debug::getInstance()->createStackTrace( (*m_trace) );
	this->operator=( other );
}

Exception::~Exception()
{
	if( m_what )
	{
		free( m_what );
		m_what = NULL;
	}
	if( m_trace )
	{
		delete m_trace;
		m_trace = NULL;
	}
}

const Exception& Exception::operator=( const Exception& other )
{
	if( this == &other ) return (*this);
	if( m_what )
	{
		free( m_what );
		m_what = NULL;
	}
	if( other.m_what ) m_what = _strdup( other.m_what );
	return (*this);
}

const char *Exception::what() const
{
	if( !m_what ) return "(no comment)";
	return m_what;
}

void Exception::setWhat( const char *_Format, ... )
{
	if( m_what )
	{
		free( m_what );
		m_what = NULL;
	}
	va_list _ArgPtr;
	va_start( _ArgPtr, _Format );
	// _vscprintf(): Returns the number of characters in the formatted string using a pointer to a list of arguments.
	size_t what_len = _vscprintf( _Format, _ArgPtr );
	m_what = (char *)malloc( what_len + 16 );
	if( m_what )
	{
		va_start( _ArgPtr, _Format );
		vsprintf( m_what, _Format, _ArgPtr );
	}
}

void Exception::logStackTrace()
{
	if( !m_trace )
	{
		Log( L"Exception: no stack trace!" );
		return;
	}
	m_trace->logStackTrace();
}

