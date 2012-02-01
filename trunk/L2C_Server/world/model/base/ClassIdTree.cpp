#include "pch.h"
#include "ClassIdTree.h"

// mem for static vars
ClassIdTree *ClassIdTree::_instance = NULL;
int ClassIdTree::_refCount = 0;

ClassIdTree *ClassIdTree::getInstance()
{
	if( !_instance )
	{
		_instance = new ClassIdTree();
		_refCount++;
	}
	return _instance;
}

void ClassIdTree::freeInstance()
{
	if( _instance )
	{
		_refCount--;
		if( _refCount == 0 )
		{
			delete _instance;
			_instance = NULL;
		}
	}
}

ClassIdTree::ClassIdTree()
{
	int i;
	for( i=0; i<NUM_CLASS_IDS; i++ ) m_class[i] = NULL;

	// add info about all classes
	addClassId( 0x00, L"Fighter",         false, false, RACE_Human, ClassId::CID_NONE );
	addClassId( 0x01, L"Warrior",         false, false, RACE_Human, ClassId::CID_HUMAN_FIGHTER );
	addClassId( 0x02, L"Gladiator",       false, false, RACE_Human, ClassId::CID_WARRIOR );
	addClassId( 0x03, L"Warlord",         false, false, RACE_Human, ClassId::CID_WARRIOR );
	addClassId( 0x04, L"Knight",          false, false, RACE_Human, ClassId::CID_HUMAN_FIGHTER );
	addClassId( 0x05, L"Palladin",        false, false, RACE_Human, ClassId::CID_KNIGHT );
	addClassId( 0x06, L"Dark Avenger",    false, false, RACE_Human, ClassId::CID_KNIGHT );
	addClassId( 0x07, L"Rogue",           false, false, RACE_Human, ClassId::CID_HUMAN_FIGHTER );
	addClassId( 0x08, L"Treasure Hunter", false, false, RACE_Human, ClassId::CID_ROGUE );
	addClassId( 0x09, L"Hawkeye",         false, false, RACE_Human, ClassId::CID_ROGUE );

	addClassId( 0x0A, L"Mage",        true, false, RACE_Human, ClassId::CID_NONE );
	addClassId( 0x0B, L"Wizard",      true, false, RACE_Human, ClassId::CID_HUMAN_MAGE );
	addClassId( 0x0C, L"Sorceror",    true, false, RACE_Human, ClassId::CID_HUMAN_WIZARD );
	addClassId( 0x0D, L"Necromancer", true, false, RACE_Human, ClassId::CID_HUMAN_WIZARD );
	addClassId( 0x0E, L"Warlock",     true, true,  RACE_Human, ClassId::CID_HUMAN_WIZARD );
	addClassId( 0x0F, L"Cleric",      true, false, RACE_Human, ClassId::CID_HUMAN_MAGE );
	addClassId( 0x10, L"Bishop",      true, false, RACE_Human, ClassId::CID_CLERIC );
	addClassId( 0x11, L"Prophet",     true, false, RACE_Human, ClassId::CID_CLERIC );

	addClassId( 0x12, L"Elven fighter", false, false, RACE_Elf, ClassId::CID_NONE );
	addClassId( 0x13, L"Elven Knight",  false, false, RACE_Elf, ClassId::CID_ELVEN_FIGHTER );
	addClassId( 0x14, L"Temple Knight", false, false, RACE_Elf, ClassId::CID_ELVEN_KNIGHT );
	addClassId( 0x15, L"Swordsinger",   false, false, RACE_Elf, ClassId::CID_ELVEN_KNIGHT );
	addClassId( 0x16, L"Elven Scout",   false, false, RACE_Elf, ClassId::CID_ELVEN_FIGHTER );
	addClassId( 0x17, L"Plains Walker", false, false, RACE_Elf, ClassId::CID_ELVEN_SCOUT );
	addClassId( 0x18, L"Silver Ranger", false, false, RACE_Elf, ClassId::CID_ELVEN_SCOUT );

	addClassId( 0x19, L"Elven Mage",        true, false, RACE_Elf, ClassId::CID_NONE );
	addClassId( 0x1A, L"Elven Wizard",      true, false, RACE_Elf, ClassId::CID_ELVEN_MAGE );
	addClassId( 0x1B, L"Spellsinger",       true, false, RACE_Elf, ClassId::CID_ELVEN_WIZARD );
	addClassId( 0x1C, L"Elemenal Summoner", true, true,  RACE_Elf, ClassId::CID_ELVEN_WIZARD );
	addClassId( 0x1D, L"Elven Oracle",      true, false, RACE_Elf, ClassId::CID_ELVEN_MAGE );
	addClassId( 0x1E, L"Elven Elder",       true, false, RACE_Elf, ClassId::CID_ELVEN_ORACLE );

	addClassId( 0x1F, L"Dark Fighter",   false, false, RACE_DarkElf, ClassId::CID_NONE );
	addClassId( 0x20, L"Palus Knight",   false, false, RACE_DarkElf, ClassId::CID_DARK_FIGHTER );
	addClassId( 0x21, L"Shillen Knight", false, false, RACE_DarkElf, ClassId::CID_PALUS_KNIGHT );
	addClassId( 0x22, L"Bladedancer",    false, false, RACE_DarkElf, ClassId::CID_PALUS_KNIGHT );
	addClassId( 0x23, L"Assasin",        false, false, RACE_DarkElf, ClassId::CID_DARK_FIGHTER );
	addClassId( 0x24, L"Abyss Walker",   false, false, RACE_DarkElf, ClassId::CID_ASSASIN );
	addClassId( 0x25, L"Phantom Ranger", false, false, RACE_DarkElf, ClassId::CID_ASSASIN );

	addClassId( 0x26, L"Dark Mage",        true, false, RACE_DarkElf, ClassId::CID_NONE );
	addClassId( 0x27, L"Dark Wizard",      true, false, RACE_DarkElf, ClassId::CID_DARK_MAGE );
	addClassId( 0x28, L"Spellhowler",      true, false, RACE_DarkElf, ClassId::CID_DARK_WIZARD );
	addClassId( 0x29, L"Phantom Summoner", true, true,  RACE_DarkElf, ClassId::CID_DARK_WIZARD );
	addClassId( 0x2A, L"Shillen Oracle",   true, false, RACE_DarkElf, ClassId::CID_DARK_MAGE );
	addClassId( 0x2B, L"Shillen ELder",    true, false, RACE_DarkElf, ClassId::CID_SHILLEN_ORACLE );

	addClassId( 0x2C, L"Orc Fighter", false, false, RACE_Orc, ClassId::CID_NONE );
	addClassId( 0x2D, L"Orc Rider",   false, false, RACE_Orc, ClassId::CID_ORC_FIGHTER );
	addClassId( 0x2E, L"Destroyer",   false, false, RACE_Orc, ClassId::CID_ORC_RIDER );
	addClassId( 0x2F, L"Orc Monk",    false, false, RACE_Orc, ClassId::CID_ORC_FIGHTER );
	addClassId( 0x30, L"Tyrant",      false, false, RACE_Orc, ClassId::CID_ORC_MONK );

	addClassId( 0x31, L"Orc Mage",   true, false, RACE_Orc, ClassId::CID_NONE );
	addClassId( 0x32, L"Orc Shaman", true, false, RACE_Orc, ClassId::CID_ORC_MAGE );
	addClassId( 0x33, L"Overlord",   true, false, RACE_Orc, ClassId::CID_ORC_SHAMAN );
	addClassId( 0x34, L"Warcryer",   true, false, RACE_Orc, ClassId::CID_ORC_SHAMAN );

	addClassId( 0x35, L"Dwarven Fighter", false, false, RACE_Dwarf, ClassId::CID_NONE );
	addClassId( 0x36, L"Scavenger",       false, false, RACE_Dwarf, ClassId::CID_DWARVEN_FIGHTER );
	addClassId( 0x37, L"Bounty Hunter",   false, false, RACE_Dwarf, ClassId::CID_SCAVENGER );
	addClassId( 0x38, L"Artisan",         false, false, RACE_Dwarf, ClassId::CID_DWARVEN_FIGHTER );
	addClassId( 0x39, L"Warsmith",        false, false, RACE_Dwarf, ClassId::CID_ARTISAN );
	
	// 3rd prof
	addClassId( 0x58, L"Duelist",         false, false, RACE_Human, ClassId::CID_GLADIATOR );
	addClassId( 0x59, L"Dreadnought",     false, false, RACE_Human, ClassId::CID_WARLORD );
	addClassId( 0x5A, L"Phoenix Knight",  false, false, RACE_Human, ClassId::CID_PALLADIN );
	addClassId( 0x5B, L"Hell Knight",     false, false, RACE_Human, ClassId::CID_DARK_AVENGER );
	addClassId( 0x5C, L"Sagittarius",     false, false, RACE_Human, ClassId::CID_HAWKEYE );
	addClassId( 0x5D, L"Adventurer",      false, false, RACE_Human, ClassId::CID_TREASURE_HUNTER );

	addClassId( 0x5E, L"Archmage",        true, false, RACE_Human, ClassId::CID_SORCEROR );
	addClassId( 0x5F, L"Soultaker",       true, false, RACE_Human, ClassId::CID_NECROMANCER );
	addClassId( 0x60, L"Arcana Lord",     true, true,  RACE_Human, ClassId::CID_WARLOCK );
	addClassId( 0x61, L"Cardinal",        true, false, RACE_Human, ClassId::CID_BISHOP );
	addClassId( 0x62, L"Hierophant",      true, false, RACE_Human, ClassId::CID_PROPHET );

	addClassId( 0x63, L"Eva's Templar",      false, false, RACE_Elf, ClassId::CID_TEMPLE_KNIGHT );
	addClassId( 0x64, L"Sword Muse",         false, false, RACE_Elf, ClassId::CID_SWORDSINGER );
	addClassId( 0x65, L"Wind Rider",         false, false, RACE_Elf, ClassId::CID_PLAINSWALKER );
	addClassId( 0x66, L"Moonlight Sentinel", false, false, RACE_Elf, ClassId::CID_SILVER_RANGER );
	addClassId( 0x67, L"Mustic Muse",        true,  false, RACE_Elf, ClassId::CID_SPELLSINGER );
	addClassId( 0x68, L"Elemental Master",   true,  true,  RACE_Elf, ClassId::CID_ELEMENTAL_SUMMONER );
	addClassId( 0x69, L"Eva's Saint",        true,  false, RACE_Elf, ClassId::CID_ELVEN_ELDER );

	addClassId( 0x6A, L"Shillen Templar",    false, false, RACE_DarkElf, ClassId::CID_SHILLEN_KNIGHT );
	addClassId( 0x6B, L"Spectral Dancer",    false, false, RACE_DarkElf, ClassId::CID_BLADEDANCER );
	addClassId( 0x6C, L"Ghost Hunter",       false, false, RACE_DarkElf, ClassId::CID_ABYSS_WALKER );
	addClassId( 0x6D, L"Ghost Sentinel",     false, false, RACE_DarkElf, ClassId::CID_PHANTOM_RANGER );

	addClassId( 0x6E, L"Storm Screamer",     true, false, RACE_DarkElf, ClassId::CID_SPELLHOWLER );
	addClassId( 0x6F, L"Spectral Master",    true, true,  RACE_DarkElf, ClassId::CID_PHANTOM_SUMMONER );
	addClassId( 0x70, L"Shillen Saint",      true, false, RACE_DarkElf, ClassId::CID_SHILLEN_ELDER );

	addClassId( 0x71, L"Titan",              false, false, RACE_Orc, ClassId::CID_DESTROYER );
	addClassId( 0x72, L"Grand Khavatari",    false, false, RACE_Orc, ClassId::CID_TYRANT );
	addClassId( 0x73, L"Dominator",          true,  false, RACE_Orc, ClassId::CID_OVERLORD );
	addClassId( 0x74, L"Doomcryer",          true,  false, RACE_Orc, ClassId::CID_WARCRYER );

	addClassId( 0x75, L"Fortune Seeker",     false, false, RACE_Dwarf, ClassId::CID_BOUNTY_HUNTER );
	addClassId( 0x76, L"Maestro",            false, false, RACE_Dwarf, ClassId::CID_WARSMITH );

	addClassId( 0x7B, L"Kamael Male Soldier",   false, false, RACE_Kamael, ClassId::CID_NONE );
	addClassId( 0x7C, L"Kamael Female Soldier", false, false, RACE_Kamael, ClassId::CID_NONE );
	addClassId( 0x7D, L"Trooper",               false, false, RACE_Kamael, ClassId::CID_KAMAEL_MALE_SOLDIER );
	addClassId( 0x7E, L"Warder",                false, false, RACE_Kamael, ClassId::CID_KAMAEL_FEMALE_SOLDIER );

	addClassId( 0x7F, L"Berserker",             false, false, RACE_Kamael, ClassId::CID_TROOPER );
	addClassId( 0x80, L"Male Soulbreaker",      false, false, RACE_Kamael, ClassId::CID_TROOPER );
	addClassId( 0x81, L"Female Soulbreaker",    false, false, RACE_Kamael, ClassId::CID_WARDER );
	addClassId( 0x82, L"Arbalester",            false, false, RACE_Kamael, ClassId::CID_WARDER );

	addClassId( 0x83, L"Doombringer",           false, false, RACE_Kamael, ClassId::CID_BERSERKER );
	addClassId( 0x84, L"Male Soulhound",        false, false, RACE_Kamael, ClassId::CID_MALE_SOULBREAKER );
	addClassId( 0x85, L"Female Soulhound",      false, false, RACE_Kamael, ClassId::CID_FEMALE_SOULBREAKER );
	addClassId( 0x86, L"Trickster",             false, false, RACE_Kamael, ClassId::CID_ARBALESTER );
	addClassId( 0x87, L"Inspector",             false, false, RACE_Kamael, ClassId::CID_NONE ); // Hidden subclass, no first class which it comes from
	addClassId( 0x88, L"Judicator",             false, false, RACE_Kamael, ClassId::CID_INSPECTOR );
}

ClassIdTree::~ClassIdTree()
{
	int i;
	for( i=0; i<NUM_CLASS_IDS; i++ )
	{
		if( m_class[i] )
		{
			delete m_class[i];
			m_class[i] = NULL;
		}
	}
}

void ClassIdTree::addClassId( int id, const wchar_t *name, bool isMage, bool isSummoner, Race race, int parentId )
{
	if( (id>=0) && (id<NUM_CLASS_IDS) )
	{
		assert( m_class[id] == NULL );
		if( m_class[id] == NULL )
			m_class[id] = new ClassId( id, name, isMage, isSummoner, race, parentId );
	}
}

const ClassId *ClassIdTree::getClassId( int class_id ) const
{
	if( (class_id>=0) && (class_id<NUM_CLASS_IDS) ) return m_class[class_id];
	return NULL;
}

const ClassId *ClassIdTree::getParentClassId( int class_id ) const
{
	const ClassId *c = getClassId( class_id );
	if( c ) return getClassId( c->getParentId() );
	return NULL;
}

