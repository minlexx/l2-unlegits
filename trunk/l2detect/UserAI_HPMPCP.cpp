#include "stdafx.h"
#include "UserAI.h"
#include "Logger.h"
//
#include "GameClient.h" // to have reference to global GameClient object
#include "PacketInjector.h" // to inject UseItem packets

extern class GameClient *g_game_client; // in main.cpp

void UserAI::UAI_CheckHP( class UserAI *cls )
{
	int i = 0;
	unsigned int curTickCount = GetTickCount();
	double hp_percent = 101.1;
	if( (cls->usr.hp_max > 0) && (cls->usr.hp > 0) ) hp_percent = 100.0 * cls->usr.hp / cls->usr.hp_max;
	unsigned int objectID;
	unsigned int count;
	int ipercent = (int)hp_percent;
	if( hp_percent < 100.0 )
	{
		//log_error( LOG_OK, "UserAIThread(): HP < 100 %\n" );
		HealItem hitem;
		hitem.itemID = 0;
		i = 0;
		objectID = 0;
		count = 0;
		int found_index = -1;
		while( 1 )
		{
			hitem.itemID = 0;
			if( !cls->healItems.getHPItem( i, hitem ) ) break;
			i++;
			if( i >= MAXNUMHEALITEMS ) break;
			if( (hitem.lastUseTime + (hitem.reuseDelayMsec) < curTickCount) && 
				(hitem.priority > 0) &&
				(hitem.percentUse >= ipercent) )
			{
				//log_error( LOG_OK, "UserAIThread(): HP: found usable item [%s] ID=%u pri=%d (i = %d)\n",
				//	hitem.itemName, hitem.itemID, hitem.priority, i );
				// check inventory.. do we have this item?
				UserInventoryItem it;
				it.objectID = 0;
				it.count = 0;
				it.isEquipped = 0;
				cls->inv.getItemInfoByItemId( hitem.itemID, it );
				objectID = it.objectID;
				count = (int)it.count;
				if( (objectID>0) && (count>0) )
				{
					// yes!
					found_index = i-1;
					//log_error( LOG_OK, "UserAIThread(): HP: have in inventory, oid=[%u], count %u\n", objectID, count );
					break;
				}
				//else
				//{
					//log_error( LOG_OK, "USerAIThread(): HP: not in inventory.. [%s] ID=%u\n",
					//	hitem.itemName, hitem.itemID );
				//}
			}
			/*else
			{
				if( hitem.itemID > 0 )
				{
					log_error( LOG_OK, "UserAIThread(): HP: item [%s] ID=%u next use time is %u (now %u, left %u)\n",
						hitem.itemName, hitem.itemID,
						hitem.lastUseTime + (hitem.reuseDelaySecs*1000), curTickCount,
						hitem.lastUseTime + (hitem.reuseDelaySecs*1000) - curTickCount );
				}
			}*/
		}
		//
		if( hitem.itemID > 0 )
		{
			// UseItem
			log_error( LOG_USERAI, "HP: ready to UseItem [%s] ID %u (oid %u) (HP %d <= %d )\n",
				hitem.itemName, hitem.itemID, objectID,
				ipercent, hitem.percentUse );
			cls->healItems.markUsedNow( HealItemsTable::HIT_HP, found_index, curTickCount );
			PGen_UseItem( objectID );
		}
	}
}

void UserAI::UAI_CheckMP( class UserAI *cls )
{
	int i = 0;
	unsigned int curTickCount = GetTickCount();
	double mp_percent = 101.1;
	if( (cls->usr.mp_max > 0) && (cls->usr.mp > 0) ) mp_percent = 100.0 * cls->usr.mp / cls->usr.mp_max;
	unsigned int objectID;
	unsigned int count;
	int ipercent = (int)mp_percent;
	if( mp_percent < 100.0 )
	{
		//log_error( LOG_OK, "UserAIThread(): HP < 100 %\n" );
		HealItem hitem;
		hitem.itemID = 0;
		i = 0;
		objectID = 0;
		count = 0;
		int found_index = -1;
		while( 1 )
		{
			hitem.itemID = 0;
			if( !cls->healItems.getMPItem( i, hitem ) ) break;
			i++;
			if( i >= MAXNUMHEALITEMS ) break;
			if( (hitem.lastUseTime + (hitem.reuseDelayMsec) < curTickCount) && // nextReuseTime passed
				(hitem.priority > 0) &&  // we are allowed to use this item
				(hitem.percentUse >= ipercent) ) // Use GHP when hp below 50%, we have 40%... 50>=40
			{
				//log_error( LOG_OK, "UserAIThread(): HP: found usable item [%s] ID=%u pri=%d (i = %d)\n",
				//	hitem.itemName, hitem.itemID, hitem.priority, i );
				// check inventory.. do we have this item?
				UserInventoryItem it;
				it.objectID = 0;
				it.count = 0;
				it.isEquipped = 0;
				cls->inv.getItemInfoByItemId( hitem.itemID, it );
				objectID = it.objectID;
				count = (int)it.count;
				if( (objectID>0) && (count>0) )
				{
					// yes!
					found_index = i-1;
					//log_error( LOG_OK, "UserAIThread(): HP: have in inventory, oid=[%u], count %u\n",
					//	objectID, count );
					break;
				}
				//else
				//{
					//log_error( LOG_OK, "USerAIThread(): HP: not in inventory.. [%s] ID=%u\n",
					//	hitem.itemName, hitem.itemID );
				//}
			}
		}
		//
		if( hitem.itemID > 0 )
		{
			// UseItem
			log_error( LOG_USERAI, "MP: ready to UseItem [%s] ID %u (oid %u) (MP %d <= %d )\n",
				hitem.itemName, hitem.itemID, objectID,
				ipercent, hitem.percentUse );
			cls->healItems.markUsedNow( HealItemsTable::HIT_MP, found_index, curTickCount );
			PGen_UseItem( objectID );
		}
	}
}


void UserAI::UAI_CheckCP( class UserAI *cls )
{
	int i = 0;
	unsigned int curTickCount = GetTickCount();
	double cp_percent = 101.1;
	if( (cls->usr.cp_max > 0) && (cls->usr.cp > 0) ) cp_percent = 100.0 * cls->usr.cp / cls->usr.cp_max;
	unsigned int objectID;
	unsigned int count;
	int ipercent = (int)cp_percent;
	if( cp_percent < 100.0 )
	{
		//log_error( LOG_OK, "UserAIThread(): HP < 100 %\n" );
		HealItem hitem;
		hitem.itemID = 0;
		i = 0;
		objectID = 0;
		count = 0;
		int found_index = -1;
		while( 1 )
		{
			hitem.itemID = 0;
			if( !cls->healItems.getCPItem( i, hitem ) ) break;
			i++;
			if( i >= MAXNUMHEALITEMS ) break;
			if( (hitem.lastUseTime + (hitem.reuseDelayMsec) < curTickCount) &&
				(hitem.priority > 0) &&
				(hitem.percentUse >= ipercent) )
			{
				//log_error( LOG_OK, "UserAIThread(): HP: found usable item [%s] ID=%u pri=%d (i = %d)\n",
				//	hitem.itemName, hitem.itemID, hitem.priority, i );
				// check inventory.. do we have this item?
				UserInventoryItem it;
				it.objectID = 0;
				it.count = 0;
				it.isEquipped = 0;
				cls->inv.getItemInfoByItemId( hitem.itemID, it );
				objectID = it.objectID;
				count = (int)it.count;
				if( (objectID>0) && (count>0) )
				{
					// yes!
					found_index = i-1;
					//log_error( LOG_OK, "UserAIThread(): HP: have in inventory, oid=[%u], count %u\n", objectID, count );
					break;
				}
				//else
				//{
					//log_error( LOG_OK, "USerAIThread(): HP: not in inventory.. [%s] ID=%u\n", hitem.itemName, hitem.itemID );
				//}
			}
		}
		//
		if( hitem.itemID > 0 )
		{
			// UseItem
			log_error( LOG_USERAI, "CP: ready to UseItem [%s] ID %u (oid %u) (CP %d <= %d )\n",
				hitem.itemName, hitem.itemID, objectID,
				ipercent, hitem.percentUse );
			cls->healItems.markUsedNow( HealItemsTable::HIT_CP, found_index, curTickCount );
			PGen_UseItem( objectID );
		}
	}
}
