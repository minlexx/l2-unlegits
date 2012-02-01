#ifndef H_USER_INVENTORY
#define H_USER_INVENTORY

#define USERINV_MAX_ITEMS 256

// ===========================================

class UserInventoryItem
{
public:
	UserInventoryItem();
	UserInventoryItem( const UserInventoryItem& other );
	const UserInventoryItem& operator=( const UserInventoryItem& other );
public:
	void getItemName( wchar_t *out, size_t maxCount );
	void setUnused();
	bool isUnused() { return (objectID == 0); }
public:
	unsigned  int itemID;
	unsigned  int objectID;
	unsigned long long int count;
	unsigned  int type1;
	unsigned  int type2;
	unsigned  int locationSlot;
	int           isEquipped;
	int           enchantLevel;
	int           mana;
};

// ============================================

class UserInventory
{
public:
	UserInventory();
	~UserInventory();
	void clear();
public:
	int addItem( UserInventoryItem *it );
	int delItem( UserInventoryItem *it );
	int updateItem( UserInventoryItem *it );
public:
	int getItemInfoByItemId( unsigned int itemID, UserInventoryItem *it );
	int getItemInfoByObjectId( unsigned int objectID, UserInventoryItem *it );
public:
	void parse_ItemList( L2GamePacket *p, L2_VERSION ver );
	void parse_InventoryUpdate( L2GamePacket *p, L2_VERSION ver );
public:
	int itemCount;
	int invSize;
	UserInventoryItem item[USERINV_MAX_ITEMS];
};

#endif
