#ifndef H_USERAI_INVISGM
#define H_USERAI_INVISGM

class UserAI_InivisGMManager
{
public:
	static const int ACT_MOVE = 1;
	static const int ACT_STOP_MOVE = 2;
	static const int ACT_SKILL_USE = 3;
	static const int ACT_SKILL_LAUNCH = 4;
public:
	UserAI_InivisGMManager();
	~UserAI_InivisGMManager();
public:
	void clear();
	void handlePossibleInvisGM( unsigned int objectID, int actionId,
		int x, int y, int z,
		int dx, int dy, int dz,
		int skillId, int skillLevel );
	void deletePossibleInvisGM( unsigned int objectID );
	bool isInvisGM( unsigned int objectID );

protected:
	void generate_CharInfo( unsigned int objectID, int x, int y, int z );

protected:
	static const int MAX_INVIS_GMS = 16;
	unsigned int     gm_oids[MAX_INVIS_GMS];
};

#endif
