#include "stdafx.h"
#include "Logger.h"
#include "UserAI_LockTargetMgr.h"
#include "PacketInjector.h"
#include "GameClient.h"
extern class GameClient *g_game_client;

UserAI_LockTargetMgr::UserAI_LockTargetMgr()
{
	m_toid = 0;
	m_enabled = true;
}

UserAI_LockTargetMgr::~UserAI_LockTargetMgr()
{
	m_toid = 0;
	m_enabled = false;
}

void UserAI_LockTargetMgr::setEnable( bool b )
{
	m_enabled = b;
}

bool UserAI_LockTargetMgr::isEnabled() const
{
	return m_enabled;
}

void UserAI_LockTargetMgr::onUserTargetSelected( unsigned int targetObjectID )
{
	m_toid = targetObjectID;
}

void UserAI_LockTargetMgr::onUserRequestAction( unsigned int actionObjectID )
{
	if( g_game_client->ai.usr.targetObjectID != actionObjectID )
	{
		m_toid = 0;
		log_error( LOG_USERAI, "[LockTarget]: user wishes to change target\n" );
	}
}

void UserAI_LockTargetMgr::onUserRequestUnselectTarget()
{
	log_error( LOG_USERAI, "[LockTarget]: user wishes to unselect target\n" );
	m_toid = 0;
}

void UserAI_LockTargetMgr::onUserTargetLost()
{
	if( !m_enabled ) return;
	if( m_toid > 0 )
	{
		// check existance of prev. user target
		int idx = -1;
		L2OBJECT_TYPE objType = L2OT_NONE;
		if( WorldObjectTree_GetInfoByObjectID( m_toid, &objType, &idx ) )
		{
			// taget PC/NPC with valid index
			if( ((objType == L2OT_PC) || (objType == L2OT_NPC)) && (idx >= 0) )
			{
				// retarget again
				PGen_Action( m_toid, g_game_client->ai.usr.x,
					g_game_client->ai.usr.y, g_game_client->ai.usr.z, 0 );
				log_error( LOG_USERAI, "[LockTarget] retargeting [%u]\n", m_toid );
			}
			else
			{
				log_error( LOG_ERROR, "[LockTarget] onUserTargetLost(): incorrect objType or idx!\n" );
				m_toid = 0;
			}
		}
		else
		{
			m_toid = 0;
			log_error( LOG_USERAI, "[LockTarget]: target lost and does not exist in visible space\n" );
		}
	}
}
