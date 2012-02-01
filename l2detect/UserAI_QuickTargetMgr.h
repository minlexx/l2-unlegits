#pragma once
#include "UserAI_QuickTargetSetup.h"

class UserAI_QuickTargetManager
{
public:
	struct PLAYER_PRI_INFO
	{
		L2Player *player_ptr;
		double    distance;
		int       class_pri;
		int       total_pri;
	};
	typedef std::list<UserAI_QuickTargetManager::PLAYER_PRI_INFO> PlayerPriorityList;

public:
	UserAI_QuickTargetManager();
	~UserAI_QuickTargetManager();
	void clear();

public:
	void refrestCurrentTargetsList( const UserAI_QuickTargetSetup *cfg );
	const PlayerPriorityList& getListAll() const;
	const PlayerPriorityList& getListEnemy() const;

protected:
	void createInitialList( const UserAI_QuickTargetSetup *cfg );
	//void createTargetListByDistance( const UserAI_QuickTargetSetup *cfg );
	//void createTargetListByPriority( const UserAI_QuickTargetSetup *cfg );
	void createTargetListUseBoth( const UserAI_QuickTargetSetup *cfg );

protected:
	PlayerPriorityList m_list_all;
	PlayerPriorityList m_list_enemy;
};
