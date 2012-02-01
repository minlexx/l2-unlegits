#pragma once
#include "l2c_utils.h"
#include "l2c_exception.h"

class L2C_UTILS_API MysqlQuery
{
	friend class MysqlConnection;
public:
	MysqlQuery();
	MysqlQuery( const wchar_t *_Format, ... );
	~MysqlQuery();
public:
	void clear();
	void create( const wchar_t *_Format, ... );
	int getNumRows() const;
	int getNumFields() const;
public:
	bool  getNextRow();
	// get field content by field index
	char *getFieldStr( int idx );
	char *getFieldStr( const wchar_t *field_name ); // really needed?
	char *getFieldStr( const char    *field_name );
	// get field content as unicode strings
	void getFieldStrW( int idx,                   wchar_t *out, size_t maxCount );
	void getFieldStrW( const char    *field_name, wchar_t *out, size_t maxCount );
	void getFieldStrW( const wchar_t *field_name, wchar_t *out, size_t maxCount );
	// typecasts
	int                    getFieldInt( int idx );
	unsigned int           getFieldUInt( int idx );
	long long int          getFieldInt64( int idx );
	unsigned long long int getFieldUInt64( int idx );
	double                 getFieldDouble( int idx );
	bool                   getFieldBool( int idx );
	// typecasts - get field by name
	int                    getFieldInt( const char *field_name );
	unsigned int           getFieldUInt( const char *field_name );
	long long int          getFieldInt64( const char *field_name );
	unsigned long long int getFieldUInt64( const char *field_name );
	double                 getFieldDouble( const char *field_name );
	bool                   getFieldBool( const char *field_name );
protected:
	wchar_t                    *q;
	MYSQL_RES                  *result;
	MYSQL_ROW                   row;
	int                         num_rows;
	int                         num_fields;
	char                        NULL_RET[8];
};



class L2C_UTILS_API MysqlConnection
{
	friend class MysqlConnectionManager;
protected:
	MysqlConnection();
	~MysqlConnection();
protected:
	bool connect( const wchar_t *server, const wchar_t *user, const wchar_t *pass, const wchar_t *db );
	bool close();

public:
	char *escapeStringMalloc( const char *in );
	const wchar_t *getErrorStr();
	bool isConnected();
	bool executeQuery( MysqlQuery& q );

protected:
	MYSQL   m_mysql;
	bool    m_connected;
	wchar_t m_errstr[256];
};



class L2C_UTILS_API MysqlConnectionManager
{
public:
	MysqlConnectionManager();
	~MysqlConnectionManager();
	bool initialize( int maxConnections, const wchar_t *server, const wchar_t *user,
		const wchar_t *pass, const wchar_t *db );
	void closeAllConnections();
public:
	MysqlConnection      *getConnection();
	bool                  releaseConnection( MysqlConnection *con );
	int                   getCountActiveConnections() const;
	int                   getCountMaxConnections() const;
protected:
	static bool           m_wasLibraryInit;
	bool                  m_initialized;
	wchar_t               m_server[128];
	wchar_t               m_user[128];
	wchar_t               m_pass[128];
	wchar_t               m_db[128];
	int                   m_maxConnections;
	int                   m_numActiveConnections;
	MysqlConnection     **m_connections;
	bool                 *m_connbusy;
	CRITICAL_SECTION      m_lock;
protected:
	void Lock();
	void Unlock();
};
