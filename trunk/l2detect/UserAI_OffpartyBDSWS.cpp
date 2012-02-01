#include "stdafx.h"
#include "UserAI_OffpartyBDSWS.h"
#include "utils.h"

UserAI_OffpartyBDSWS::UserAI_OffpartyBDSWS()
{
	allEnabled = 0;
	inv_bd_enable = inv_sws_enable = 0;
	nameBD[0] = nameSWS[0] = 0;
	inv_bd_secs = inv_sws_secs = 125;
	dismiss_bd_secs = dismiss_sws_secs = 15;
}

UserAI_OffpartyBDSWS::~UserAI_OffpartyBDSWS(void)
{
	allEnabled = 0;
	inv_bd_enable = inv_sws_enable = 0;
	nameBD[0] = nameSWS[0] = 0;
	inv_bd_secs = inv_sws_secs = 125;
	dismiss_bd_secs = dismiss_sws_secs = 15;
}

void UserAI_OffpartyBDSWS::setNameBD_a( const char *anameBD )
{
	if( !anameBD ) return;
	MultiByteToWideChar( CP_ACP, 0, anameBD, -1, this->nameBD, 63 );
}

void UserAI_OffpartyBDSWS::setNameSWS_a( const char *anameSWS )
{
	if( !anameSWS ) return;
	MultiByteToWideChar( CP_ACP, 0, anameSWS, -1, this->nameSWS, 63 );
}

bool UserAI_OffpartyBDSWS::readFromFile( const char *fileName )
{
	if( !fileName ) return false;
	FILE *f = fopen( fileName, "rt" );
	if( !f ) return false;
	char line[256];
	while( !feof( f ) )
	{
		if( freadline( f, line, sizeof(line) ) <= 0 ) continue;
		char *epos = strchr( line, '=' );
		if( !epos ) continue;
		(*epos) = 0;
		epos++;
		if( strcmp( line, "allEnabled" ) == 0 ) sscanf( epos, "%d", &(this->allEnabled) );
		if( strcmp( line, "inv_bd_enable" ) == 0 ) sscanf( epos, "%d", &(this->inv_bd_enable) );
		if( strcmp( line, "inv_sws_enable" ) == 0 ) sscanf( epos, "%d", &(this->inv_sws_enable) );
		if( strcmp( line, "inv_bd_secs" ) == 0 ) sscanf( epos, "%d", &(this->inv_bd_secs) );
		if( strcmp( line, "inv_sws_secs" ) == 0 ) sscanf( epos, "%d", &(this->inv_sws_secs) );
		if( strcmp( line, "dismiss_bd_secs" ) == 0 ) sscanf( epos, "%d", &(this->dismiss_bd_secs) );
		if( strcmp( line, "dismiss_sws_secs" ) == 0 ) sscanf( epos, "%d", &(this->dismiss_sws_secs) );
		if( strcmp( line, "nameBD" ) == 0 )
			MultiByteToWideChar( CP_ACP, 0, epos, -1, this->nameBD, 63 );
		if( strcmp( line, "nameSWS" ) == 0 )
			MultiByteToWideChar( CP_ACP, 0, epos, -1, this->nameSWS, 63 );
	}
	fclose( f );
	allEnabled = false; // disabled by default
	return true;
}

bool UserAI_OffpartyBDSWS::saveToFile( const char *fileName )
{
	if( !fileName ) return false;
	FILE *f = fopen( fileName, "wt" );
	if( !f ) return false;
	// codepage
	char anameBD[64];
	char anameSWS[64];
	WideCharToMultiByte( CP_ACP, 0, this->nameBD, -1, anameBD, 63, NULL, NULL );
	WideCharToMultiByte( CP_ACP, 0, this->nameSWS, -1, anameSWS, 63, NULL, NULL );
	//
	fprintf( f, "allEnabled=0\n" ); // disabled in file
	// bd
	fprintf( f, "[BD]\n" );
	fprintf( f, "inv_bd_enable=%d\n", inv_bd_enable );
	fprintf( f, "nameBD=%s\n", anameBD );
	fprintf( f, "inv_bd_secs=%d\n", inv_bd_secs );
	fprintf( f, "dismiss_bd_secs=%d\n", dismiss_bd_secs );
	// sws
	fprintf( f, "[SWS]\n" );
	fprintf( f, "inv_sws_enable=%d\n", inv_sws_enable );
	fprintf( f, "nameSWS=%s\n", anameSWS );
	fprintf( f, "inv_sws_secs=%d\n", inv_sws_secs );
	fprintf( f, "dismiss_sws_secs=%d\n", dismiss_sws_secs );
	fclose( f );
	return true;
}


//bool allEnabled;
//int inv_bd_enable;
//int inv_sws_enable;
//wchar_t nameBD[64];
//wchar_t nameSWS[64];
//int inv_bd_secs;
//int inv_sws_secs;
//int dismiss_bd_secs;
//int dismiss_sws_secs;
