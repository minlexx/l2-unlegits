#pragma once

class FailedLoginAttempt
{
public:
	FailedLoginAttempt();
	FailedLoginAttempt( const char *lastPassword );
	FailedLoginAttempt( const FailedLoginAttempt& other );
	const FailedLoginAttempt& operator=( const FailedLoginAttempt& other );

public:
	void increaseCounter( const char *password );
	int getCount() const;

protected:
	int         m_count;
	time_t      m_lastAttemptTime;
	std::string m_lastPassword;
};
