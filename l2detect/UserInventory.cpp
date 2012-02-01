#include "stdafx.h"
#include "UserInventory.h"
#include "Logger.h"
#include "ConfigIni.h"

extern class CConfig g_cfg;

UserInventory::UserInventory()
{
	clear();
}

UserInventory::~UserInventory()
{
	clear();
}

void UserInventory::clear()
{
	itemCount = 0;
	int i;
	for( i=0; i<USERINV_MAX_ITEMS; i++ )
	{
		item[i].objectID = 0;
		item[i].itemID = 0;
		item[i].count = 0;
		item[i].isEquipped = 0;
	}
}

int UserInventory::addItem( UserInventoryItem& it )
{
	int add_idx = -1;
	int i;
	for( i=0; i<USERINV_MAX_ITEMS; i++ )
	{
		if( item[i].objectID == 0 )
		{
			add_idx = i;
			break;
		}
	}
	if( add_idx == -1 ) return add_idx;
	itemCount++;
	item[add_idx].objectID   = it.objectID;
	item[add_idx].itemID     = it.itemID;
	item[add_idx].count      = it.count;
	item[add_idx].isEquipped = it.isEquipped;
	return add_idx;
}

int UserInventory::delItem( unsigned int itemID, unsigned int objectID )
{
	int del_idx = -1;
	int i;
	for( i=0; i<USERINV_MAX_ITEMS; i++ )
	{
		if( (item[i].objectID == objectID) && (item[i].itemID == itemID) )
		{
			del_idx = i;
			break;
		}
	}
	if( del_idx == -1 ) return del_idx;
	item[del_idx].objectID = 0;
	return del_idx;
}

int UserInventory::updateItem( UserInventoryItem& it )
{
	int upd_idx = -1;
	int i;
	for( i=0; i<USERINV_MAX_ITEMS; i++ )
	{
		if( (item[i].objectID == it.objectID) && (item[i].itemID == it.itemID) )
		{
			upd_idx = i;
			break;
		}
	}
	if( upd_idx == -1 ) return upd_idx;
	item[upd_idx].count      = it.count;
	item[upd_idx].isEquipped = it.isEquipped;
	return upd_idx;
}

int UserInventory::getItemInfoByItemId( unsigned int itemID, UserInventoryItem& it )
{
	int i;
	int idx = -1;
	for( i=0; i<USERINV_MAX_ITEMS; i++ )
	{
		if( item[i].itemID == itemID )
		{
			idx = i;
			break;
		}
	}
	if( idx == -1 ) return idx;
	it.objectID   = item[idx].objectID;
	it.count      = item[idx].count;
	it.isEquipped = item[idx].isEquipped;
	return idx;
}

void UserInventory::parse_ItemList( void *l2_game_packet )
{
	if( !l2_game_packet ) return;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	clear();
	int i = 0;
	// parse
	p->getPacketType();
	(int)p->readUShort(); // writeH(_showWindow ? 0x01 : 0x00);
	int icount = (int)p->readUShort(); //writeH(count);

	try {

	UserInventoryItem it;
	//
	for( i=0; i<icount; i++ )
	{
		p->readShort();                 // writeH(temp.getItem().getType1()); // item type1		
		it.objectID = p->readUInt();    // writeD(temp.getObjectId());
		it.itemID = p->readUInt();      // writeD(temp.getItemId());
		p->readInt();                   // writeD(temp.getLocationSlot()); // T1
		if( g_cfg.L2_version <= (int)L2_VERSION_T22 )
			it.count = p->readUInt();   // writeD(temp.getCount());
		else if( g_cfg.L2_version >= (int)L2_VERSION_T23 )
			it.count = p->readUInt64(); // writeQ(temp.getCount());
		p->readShort();                 // writeH(temp.getItem().getType2()); // item type2
		p->readShort();                 // writeH(temp.getCustomType1()); // item type3
		it.isEquipped = p->readShort(); // writeH(temp.isEquipped() ? 0x01 : 0x00);
		p->readInt();                   // writeD(temp.getItem().getBodyPart());
		p->readShort();                 // writeH(temp.getEnchantLevel()); // enchant level
		//race tickets
		p->readShort();                 // writeH(temp.getCustomType2()); // item type3
		p->readInt();                   // writeD(temp.getAugmentation().getAugmentationId());
		p->readInt();                   // writeD(temp.getMana()); // -1 if not time limited

		// in gracia final elemental attributes are not Ds, they are Hs
		if( g_cfg.L2_version <= L2_VERSION_T22 )
		{
			// T1
			p->readInt(); // writeD(temp.getAttackElementType());
			p->readInt(); // writeD(temp.getAttackElementPower());
			// elementals
			p->readInt(); p->readInt(); p->readInt();
			p->readInt(); p->readInt(); p->readInt();

			// depends on version!!!!
			if( g_cfg.L2_version >= L2_VERSION_T2 )
			{
				// T2
				p->readInt(); // writeD(0x00);
			}
		}
		else if( g_cfg.L2_version >= L2_VERSION_T23 )
		{
			// T1
			p->readH(); // writeH(temp.getAttackElementType());
			p->readH(); // writeH(temp.getAttackElementPower());
			// elementals
			p->readH(); p->readH(); p->readH();
			p->readH(); p->readH(); p->readH();

			p->readD(); // writeD(temp.isTimeLimitedItem() ? (int) (temp.getRemainingTime()/1000) : -1);
		}

		// Gracia Epilogue has 3 Hs more
		if( (g_cfg.L2_version >= (int)L2_VERSION_T24) && (g_cfg.GraciaEpilogueProtocol_148_hacks == 0) )
		{
			p->readH(); // Enchant effect 1
			p->readH(); // Enchant effect 2
			p->readH(); // Enchant effect 3
		}

		//log_error( LOG_OK, "ItemList: adding %u items %u (%u)\n", count, itemID, objectID );
		addItem( it );
	}

	} // try
	catch( L2P_ReadException& e )
	{
		log_error( LOG_ERROR, "L2P_ReadException in UserAI_parse_ItemList (%d/%d):\n%s\n",
			i, icount, e.what() );
	}
}

//void UserInventory::parse_InventoryUpdate( const unsigned char *bytes, unsigned int length )
void UserInventory::parse_InventoryUpdate( void *l2_game_packet )
{
	if( !l2_game_packet ) return;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	int i = 0;
	//
	p->getPacketType();
	int icount = (int)p->readUShort(); // writeH(count);

	try {

	//
	//log_error( LOG_OK, "InventoryUpdate: %d items\n", icount );
	//
	UserInventoryItem it;
	unsigned short update_type;
	//
	const unsigned short UT_ADD = 0x0001;
	const unsigned short UT_MOD = 0x0002;
	const unsigned short UT_DEL = 0x0003;
	//
	for( i=0; i<icount; i++ )
	{
		update_type = p->readUShort(); // writeH(item.getChange()); // Update type : 01-add, 02-modify, 03-remove
		p->readShort();                // writeH(temp.getItem().getType1()); // item type1
		it.objectID = p->readUInt();      // writeD(temp.getObjectId());
		it.itemID = p->readUInt();        // writeD(temp.getItemId());
		p->readInt();                  // writeD(temp.getLocationSlot()); // T1
		if( g_cfg.L2_version <= L2_VERSION_T22 )
			it.count = p->readUInt();     // writeD(temp.getCount());
		else if( g_cfg.L2_version >= L2_VERSION_T23 )
			it.count = p->readUInt64();     // writeQ(temp.getCount());
		
		p->readShort();                 // writeH(temp.getItem().getType2()); // item type2
		p->readShort();                 // writeH(temp.getCustomType1()); // item type3
		it.isEquipped = p->readShort(); // writeH(temp.isEquipped() ? 0x01 : 0x00);
		p->readInt();                   // writeD(temp.getItem().getBodyPart());
		p->readShort();                 // writeH(temp.getEnchantLevel()); // enchant level
		//race tickets
		p->readShort();                 // writeH(temp.getCustomType2()); // item type3
		p->readInt();                   // writeD(temp.getAugmentation().getAugmentationId());
		p->readInt();                   // writeD(temp.getMana());
		
		// in gracia final elemental attributes are not Ds, they are Hs
		if( g_cfg.L2_version <= L2_VERSION_T22 )
		{
			// T1
			p->readInt(); // writeD(temp.getAttackElementType());
			p->readInt(); // writeD(temp.getAttackElementPower());
			// elementals
			p->readInt(); p->readInt(); p->readInt();
			p->readInt(); p->readInt(); p->readInt();

			// depends on version!!!!
			if( g_cfg.L2_version >= L2_VERSION_T2 )
			{
				// T2
				p->readInt(); // writeD(0x00);
			}
		}
		else if( g_cfg.L2_version >= L2_VERSION_T23 )
		{
			// T1
			p->readH(); // writeH(temp.getAttackElementType());
			p->readH(); // writeH(temp.getAttackElementPower());
			// elementals
			p->readH(); p->readH(); p->readH();
			p->readH(); p->readH(); p->readH();

			p->readD(); // writeD(temp.isTimeLimitedItem() ? (int) (temp.getRemainingTime()/1000) : -1);

			// Gracia Epilogue has 3 Hs more
			if( (g_cfg.L2_version >= (int)L2_VERSION_T24) && (g_cfg.GraciaEpilogueProtocol_148_hacks == 0) )
			{
				p->readH(); // Enchant effect 1
				p->readH(); // Enchant effect 2
				p->readH(); // Enchant effect 3
			}
		}

		//log_error( LOG_OK, "InventoryUpdate: action: %d for %u items ID %u (%u)\n",
		//	(int)update_type, count, itemID, objectID );
		if( update_type == UT_ADD ) addItem( it );
		if( update_type == UT_MOD ) updateItem( it );
		if( update_type == UT_DEL ) delItem( it.itemID, it.objectID );
	}
	//
	} // try
	catch( L2P_ReadException& e )
	{
		log_error( LOG_ERROR, "L2P_ReadException in UserAI_parse_InventoryUpdate (%d/%d):\n%s\n",
			i, icount, e.what() );
	}
}

