#pragma once

enum L2ItemType
{
	TYPE_WEAPON,
	TYPE_ARMOR,
	TYPE_ETCITEM
};

enum L2ItemSubType
{
	SYBTYPE_INCORRECT = 0,
	// weapons
	WEAPON_NONE          = 1,  // "Shield" // mask 2 (2^1)
	WEAPON_SWORD         = 2,  // "Sword" // mask 4 (2^2)
	WEAPON_BLUNT         = 3,  // "Blunt" // mask 8 (2^3)
	WEAPON_DAGGER        = 4,  // "Dagger" // mask 16 (2^4)
	WEAPON_BOW           = 5,  // "Bow" // mask 32 (2^5)
	WEAPON_POLE          = 6,  // "Pole" // mask 64 (2^6)
	WEAPON_ETC           = 7,  // "Etc" // mask 128 (2^7)
	WEAPON_FIST          = 8,  // "Fist" // mask 256 (2^8)
	WEAPON_DUAL          = 9,  // "Dual Sword" // mask 512 (2^9)
	WEAPON_DUALFIST      = 10, // "Dual Fist" // mask 1024 (2^10)
	WEAPON_BIGSWORD      = 11, // "Big Sword" // Two Handed Swords // mask 2048 (2^11)
	WEAPON_PET           = 12, // "Pet" // mask 4096 (2^12)
	WEAPON_ROD           = 13, // "Rod" // mask 8192 (2^13)
	WEAPON_BIGBLUNT      = 14, // "Big Blunt" // mask 16384 (2^14)
	WEAPON_ANCIENT_SWORD = 15, // "Ancient Sword" // mask 32768 (2^15)
	WEAPON_CROSSBOW      = 16, // "Crossbow" // mask 65536 (2^16)
	WEAPON_RAPIER        = 17, // "Rapier" // mask 131072 (2^17)
	WEAPON_DUAL_DAGGER   = 18, // "Dual Dagger" // mask 262144 (2^18)
	// armors
	ARMOR_NONE           = 19, // "None"
	ARMOR_LIGHT          = 20, // "Light"
	ARMOR_HEAVY          = 21, // "Heavy"
	ARMOR_MAGIC          = 22, // "Magic"
	ARMOR_PET            = 23, // "Pet"
	ARMOR_SIGIL          = 24, // "Sigil"
	// etc items
	ETCITEM_ARROW        = 25, // "Arrow"
	ETCITEM_BOLT         = 26, // "Bolt"
	ETCITEM_MATERIAL     = 27, // "Material"
	ETCITEM_PET_COLLAR   = 28, // "PetCollar"
	ETCITEM_POTION       = 29, // "Potion"
	ETCITEM_RECEIPE      = 30, // "Receipe"
	ETCITEM_SCROLL       = 31, // "Scroll"
	ETCITEM_QUEST        = 32, // "Quest"
	ETCITEM_MONEY        = 33, // "Money"
	ETCITEM_OTHER        = 34, // "Other"
	ETCITEM_SPELLBOOK    = 35, // "Spellbook"
	ETCITEM_SEED         = 36, // "Seed"
	ETCITEM_SHOT         = 37, // "Shot"
	ETCITEM_HERB         = 38, // "Herb"
	ETCITEM_DYE          = 39 // "dye"
};

enum L2ItemSlot
{
	SLOT_NONE       = 0x00000000,
	SLOT_UNDERWEAR  = 0x00000001,
	SLOT_R_EAR      = 0x00000002,
	SLOT_L_EAR      = 0x00000004,
	SLOT_LR_EAR     = 0x00000006,
	SLOT_NECK       = 0x00000008,
	SLOT_R_FINGER   = 0x00000010,
	SLOT_L_FINGER   = 0x00000020,
	SLOT_LR_FINGER  = 0x00000030,
	SLOT_HEAD       = 0x00000040,
	SLOT_R_HAND     = 0x00000080,
	SLOT_L_HAND     = 0x00000100,
	SLOT_GLOVES     = 0x00000200,
	SLOT_CHEST      = 0x00000400,
	SLOT_LEGS       = 0x00000800,
	SLOT_FEET       = 0x00001000,
	SLOT_BACK       = 0x00002000,
	SLOT_LR_HAND    = 0x00004000,
	SLOT_FULL_ARMOR = 0x00008000,
	SLOT_HAIR       = 0x00010000,
	SLOT_ALLDRESS   = 0x00020000,
	SLOT_HAIR2      = 0x00040000,
	SLOT_HAIRALL    = 0x00080000,
	SLOT_R_BRACELET = 0x00100000,
	SLOT_L_BRACELET = 0x00200000,
	SLOT_DECO       = 0x00400000,
	SLOT_BELT       = 0x00800000, //0x10000000,
	SLOT_WOLF       = 0x01000000, //-100,
	SLOT_HATCHLING  = 0x02000000, //-101,
	SLOT_STRIDER    = 0x04000000, //-102,
	SLOT_BABYPET    = 0x08000000, //-103,
	SLOT_GREATWOLF  = 0x10000000  //-104
};

enum L2ItemMaterial
{
	MATERIAL_NONE        = -1,
	MATERIAL_STEEL       = 0x00,
	MATERIAL_FINE_STEEL  = 0x01,
	MATERIAL_BLOOD_STEEL = 0x02,
	MATERIAL_BRONZE      = 0x03,
	MATERIAL_SILVER      = 0x04,
	MATERIAL_GOLD        = 0x05,
	MATERIAL_MITHRIL     = 0x06,
	MATERIAL_ORIHARUKON  = 0x07,
	MATERIAL_PAPER       = 0x08,
	MATERIAL_WOOD        = 0x09,
	MATERIAL_CLOTH       = 0x0a,
	MATERIAL_LEATHER     = 0x0b,
	MATERIAL_BONE        = 0x0c,
	MATERIAL_HORN        = 0x0d,
	MATERIAL_DAMASCUS    = 0x0e,
	MATERIAL_ADAMANTAITE = 0x0f,
	MATERIAL_CHRYSOLITE  = 0x10,
	MATERIAL_CRYSTAL     = 0x11,
	MATERIAL_LIQUID      = 0x12,
	MATERIAL_SCALE_OF_DRAGON = 0x13,
	MATERIAL_DYESTUFF    = 0x14,
	MATERIAL_COBWEB      = 0x15,
	MATERIAL_SEED        = 0x16,
	MATERIAL_COTTON      = 0x17
};

enum L2ItemCrystal
{
	CRYSTAL_NONE = 0x00,
	CRYSTAL_D    = 0x01,
	CRYSTAL_C    = 0x02,
	CRYSTAL_B    = 0x03,
	CRYSTAL_A    = 0x04,
	CRYSTAL_S    = 0x05,
	CRYSTAL_S80  = 0x06,
	CRYSTAL_S84  = 0x07
};
