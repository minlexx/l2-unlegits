#pragma once

#include "l2c_utils.h"

class L2C_UTILS_API L2C_ConfigFile
{
public:
	static const int MAX_SETTINGS = 256;
public:
	L2C_ConfigFile();
	~L2C_ConfigFile();
public:
	bool load( const wchar_t *filename );
	void clear();
public:
	const wchar_t *getValueStrW( const wchar_t *varName );
	void           getValueStrA( const wchar_t *varName, char *out, size_t maxCount );
	int            getValueInt( const wchar_t *varName );
	long long int  getValueInt64( const wchar_t *varName );
	bool           getValueBool( const wchar_t *varName );
	double         getValueDouble( const wchar_t *varName );
public:
	const wchar_t *getValueStrW( const wchar_t *varName, const wchar_t *defVal );
	void           getValueStrA( const wchar_t *varName, char *out, size_t maxCount, const char *defVal );
	int            getValueInt( const wchar_t *varName, int defVal );
	long long int  getValueInt64( const wchar_t *varName, long long int defVal );
	bool           getValueBool( const wchar_t *varName, bool defVal );
	double         getValueDouble( const wchar_t *varName, double defVal );
public:
	int            getVarsCount() const;
protected:
	int      m_nVars;
	wchar_t *m_varName[MAX_SETTINGS];
	wchar_t *m_varValue[MAX_SETTINGS];
};
