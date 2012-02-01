#ifndef H_BUYSELLLIST
#define H_BUYSELLLIST

class TradeItem
{
public:
	TradeItem();
	TradeItem( const TradeItem& other );
	~TradeItem();
	TradeItem& operator=( const TradeItem& other );
public:
	void setUnused();
	bool isUnused() { return ((itemID == 0) && (objectID == 0)); }
	bool isStackableGuess();
public:
	unsigned int  itemID;
	unsigned int  objectID;
	long long int count;
	long long int price;
	long long int storePrice;
	int           enchantLevel;
	int           attributeAtkType;
	int           attributeAtkValue;
	int           attributeDefFire;
	int           attributeDefWater;
	int           attributeDefWind;
	int           attributeDefEarth;
	int           attributeDefHoly;
	int           attributeDefUnholy;
	int           type1;
	int           type2;
	int           bodyPart;
};

class TradeItemsList
{
public:
	static const int MAX_TRADE_ITEMS = 128;
public:
	TradeItemsList();
	~TradeItemsList();
public:
	void clear();
	void addItem( const TradeItem& it );
public:
	int findItemIdxByItemId( unsigned int itemID );
	void removeCellAndShift( int cell_idx );
public:
	unsigned int   listID; // may be seller/buyer objectID for private stores
	long long int  curMoney;
	int            itemCount;
	TradeItem      item[MAX_TRADE_ITEMS];
	int            packageSale;
	wchar_t        message[128]; // here is seller/buyer name for private stores
	int            isCommonCraft;
};

#endif
