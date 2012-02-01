#include "stdafx.h"
#include "Logger.h"
#include "L2ClientAI.h"

AI_INTENTION::AI_INTENTION()
{
	m_type = INT_TYPE_NONE;
	m_npcdlg_string[0] = 0;
}

AI_INTENTION::AI_INTENTION( const AI_INTENTION& other )
{
	m_npcdlg_string[0] = 0;
	operator=( other );
}

const AI_INTENTION& AI_INTENTION::operator=( const AI_INTENTION& other )
{
	if( this == &other ) return (*this);
	m_type = other.m_type;
	m_x = other.m_x; m_y = other.m_y; m_z = other.m_z;
	m_target_oid = other.m_target_oid;
	wcsncpy( m_npcdlg_string, other.m_npcdlg_string, sizeof(m_npcdlg_string)/2 );
	return (*this);
}

void AI_INTENTION::set_GOTO( int x, int y, int z )
{
	m_type = INT_TYPE_GOTO;
	m_x = x; m_y = y; m_z = z;
}

void AI_INTENTION::set_ATTACK( unsigned int target_oid )
{
	m_type = INT_TYPE_ATTACK;
	m_target_oid = target_oid;
}

void AI_INTENTION::set_NPC_TALK( unsigned int npc_oid, const wchar_t *dlg_str )
{
	m_type = INT_TYPE_NPC_TALK;
	m_target_oid = npc_oid;
	wcsncpy( m_npcdlg_string, dlg_str, 255 );    m_npcdlg_string[255] = 0;
}







AiIntentionQueue::AiIntentionQueue()
{
	m_q.clear();
}

AiIntentionQueue::~AiIntentionQueue()
{
	m_q.clear();
}

void AiIntentionQueue::pushIntention( AI_INTENTION i )
{
	m_q.push_back( i );
}

AI_INTENTION AiIntentionQueue::getIntention()
{
	AI_INTENTION i = *m_q.begin();
	m_q.pop_front();
	return i;
}

bool AiIntentionQueue::hasIntentions() const
{
	return m_q.size() > 0;
}










L2ClientAI::L2ClientAI( IL2Client *pinterfaceL2Client )
{
	m_cl = pinterfaceL2Client;
	m_last_think_time = GetTickCount() - THINK_DELAY - THINK_DELAY;
	m_last_follow_time = m_last_think_time;
	m_follow_enabled = false;
	m_follow_oid = 0;
}

L2ClientAI::~L2ClientAI()
{
}

void L2ClientAI::think()
{
	unsigned int t_now = (unsigned int)GetTickCount();
	if( t_now - m_last_think_time < THINK_DELAY ) return;
	m_last_think_time = t_now;
	// check for follow
	follow();
	// check for intentions
	if( !m_int_queue.hasIntentions() )
	{
		//log_error( LOG_USERAI, "L2ClientAI:think: no intentions" );
		return;
	}
	AI_INTENTION i = m_int_queue.getIntention();
	switch( i.getType() )
	{
	case INT_TYPE_NONE: break;
	case INT_TYPE_GOTO: onIntentionGoto( i ); break;
	case INT_TYPE_ATTACK: onIntentionAttack( i ); break;
	case INT_TYPE_NPC_TALK: onIntentionNpcTalk( i ); break;
	}
}

void L2ClientAI::setFollow( bool enable, unsigned int oid )
{
	m_follow_enabled = enable;
	m_follow_oid = oid;
}


void L2ClientAI::pushIntentionGoto( int x, int y, int z )
{
	AI_INTENTION i;
	i.set_GOTO( x, y, z );
	m_int_queue.pushIntention( i );
}

void L2ClientAI::pushIntentionAttack( unsigned int target_oid )
{
	AI_INTENTION i;
	i.set_ATTACK( target_oid );
	m_int_queue.pushIntention( i );
}

void L2ClientAI::pushIntentionNpcTalk( unsigned int npc_oid, const wchar_t *dlg_str )
{
	AI_INTENTION i;
	i.set_NPC_TALK( npc_oid, dlg_str );
	m_int_queue.pushIntention( i );
}


void L2ClientAI::follow()
{
	if( m_follow_enabled == false ) return;
	UserInfo *user = m_cl->get_UserInfo();
	CharArray *chars = m_cl->get_WorldChars();
	int char_idx = chars->FindCharByObjectID( m_follow_oid );
	if( char_idx >= 0 )
	{
		L2Player *target = chars->chars_array[char_idx];
		// compare coordinates
		int dx = target->x - user->x;
		int dy = target->y - user->y;
		int dz = target->z - user->z;
		if( abs(dx) > 50 || abs(dy) > 50 || abs(dz) > 50 )
		{
			unsigned int t_now = (unsigned int)GetTickCount();
			if( t_now - m_last_follow_time >= 1000 )
			{
				m_last_follow_time = t_now;
				m_cl->game_MoveBackwardToLocation( target->x, target->y, target->z );
				log_error( LOG_USERAI, "L2ClientAI:follow: following [%S] to (%d,%d,%d)\n", target->charName, target->x, target->y, target->z );
			}
		}
	}
	else // target not found in world
	{
		setFollow( false,  0 );
		log_error( LOG_USERAI, "L2ClientAI:follow: target not found, cancel follow\n" );
	}
}

void L2ClientAI::onIntentionGoto( AI_INTENTION& wish )
{
	(void)wish;
}

void L2ClientAI::onIntentionAttack( AI_INTENTION& wish )
{
	(void)wish;
}

void L2ClientAI::onIntentionNpcTalk( AI_INTENTION& wish )
{
	(void)wish;
}
