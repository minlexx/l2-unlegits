#ifndef H_USERINFO
#define H_USERINFO

class UserInfo: public L2Player
{
public:
	UserInfo() { setUnused(); }
	virtual ~UserInfo() { setUnused(); }
public:
	virtual void setUnused();
public:
	// targeting
	unsigned int targetObjectID;
	double targetCurHp;
	double targetMaxHp;
	double targetCurMp;
	double targetMaxMp;
	// dead?
	int isDead;
	int canResurrectToVillage;
	int canResurrectToCH;
	int canResurrectToCastle;
	int canResurrectToFortress;
	int canResurrectToSiegeHQ;
	int canResurrectFixed;
};

#endif
