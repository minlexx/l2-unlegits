#include "pch.h"
#include "l2c_lock.h"

CriticalSection::CriticalSection()
{
	m_locked = false;
	InitializeCriticalSectionAndSpinCount( &m_lock, 10 );
}

CriticalSection::~CriticalSection()
{
	if( m_locked ) LeaveCriticalSection( &m_lock );
	m_locked = false;
	DeleteCriticalSection( &m_lock );
}

void CriticalSection::Lock()
{
	if( m_locked ) return;
	EnterCriticalSection( &m_lock );
	m_locked = true;
}

void CriticalSection::Unlock()
{
	if( !m_locked ) return;
	LeaveCriticalSection( &m_lock );
	m_locked = false;
}

bool CriticalSection::TryLock( unsigned int wait_milliseconds )
{
	if( m_locked ) return true;
	if( TryEnterCriticalSection( &m_lock ) ) return true;
	if( wait_milliseconds > 0 )
	{
		Sleep( (DWORD)wait_milliseconds );
		if( TryEnterCriticalSection( &m_lock ) ) return true;
	}
	return false;
}

bool CriticalSection::isLocked() const
{
	return m_locked;
}
