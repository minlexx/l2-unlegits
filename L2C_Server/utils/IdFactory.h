#pragma once
#include "l2c_utils.h"

// warning C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#pragma warning (disable: 4290)

class Exception;

class IdFactory
{
public:
	static const unsigned int MIN_OBJECT_ID = 0x10000000;
	static const unsigned int MAX_OBJECT_ID = 0x7FFFFFFF;
	static const unsigned int OBJECT_ID_COUNT = MAX_OBJECT_ID - MIN_OBJECT_ID + 1;

public:
	class NoFreeObjectIdException: public std::exception {};

public:
	IdFactory();
	~IdFactory();
	bool init();
	void clear();

public:
	unsigned int getUsedCount();
	unsigned int getNextObjectId() throw(NoFreeObjectIdException);
	bool         releaseObjectId( unsigned int o );
	bool         isUsedObjectId( unsigned int o );

public:
	unsigned long long int getUsedMemoryBytes() const;

protected:
	unsigned int  findUnusedOid() const;
	void          fillObjectIdsFromDB();
	void          fillObjectIdsFromDB_insertOne( unsigned int oid );

protected:
	std::bitset<OBJECT_ID_COUNT> *m_bitset;
	unsigned long long int        m_usedMemory;
	CriticalSection               m_lock;
	unsigned int                  m_lastOid;
};
