#pragma once

class UserAI_FolAsiCfg
{
public:
	UserAI_FolAsiCfg();
	~UserAI_FolAsiCfg();
public:
	void clear();
public:
	bool loadFromFile( const char *fn );
	bool saveToFile( const char *fn );
public:
	int     m_follow_enable;
	int     m_follow_dist;
	wchar_t m_follow_name[128];
	int     m_follow_only_in_party;
	int     m_assist_enable;
	wchar_t m_assist_name[128];
	int     m_assist_only_in_party;
	int     m_assist_dont_ally;
};
