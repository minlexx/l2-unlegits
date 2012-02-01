#include "stdafx.h"
#include "Logger.h"
#include "UserAI_QuickTargetSetup.h"
#include "utils.h"

UserAI_QuickTargetSetup::UserAI_QuickTargetSetup()
{
	clear();
}

UserAI_QuickTargetSetup::~UserAI_QuickTargetSetup()
{
	clear();
}

void UserAI_QuickTargetSetup::clear()
{
	m_classPri.initDefaultPriorityList();
	m_distPri.initDefaultPriorities();
	m_vk_key = 0;
	m_announce_target_in_party_chat = 0;
	m_max_dist = 1500;
}

ClassPriorities *UserAI_QuickTargetSetup::getClassPriorities()
{
	return &m_classPri;
}

DistancePriorities *UserAI_QuickTargetSetup::getDistancePriorities()
{
	return &m_distPri;
}

const ClassPriorities *UserAI_QuickTargetSetup::getClassPriorities() const
{
	return &m_classPri;
}

const DistancePriorities *UserAI_QuickTargetSetup::getDistancePriorities() const
{
	return &m_distPri;
}

int UserAI_QuickTargetSetup::getKey() const
{
	return m_vk_key;
}

void UserAI_QuickTargetSetup::setKey( int vk_key )
{
	m_vk_key = vk_key;
}

int UserAI_QuickTargetSetup::getAnnounceTargetInPartyChat() const
{
	return m_announce_target_in_party_chat;
}

void UserAI_QuickTargetSetup::setAnnounceTargetInPartyChat( int ann )
{
	m_announce_target_in_party_chat = ann;
}

int UserAI_QuickTargetSetup::getMaxDist() const
{
	return m_max_dist;
}

void UserAI_QuickTargetSetup::setMaxDist( int maxDist )
{
	if( maxDist > 1 ) m_max_dist = maxDist;
}


bool UserAI_QuickTargetSetup::saveToFile( const char *fn )
{
	int i;
	FILE *f = fopen( fn, "wt" );
	if( !f ) return false;
	//
	fprintf( f, "[main]\n" );
	fprintf( f, "vk_key=%d\n", m_vk_key );
	fprintf( f, "max_dist=%d\n", m_max_dist );
	fprintf( f, "announce_target_in_party_chat=%d\n", m_announce_target_in_party_chat );
	// opts
	/*fprintf( f, "[opts]\n" );
	fprintf( f, "dont_target_dead=%d\n", m_dont_target_dead );
	fprintf( f, "dont_target_party=%d\n", m_dont_target_party );
	fprintf( f, "dont_target_clan=%d\n", m_dont_target_clan );
	fprintf( f, "dont_target_ally=%d\n", m_dont_target_ally );
	fprintf( f, "target_only_cw=%d\n", m_target_only_cw );
	fprintf( f, "allow_target_pk=%d\n", m_allow_target_pk );
	fprintf( f, "allow_target_flagged=%d\n", m_allow_target_flagged );*/
	// class priorities
	fprintf( f, "[class_pri]\n" );
	for( i=0; i<L2MaxClasses; i++ )
	{
		int pri = m_classPri.getPriForClass( i );
		if( pri >= 0 )
			fprintf( f, "%d=%d\n", i, pri );
	}
	// distance priorities
	fprintf( f, "[dist_pri]\n" );
	for( i=0; i<m_distPri.MAXN; i++ )
	{
		int min=-1, max=-1, pri_mod=0;
		m_distPri.getRangeInfo( i, &min, &max, &pri_mod );
		if( min>=0 && max>=0 )
		{
			fprintf( f, "%d:%d:%d\n", min, max, pri_mod );
		}
	}
	//
	fclose( f );
	return true;
}

bool UserAI_QuickTargetSetup::loadFromFile( const char *fn )
{
	FILE *f = fopen( fn, "rt" );
	if( !f )
	{
		// some default setup
		m_classPri.initDefaultPriorityList();
		m_distPri.initDefaultPriorities();
		m_vk_key = (int)'Q'; // default to 'Q'
		m_max_dist = 2500;
		return false;
	}
	//
	int nLine = 0;
	char line[512];
	while( nLine < 14 )
	{
		freadline( f, line, sizeof(line) );
		if( line[0] == 0 ) continue;
		nLine++;
		// comments
		if( line[0] == '[' ) continue;
		// basic parse
		char *eq = strchr( line, '=' );
		char *val = eq+1;
		*eq = 0;
		// main
		if( strstr( line, "vk_key" ) == line )
			sscanf( val, "%d", &m_vk_key );
		if( strstr( line, "max_dist" ) == line )
			sscanf( val, "%d", &m_max_dist );
		if( strstr( line, "announce_target_in_party_chat" ) == line )
			sscanf( val, "%d", &m_announce_target_in_party_chat );
		// opts
		/*if( strstr( line, "dont_target_dead" ) == line )
			sscanf( val, "%d", &m_dont_target_dead );
		if( strstr( line, "dont_target_party" ) == line )
			sscanf( val, "%d", &m_dont_target_party );
		if( strstr( line, "dont_target_clan" ) == line )
			sscanf( val, "%d", &m_dont_target_clan );
		if( strstr( line, "dont_target_ally" ) == line )
			sscanf( val, "%d", &m_dont_target_ally );
		if( strstr( line, "target_only_cw" ) == line )
			sscanf( val, "%d", &m_target_only_cw );
		if( strstr( line, "allow_target_pk" ) == line )
			sscanf( val, "%d", &m_allow_target_pk );
		if( strstr( line, "allow_target_flagged" ) == line )
			sscanf( val, "%d", &m_allow_target_flagged );*/
	}
	// class priorities
	while( !feof( f ) )
	{
		freadline( f, line, sizeof(line) );
		if( line[0] == 0 ) continue;
		if( _stricmp( line, "[dist_pri]" ) == 0 ) break;
		// basic parse
		char *eq = strchr( line, '=' );
		char *val = eq+1;
		*eq = 0;
		//
		int classId = 0, pri = 0;
		sscanf( line, "%d", &classId );
		sscanf( val, "%d", &pri );
		m_classPri.setPriForClass( classId, pri );
	}
	// distance priorities
	m_distPri.removeAll();
	while( !feof( f ) )
	{
		freadline( f, line, sizeof(line) );
		if( line[0] == 0 ) continue;
		// parse
		char *delim = ":";
		char *smin = strtok( line, delim );
		char *smax = strtok( NULL, delim );
		char *spri = strtok( NULL, delim );
		int imin=0, imax=0, pri_mod=0;
		sscanf( smin, "%d", &imin );
		sscanf( smax, "%d", &imax );
		sscanf( spri, "%d", &pri_mod );
		if( m_distPri.addRangePri( imin, imax, pri_mod ) == -1 )
			log_error( LOG_ERROR, "UserAI_QuickTargetSetup::loadFromFile( \"%s\" ): error adding dist pri [%d,%d,%d]\n",
				fn, imin, imax, pri_mod );
	}
	//
	fclose( f );
	return true;
}
