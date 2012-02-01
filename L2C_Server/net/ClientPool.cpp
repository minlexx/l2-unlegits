#include "pch.h"
#include "Log.h"
#include "ClientPool.h"
#include "GS.h"
#include "utils/Exception.h"

ClientPool::ClientPool()
{
	m_clients = new std::vector<GameClient *>();
}

ClientPool::~ClientPool()
{
	if( m_clients )
	{
		delete m_clients;
		m_clients = NULL;
	}
}

int ClientPool::getCurrentOnline()
{
	int ret = 0;
	m_lock.Lock();
	if( m_clients ) ret = m_clients->size();
	m_lock.Unlock();
	return ret;
}

bool ClientPool::addClient( SOCKET s_cl, const char *ip, int port )
{
	if( !m_clients || !ip || (s_cl == INVALID_SOCKET) || (port == 0) ) return false;
	bool ret = false;
	m_lock.Lock();
	// check current online
	int max_online = GameServer::getInstance()->getMaxPlayers();
	int cur_online = m_clients->size();
	if( cur_online < max_online )
	{
		// create client
		GameClient *pl = new GameClient( s_cl, ip, port );
		if( pl )
		{
			m_clients->push_back( pl );
			ret = pl->startThread();
		}
	}
	else // maximum online reached
	{
		LogWarning( L"Maximum online count (%d) is reached, cannot accept new clients", max_online );
		ret = false;
		// close client connection
		L2PNet_shutdown( s_cl );
		L2PNet_closesocket( s_cl );
	}
	m_lock.Unlock();
	return ret;
}

void ClientPool::disconnectAllPlayers()
{
	if( !m_clients ) return;
	m_lock.Lock();
	try
	{
		std::vector<GameClient *>::iterator iter;
		for( iter = m_clients->begin(); iter != m_clients->end(); iter++ )
		{
			GameClient *pl = (*iter);
			if( pl )
			{
				pl->disconnectForceAndWaitThreadToStop( true, false );
				delete pl;
			}
		}
		m_clients->clear();
	}
	catch( std::exception& e )
	{
		LogError( L"ClientPool::disconnectAllPlayers(): excpt %S", e.what() );
	}
	catch( Exception& e )
	{
		LogError( L"ClientPool::disconnectAllPlayers(): Exception: %S", e.what() );
		e.logStackTrace();
	}
	m_lock.Unlock();
}

bool ClientPool::disconnectPlayerByAccount( const wchar_t *accountName, bool reasonDualLogin )
{
	if( !m_clients ) return false;
	bool ret = false;
	m_lock.Lock();
	try
	{
		std::vector<GameClient *>::iterator iter;
		for( iter = m_clients->begin(); iter != m_clients->end(); iter++ )
		{
			GameClient *pl = (*iter);
			if( pl )
			{
				if( wcscmp( accountName, pl->getAccount() ) == 0 )
				{
					pl->disconnectForceAndWaitThreadToStop( false, reasonDualLogin );
					m_clients->erase( iter );
					ret = true;
					delete pl;
				}
			}
		}
	}
	catch( std::exception& e )
	{
		LogError( L"ClientPool::disconnectPlayerByAccount( %s ): excpt %S", accountName, e.what() );
	}
	catch( Exception& e )
	{
		LogError( L"ClientPool::disconnectPlayerByAccount(): Exception: %S", e.what() );
		e.logStackTrace();
	}
	m_lock.Unlock();
	return ret;
}

bool ClientPool::notifyClientThreadEnded( GameClient *ptr )
{
	if( !m_clients || !ptr ) return false;
	bool ret = false;
	m_lock.Lock();
	try
	{
		std::vector<GameClient *>::iterator iter;
		for( iter = m_clients->begin(); iter != m_clients->end(); iter++ )
		{
			GameClient *pl = (*iter);
			if( pl == ptr )
			{
#ifdef _DEBUG
				LogDebug( L"ClientPool: deleted player %s", pl->toString() );
#endif
				m_clients->erase( iter );
				ret = true;
				break;
			}
		}
	}
	catch( std::exception& e )
	{
		LogError( L"ClientPool::notifyClientThreadEnded(): excpt %S", e.what() );
	}
	catch( Exception& e )
	{
		LogError( L"ClientPool::notifyClientThreadEnded(): Exception: %S", e.what() );
		e.logStackTrace();
	}
	m_lock.Unlock();
	if( !ret )
	{
		LogWarning( L"ClientPool: not found player to del %s", ptr->toString() );
	}
	return ret;
}

void ClientPool::notifyClientAuthResponse( const wchar_t *accountName, bool ok )
{
	if( !m_clients || !accountName ) return;
	m_lock.Lock();
	try
	{
		std::vector<GameClient *>::iterator iter;
		for( iter = m_clients->begin(); iter != m_clients->end(); iter++ )
		{
			GameClient *pl = (*iter);
			if( pl )
			{
				if( wcscmp( pl->getAccount(), accountName ) == 0 )
				{
					if( !ok ) // client auth failed
					{
						pl->disconnectForceAndWaitThreadToStop( false, false );
						m_clients->erase( iter );
						delete pl;
					}
					else // auth ok
					{
						pl->notifySessionKeysOK();
					}
					break;
				}
			}
		}
	}
	catch( std::exception& e )
	{
		LogError( L"ClientPool::notifyClientAuthResponse(): excpt %S", e.what() );
	}
	catch( Exception& e )
	{
		LogError( L"ClientPool::notifyClientAuthResponse(): Exception: %S", e.what() );
		e.logStackTrace();
	}
	m_lock.Unlock();
}

void ClientPool::dumpClientsToLog()
{
	if( !m_clients ) return;
	int n = 0;
	Log( L"===== ClientPool clients dump =====" );
	m_lock.Lock();
	try
	{
		std::vector<GameClient *>::iterator iter;
		for( iter = m_clients->begin(); iter != m_clients->end(); iter++ )
		{
			GameClient *pl = (*iter);
			if( pl )
			{
				Log( L"  %d: %s", n, pl->toString() );
				n++;
			}
		}
		m_clients->clear();
	}
	catch( std::exception& e )
	{
		LogError( L"ClientPool::dumpClientsToLog(): excpt %S", e.what() );
	}
	catch( Exception& e )
	{
		LogError( L"ClientPool::dumpClientsToLog(): Exception: %S", e.what() );
		e.logStackTrace();
	}
	m_lock.Unlock();
	Log( L"===== ClientPool clients dump end =====" );
}
