#include "pch.h"
#include "ItemTable.h"
#include "Log.h"
#include "../GS.h"
#include "utils/Exception.h"

ItemTable *ItemTable::s_instance = NULL;
int ItemTable::s_refCount = 0;

ItemTable *ItemTable::getInstance()
{
	if( s_instance == NULL )
	{
		s_instance = new ItemTable();
		s_refCount++;
	}
	return s_instance;
}

void ItemTable::freeInstance()
{
	if( !s_instance ) return;
	s_refCount--;
	if( s_refCount > 0 ) return;
	delete s_instance;
	s_instance = NULL;
}

ItemTable::ItemTable()
{
	m_allTemplates = NULL;
	//m_armorTemplates = NULL;
	//m_weaponTemplates = NULL;
	//m_etcItemTemplates = NULL;
	m_cnt_all = 0;
	//m_cnt_armors = m_cnt_weapons = m_cnt_etcItems = 0;
	m_nTotalBytesAllocated = 0;
	m_maxItemId = 0;
}

ItemTable::~ItemTable()
{
	m_lock.Lock();
	unsigned int i;
	if( m_allTemplates )
	{
		if( m_maxItemId > 0 )
		{
			for( i=0; i<m_maxItemId; i++ )
			{
				if( m_allTemplates[i] ) delete m_allTemplates[i];
				m_allTemplates[i] = NULL;
			}
		}
		free( m_allTemplates );
		m_allTemplates = NULL;
	}
	m_lock.Unlock();
	m_nTotalBytesAllocated = 0;
	m_maxItemId = 0;
	m_cnt_all = 0;
}

unsigned int ItemTable::select_count( void *vcon, const wchar_t *table )
{
	unsigned int ret = 0;
	MysqlConnection *con = (MysqlConnection *)vcon;
	MysqlQuery q;
	q.create( L"SELECT COUNT(*) FROM `%s`", table );
	if( con->executeQuery( q ) )
	{
		if( q.getNextRow() ) ret = q.getFieldUInt( 0 );
		else LogError( L"ItemTable::select_count( %s ): q.getNextRow() failed!", table );
	}
	else LogError( L"ItemTable::select_count( %s ): Mysql error: %s", table, con->getErrorStr() );
	return ret;
}

unsigned int ItemTable::select_max( void *vcon, const wchar_t *table )
{
	unsigned int ret = 0;
	MysqlConnection *con = (MysqlConnection *)vcon;
	MysqlQuery q;
	q.create( L"SELECT MAX(item_id) FROM `%s`", table );
	if( con->executeQuery( q ) )
	{
		if( q.getNextRow() ) ret = q.getFieldUInt( 0 );
		else LogError( L"ItemTable::select_max( %s ): q.getNextRow() failed!", table );
	}
	else LogError( L"ItemTable::select_max( %s ): Mysql error: %s", table, con->getErrorStr() );
	return ret;
}

void ItemTable::load()
{
	m_lock.Lock();
	try
	{
		// get GS DB conn
		GameServer *gs = GameServer::getInstance();
		MysqlConnection *con = gs->getDBConnection();
		// count items, armors, weapons in DB
		unsigned int etc_count = select_count( con, L"etcitem" );
		unsigned int arm_count = select_count( con, L"armor" );
		unsigned int wea_count = select_count( con, L"weapon" );
		Log( L"ItemTable: loading %u armors, %u weapons, %u etcitems", arm_count, wea_count, etc_count );
		unsigned int max_itemId = 0;
		unsigned int max_armor   = select_max( con, L"armor" );
		unsigned int max_etcItem = select_max( con, L"etcitem" );
		unsigned int max_weapon  = select_max( con, L"weapon" );
		max_itemId = max_armor;
		if( max_weapon > max_itemId ) max_itemId = max_weapon;
		if( max_etcItem > max_itemId ) max_itemId = max_etcItem;
		LogDebug( L"ItemTable: max itemId = %u (armor %u, etc %u, weapon %u)", max_itemId, max_armor, max_etcItem, max_weapon );
		// allocate m_allTemplates
		m_allTemplates = (L2ItemTemplate **)malloc( sizeof(void *) * (max_itemId+1) );
		if( !m_allTemplates )
		{
			m_lock.Unlock();
			LogError( L"ItemTable::load(): cannot allocate %u bytes\n", (unsigned int)(sizeof(void *) * (max_itemId+1)) );
			return;
		}
		memset( m_allTemplates, 0, sizeof(void *) * (max_itemId+1) );
		m_cnt_all = etc_count + arm_count + wea_count;
		m_maxItemId = max_itemId;
		m_nTotalBytesAllocated = sizeof(void *) * (max_itemId+1);
		//
		loadEtcItems( con );
		loadArmors( con );
		loadWeapons( con );
		//
		// release GS DB conn
		gs->releaseDBConnection( con );
	}
	catch( Exception& e )
	{
		LogError( L"ItemTable load error: %S", e.what() );
		e.logStackTrace();
	}
	m_lock.Unlock();
	Log( L"ItemTable: loaded, used %I64u bytes", m_nTotalBytesAllocated );
}

void ItemTable::reload()
{
	// TODO: maybe better way?
	Log( L"ItemTable: reload starting..." );
	m_lock.Lock();
	this->~ItemTable();
	this->load();
	m_lock.Unlock();
	Log( L"ItemTable: reload complete." );
}

void ItemTable::loadEtcItems( void *vcon )
{
	MysqlConnection *con = (MysqlConnection *)vcon;
	MysqlQuery q;
	q.create( L"SELECT item_id, name, crystallizable, item_type, weight, consume_type, material, "
		L" crystal_type, duration, time, price, crystal_count, sellable, dropable, destroyable, "
		L" tradeable, handler, skill FROM etcitem" );
	if( con->executeQuery( q ) )
	{
		while( q.getNextRow() )
		{
			StatsSet set;
			L2ItemSubType etcItemType = SYBTYPE_INCORRECT;
			int item_id = 0;
			char *str = NULL;
			L2ItemSlot bodypart = SLOT_NONE;
			int type2 = L2ItemTemplate::TYPE2_OTHER;
			wchar_t wname[256] = {0};

			// get item ID
			item_id = q.getFieldInt( "item_id" );
			set.setInt( "item_id", item_id );

			str = q.getFieldStr( "item_type" );
			if( _stricmp( str, "none" ) == 0 )
				etcItemType = ETCITEM_OTHER; // only for default
			else if ( _stricmp( str, "castle_guard") == 0 )
				etcItemType = ETCITEM_SCROLL; // dummy
			else if( _stricmp( str, "material") == 0 )
				etcItemType = ETCITEM_MATERIAL;
			else if( _stricmp( str, "pet_collar") == 0 )
				etcItemType = ETCITEM_PET_COLLAR;
			else if( _stricmp( str, "potion") == 0 )
				etcItemType = ETCITEM_POTION;
			else if( _stricmp( str, "recipe") == 0 )
				etcItemType = ETCITEM_RECEIPE;
			else if( _stricmp( str, "scroll") == 0 )
				etcItemType = ETCITEM_SCROLL;
			else if( _stricmp( str, "seed") == 0 )
				etcItemType = ETCITEM_SEED;
			else if( _stricmp( str, "shot") == 0 )
				etcItemType = ETCITEM_SHOT;
			else if( _stricmp( str, "spellbook") == 0 )
				etcItemType = ETCITEM_SPELLBOOK; // Spellbook, Amulet, Blueprint
			else if( _stricmp( str, "herb") == 0 )
				etcItemType = ETCITEM_HERB;
			else if( _stricmp( str, "arrow") == 0 )
			{
				etcItemType = ETCITEM_ARROW;
				bodypart = SLOT_L_HAND;
			}
			else if( _stricmp( str, "bolt") == 0 )
			{
				etcItemType = ETCITEM_BOLT;
				bodypart = SLOT_L_HAND;
			}
			else if( _stricmp( str, "quest") == 0 )
			{
				etcItemType = ETCITEM_QUEST;
				type2 = L2ItemTemplate::TYPE2_QUEST;
			}
			else if( _stricmp( str, "lure") == 0 )
			{
				etcItemType = ETCITEM_OTHER;
				bodypart = SLOT_L_HAND;
			}
			else if( _stricmp( str, "dye") == 0 )
				etcItemType = ETCITEM_DYE;
			else if( _stricmp( str, "lotto") == 0 )
				etcItemType = ETCITEM_OTHER;
			else if( _stricmp( str, "race_ticket") == 0 )
				etcItemType = ETCITEM_OTHER;
			else if( _stricmp( str, "harvest") == 0 )
				etcItemType = ETCITEM_OTHER;
			else if( _stricmp( str, "ticket_of_lord") == 0 )
				etcItemType = ETCITEM_OTHER;
			else
			{
				q.getFieldStrW( "name", wname, 255 ); wname[255] = 0;
				LogWarning( L"ItemTable::loadEtcItems(): unknown etcitem type: %S (item_id %d [%s])",
					str, item_id, wname );
				etcItemType = ETCITEM_OTHER;
			}
			
			set.setInt( "type1", L2ItemTemplate::TYPE1_ITEM_QUESTITEM_ADENA );
			set.setInt( "type2", type2 );
			set.setInt( "bodypart", (int)bodypart );
			set.setBool( "crystallizable", q.getFieldBool( "crystallizable" ) );
			set.setInt( "crystal_count", q.getFieldInt( "crystal_count" ) );
			set.setBool( "sellable", q.getFieldBool( "sellable" ) );
			set.setBool( "dropable", q.getFieldBool( "dropable" ) );
			set.setBool( "destroyable", q.getFieldBool( "destroyable" ) );
			set.setBool( "tradeable", q.getFieldBool( "tradeable" ) );
			set.setString( "handler", q.getFieldStr( "handler" ) );
			// item.set.set("skill", rset.getString("skill")); // TODO: etc item skills
			
			str = q.getFieldStr( "consume_type" );
			if( _stricmp( str, "asset" ) == 0 )
			{
				etcItemType = ETCITEM_MONEY;
				set.setBool( "stackable", true );
				set.setInt( "type2", L2ItemTemplate::TYPE2_MONEY );
			}
			else if ( _stricmp( str, "stackable" ) == 0 )
			{
				set.setBool( "stackable", true );
			}
			else
			{
				set.setBool( "stackable", false );
			}
			
			L2ItemMaterial material = L2ItemTemplate::getMaterialByName( q.getFieldStr( "material" ) );
			set.setInt( "material", (int)material );
			L2ItemCrystal crystal = L2ItemTemplate::getCrystalTypeByName( q.getFieldStr( "crystal_type" ) );
			set.setInt( "crystal_type", (int)crystal );
			set.setInt( "weight", q.getFieldInt( "weight" ) );
			// name
			q.getFieldStrW( "name", wname, 255 ); wname[255] = 0;
			set.setWString( "name", wname );
			//
			set.setInt( "duration", q.getFieldInt( "duration" ) );
			set.setInt( "time", q.getFieldInt( "time" ) );
			set.setInt( "price", q.getFieldInt( "price" ) );
			//
			// create and insert template
			L2EtcItemTemplate *tmpl = new L2EtcItemTemplate( etcItemType, set );
			m_allTemplates[item_id] = (L2ItemTemplate *)tmpl;

			// count bytes allocated
			if( tmpl ) m_nTotalBytesAllocated += (sizeof(L2EtcItemTemplate) + 2*wcslen(tmpl->getName()));
		} // next row
	}
	else
		LogError( L"ItemTable::loadEtcItems(): MySQL error: %s", con->getErrorStr() );
}

void ItemTable::loadArmors( void *vcon )
{
	MysqlConnection *con = (MysqlConnection *)vcon;
	MysqlQuery q;
	q.create( L"SELECT item_id, name, bodypart, crystallizable, armor_type, weight,"
		L" material, crystal_type, avoid_modify, duration, time, p_def, m_def, mp_bonus,"
		L" price, crystal_count, sellable, dropable, destroyable, tradeable, enchant4_skill, skill FROM armor" );
	if( con->executeQuery( q ) )
	{
		while( q.getNextRow() )
		{
			StatsSet set;
			int item_id;
			int bodypart_i = 0;
			L2ItemSlot bodypart = SLOT_NONE;
			wchar_t wstr[256] = {0};
			char *str = NULL;
			//
			L2ItemSubType armorType = L2ItemTemplate::getArmorTypeByName( q.getFieldStr( "armor_type" ) );
			item_id = q.getFieldInt( 0 );
			set.setInt( "item_id", item_id );
			q.getFieldStrW( "name", wstr, 255 );
			set.setWString( "name", wstr );
			bodypart = L2ItemTemplate::getSlotByName( q.getFieldStr( "bodypart" ) );
			bodypart_i = (int)bodypart;
			set.setInt( "bodypart", bodypart_i );
			set.setBool( "crystallizable", q.getFieldBool( "crystallizable" ) );
			set.setInt( "crystal_count", q.getFieldInt( "crystal_count" ) );
			set.setBool( "sellable", q.getFieldBool( "sellable" ) );
			set.setBool( "dropable", q.getFieldBool( "dropable" ) );
			set.setBool( "destroyable", q.getFieldBool( "destroyable" ) );
			set.setBool( "tradeable", q.getFieldBool( "tradeable" ) );
			set.setString( "enchant4_skill", q.getFieldStr( "enchant4_skill" ) );
			set.setString( "skill", str = q.getFieldStr( "skill" ) );

			// determine type1 & type2
			if( bodypart == SLOT_NECK || bodypart == SLOT_HAIR
				|| bodypart == SLOT_HAIR2 || bodypart == SLOT_HAIRALL
				|| (bodypart & SLOT_L_EAR) != 0 || (bodypart & SLOT_L_FINGER) != 0)
			{
				set.setInt( "type1", L2ItemTemplate::TYPE1_WEAPON_RING_EARRING_NECKLACE );
				set.setInt( "type2", L2ItemTemplate::TYPE2_ACCESSORY );
			}
			else
			{
				set.setInt( "type1", L2ItemTemplate::TYPE1_SHIELD_ARMOR );
				set.setInt( "type2", L2ItemTemplate::TYPE2_SHIELD_ARMOR );
			}

			set.setInt( "weight", q.getFieldInt( "weight" ) );
			set.setInt( "material", (int)L2ItemTemplate::getMaterialByName( q.getFieldStr( "material" ) ) );
			set.setInt( "crystal_type", (int)L2ItemTemplate::getCrystalTypeByName( q.getFieldStr( "crystal_type" ) ) );
			set.setInt( "avoid_modify", q.getFieldInt( "avoid_modify" ) );
			set.setInt( "duration", q.getFieldInt( "duration" ) );
			set.setInt( "time", q.getFieldInt( "time" ) );
			set.setInt( "p_def", q.getFieldInt( "p_def" ) );
			set.setInt( "m_def", q.getFieldInt( "m_def" ) );
			set.setInt( "mp_bonus", q.getFieldInt( "mp_bonus" ) );
			set.setInt( "price", q.getFieldInt( "price" ) );

			// deal with pet items: type1, type2 & bodypart
			if( armorType == ARMOR_PET )
			{
				if( bodypart == SLOT_NECK )
				{
					set.setInt( "type1", L2ItemTemplate::TYPE1_WEAPON_RING_EARRING_NECKLACE );
					set.setInt( "type2", L2ItemTemplate::TYPE2_ACCESSORY );
					set.setInt( "bodypart", (int)SLOT_NECK );
				}
				else
				{
					set.setInt( "type1", L2ItemTemplate::TYPE1_SHIELD_ARMOR );
					L2ItemSlot temp_b = SLOT_NONE;
					//int temp_b = 0;
					set.getInt( "bodypart", (int *)&temp_b );
					switch( temp_b )
					{
					case SLOT_WOLF:
						set.setInt( "type2", L2ItemTemplate::TYPE2_PET_WOLF );
						break;
					case SLOT_GREATWOLF:
						set.setInt( "type2", L2ItemTemplate::TYPE2_PET_EVOLVEDWOLF );
						break;
					case SLOT_HATCHLING:
						set.setInt( "type2", L2ItemTemplate::TYPE2_PET_HATCHLING );
						break;
					case SLOT_BABYPET:
						set.setInt( "type2", L2ItemTemplate::TYPE2_PET_BABY );
						break;
					default:
						set.setInt( "type2", L2ItemTemplate::TYPE2_PET_STRIDER );
						break;
					}
					set.setInt( "bodypart", (int)SLOT_CHEST );
				}
			}

			// create and insert template
			L2ArmorTemplate *tmpl = new L2ArmorTemplate( armorType, set );
			m_allTemplates[item_id] = (L2ItemTemplate *)tmpl;

			// count bytes allocated
			if( tmpl ) m_nTotalBytesAllocated += (sizeof(L2ArmorTemplate) + 2*wcslen(tmpl->getName()));
			//else throw std::bad_alloc( "ItemTable:loadArmors: new L2ArmorTemplate" );
		}
	}
	else
		LogError( L"ItemTable::loadArmors(): MySQL error: %s", con->getErrorStr() );
}

void ItemTable::loadWeapons( void *vcon )
{
	MysqlConnection *con = (MysqlConnection *)vcon;
	MysqlQuery q;
	q.create( L"SELECT item_id, name, bodypart, crystallizable, weight, soulshots, spiritshots,"
		L" material, crystal_type, p_dam, rnd_dam, weaponType, critical, hit_modify, avoid_modify,"
		L" shield_def, shield_def_rate, atk_speed, mp_consume, m_dam, duration, time, price, crystal_count,"
		L" sellable, dropable, destroyable, tradeable, skill,enchant4_skill_id,enchant4_skill_lvl, "
		L" onCast_skill_id, onCast_skill_lvl, onCast_skill_chance, onCrit_skill_id, "
		L" onCrit_skill_lvl, onCrit_skill_chance, change_weaponId FROM weapon" );
	if( con->executeQuery( q ) )
	{
		while( q.getNextRow() )
		{
			StatsSet set;
			int item_id;
			int bodypart_i = 0;
			L2ItemSlot bodypart = SLOT_NONE;
			wchar_t wstr[256] = {0};
			//
			L2ItemSubType weaponType = L2ItemTemplate::getWeaponTypeByName( q.getFieldStr( "weaponType" ) );
			item_id = q.getFieldInt( 0 );
			set.setInt( "item_id", item_id );
			q.getFieldStrW( "name", wstr, 255 );
			set.setWString( "name", wstr );

			// lets see if this is a shield
			if( weaponType == WEAPON_NONE )
			{
				set.setInt( "type1", L2ItemTemplate::TYPE1_SHIELD_ARMOR );
				set.setInt( "type2", L2ItemTemplate::TYPE2_SHIELD_ARMOR );
			}
			else
			{
				set.setInt( "type1", L2ItemTemplate::TYPE1_WEAPON_RING_EARRING_NECKLACE );
				set.setInt( "type2", L2ItemTemplate::TYPE2_WEAPON );
			}

			bodypart = L2ItemTemplate::getSlotByName( q.getFieldStr( "bodypart" ) );
			bodypart_i = (int)bodypart;
			set.setInt( "bodypart", bodypart_i );

			set.setInt( "material", (int)L2ItemTemplate::getMaterialByName( q.getFieldStr( "material" ) ) );
			set.setInt( "crystal_type", (int)L2ItemTemplate::getCrystalTypeByName( q.getFieldStr( "crystal_type" ) ) );

			set.setBool( "crystallizable", q.getFieldBool( "crystallizable" ) );
			set.setInt( "weight", q.getFieldInt( "weight" ) );
			set.setInt( "soulshots", q.getFieldInt( "soulshots" ) );
			set.setInt( "spiritshots", q.getFieldInt( "spiritshots" ) );
			set.setInt( "p_dam", q.getFieldInt( "p_dam" ) );
			set.setInt( "rnd_dam", q.getFieldInt( "rnd_dam" ) );
			set.setInt( "critical", q.getFieldInt( "critical" ) );
			set.setInt( "hit_modify", q.getFieldInt( "hit_modify" ) );
			set.setInt( "avoid_modify", q.getFieldInt( "avoid_modify" ) );
			set.setInt( "shield_def", q.getFieldInt( "shield_def" ) );
			set.setInt( "shield_def_rate", q.getFieldInt( "shield_def_rate" ) );
			set.setInt( "atk_speed", q.getFieldInt( "atk_speed" ) );
			set.setInt( "mp_consume", q.getFieldInt( "mp_consume" ) );
			set.setInt( "m_dam", q.getFieldInt( "m_dam" ) );
			set.setInt( "duration", q.getFieldInt( "duration" ) );
			set.setInt( "time", q.getFieldInt( "time" ) );
			set.setInt( "price", q.getFieldInt( "price" ) );
			set.setInt( "crystal_count", q.getFieldInt( "crystal_count" ) );
			set.setBool( "sellable", q.getFieldBool( "sellable" ) );
			set.setBool( "dropable", q.getFieldBool( "dropable" ) );
			set.setBool( "destroyable", q.getFieldBool( "destroyable" ) );
			set.setBool( "tradeable", q.getFieldBool( "tradeable" ) );
			set.setString( "skill", q.getFieldStr( "skill" ) );

			set.setInt( "enchant4_skill_id", q.getFieldInt( "enchant4_skill_id" ) );
			set.setInt( "enchant4_skill_lvl", q.getFieldInt( "enchant4_skill_lvl" ) );
			
			/*item.set.set("onCast_skill_id", rset.getInt("onCast_skill_id"));
			item.set.set("onCast_skill_lvl", rset.getInt("onCast_skill_lvl"));
			item.set.set("onCast_skill_chance", rset.getInt("onCast_skill_chance"));
			
			item.set.set("onCrit_skill_id", rset.getInt("onCrit_skill_id"));
			item.set.set("onCrit_skill_lvl", rset.getInt("onCrit_skill_lvl"));
			item.set.set("onCrit_skill_chance", rset.getInt("onCrit_skill_chance"));*/
			
			set.setInt( "change_weaponId", q.getFieldInt( "change_weaponId" ) );

			if( weaponType == WEAPON_PET )
			{
				set.setInt( "type1", L2ItemTemplate::TYPE1_WEAPON_RING_EARRING_NECKLACE );
				if( bodypart == SLOT_WOLF )
					set.setInt( "type2", L2ItemTemplate::TYPE2_PET_WOLF );
				else if( bodypart == SLOT_GREATWOLF )
					set.setInt( "type2", L2ItemTemplate::TYPE2_PET_EVOLVEDWOLF );
				else if( bodypart == SLOT_HATCHLING )
					set.setInt( "type2", L2ItemTemplate::TYPE2_PET_HATCHLING );
				else if( bodypart == SLOT_BABYPET )
					set.setInt( "type2", L2ItemTemplate::TYPE2_PET_BABY );
				else
					set.setInt( "type2", L2ItemTemplate::TYPE2_PET_STRIDER );

				bodypart = SLOT_R_HAND;
				set.setInt( "bodypart", (int)SLOT_R_HAND);
			}

			// create and insert template
			L2WeaponTemplate *tmpl = new L2WeaponTemplate( weaponType, set );
			m_allTemplates[item_id] = (L2ItemTemplate *)tmpl;

			// count bytes allocated
			if( tmpl ) m_nTotalBytesAllocated += (sizeof(L2WeaponTemplate) + 2*wcslen(tmpl->getName()));
			//else throw std::bad_alloc( "ItemTable:loadWeapons new L2WeaponTemplate" );
		}
	}
	else
		LogError( L"ItemTable::loadWeapons(): MySQL error: %s", con->getErrorStr() );
}

const L2ItemTemplate *ItemTable::getTemplate( unsigned int itemId )
{
	if( !m_allTemplates ) return NULL;
	if( itemId < 0 || itemId > m_maxItemId )
	{
		LogError( L"ItemTable::getTemplate( %u ): request to incorrect itemId (max id is %u)\n",
			itemId, m_maxItemId );
		return NULL;
	}
	return (const L2ItemTemplate *)m_allTemplates[itemId];
}
