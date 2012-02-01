#pragma once
#include "Debugging.h"

class Exception
{
public:
	Exception();
	Exception( const char *_Format, ... );
	Exception( const Exception& other );
	virtual ~Exception();

	const Exception& operator=( const Exception& other );

public:
	virtual void setWhat( const char *_Format, ... );
	virtual const char *what() const;
	virtual void logStackTrace();

protected:
	char            *m_what;
	DebugStackTrace *m_trace;
};
