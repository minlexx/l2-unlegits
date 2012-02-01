#include "stdafx.h"
#include "UserAI_FolAsiCfg.h"
#include "Logger.h"
#include "utils.h"

UserAI_FolAsiCfg::UserAI_FolAsiCfg()
{
	clear();
}

UserAI_FolAsiCfg::~UserAI_FolAsiCfg()
{
	clear();
}

void UserAI_FolAsiCfg::clear()
{
	m_follow_enable = 0;
	m_follow_dist = 200;
	memset( m_follow_name, 0, sizeof(m_follow_name) );
	m_follow_only_in_party = 1;
	m_assist_enable = 0;
	memset( m_assist_name, 0, sizeof(m_assist_name) );
	m_assist_only_in_party = 1;
	m_assist_dont_ally = 1;
}

bool UserAI_FolAsiCfg::loadFromFile( const char *fn )
{
	FILE *f = fopen( fn, "rt" );
	if( !f )
	{
		log_error( LOG_ERROR, "UserAI_FolAsiCfg::loadFromFile( \"%s\" ): fopen error \"rt\"\n", fn );
		return false;
	}
	char line[512] = {0};
	while( !feof( f ) )
	{
		freadline( f, line, sizeof(line)-1 );
		if( line[0] == 0 ) continue;
		if( line[0] == '#' ) continue;
		char *value = strchr( line, '=' );
		if( !value ) continue;
		*value = 0;
		value++;
		// foll
		if( strstr( line, "m_follow_enable" ) == line ) sscanf( value, "%d", &m_follow_enable );
		if( strstr( line, "m_follow_dist" ) == line ) sscanf( value, "%d", &m_follow_dist );
		if( strstr( line, "m_follow_only_in_party" ) == line ) sscanf( value, "%d", &m_follow_only_in_party );
		if( strstr( line, "m_follow_name" ) == line ) swprintf( m_follow_name, 127, L"%S", value );
		// asi
		if( strstr( line, "m_assist_enable" ) == line ) sscanf( value, "%d", &m_assist_enable );
		if( strstr( line, "m_assist_dont_ally" ) == line ) sscanf( value, "%d", &m_assist_dont_ally );
		if( strstr( line, "m_assist_only_in_party" ) == line ) sscanf( value, "%d", &m_assist_only_in_party );
		if( strstr( line, "m_assist_name" ) == line ) swprintf( m_assist_name, 127, L"%S", value );
	}
	fclose( f );
	return true;
}

bool UserAI_FolAsiCfg::saveToFile( const char *fn )
{
	FILE *f = fopen( fn, "wt" );
	if( !f )
	{
		log_error( LOG_ERROR, "UserAI_FolAsiCfg::saveToFile( \"%s\" ): fopen error \"wt\"\n", fn );
		return false;
	}
	fprintf( f, "m_follow_enable=%d\n", m_follow_enable );
	fprintf( f, "m_follow_dist=%d\n", m_follow_dist );
	fprintf( f, "m_follow_name=%S\n", m_follow_name );
	fprintf( f, "m_follow_only_in_party=%d\n", m_follow_only_in_party );
	//
	fprintf( f, "m_assist_enable=%d\n", m_assist_enable );
	fprintf( f, "m_assist_name=%S\n", m_assist_name );
	fprintf( f, "m_assist_only_in_party=%d\n", m_assist_only_in_party );
	fprintf( f, "m_assist_dont_ally=%d\n", m_assist_dont_ally );
	fclose( f );
	return true;
}

