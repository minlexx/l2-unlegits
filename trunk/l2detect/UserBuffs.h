#ifndef H_USER_BUFFS
#define H_USER_BUFFS

class UserBuff
{
public:
	UserBuff() { clear(); }
	UserBuff( const UserBuff& other ) { this->operator=( other ); }
	UserBuff& operator=( const UserBuff& other )
	{
		this->skillID = other.skillID;
		this->skillLvl = other.skillLvl;
		this->duration = other.duration;
		wcscpy( this->cached_skillName, other.cached_skillName );
		return (*this);
	}
	~UserBuff() { clear(); }
public:
	void clear() { skillID = 0; skillLvl = 0; duration = -1; cached_skillName[0] = 0; }
	wchar_t *getSkillNameW();
	void getSkillName( wchar_t *out, size_t maxCount );
public:
	unsigned int skillID;
	int skillLvl;
	int duration;
protected:
	wchar_t cached_skillName[256];
};





class UserBuffs
{
public:
	static const int USER_MAX_BUFFS = 64;
	static const int USER_MAX_SHORT_BUFFS = 16;
public:
	UserBuffs();
	~UserBuffs();
	void clear();
	void clearShortBuffs();
public:
	int addBuff( UserBuff *addbuff );
	int addShortBuff( UserBuff *addbuff );
public:
	int getBuffnfoBySkillId( unsigned int skillID, UserBuff *outbuff );
public:
	void parse_AbnormalStatusUpdate( void *l2_game_packet );
	void parse_ShortBuffStatusUpdate( void *l2_game_packet );
public:
	void process_BuffTick( unsigned int curTick );
public:
	int buffCount;
	int shortBuffCount;
	UserBuff buff[USER_MAX_BUFFS];
	UserBuff short_buff[USER_MAX_SHORT_BUFFS];
	unsigned int lastBuffKnownTick;
	unsigned int lastShortBuffKnownTick;
};

#endif
