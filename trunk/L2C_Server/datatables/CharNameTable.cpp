#include "pch.h"
#include "CharNameTable.h"
#include "GS.h"
#include "Log.h"

CharNameTable *CharNameTable::s_instance = NULL;
int CharNameTable::s_refCount = 0;

CharNameTable *CharNameTable::getInstance()
{
	if( !s_instance )
	{
		s_instance = new CharNameTable();
		s_refCount++;
	}
	return s_instance;
}

void CharNameTable::freeInstance()
{
	if( s_instance == NULL ) return;
	s_refCount--;
	if( s_refCount == 0 )
	{
		delete s_instance;
		s_instance = NULL;
	}
}

CharNameTable::CharNameTable()
{
	//
}

CharNameTable::~CharNameTable()
{
	//
}

void CharNameTable::LockDB()
{
	m_lock.Lock();
}

void CharNameTable::UnlockDB()
{
	m_lock.Unlock();
}

bool CharNameTable::doesCharNameExist( const wchar_t *charName )
{
	bool ret = true;
	m_lock.Lock();
	{
		GameServer *gs = GameServer::getInstance();
		MysqlConnection *con = gs->getDBConnection();
		MysqlQuery q;
		q.create( L"SELECT account_name FROM characters WHERE char_name='%s'", charName );
		if( con->executeQuery( q ) )
		{
			int num_chars = q.getNumRows();
			assert( num_chars >= 0 );
			if( num_chars == 0 ) ret = false;
		}
		else
			LogError( L"CharNameTable::doesCharNameExist(): MySQL ERROR: %s\n", con->getErrorStr() );
		gs->releaseDBConnection( con );
	}
	m_lock.Unlock();
	return ret;
}

int CharNameTable::getAccountCharCount( const wchar_t *accountName )
{
	if( !accountName ) return 0;
	// SELECT COUNT(char_name) FROM characters WHERE account_name=?
	int num_chars = 0;
	m_lock.Lock();
	{
		GameServer *gs = GameServer::getInstance();
		MysqlConnection *con = gs->getDBConnection();
		MysqlQuery q;
		q.create( L"SELECT COUNT(char_name) FROM characters WHERE account_name='%s'", accountName );
		if( con->executeQuery( q ) )
		{
			if( q.getNextRow() ) num_chars = q.getFieldInt( 0 );
		}
		else
			LogError( L"CharNameTable::getAccountCharCount(): MySQL ERROR: %s\n", con->getErrorStr() );
		gs->releaseDBConnection( con );
	}
	m_lock.Unlock();
	return num_chars;
}
