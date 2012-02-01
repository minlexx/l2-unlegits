#ifndef L2DATA_H_
#define L2DATA_H_

#include "L2Data_Recipes.h"

#define L2MaxRaces 6
#define L2MaxClasses 137
#define L2MaxSexes 2
#define L2MaxRacePics 12
#define L2MaxNPCRaces 25

void L2Data_init();

const char *L2Data_getRace( int raceID );
const char *L2Data_getClass( int classID );
const char *L2Data_getSex( int sexID );
const char *L2Data_getRacePic( int raceID, int sexID );
const char *L2Data_getNPCRace( int npcRaceID );

void L2Data_SM_init();
void L2Data_SM_free();
const char *L2Data_SM_get( unsigned int systemMessageID );

bool L2Data_DB_Init( char *outErrMsg,
					const char *items_db_filename,
					const char *mobs_db_filename,
					const char *skills_db_filename );
void L2Data_DB_Free();

// title can be NULL
bool L2Data_DB_GetNPCNameTitleByID( unsigned int npcID, char *name, char *title );
// pic can be NULL
bool L2Data_DB_GetItemNamePicByID( unsigned int itemID, char *name, char *pic );
bool L2Data_DB_GetSkillNameByID( unsigned int skillID, char *name );

typedef struct st_L2Data_DB_Result
{
	int rowCount; // row count in result set
	int colCount; // columns count in result set
	char ***rows; // row data: row[0] is 1st row; row[0][0] is 1st column; row[0][0][0] is 1st char in string
	// input parameter
	int inMaxRowsToGet; // queries do not change this parameter.
} L2Data_DB_Result, *PL2Data_DB_Result;

void L2Data_DB_Free_Result( L2Data_DB_Result *pResult );

bool L2Data_DB_Query_NPCs_ByName( const char *request, L2Data_DB_Result *pResult );
bool L2Data_DB_Query_Items_ByName( const char *request, L2Data_DB_Result *pResult );
bool L2Data_DB_Query_Skills_ByName( const char *request, L2Data_DB_Result *pResult );

#endif /* L2DATA_H_ */
