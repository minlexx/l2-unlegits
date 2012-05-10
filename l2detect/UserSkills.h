#ifndef H_USERSKILLS
#define H_USERSKILLS

class UserSkill
{
public:
	UserSkill();
	~UserSkill();
	UserSkill( const UserSkill& other );
	UserSkill& operator=( const UserSkill& other );
public:
	void setUnused();
	bool isUnused() { return (skillID == 0); }
	void getSkillName( wchar_t *out, size_t maxCount );
	void getSkillName( char *out, size_t maxCount );
public:
	unsigned int skillID;
	unsigned int level;
	int isPassive;
	unsigned char isDisabled;
	unsigned char isEnchanted;
	unsigned int coolTimeReuse; // ms
	unsigned int coolTimeRemaining; // ms
	unsigned int castStartTick;
public:
	void process_coolTime( unsigned int curTick );
};

class UserSkills
{
public:
	static const int USERSKILL_MAX_SKILLS = 128;
public:
	UserSkills();
	~UserSkills();
	void clear();
public:
	int addSkill( UserSkill *addskill );
public:
	int getSkillInfoBySkillId( unsigned int skillID, UserSkill *outskill );
public:
	void parse_SkillList( L2GamePacket *p );
	void parse_SkillCoolTime( L2GamePacket *p );
	void parse_MagicSkillUse( L2GamePacket *p );
public:
	int skillCount;
	UserSkill skill[USERSKILL_MAX_SKILLS];
public:
	unsigned int castMsecStart;
	unsigned int castMsecLeft;
	unsigned int castMsecTickStart;
public:
	void startCasting( unsigned int time );
	void stopCasting();
	bool isCasting() const { return (castMsecLeft > 0); }
	void processCasting( unsigned int curTick );
	void processCoolTimes( unsigned int curTick );
};

#endif
