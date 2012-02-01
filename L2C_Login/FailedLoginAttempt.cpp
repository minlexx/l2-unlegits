#include "pch.h"
#include "FailedLoginAttempt.h"

FailedLoginAttempt::FailedLoginAttempt()
{
	m_count = 0;
	m_lastPassword.clear();
	m_lastAttemptTime = 0;
}

FailedLoginAttempt::FailedLoginAttempt( const char *lastPassword )
{
	m_count = 1;
	m_lastPassword = lastPassword;
	m_lastAttemptTime = time( NULL );
}

FailedLoginAttempt::FailedLoginAttempt( const FailedLoginAttempt& other )
{
	this->operator=( other );
}

const FailedLoginAttempt& FailedLoginAttempt::operator=( const FailedLoginAttempt& other )
{
	if( this == &other ) return (*this);
	m_count = other.m_count;
	m_lastPassword = other.m_lastPassword;
	m_lastAttemptTime = other.m_lastAttemptTime;
	return (*this);
}

void FailedLoginAttempt::increaseCounter( const char *password )
{
	std::string new_pass = password;
	time_t new_time = time( NULL );
	if( m_lastPassword != new_pass )
	{
		// check if theres a long time since last wrong try
		if( new_time - m_lastAttemptTime > 300 ) // 5 min
			m_count++;
		else // restart counter
			m_count = 1;
		m_lastPassword = new_pass;
		m_lastAttemptTime = new_time;
	}
	else // trying the same password is not brute force
	{
		m_lastAttemptTime = new_time;
	}
}

int FailedLoginAttempt::getCount() const
{
	return m_count;
}

