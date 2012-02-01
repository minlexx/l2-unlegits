#ifndef H_HEALITEMS_TABLE
#define H_HEALITEMS_TABLE

class HealItem
{
public:
	HealItem();
	HealItem( const HealItem &other );
	//HealItem& operator=( const HealItem& other );
public:
	void GetItemNameW( wchar_t *out );
public:
	char itemName[128];
	unsigned int itemID;
	unsigned int reuseDelayMsec;
	int priority;
	unsigned int lastUseTime;
	int percentUse;
};

#define MAXNUMHEALITEMS 16
// markUsedNow constants
#define HIT_MARKUSED_HP 0
#define HIT_MARKUSED_MP 1
#define HIT_MARKUSED_CP 2

class HealItemsTable
{
public:
	typedef enum eHEALITEM_TYPE
	{
		HIT_HP = 0,
		HIT_MP,
		HIT_CP
	} HEALITEM_TYPE;
public:
	HealItemsTable();
	~HealItemsTable();
public:
	void clear();
	bool LoadFromFile( const char *fileName );
	bool SaveToFile( const char *fileName );
	bool getHPItem( int idx, HealItem& out );
	bool getMPItem( int idx, HealItem& out );
	bool getCPItem( int idx, HealItem& out );

	// type: 0 - hp; 1 - mp; 2 - cp
	bool markUsedNow( HEALITEM_TYPE type, int idx, unsigned int tickCount );

	bool delItemFromTable( HEALITEM_TYPE type, int index );
	bool addHealItem( HEALITEM_TYPE type, const HealItem *example );
	bool setHealItem( HEALITEM_TYPE type, int idx, const HealItem *example );

public:
	int enableHealHP;
	int enableHealMP;
	int enableHealCP;

protected:
	HealItem hp_healers[MAXNUMHEALITEMS];
	HealItem mp_healers[MAXNUMHEALITEMS];
	HealItem cp_healers[MAXNUMHEALITEMS];
	void sort();
};

#endif
