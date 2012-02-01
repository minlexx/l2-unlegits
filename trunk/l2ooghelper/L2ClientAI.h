#pragma once
#include "IL2Client.h"

enum AI_INTENTION_TYPE
{
	INT_TYPE_NONE = 0,
	INT_TYPE_GOTO,
	INT_TYPE_ATTACK,
	INT_TYPE_NPC_TALK
};


class AI_INTENTION
{
public:
	AI_INTENTION();
	AI_INTENTION( const AI_INTENTION& other );
	const AI_INTENTION& operator=( const AI_INTENTION& other );
public:
	AI_INTENTION_TYPE getType() const { return m_type; }
	int               getX() const { return m_x; }
	int               getY() const { return m_y; }
	int               getZ() const { return m_z; }
	unsigned int      getTargetOid() const { return m_target_oid; }
public:
	void              set_NONE() { m_type = INT_TYPE_NONE; }
	void              set_GOTO( int x, int y, int z );
	void              set_ATTACK( unsigned int target_oid );
	void              set_NPC_TALK( unsigned int npc_oid, const wchar_t *dlg_str );
protected:
	AI_INTENTION_TYPE m_type;
	int               m_x;
	int               m_y;
	int               m_z;
	unsigned int      m_target_oid;
	wchar_t           m_npcdlg_string[256];
};


class AiIntentionQueue
{
public:
	AiIntentionQueue();
	~AiIntentionQueue();
public:
	void         pushIntention( AI_INTENTION i );
	AI_INTENTION getIntention();
	bool         hasIntentions() const;
protected:
	std::list<AI_INTENTION> m_q;
};


class L2ClientAI
{
public:
	static const int THINK_DELAY = 500; // ms
public:
	L2ClientAI( IL2Client *pinterfaceL2Client );
	~L2ClientAI();
public:
	void         think();
	void         setFollow( bool enable, unsigned int oid );
public:
	void         pushIntentionGoto( int x, int y, int z );
	void         pushIntentionAttack( unsigned int target_oid );
	void         pushIntentionNpcTalk( unsigned int npc_oid, const wchar_t *dlg_str );
protected:
	void         follow();
	void         onIntentionGoto( AI_INTENTION& wish );
	void         onIntentionAttack( AI_INTENTION& wish );
	void         onIntentionNpcTalk( AI_INTENTION& wish );
protected:
	IL2Client        *m_cl;
	AiIntentionQueue  m_int_queue;
	unsigned int      m_last_think_time;
	unsigned int      m_last_follow_time;
	bool              m_follow_enabled;
	unsigned int      m_follow_oid;
};
