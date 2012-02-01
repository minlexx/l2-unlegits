#pragma once
#include "l2c_utils.h"

class L2C_UTILS_API L2C_Exception
{
public:
	L2C_Exception();
	L2C_Exception( const char *_Format, ... );
	~L2C_Exception();
public:
	const char *what() const;
protected:
	char *m_what;
};

class L2C_UTILS_API MysqlException: public L2C_Exception
{
public:
	MysqlException( void *pvMysql );
};
