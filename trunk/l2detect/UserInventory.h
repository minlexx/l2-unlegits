#ifndef H_USER_INVENTORY
#define H_USER_INVENTORY

#define USERINV_MAX_ITEMS 256

// ===========================================

class UserInventoryItem
{
public:
	UserInventoryItem(): itemID(0), objectID(0), count(0) {}
	UserInventoryItem( unsigned int ItemID, unsigned int ObjectID, unsigned int Count ):
		itemID(ItemID), objectID(ObjectID), count(Count) {}
public:
	unsigned int itemID;
	unsigned int objectID;
	unsigned long long int count;
	int isEquipped;
};

// ============================================

class UserInventory
{
public:
	UserInventory();
	~UserInventory();
	void clear();
public:
	//int addItem( unsigned int itemID, unsigned int objectID, unsigned long long int count );
	int addItem( UserInventoryItem& it );
	int delItem( unsigned int itemID, unsigned int objectID );
	//int updateItem( unsigned int itemID, unsigned int objectID, unsigned long long int count );
	int updateItem( UserInventoryItem& it );
public:
	int getItemInfoByItemId( unsigned int itemID, UserInventoryItem& it );
public:
	void parse_ItemList( void *l2_game_packet );
	void parse_InventoryUpdate( void *l2_game_packet );
protected:
	int itemCount;
	UserInventoryItem item[USERINV_MAX_ITEMS];
};

#endif
