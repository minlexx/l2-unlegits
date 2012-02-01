#ifndef H_USER_ETC_STATUS
#define H_USER_ETC_STATUS

class UserEtcStatus
{
public:
	UserEtcStatus() { clear(); }
	~UserEtcStatus() { clear(); }
public:
	void clear();
public:
	void parse_EtcStatusUpdate( void *l2_game_packet );
public:
	unsigned int forceLevel;
	unsigned int weightPenaltyLevel;
	unsigned int blockAllChat;
	unsigned int dangerArea;
	unsigned int gradePenalty;
	unsigned int charmOfCourage;
	unsigned int deathPenaltyLevel;
	unsigned int souls;
};

#endif
