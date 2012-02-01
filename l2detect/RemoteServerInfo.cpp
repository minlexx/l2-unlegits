#include "stdafx.h"
#include "RemoteServerInfo.h"

RemoteServerInfo::RemoteServerInfo(void)
{
	_tcscpy( str_desc, TEXT("[no_desc]") );
	strcpy( str_ip, "0.0.0.0" );
	memset( m_desc, 0, sizeof(m_desc) );
	memset( m_ip, 0, sizeof(m_ip) );
	m_port = 0;
}

RemoteServerInfo::RemoteServerInfo( const RemoteServerInfo& other )
{
	this->setDesc( other.desc() );
	this->setIP( other.ip() );
	this->setPort( other.port() );
}

RemoteServerInfo &RemoteServerInfo::operator=( const RemoteServerInfo& other )
{
	this->setDesc( other.desc() );
	this->setIP( other.ip() );
	this->setPort( other.port() );
	return (*this);
}

RemoteServerInfo::~RemoteServerInfo(void)
{
	m_desc[0] = 0;
	m_ip[0] = 0;
	m_port = 0;
}


LPCTSTR RemoteServerInfo::desc() const
{
	if( m_desc[0] == 0 ) return str_desc;
	((RemoteServerInfo *)this)->m_desc[127] = 0; // close string for sure
	return m_desc;
}

const char *RemoteServerInfo::ip() const
{
	if( m_ip[0] == 0 ) return str_ip;
	((RemoteServerInfo *)this)->m_ip[15] = 0; // close string for sure
	return m_ip;
}

void RemoteServerInfo::setDesc( LPCTSTR desc )
{
	m_desc[0] = 0;
	if( !desc ) return;
	_tcsncpy( m_desc, desc, 127 );
}

void RemoteServerInfo::setIP( const char *ip )
{
	m_ip[0] = 0;
	if( !ip ) return;
	strncpy( m_ip, ip, 15 );
}
