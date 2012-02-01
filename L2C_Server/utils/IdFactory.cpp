#include "pch.h"
#include "IdFactory.h"
#include "GS.h"
#include "Log.h"
//#include "Exception.h"

IdFactory::IdFactory()
{
	m_bitset = NULL;
	m_usedMemory = 0;
	m_lastOid = MIN_OBJECT_ID;
}

IdFactory::~IdFactory()
{
	clear();
}

bool IdFactory::init()
{
	if( m_bitset ) return false;
	m_lock.Lock();
	MEMORYSTATUSEX ms;
	ms.dwLength = sizeof(ms);
	GlobalMemoryStatusEx( &ms );
	unsigned long long mem_before = ms.ullAvailVirtual;
	unsigned long long mem_after  = 0;
	m_bitset = new std::bitset<OBJECT_ID_COUNT>();
	if( m_bitset )
	{
		ms.dwLength = sizeof(ms);
		GlobalMemoryStatusEx( &ms );
		mem_after = ms.ullAvailVirtual;
		m_usedMemory = mem_before - mem_after;
		m_lastOid = MIN_OBJECT_ID;
		// restore all currently used objectId's
		fillObjectIdsFromDB();
		m_lock.Unlock();
		return true;
	}
	m_lock.Unlock();
	return false;
}

void IdFactory::clear()
{
	m_lock.Lock();
	if( m_bitset )
	{
		delete m_bitset;
		m_bitset = NULL;
		m_usedMemory = 0;
	}
	m_lastOid = MIN_OBJECT_ID;
	m_lock.Unlock();
}

unsigned int IdFactory::getUsedCount()
{
	if( !m_bitset ) return 0;
	unsigned int r = 0;
	m_lock.Lock();
	r = m_bitset->count();
	m_lock.Unlock();
	return r;
}

unsigned int IdFactory::getNextObjectId() throw(NoFreeObjectIdException)
{
	if( !m_bitset ) return 0;
	m_lock.Lock();
	unsigned int o = findUnusedOid();
	if( o == 0 )
	{
		m_lock.Unlock();
		throw NoFreeObjectIdException();
	}
	m_bitset->set( o-MIN_OBJECT_ID );
	m_lastOid = o+1; // next time try to give next objectId
	if( m_lastOid > MAX_OBJECT_ID ) m_lastOid = MIN_OBJECT_ID;
	m_lock.Unlock();
	return o;
}

bool IdFactory::releaseObjectId( unsigned int o )
{
	if( !m_bitset ) return false;
	m_lock.Lock();
	bool ret = false;
	if( m_bitset->test( o ) )
	{
		ret = true;
		m_bitset->reset( o );
	}
	m_lock.Unlock();
	return ret;
}

bool IdFactory::isUsedObjectId( unsigned int o )
{
	if( !m_bitset ) return false;
	bool ret = false;
	m_lock.Lock();
	ret = m_bitset->test( o );
	m_lock.Unlock();
	return ret;
}

unsigned long long int IdFactory::getUsedMemoryBytes() const
{
	return m_usedMemory;
}

unsigned int IdFactory::findUnusedOid() const
{
	if( !m_bitset ) return 0;
	// lock should be locked!
	unsigned int o = m_lastOid;
	unsigned int start_o = o;
	while( o >= MIN_OBJECT_ID )
	{
#ifdef _DEBUG_IDFACTORY_INT
		try
		{
#endif /*_DEBUG_IDFACTORY_INT*/
			if( !m_bitset->test( o-MIN_OBJECT_ID ) ) return o;
#ifdef _DEBUG_IDFACTORY_INT
		}
		catch( std::exception& e )
		{
			printf( "std::exception: %s\n", e.what() );
			printf( "o = %u (m_lastOid = %u, start_o = %u)\n", o, m_lastOid, start_o );
		}
#endif /*_DEBUG_IDFACTORY_INT*/
		o++;
		if( o > MAX_OBJECT_ID ) o = MIN_OBJECT_ID;
		if( o == start_o ) break; // search reached start point
	}
	return 0;
}

void IdFactory::fillObjectIdsFromDB()
{
	MysqlConnection *con = GameServer::getInstance()->getDBConnection();
	MysqlQuery q;
	//"SELECT charId FROM characters"
	//"SELECT object_id FROM items"
	//"SELECT object_id FROM itemsonground
	//"SELECT clan_id FROM clan_data"
	q.create( L"SELECT charId FROM characters UNION SELECT object_id FROM items "
		L"UNION SELECT object_id FROM itemsonground UNION SELECT clan_id FROM clan_data" );
	if( con->executeQuery( q ) )
	{
		while( q.getNextRow() )
		{
			unsigned int oid = q.getFieldUInt( 0 );
			fillObjectIdsFromDB_insertOne( oid );
		}
	}
	else
	{
		LogError( L"IdFactory::fillObjectIdsFromDB(): MySQL ERROR: %s\n", con->getErrorStr() );
	}

	GameServer::getInstance()->releaseDBConnection( con );
}

void IdFactory::fillObjectIdsFromDB_insertOne( unsigned int oid )
{
	if( oid >= IdFactory::MIN_OBJECT_ID  &&  oid <= IdFactory::MAX_OBJECT_ID )
	{
		m_bitset->set( oid - IdFactory::MIN_OBJECT_ID );
	}
	else
		LogError( L"IdFactory: RESTORE: oid [%u] is not between MIN_OBJECT_ID[%u] and MAX_OBJECT_ID[%u]!\n",
			oid, IdFactory::MIN_OBJECT_ID, IdFactory::MAX_OBJECT_ID );
}
