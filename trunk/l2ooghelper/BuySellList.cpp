#include "stdafx.h"
#include "BuySellList.h"

TradeItem::TradeItem()
{
	setUnused();
}

TradeItem::~TradeItem()
{
	setUnused();
}

TradeItem::TradeItem( const TradeItem& other )
{
	setUnused();
	this->operator=( other );
}

TradeItem& TradeItem::operator=( const TradeItem& other )
{
	this->itemID = other.itemID;
	this->objectID = other.objectID;
	this->count = other.count;
	this->price = other.price;
	this->storePrice = other.storePrice;
	this->enchantLevel = other.enchantLevel;
	this->attributeAtkType = other.attributeAtkType;
	this->attributeAtkValue = other.attributeAtkValue;
	this->attributeDefFire = other.attributeDefFire;
	this->attributeDefWater = other.attributeDefWater;
	this->attributeDefWind = other.attributeDefWind;
	this->attributeDefEarth = other.attributeDefEarth;
	this->attributeDefHoly = other.attributeDefHoly;
	this->attributeDefUnholy = other.attributeDefUnholy;
	this->type1 = other.type1;
	this->type2 = other.type2;
	this->bodyPart = other.bodyPart;
	//this->packageSale = other.packageSale;
	return (*this);
}

void TradeItem::setUnused()
{
	//packageSale = false;
	type1 = type2 = bodyPart = 0;
	itemID = objectID = 0;
	count = price = storePrice = enchantLevel = 0;
	attributeAtkType = attributeAtkValue = 0;
	attributeDefFire = attributeDefWater = attributeDefWind =
		attributeDefEarth = attributeDefHoly = attributeDefUnholy = 0;
}

bool TradeItem::isStackableGuess()
{
	// analyze item type
	// item type1   0-weapon/ring/earring/necklace  1-armor/shield  4-item/questitem/adena
	// item type2   0-weapon  1-shield/armor  2-ring/earring/necklace  3-questitem  4-adena  5-item
	//
	// usually weapons, jewelry, armors, shields are not stackable
	// items (usually) and adena are stackable
	// some quest items can be stackable, this is unknown
	//
	// TODO: TradeItem::isStackableGuess(): we can try also analyse bodyPart
	// FIXED: try to use only type2 to detect stakability
	//if( (type1 == 0) || (type1 == 1) ) return false; // WeapArmShldJew
	if( (type2 == 0) || (type2 == 1) || (type2 == 2) ) return false; // WeapArmShldJew
	if( type2 == 4 ) return true; // Adena is always stackable :)
	// for quest items and items think that they are stackable >_<
	return true;
}



//static const int MAX_TRADE_ITEMS = 128;
TradeItemsList::TradeItemsList()
{
	clear();
}

TradeItemsList::~TradeItemsList()
{
	clear();
}

void TradeItemsList::clear()
{
	packageSale = 0;
	isCommonCraft = 0;
	listID = 0;
	curMoney = 0;
	itemCount = 0;
	int i;
	for( i=0; i<MAX_TRADE_ITEMS; i++ ) item[i].setUnused();
	message[0] = 0;
}

void TradeItemsList::addItem( const TradeItem& it )
{
	if( itemCount >= MAX_TRADE_ITEMS ) return;
	item[itemCount] = it;
	itemCount++;
}

int TradeItemsList::findItemIdxByItemId( unsigned int itemID )
{
	int i;
	for( i=0; i<MAX_TRADE_ITEMS; i++ )
	{
		if( item[i].itemID == itemID ) return i;
	}
	return -1;
}

void TradeItemsList::removeCellAndShift( int cell_idx )
{
	if( (cell_idx<0) || (cell_idx>=MAX_TRADE_ITEMS) ) return;
	item[cell_idx].setUnused();
	int i;
	for( i=cell_idx; i<(MAX_TRADE_ITEMS-1); i++ ) item[i] = item[i+1];
	item[i].setUnused();
	this->itemCount--;
}

/*unsigned int listID;
int curMoney;
int itemCount;
TradeItem item[MAX_TRADE_ITEMS];*/
