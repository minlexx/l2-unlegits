#include "stdafx.h"
#include "UserAI_QuickTargetMgr.h"
#include "CharArray.h"
#include "GameClient.h"

extern class GameClient *g_game_client;

UserAI_QuickTargetManager::UserAI_QuickTargetManager()
{
	clear();
}

UserAI_QuickTargetManager::~UserAI_QuickTargetManager()
{
	clear();
}

void UserAI_QuickTargetManager::clear()
{
	m_list_all.clear();
	m_list_enemy.clear();
}

const UserAI_QuickTargetManager::PlayerPriorityList& UserAI_QuickTargetManager::getListAll() const
{
	return m_list_all;
}

const UserAI_QuickTargetManager::PlayerPriorityList& UserAI_QuickTargetManager::getListEnemy() const
{
	return m_list_enemy;
}

void UserAI_QuickTargetManager::refrestCurrentTargetsList( const UserAI_QuickTargetSetup *cfg )
{
	// first lock chars array to prevent deleting from it before final list is created
	CharArray_Lock();
	// create targets list
	createInitialList( cfg );
	createTargetListUseBoth( cfg );
	// allow to modify chars array
	CharArray_Unlock();
}

void UserAI_QuickTargetManager::createInitialList( const UserAI_QuickTargetSetup *cfg )
{
	UNREFERENCED_PARAMETER( cfg ); // TODO: remove this after method complete
	/*int i = 0, nChars = 0, nMaxChars = CharArray_GetCount();
	m_init_list.clear();
	int usr_x = g_game_client->ai.usr.x;
	int usr_y = g_game_client->ai.usr.y;
	const ClassPriorities *pri_cfg = (const_cast<UserAI_QuickTargetSetup *>(cfg))->getClassPriorities();
	for( i=0; i<CHARARRAY_MAX_CHARS; i++ )
	{
		if( !chars_array[i] ) continue;
		L2Player *pl = chars_array[i];
		// detect clan war state
		bool isEnemy = false, is2side = false;
		utils_detectClanWar( pl->relation, &isEnemy, &is2side );
		bool allow_target = false; // initially disable targeting this player
		// targeting ONLY clan wars? clan war is FIRST filter
		if( cfg->opt_targetOnlyCW() )
		{
			if( isEnemy ) // clan wars always can be targeted
			{
				allow_target = true; // allowed targeting only enemies AND player is enemy
			}
			else // not a clan war, but some filters may allow targeting
			{
				if( cfg->opt_allowTargetFlagged() && pl->pvpFlag ) allow_target = true; // allowed to target flagged
				if( cfg->opt_allowTargetPK() && (pl->karma > 0) ) allow_target = true; // allow to target PK
			}
		}
		else // allowed targeting anyone
			allow_target = true;
		if( allow_target ) // clanwar filter allowed targeting
		{
			// filter other options
			// if we here, allow_include is true; but other filters may disable targeting
			if( cfg->opt_dontTargetDead() && pl->isAlikeDead ) allow_target = false;
			if( cfg->opt_dontTargetParty() && g_game_client->ai.party.isInParty( pl->objectID, NULL ) ) allow_target = false;
			if( cfg->opt_dontTargetClan() && g_game_client->ai.usr.clanID == pl->clanID ) allow_target = false;
			if( cfg->opt_dontTargetAlly() && g_game_client->ai.usr.allyID == pl->allyID ) allow_target = false;
		}
		// targeting is allowed, include player in list
		if( allow_target )
		{
			int dx = pl->x - usr_x;
			int dy = pl->y - usr_y;
			PLAYER_PRI_INFO st;
			st.player_ptr   = pl;
			st.distance     = (double)sqrt( (double)(dx*dx) + (double)(dy*dy) );
			st.class_pri    = pri_cfg->getPriForClass( pl->classID );
			st.total_pri    = st.class_pri;
			// also check max distance!!!
			if( st.distance <= (double)cfg->getMaxDist() )
				m_init_list.push_back( st );
		}
		nChars++;
		if( nChars >= nMaxChars ) break;
	}*/
}


/*class dist_comparator:	public std::binary_function<UserAI_QuickTargetManager::PLAYER_PRI_INFO, UserAI_QuickTargetManager::PLAYER_PRI_INFO, bool>
{
public:
	bool operator()( const UserAI_QuickTargetManager::PLAYER_PRI_INFO& _Left, const UserAI_QuickTargetManager::PLAYER_PRI_INFO& _Right ) const
	{
		return (_Left.distance > _Right.distance);
	}
};

class class_pri_comparator:	public std::binary_function<UserAI_QuickTargetManager::PLAYER_PRI_INFO, UserAI_QuickTargetManager::PLAYER_PRI_INFO, bool>
{
public:
	bool operator()( const UserAI_QuickTargetManager::PLAYER_PRI_INFO& _Left, const UserAI_QuickTargetManager::PLAYER_PRI_INFO& _Right ) const
	{
		return (_Left.class_pri > _Right.class_pri);
	}
};*/

class total_pri_comparator:	public std::binary_function<UserAI_QuickTargetManager::PLAYER_PRI_INFO, UserAI_QuickTargetManager::PLAYER_PRI_INFO, bool>
{
public:
	bool operator()( const UserAI_QuickTargetManager::PLAYER_PRI_INFO& _Left, const UserAI_QuickTargetManager::PLAYER_PRI_INFO& _Right ) const
	{
		return (_Left.total_pri > _Right.total_pri);
	}
};

/*void UserAI_QuickTargetManager::createTargetListByDistance( const UserAI_QuickTargetSetup *cfg )
{
	UNREFERENCED_PARAMETER(cfg);
	if( m_init_list.size() < 1 ) return;
	m_init_list.sort( dist_comparator() );
}

void UserAI_QuickTargetManager::createTargetListByPriority( const UserAI_QuickTargetSetup *cfg )
{
	UNREFERENCED_PARAMETER(cfg);
	if( m_init_list.size() < 1 ) return;
	m_init_list.sort( class_pri_comparator() );
}*/

void UserAI_QuickTargetManager::createTargetListUseBoth( const UserAI_QuickTargetSetup *cfg )
{
	UNREFERENCED_PARAMETER(cfg);
	if( m_list_all.size() < 1 ) return;
	// calc total priorities
	const DistancePriorities *dist_pri = cfg->getDistancePriorities();
	UserAI_QuickTargetManager::PlayerPriorityList::iterator iter;
	for( iter = m_list_all.begin();  iter != m_list_all.end();  iter++ )
	{
		iter->total_pri = iter->class_pri + dist_pri->getPriorityForDistance( iter->distance );
	}
	//
	m_list_all.sort( total_pri_comparator() );
}
