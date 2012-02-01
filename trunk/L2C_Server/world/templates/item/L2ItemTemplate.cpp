#include "pch.h"
#include "L2ItemTemplate.h"
#include "l2c_utils.h"
#include "Log.h"

L2ItemTemplate::L2ItemTemplate( L2ItemType itemType, L2ItemSubType subType, StatsSet& set )
{
	m_type = itemType;
	m_subType = subType;
	int temp_i;
	//   id
	set.getInt( "item_id", &m_itemId );
	//   name
	std::wstring wstr_name;
	set.getWString( "name", wstr_name );
	m_name = NULL;
	if( wstr_name.length() > 0 )
		m_name = _wcsdup( wstr_name.c_str() );
	set.getInt( "type1", &m_type1, 0 );
	set.getInt( "type2", &m_type2, 0 );
	set.getInt( "weight", &m_weight, 0 );
	set.getBool( "crystallizable", &m_crystallizable, false );
	set.getBool( "stackable", &m_stackable, false );
	set.getInt( "material", (int *)&m_materialType );
	set.getInt( "crystal_type", (int *)&m_crystalType, (int)CRYSTAL_NONE ); // default to none-grade
	set.getInt( "duration", &m_duration ); // default 0 or -1?
	set.getInt( "time", &m_time ); // default seems to be -1
	set.getInt( "bodypart", &temp_i );
	m_bodyPart = (L2ItemSlot)temp_i;
	set.getInt( "price", &m_referencePrice );
	set.getInt( "crystal_count", &m_crystalCount, 0 );
	set.getBool( "sellable", &m_sellable, true );
	set.getBool( "dropable", &m_dropable, true );
	set.getBool( "destroyable", &m_destroyable, true );
	set.getBool( "tradeable", &m_tradeable, true );
	// checks by item id
	m_common = (m_itemId >= 12006 && m_itemId <= 12361) || (m_itemId >= 11605 && m_itemId <= 12308);
	m_heroItem = (m_itemId >= 6611 && m_itemId <= 6621) || (m_itemId >= 9388 && m_itemId <= 9390) || m_itemId == 6842;
	m_pvpItem = (m_itemId >= 10667 && m_itemId <= 10792) || (m_itemId >= 10793 && m_itemId <= 10835) || (m_itemId >= 12852 && m_itemId <= 12977) || (m_itemId >= 14363 && m_itemId <= 14519) || (m_itemId >= 14520 && m_itemId <= 14525) || m_itemId == 14528 || m_itemId == 14529 || m_itemId == 14558;
}

L2ItemTemplate::~L2ItemTemplate()
{
	if( m_name )
	{
		free( m_name );
		m_name = NULL;
	}
}


L2ItemSubType L2ItemTemplate::getArmorTypeByName( const char *name )
{
	L2ItemSubType ret = SYBTYPE_INCORRECT;
	if( !name ) return ret;
	if( _stricmp( name, "none" ) == 0 ) ret = ARMOR_NONE;
	else if( _stricmp( name, "light" ) == 0 ) ret = ARMOR_LIGHT;
	else if( _stricmp( name, "heavy" ) == 0 ) ret = ARMOR_HEAVY;
	else if( _stricmp( name, "magic" ) == 0 ) ret = ARMOR_MAGIC;
	else if( _stricmp( name, "pet" ) == 0 ) ret = ARMOR_PET;
	else if( _stricmp( name, "sigil" ) == 0 ) ret = ARMOR_SIGIL;
	if( ret == SYBTYPE_INCORRECT )
		LogWarning( L"L2ItemTemplate::getArmorTypeByName(): unknown name [%S]", name );
	return ret;
}

L2ItemSubType L2ItemTemplate::getWeaponTypeByName( const char *name )
{
	L2ItemSubType ret = SYBTYPE_INCORRECT;
	if( !name ) return ret;
	if( _stricmp( name, "none" ) == 0 ) ret = WEAPON_NONE; // shield
	else if( _stricmp( name, "sword" ) == 0 ) ret = WEAPON_SWORD;
	else if( _stricmp( name, "blunt" ) == 0 ) ret = WEAPON_BLUNT;
	else if( _stricmp( name, "dagger" ) == 0 ) ret = WEAPON_DAGGER;
	else if( _stricmp( name, "bow" ) == 0 ) ret = WEAPON_BOW;
	else if( _stricmp( name, "pole" ) == 0 ) ret = WEAPON_POLE;
	else if( _stricmp( name, "etc" ) == 0 ) ret = WEAPON_ETC;
	else if( _stricmp( name, "fist" ) == 0 ) ret = WEAPON_FIST;
	else if( _stricmp( name, "dual" ) == 0 ) ret = WEAPON_DUAL;
	else if( _stricmp( name, "dualfist" ) == 0 ) ret = WEAPON_DUALFIST;
	else if( _stricmp( name, "bigsword" ) == 0 ) ret = WEAPON_BIGSWORD;
	else if( _stricmp( name, "pet" ) == 0 ) ret = WEAPON_PET;
	else if( _stricmp( name, "rod" ) == 0 ) ret = WEAPON_ROD;
	else if( _stricmp( name, "bigblunt" ) == 0 ) ret = WEAPON_BIGBLUNT;
	else if( _stricmp( name, "ancient" ) == 0 ) ret = WEAPON_ANCIENT_SWORD;
	else if( _stricmp( name, "crossbow" ) == 0 ) ret = WEAPON_CROSSBOW;
	else if( _stricmp( name, "rapier" ) == 0 ) ret = WEAPON_RAPIER;
	else if( _stricmp( name, "dualdagger" ) == 0 ) ret = WEAPON_DUAL_DAGGER;
	if( ret == SYBTYPE_INCORRECT )
		LogWarning( L"L2ItemTemplate::getWeaponTypeByName(): unknown name [%S]", name );
	return ret;
}

L2ItemSlot L2ItemTemplate::getSlotByName( const char *name )
{
	if( !name ) return SLOT_NONE;
	L2ItemSlot ret = SLOT_NONE;
	//
	if( _stricmp( name, "shirt" ) == 0 ) ret = SLOT_UNDERWEAR;
	else if( _stricmp( name, "lbracelet" ) == 0 ) ret = SLOT_L_BRACELET;
	else if( _stricmp( name, "rbracelet" ) == 0 ) ret = SLOT_R_BRACELET;
	else if( _stricmp( name, "talisman" ) == 0 ) ret = SLOT_DECO;
	else if( _stricmp( name, "chest" ) == 0 ) ret = SLOT_CHEST;
	else if( _stricmp( name, "fullarmor" ) == 0 ) ret = SLOT_FULL_ARMOR;
	else if( _stricmp( name, "head" ) == 0 ) ret = SLOT_HEAD;
	else if( _stricmp( name, "hair" ) == 0 ) ret = SLOT_HAIR;
	else if( _stricmp( name, "face" ) == 0 ) ret = SLOT_HAIR2;
	else if( _stricmp( name, "hair2" ) == 0 ) ret = SLOT_HAIR2;
	else if( _stricmp( name, "dhair" ) == 0 ) ret = SLOT_HAIRALL;
	else if( _stricmp( name, "hairall" ) == 0 ) ret = SLOT_HAIRALL;
	else if( _stricmp( name, "underwear" ) == 0 ) ret = SLOT_UNDERWEAR;
	else if( _stricmp( name, "back" ) == 0 ) ret = SLOT_BACK;
	else if( _stricmp( name, "neck" ) == 0 ) ret = SLOT_NECK;
	else if( _stricmp( name, "legs" ) == 0 ) ret = SLOT_LEGS;
	else if( _stricmp( name, "feet" ) == 0 ) ret = SLOT_FEET;
	else if( _stricmp( name, "gloves" ) == 0 ) ret = SLOT_GLOVES;
	//else if( _stricmp( name, "chest,legs" ) == 0 ) ret = SLOT_CHEST | SLOT_LEGS;
	else if( _stricmp( name, "belt" ) == 0 ) ret = SLOT_BELT;
	else if( _stricmp( name, "rhand" ) == 0 ) ret = SLOT_R_HAND;
	else if( _stricmp( name, "lhand" ) == 0 ) ret = SLOT_L_HAND;
	else if( _stricmp( name, "lrhand" ) == 0 ) ret = SLOT_LR_HAND;
	//else if( _stricmp( name, "rear,lear" ) == 0 ) ret = SLOT_R_EAR | SLOT_L_EAR;
	//else if( _stricmp( name, "rfinger,lfinger" ) == 0 ) ret = SLOT_R_FINGER | SLOT_L_FINGER;
	else if( _stricmp( name, "wolf" ) == 0 ) ret = SLOT_WOLF;
	else if( _stricmp( name, "greatwolf" ) == 0 ) ret = SLOT_GREATWOLF;
	else if( _stricmp( name, "hatchling" ) == 0 ) ret = SLOT_HATCHLING;
	else if( _stricmp( name, "strider" ) == 0 ) ret = SLOT_STRIDER;
	else if( _stricmp( name, "babypet" ) == 0 ) ret = SLOT_BABYPET;
	else if( _stricmp( name, "none" ) == 0 ) ret = SLOT_NONE;
	//
	if( ret == -1 )
	{
		LogError( L"L2ItemTemplate::getSlotIdByName(): unknown bodypart slot name [%S]", name );
		return SLOT_NONE;
	}
	return ret;
}

L2ItemMaterial L2ItemTemplate::getMaterialByName( const char *name )
{
	if( !name ) return MATERIAL_NONE;
	L2ItemMaterial ret = MATERIAL_NONE;
	if( _stricmp( name, "steel" ) == 0 ) ret = MATERIAL_STEEL;
	else if( _stricmp( name, "fine_steel" ) == 0 ) ret = MATERIAL_FINE_STEEL;
	else if( _stricmp( name, "blood_steel" ) == 0 ) ret = MATERIAL_BLOOD_STEEL;
	else if( _stricmp( name, "bronze" ) == 0 ) ret = MATERIAL_BRONZE;
	else if( _stricmp( name, "silver" ) == 0 ) ret = MATERIAL_SILVER;
	else if( _stricmp( name, "gold" ) == 0 ) ret = MATERIAL_GOLD;
	else if( _stricmp( name, "mithril" ) == 0 ) ret = MATERIAL_MITHRIL;
	else if( _stricmp( name, "oriharukon" ) == 0 ) ret = MATERIAL_ORIHARUKON;
	else if( _stricmp( name, "paper" ) == 0 ) ret = MATERIAL_PAPER;
	else if( _stricmp( name, "wood" ) == 0 ) ret = MATERIAL_WOOD;
	else if( _stricmp( name, "cloth" ) == 0 ) ret = MATERIAL_CLOTH;
	else if( _stricmp( name, "leather" ) == 0 ) ret = MATERIAL_LEATHER;
	else if( _stricmp( name, "bone" ) == 0 ) ret = MATERIAL_BONE;
	else if( _stricmp( name, "horn" ) == 0 ) ret = MATERIAL_HORN;
	else if( _stricmp( name, "damascus" ) == 0 ) ret = MATERIAL_DAMASCUS;
	else if( _stricmp( name, "adamantaite" ) == 0 ) ret = MATERIAL_ADAMANTAITE;
	else if( _stricmp( name, "chrysolite" ) == 0 ) ret = MATERIAL_CHRYSOLITE;
	else if( _stricmp( name, "crystal" ) == 0 ) ret = MATERIAL_CRYSTAL;
	else if( _stricmp( name, "liquid" ) == 0 ) ret = MATERIAL_LIQUID;
	else if( _stricmp( name, "scale_of_dragon" ) == 0 ) ret = MATERIAL_SCALE_OF_DRAGON;
	else if( _stricmp( name, "dyestuff" ) == 0 ) ret = MATERIAL_DYESTUFF;
	else if( _stricmp( name, "cobweb" ) == 0 ) ret = MATERIAL_COBWEB;
	else if( _stricmp( name, "seed" ) == 0 ) ret = MATERIAL_SEED;
	else if( _stricmp( name, "cotton" ) == 0 ) ret = MATERIAL_COTTON;
	else
	{
		LogError( L"L2ItemTemplate::getMaterialIdByName(): unknown material name [%S]", name );
	}
	return ret;
}

L2ItemCrystal L2ItemTemplate::getCrystalTypeByName( const char *name )
{
	if( !name ) return CRYSTAL_NONE;
	L2ItemCrystal ret = CRYSTAL_NONE;
	if( _stricmp( name, "none" ) == 0 ) ret = CRYSTAL_NONE;
	else if( _stricmp( name, "a" ) == 0 ) ret = CRYSTAL_A;
	else if( _stricmp( name, "b" ) == 0 ) ret = CRYSTAL_B;
	else if( _stricmp( name, "c" ) == 0 ) ret = CRYSTAL_C;
	else if( _stricmp( name, "d" ) == 0 ) ret = CRYSTAL_D;
	else if( _stricmp( name, "s" ) == 0 ) ret = CRYSTAL_S;
	else if( _stricmp( name, "s80" ) == 0 ) ret = CRYSTAL_S80;
	else if( _stricmp( name, "s84" ) == 0 ) ret = CRYSTAL_S84;
	else
	{
		LogError( L"L2ItemTemplate::getCrystalTypeByName(): unknown crystal name [%S]", name );
	}
	return ret;
}
