#pragma once

class BanInfo
{
public:
	BanInfo();
	BanInfo( const BanInfo& other );
	BanInfo( const char *ipAddress, time_t expiration );
	const BanInfo& operator=( const BanInfo& other );
public:
	const char *getAddress() const;
	bool hasExpired() const;
protected:
	std::string m_ipAddress;
	time_t m_expiration;
};
