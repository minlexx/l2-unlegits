#ifndef H_DB_LAYER
#define H_DB_LAYER

void DBLayer_init();
void DBLayer_free();

void DBLayer_getItemName( unsigned int itemID, TCHAR *out, size_t maxCount );

void DBLayer_getSkillName( unsigned int skillID, TCHAR *out, size_t maxCount );

void DBLayer_getNpcNameTitle(
	unsigned int npcID,
	TCHAR *npcName,  size_t npcNameMaxCount,
	TCHAR *npcTitle, size_t npcTitleMaxCount );

void DBLayer_printStats();

#endif
