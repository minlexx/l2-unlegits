#include "stdafx.h"
#include "UserAI.h"
#include "logger.h"

/*
char soundAlert_fileNameCritical[256];
char soundAlert_fileNameWarning[256];
char soundAlert_fileNameInfo[256];
int soundAlert_alertOnClanWar; // 0 - none, 1 - info, 2 - warning, 3 - critical
int soundAlert_alertOnTargeted; */

void UserAI::loadSoundAlertsCfgFromFile( const char *filename /*= NULL*/ )
{
	const char *fnOpen = filename;
	if( filename == NULL ) fnOpen = "L2Detect_soundAlerts.ini";
	FILE *f = fopen( fnOpen, "rt" );
	if( !f )
	{
		log_error( LOG_ERROR, "UserAI::loadSoundAlertsCfgFromFile(): failed to open [%s]!\n", fnOpen );
		return;
	}
	//
	char line[512];
	while( !feof( f ) )
	{
		line[0] = 0;
		fgets( line, sizeof(line)-1, f );
		if( (line[0] == 0) || (line[0] =='#') ) continue;
		size_t ll = strlen( line );
		if( ll < 2 ) continue;
		if( (line[ll-1]=='\r') || (line[ll-1]=='\n') ) line[ll-1] = 0;
		if( (line[ll-2]=='\r') || (line[ll-2]=='\n') ) line[ll-2] = 0;
		//
		char *pvalue = strchr( line, '=' );
		if( !pvalue ) continue;
		pvalue++;
		if( strstr( line, "soundAlert_alertOnClanWar" ) == line )
			sscanf( pvalue, "%d", &(this->soundAlert_alertOnClanWar) );
		if( strstr( line, "soundAlert_alertOnTargeted" ) == line )
			sscanf( pvalue, "%d", &(this->soundAlert_alertOnTargeted) );
		if( strstr( line, "soundAlert_noPartyTargetAlert" ) == line )
			sscanf( pvalue, "%d", &(this->soundAlert_noPartyTargetAlert) );
		if( strstr( line, "soundAlert_noClanTargetAlert" ) == line )
			sscanf( pvalue, "%d", &(this->soundAlert_noClanTargetAlert) );
		if( strstr( line, "soundAlert_alertOnInvisGM" ) == line )
			sscanf( pvalue, "%d", &(this->soundAlert_alertOnInvisGM) );
		if( strstr( line, "soundAlert_screenMessage" ) == line )
			sscanf( pvalue, "%d", &(this->soundAlert_screenMessage) );
		if( strstr( line, "soundAlert_fileNameCritical" ) == line )
			strcpy( this->soundAlert_fileNameCritical, pvalue );
		if( strstr( line, "soundAlert_fileNameWarning" ) == line )
			strcpy( this->soundAlert_fileNameWarning, pvalue );
		if( strstr( line, "soundAlert_fileNameInfo" ) == line )
			strcpy( this->soundAlert_fileNameInfo, pvalue );
	}
	//
	fclose( f );
}

void UserAI::saveSoundAlertsCfgToFile( const char *filename /*= NULL*/ )
{
	const char *fnOpen = filename;
	if( filename == NULL ) fnOpen = "L2Detect_soundAlerts.ini";
	FILE *f = fopen( fnOpen, "wt" );
	if( !f )
	{
		log_error( LOG_ERROR, "UserAI::saveSoundAlertsCfgToFile(): failed to open [%s]!\n", fnOpen );
		return;
	}
	// notification sets
	fprintf( f, "soundAlert_alertOnClanWar=%d\n", soundAlert_alertOnClanWar );
	fprintf( f, "soundAlert_alertOnTargeted=%d\n", soundAlert_alertOnTargeted );
	fprintf( f, "soundAlert_noPartyTargetAlert=%d\n", soundAlert_noPartyTargetAlert );
	fprintf( f, "soundAlert_noClanTargetAlert=%d\n", soundAlert_noClanTargetAlert );
	fprintf( f, "soundAlert_screenMessage=%d\n", soundAlert_screenMessage );
	fprintf( f, "soundAlert_alertOnInvisGM=%d\n", soundAlert_alertOnInvisGM );
	// file names
	fprintf( f, "soundAlert_fileNameCritical=%s\n", soundAlert_fileNameCritical );
	fprintf( f, "soundAlert_fileNameWarning=%s\n", soundAlert_fileNameWarning );
	fprintf( f, "soundAlert_fileNameInfo=%s\n", soundAlert_fileNameInfo );
	//
	fclose( f );
}

void UserAI::playSoundAlert( int alert_type )
{
	if( soundAlert_enabled == false ) return; // sound notification disabled
	if( alert_type <= 0 ) return;
	if( alert_type > 3 ) return;
	// 0 - none, 1 - info, 2 - warning, 3 - critical
	switch( alert_type )
	{
	case 1: PlaySoundA( soundAlert_fileNameInfo,     NULL, SND_ASYNC | SND_FILENAME ); break;
	case 2: PlaySoundA( soundAlert_fileNameWarning,  NULL, SND_ASYNC | SND_FILENAME ); break;
	case 3: PlaySoundA( soundAlert_fileNameCritical, NULL, SND_ASYNC | SND_FILENAME ); break;
	}
}
