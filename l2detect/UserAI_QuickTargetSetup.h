#pragma once
#include "ClassPri.h"
#include "DistPri.h"
#include "UserAI_QuickTargetOptions.h"

class UserAI_QuickTargetSetup
{
public:
	UserAI_QuickTargetSetup();
	~UserAI_QuickTargetSetup();
	void clear();

public:
	ClassPriorities    *getClassPriorities();
	DistancePriorities *getDistancePriorities();
	const ClassPriorities    *getClassPriorities() const;
	const DistancePriorities *getDistancePriorities() const;

	int  getKey() const;
	void setKey( int vk_key );

	
	int  getAnnounceTargetInPartyChat() const;
	void setAnnounceTargetInPartyChat( int ann );

	int  getMaxDist() const;
	void setMaxDist( int maxDist );

public:
	bool loadFromFile( const char *fn );
	bool saveToFile( const char *fn );

protected:
	ClassPriorities    m_classPri;
	DistancePriorities m_distPri;
	// common quick target options
	int  m_vk_key;
	int  m_max_dist;
	int  m_announce_target_in_party_chat;
public:
	UserAI_QuickTargetOptions  opts_all;
	UserAI_QuickTargetOptions  opts_enemy;
};
