#pragma once

class StatsSet
{
public:
	StatsSet();
	~StatsSet();
	StatsSet( const StatsSet& other );
	const StatsSet& operator=( const StatsSet& other );

public:
	bool getInt(     const char *name, int *val );
	bool getInt(     const char *name, int *val, int defVal );
	bool getUInt(    const char *name, unsigned int *val );
	bool getUInt(    const char *name, unsigned int *val, unsigned int defVal );
	bool getInt64(   const char *name, long long int *val );
	bool getInt64(   const char *name, long long int *val, long long int defVal );
	bool getUInt64(  const char *name, unsigned long long int *val );
	bool getUInt64(  const char *name, unsigned long long int *val, unsigned long long int defVal );
	bool getDouble(  const char *name, double *val );
	bool getDouble(  const char *name, double *val, double defVal );
	bool getString(  const char *name, std::string& val );
	bool getString(  const char *name, std::string& val, std::string& defVal );
	bool getWString( const char *name, std::wstring& val );
	bool getWString( const char *name, std::wstring& val, std::wstring& defVal );
	bool getBool(    const char *name, bool *val );
	bool getBool(    const char *name, bool *val, bool defVal );

public:
	bool setInt(     const char *name, int val );
	bool setUInt(    const char *name, unsigned int val );
	bool setInt64(   const char *name, long long int val );
	bool setUInt64(  const char *name, unsigned long long int val );
	bool setDouble(  const char *name, double val );
	bool setString(  const char *name, std::string& val );
	bool setString(  const char *name, const char *val );
	bool setWString( const char *name, std::wstring& val );
	bool setWString( const char *name, const wchar_t *val );
	bool setBool(    const char *name, bool val );

protected:
	// map of pair: <"name", L"value">
	std::map<std::string, std::wstring> m_map;
};
