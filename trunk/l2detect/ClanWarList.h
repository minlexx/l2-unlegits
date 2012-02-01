#ifndef H_CLAN_WAR_LIST
#define H_CLAN_WAR_LIST

#define MAX_CLANWARS 32

typedef enum eCLANWARSTATE
{
	CLANWAR_NOWAR = 0,
	CLANWAR_DECLARED,
	CLANWAR_UNDERATTACK,
	CLANWAR_BOTH
} CLANWARSTATE, *LPCLANWARSTATE;

class ClanWarList
{
public:
	ClanWarList();
	~ClanWarList();
	void clear();
	void clearDeclared();
	void clearUnderAttack();
	//void parse_PledgeReceiveWarList( const unsigned char *bytes, unsigned int length );
	void parse_PledgeReceiveWarList( void *l2_game_packet );
public:
	CLANWARSTATE getWarForClan( const wchar_t *wszClanName );
public:
	int numDeclared;
	int numUnderAttack;
	wchar_t *cnDeclared[MAX_CLANWARS];
	wchar_t *cnUnderAttack[MAX_CLANWARS];
};

#endif

