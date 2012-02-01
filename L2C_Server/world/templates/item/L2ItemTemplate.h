#pragma once
#include "L2ItemType.h"
#include "../StatsSet.h"

class L2ItemTemplate
{
public:
	static const int TYPE1_WEAPON_RING_EARRING_NECKLACE = 0;
	static const int TYPE1_SHIELD_ARMOR = 1;
	static const int TYPE1_ITEM_QUESTITEM_ADENA = 4;
	
	static const int TYPE2_WEAPON = 0;
	static const int TYPE2_SHIELD_ARMOR = 1;
	static const int TYPE2_ACCESSORY = 2;
	static const int TYPE2_QUEST = 3;
	static const int TYPE2_MONEY = 4;
	static const int TYPE2_OTHER = 5;
	static const int TYPE2_PET_WOLF = 6;
	static const int TYPE2_PET_HATCHLING = 7;
	static const int TYPE2_PET_STRIDER = 8;
	static const int TYPE2_PET_BABY = 9;
	static const int TYPE2_PET_EVOLVEDWOLF = 10;
	
protected:
	//static const int crystalItemId[] = { 0, 1458, 1459, 1460, 1461, 1462, 1462, 1462 };
	//static const int crystalEnchantBonusArmor[] = { 0, 11, 6, 11, 19, 25, 25, 25 };
	//static const int crystalEnchantBonusWeapon[] = { 0, 90, 45, 67, 144, 250, 250, 250 };

public:
	L2ItemTemplate( L2ItemType itemType, L2ItemSubType subType, StatsSet& set );
	virtual ~L2ItemTemplate();

public:
	static L2ItemSubType  getArmorTypeByName( const char *name );
	static L2ItemSubType  getWeaponTypeByName( const char *name );
	static L2ItemSlot     getSlotByName( const char *name );
	static L2ItemMaterial getMaterialByName( const char *name );
	static L2ItemCrystal  getCrystalTypeByName( const char *name );

public:
	L2ItemType     getType() const              { return m_type; }
	L2ItemSubType  getSubType() const           { return m_subType; }
	bool           isArmor() const              { return (m_subType>=ARMOR_NONE) && (m_subType<=ARMOR_SIGIL); }
	bool           isWeapon() const             { return (m_subType>=WEAPON_NONE) && (m_subType<=WEAPON_DUAL_DAGGER); }
	bool           isEtcItem() const            { return (m_subType>=ETCITEM_ARROW) && (m_subType<=ETCITEM_HERB); }
	int            getId() const                { return m_itemId; }
	const wchar_t *getName() const              { return (const wchar_t *)m_name; }
	int            getType1() const             { return m_type1; }
	int            getType2() const             { return m_type2; }
	int            getWeight() const            { return m_weight; }
	L2ItemMaterial getMaterialType() const      { return m_materialType; }
	L2ItemCrystal  getCrystalType() const       { return m_crystalType; }
	int            getDuration() const          { return m_duration; }
	int            getTime() const              { return m_time; }
	L2ItemSlot     getBodyPart() const          { return m_bodyPart; }
	int            getPrice() const             { return m_referencePrice; }
	int            getCrystalCount() const      { return m_crystalCount; }
	bool           isStackable() const          { return m_stackable; }
	bool           isCrystallizable() const     { return m_crystallizable; }
	bool           isSellable() const           { return m_sellable; }
	bool           isDropable() const           { return m_dropable; }
	bool           isDestroyable() const        { return m_destroyable; }
	bool           isTradeable() const          { return m_tradeable; }
	bool           isCommon() const             { return m_common; }
	bool           isHeroItem() const           { return m_heroItem; }
	bool           isPvpItem() const            { return m_pvpItem; }

protected:
	L2ItemType     m_type; // weapon/armor/etcitem
	L2ItemSubType  m_subType; // type of weapon/type of armor/type of etcitem
	int            m_itemId;
	wchar_t       *m_name;
	int            m_type1;
	int            m_type2;
	int            m_weight;
	L2ItemMaterial m_materialType;
	L2ItemCrystal  m_crystalType; // default to none-grade
	int            m_duration;
	int            m_time;
	L2ItemSlot     m_bodyPart;
	int            m_referencePrice;
	int            m_crystalCount;
	bool           m_crystallizable;
	bool           m_stackable;
	bool           m_sellable;
	bool           m_dropable;
	bool           m_destroyable;
	bool           m_tradeable;
	bool           m_common;
	bool           m_heroItem;
	bool           m_pvpItem;
	//protected FuncTemplate[] _funcTemplates;
	//protected EffectTemplate[] _effectTemplates;
	//protected L2Skill[] _skills;
	//protected List <Condition> _preConditions = new FastList<Condition>();
	//private static final Func[] _emptyFunctionSet = new Func[0];
	//protected static final L2Effect[] _emptyEffectSet = new L2Effect[0];
};
