#include "stdafx.h"
#include "UserInventory.h"
#include "Logger.h"
#include "DBLayer.h"

UserInventoryItem::UserInventoryItem()
{
	this->objectID = 0;
	this->itemID = 0;
	this->count = 0;
	this->enchantLevel = 0;
	this->isEquipped = 0;
	this->locationSlot = 0;
	this->mana = 0;
	this->type1 = 0;
}

UserInventoryItem::UserInventoryItem( const UserInventoryItem& other )
{
	this->operator=( other );
}

const UserInventoryItem& UserInventoryItem::operator=( const UserInventoryItem& other )
{
	if( this == &other ) return (*this);
	this->objectID = other.objectID;
	this->itemID = other.itemID;
	this->count = other.count;
	this->enchantLevel = other.enchantLevel;
	this->isEquipped = other.isEquipped;
	this->locationSlot = other.locationSlot;
	this->mana = other.mana;
	this->type1 = other.type1;
	this->type2 = other.type2;
	return (*this);
}

void UserInventoryItem::setUnused()
{
	objectID = 0;
	itemID = 0;
	count = 0;
	type1 = type2 = 0;
	locationSlot = 0;
	isEquipped = 0;
	enchantLevel = 0;
	mana = 0;
}

void UserInventoryItem::getItemName( wchar_t *out, size_t maxCount )
{
	DBLayer_getItemName( itemID, out, maxCount );
}

UserInventory::UserInventory()
{
	clear();
	invSize = 0;
}

UserInventory::~UserInventory()
{
	clear();
}

void UserInventory::clear()
{
	itemCount = 0;
	int i;
	for( i=0; i<USERINV_MAX_ITEMS; i++ ) item[i].setUnused();
}

int UserInventory::addItem( UserInventoryItem *it )
{
	int add_idx = -1;
	int i;
	for( i=0; i<USERINV_MAX_ITEMS; i++ )
	{
		if( item[i].isUnused() )
		{
			add_idx = i;
			break;
		}
	}
	if( add_idx == -1 ) return add_idx;
	itemCount++;
	item[add_idx] = (*it);
	return add_idx;
}


int UserInventory::delItem( UserInventoryItem *it  )
{
	int del_idx = -1;
	int i;
	for( i=0; i<USERINV_MAX_ITEMS; i++ )
	{
		if( (item[i].objectID == it->objectID) && (item[i].itemID == it->itemID) )
		{
			del_idx = i;
			break;
		}
	}
	if( del_idx == -1 ) return del_idx;
	itemCount--;
	item[del_idx].setUnused();
	return del_idx;
}

int UserInventory::updateItem( UserInventoryItem *it  )
{
	int upd_idx = -1;
	int i;
	for( i=0; i<USERINV_MAX_ITEMS; i++ )
	{
		if( (item[i].objectID == it->objectID) && (item[i].itemID == it->itemID) )
		{
			upd_idx = i;
			break;
		}
	}
	if( upd_idx == -1 ) return upd_idx;
	item[upd_idx] = (*it);
	return upd_idx;
}

int UserInventory::getItemInfoByItemId( unsigned int itemID, UserInventoryItem *it )
{
	int i;
	int idx = -1;
	int nPassed = 0;
	for( i=0; i<USERINV_MAX_ITEMS; i++ )
	{
		if( item[i].isUnused() ) continue;
		nPassed++;
		if( item[i].itemID == itemID )
		{
			idx = i;
			break;
		}
		if( nPassed >= itemCount ) break;
	}
	if( idx == -1 ) return idx;
	(*it) = item[idx];
	return idx;
}

int UserInventory::getItemInfoByObjectId( unsigned int objectID, UserInventoryItem *it )
{
	int i;
	int idx = -1;
	int nPassed = 0;
	for( i=0; i<USERINV_MAX_ITEMS; i++ )
	{
		if( item[i].isUnused() ) continue;
		nPassed++;
		if( item[i].objectID == objectID )
		{
			idx = i;
			break;
		}
		if( nPassed >= itemCount ) break;
	}
	if( idx == -1 ) return idx;
	(*it) = item[idx];
	return idx;
}

void UserInventory::parse_ItemList( L2GamePacket *p, L2_VERSION ver )
{
	if( !p ) return;
	clear();
	int i = 0;
	p->getPacketType();
	/*int showWindow = */(int)p->readUShort(); //writeH(_showWindow ? 0x01 : 0x00);
	int icount = (int)p->readUShort(); //writeH(count);
	//log_error( LOG_OK, "ItemList: showWindow = %d, %d items\n", showWindow, icount );
	UserInventoryItem it;
	for( i=0; i<icount; i++ )
	{
		it.type1 = (unsigned int)p->readUShort();   // writeH(temp.getItem().getType1()); // item type1
		it.objectID = p->readUInt(); // writeD(temp.getObjectId());
		it.itemID = p->readUInt();   // writeD(temp.getItemId());
		it.locationSlot = p->readInt(); // writeD(temp.getLocationSlot()); // T1

		if( ver <= L2_VERSION_T22 )
			it.count = p->readUInt();    // writeD(temp.getCount());
		if( ver >= L2_VERSION_T23 )
			it.count = p->readUInt64();  // writeQ(temp.getCount());

		it.type2 = (unsigned int)p->readShort();   // writeH(temp.getItem().getType2()); // item type2
		p->readShort();           // writeH(temp.getCustomType1()); // item type3
		it.isEquipped = p->readShort(); // writeH(temp.isEquipped() ? 0x01 : 0x00);
		p->readInt();             // writeD(temp.getItem().getBodyPart());
		it.enchantLevel = p->readShort(); // writeH(temp.getEnchantLevel()); // enchant level
		//race tickets
		p->readShort();           // writeH(temp.getCustomType2()); // item type3
		p->readInt();             // writeD(temp.getAugmentation().getAugmentationId());
		it.mana = p->readInt();   // writeD(temp.getMana());

		// elements were Ds before G.Final...
		if( ver <= L2_VERSION_T22 )
		{
			p->readD();             // writeD(temp.getAttackElementType());
			p->readD();             // writeD(temp.getAttackElementPower());
			p->readD(); p->readD(); p->readD(); // for (byte i = 0; i < 6; i++)
			p->readD(); p->readD(); p->readD(); // writeD(temp.getElementDefAttr(i));
		}
		// since Gracia final they're Hs
		if( ver >= L2_VERSION_T23 )
		{
			p->readH();             // writeH(temp.getAttackElementType());
			p->readH();             // writeH(temp.getAttackElementPower());
			p->readH(); p->readH(); p->readH(); // for (byte i = 0; i < 6; i++)
			p->readH(); p->readH(); p->readH(); // writeH(temp.getElementDefAttr(i));
		}

		// // T2 (depends on version!!!!)
		if( ver >= L2_VERSION_T2 )
		{
			p->readInt(); //writeD(0x00); // remaining time for time limited item, or -1
		}

		//log_error( LOG_OK, "ItemList: adding %u items %u (%u)\n", count, itemID, objectID );
		addItem( &it );
	}
}

void UserInventory::parse_InventoryUpdate( L2GamePacket *p, L2_VERSION ver )
{
	if( !p ) return;
	int i = 0;
	p->getPacketType();
	int icount = (int)p->readUShort(); //writeH(count);
	//log_error( LOG_OK, "InventoryUpdate: %d items\n", icount );
	UserInventoryItem it;
	unsigned short update_type;
	//
	const unsigned short UT_ADD = 0x0001;
	const unsigned short UT_MOD = 0x0002;
	const unsigned short UT_DEL = 0x0003;
	//
	for( i=0; i<icount; i++ )
	{
		update_type = p->readUShort(); //writeH(item.getChange()); // Update type : 01-add, 02-modify, 03-remove
		it.type1 = (unsigned int)p->readShort(); //writeH(temp.getItem().getType1()); // item type1
		it.objectID = p->readUInt(); //writeD(temp.getObjectId());
		it.itemID = p->readUInt(); //writeD(temp.getItemId());
		it.locationSlot = p->readInt(); //writeD(temp.getLocationSlot()); // T1

		if( ver <= L2_VERSION_T22 )
			it.count = p->readUInt(); //writeD(temp.getCount());
		if( ver >= L2_VERSION_T23 )
			it.count = p->readUInt64(); //writeQ(temp.getCount());

		it.type2 = (unsigned int)p->readUShort(); //writeH(temp.getItem().getType2()); // item type2
		p->readShort(); //writeH(temp.getCustomType1()); // item type3
		it.isEquipped = p->readShort(); //writeH(temp.isEquipped() ? 0x01 : 0x00);
		p->readInt(); //writeD(temp.getItem().getBodyPart());
		it.enchantLevel = p->readShort(); //writeH(temp.getEnchantLevel()); // enchant level
		//race tickets
		p->readShort(); //writeH(temp.getCustomType2()); // item type3
		p->readInt(); //	writeD(temp.getAugmentation().getAugmentationId());
		it.mana = p->readInt(); //writeD(temp.getMana());

		// elements were Ds before G.Final...
		if( ver <= L2_VERSION_T22 )
		{
			p->readD();             // writeD(temp.getAttackElementType());
			p->readD();             // writeD(temp.getAttackElementPower());
			p->readD(); p->readD(); p->readD(); // for (byte i = 0; i < 6; i++)
			p->readD(); p->readD(); p->readD(); // writeD(temp.getElementDefAttr(i));
		}
		// since Gracia final they're Hs
		if( ver >= L2_VERSION_T23 )
		{
			p->readH();             // writeH(temp.getAttackElementType());
			p->readH();             // writeH(temp.getAttackElementPower());
			p->readH(); p->readH(); p->readH(); // for (byte i = 0; i < 6; i++)
			p->readH(); p->readH(); p->readH(); // writeH(temp.getElementDefAttr(i));
		}

		// T2
		if( ver >= L2_VERSION_T2 )
		{
			p->readInt(); //writeD(0x00); // remaining time for time limited item, or -1
		}

		//log_error( LOG_OK, "InventoryUpdate: action: %d for %u items ID %u (%u)\n",
		//	(int)update_type, count, itemID, objectID );
		if( update_type == UT_ADD ) addItem( &it );
		if( update_type == UT_MOD ) updateItem( &it );
		if( update_type == UT_DEL ) delItem( &it );
	}
}

