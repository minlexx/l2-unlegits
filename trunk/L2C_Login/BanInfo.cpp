#include "pch.h"
#include "BanInfo.h"

BanInfo::BanInfo()
{
	m_ipAddress.clear();
	m_expiration = 0;
}

BanInfo::BanInfo( const BanInfo& other )
{
	this->operator=( other );
}

BanInfo::BanInfo( const char *ipAddress, time_t expiration )
{
	m_ipAddress = ipAddress;
	m_expiration = expiration;
}

const BanInfo& BanInfo::operator=( const BanInfo& other )
{
	if( this == &other ) return (*this);
	this->m_ipAddress = other.m_ipAddress;
	this->m_expiration = other.m_expiration;
	return (*this);
}

const char *BanInfo::getAddress() const
{
	return m_ipAddress.c_str();
}

bool BanInfo::hasExpired() const
{
	time_t nowTime = time( NULL );
	if( m_expiration > 0 && nowTime > m_expiration ) return true;
	return false;
}

