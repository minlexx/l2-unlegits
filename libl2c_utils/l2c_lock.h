#pragma once
#include "l2c_utils.h"

class L2C_UTILS_API CriticalSection
{
public:
	CriticalSection();
	~CriticalSection();

public:
	void Lock();
	void Unlock();
	bool TryLock( unsigned int wait_milliseconds = 0 );
	bool isLocked() const;

protected:
	CRITICAL_SECTION m_lock;
	bool             m_locked;
};
