#pragma once
#include "Race.h"

class ClassId
{
public:
	static const int CID_NONE = -1;
	static const int CID_HUMAN_FIGHTER = 0x00;
	static const int CID_WARRIOR = 0x01;
	static const int CID_GLADIATOR = 0x02;
	static const int CID_WARLORD = 0x03;
	static const int CID_KNIGHT = 0x04;
	static const int CID_PALLADIN = 0x05;
	static const int CID_DARK_AVENGER = 0x06;
	static const int CID_ROGUE = 0x07;
	static const int CID_TREASURE_HUNTER = 0x08;
	static const int CID_HAWKEYE = 0x09;

	static const int CID_HUMAN_MAGE = 0x0A;
	static const int CID_HUMAN_WIZARD = 0x0B;
	static const int CID_SORCEROR = 0x0C;
	static const int CID_NECROMANCER = 0x0D;
	static const int CID_WARLOCK = 0x0E;
	static const int CID_CLERIC = 0x0F;
	static const int CID_BISHOP = 0x10;
	static const int CID_PROPHET = 0x11;

	static const int CID_ELVEN_FIGHTER = 0x12;
	static const int CID_ELVEN_KNIGHT = 0x13;
	static const int CID_TEMPLE_KNIGHT = 0x14;
	static const int CID_SWORDSINGER = 0x15;
	static const int CID_ELVEN_SCOUT = 0x16;
	static const int CID_PLAINSWALKER = 0x17;
	static const int CID_SILVER_RANGER = 0x18;

	static const int CID_ELVEN_MAGE = 0x19;
	static const int CID_ELVEN_WIZARD = 0x1A;
	static const int CID_SPELLSINGER = 0x1B;
	static const int CID_ELEMENTAL_SUMMONER = 0x1C;
	static const int CID_ELVEN_ORACLE = 0x1D;
	static const int CID_ELVEN_ELDER = 0x1E;

	static const int CID_DARK_FIGHTER = 0x1F;
	static const int CID_PALUS_KNIGHT = 0x20;
	static const int CID_SHILLEN_KNIGHT = 0x21;
	static const int CID_BLADEDANCER = 0x22;
	static const int CID_ASSASIN = 0x23;
	static const int CID_ABYSS_WALKER = 0x24;
	static const int CID_PHANTOM_RANGER = 0x25;

	static const int CID_DARK_MAGE = 0x26;
	static const int CID_DARK_WIZARD = 0x27;
	static const int CID_SPELLHOWLER = 0x28;
	static const int CID_PHANTOM_SUMMONER = 0x29;
	static const int CID_SHILLEN_ORACLE = 0x2A;
	static const int CID_SHILLEN_ELDER = 0x2B;

	static const int CID_ORC_FIGHTER = 0x2C;
	static const int CID_ORC_RIDER = 0x2D;
	static const int CID_DESTROYER = 0x2E;
	static const int CID_ORC_MONK = 0x2F;
	static const int CID_TYRANT = 0x30;

	static const int CID_ORC_MAGE = 0x31;
	static const int CID_ORC_SHAMAN = 0x32;
	static const int CID_OVERLORD = 0x33;
	static const int CID_WARCRYER = 0x34;

	static const int CID_DWARVEN_FIGHTER = 0x35;
	static const int CID_SCAVENGER = 0x36;
	static const int CID_BOUNTY_HUNTER = 0x37;
	static const int CID_ARTISAN = 0x38;
	static const int CID_WARSMITH = 0x39;

	static const int CID_DUELIST = 0x58;

	static const int CID_KAMAEL_MALE_SOLDIER = 0x7B;
	static const int CID_KAMAEL_FEMALE_SOLDIER = 0x7C;
	static const int CID_TROOPER = 0x7D;
	static const int CID_WARDER = 0x7E;
	static const int CID_BERSERKER = 0x7F;
	static const int CID_MALE_SOULBREAKER = 0x80;
	static const int CID_FEMALE_SOULBREAKER = 0x81;
	static const int CID_ARBALESTER = 0x82;
	static const int CID_DOOMBRINGER = 0x83;
	static const int CID_MALE_SOULHOUND = 0x84;
	static const int CID_FEMALE_SOULHOUND = 0x85;
	static const int CID_TRICKSTER = 0x86;
	static const int CID_INSPECTOR = 0x87;
	static const int CID_JUDICATOR = 0x88;

public:
	ClassId( int id, const wchar_t *name, bool isMage, bool isSummoner, Race race, int parentId );
	int  getId() const;
	const wchar_t *getName() const;
	bool isMage() const;
	bool isSummoner() const;
	Race getRace() const;
	int  getParentId() const;
public:
	// 0 - 0 profession, 1 - 1st profession, 2 - 2nd prof, 3 - 3rd prof
	int level() const;
	bool isChildOf( int classId ) const;
	bool isEqualOrChildOf( int classId ) const;
protected:
	int     m_id;
	wchar_t m_name[32];
	bool    m_isMage;
	bool    m_isSummoner;
	Race    m_race;
	int     m_parentId; // -1 - no parent class
};
