#pragma once

class UserAI_LockTargetMgr
{
public:
	UserAI_LockTargetMgr();
	~UserAI_LockTargetMgr();
public:
	void setEnable( bool b );
	bool isEnabled() const;
public:
	void onUserTargetSelected( unsigned int targetObjectID );
	void onUserRequestAction( unsigned int actionObjectID );
	void onUserRequestUnselectTarget();
	void onUserTargetLost();
protected:
	bool m_enabled;
	unsigned int m_toid;
};
