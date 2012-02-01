#include "stdafx.h"
#include "L2Data.h"

// Lineage II  races
char *L2Race[L2MaxRaces];
// Lineage II classes
char *L2Class[L2MaxClasses];
// Sex :)
char *L2Sex[L2MaxSexes];
// Races pictures depending on sex
// L2MaxRacePics = L2MaxRaces * L2MaxSexes;
char *L2RacePic[L2MaxRacePics];
// NPC races/classes
// index is level of skill id = 4416
char *L2NPCRace[L2MaxNPCRaces];

void L2Data_init()
{
// (char *) conversion because gcc4 thinks that string constants like "Human" are (const char *)
// but really all these strings are used like (const char *) pointers, so it must be OK
L2Race[0] = (char *)"Human";
L2Race[1] = (char *)"Elf";
L2Race[2] = (char *)"Dark Elf";
L2Race[3] = (char *)"Orc";
L2Race[4] = (char *)"Dwarf";
L2Race[5] = (char *)"Kamael";

memset( L2Class, 0, sizeof(L2Class) );
L2Class[0] = (char *)"Human Fighter";
L2Class[1] = (char *)"Warrior";
L2Class[2] = (char *)"Gladiator";
L2Class[3] = (char *)"Warlord";
L2Class[4] = (char *)"Knight";
L2Class[5] = (char *)"Paladin";
L2Class[6] = (char *)"Dark Avenger";
L2Class[7] = (char *)"Rogue";
L2Class[8] = (char *)"Treasure Hunter";
L2Class[9] = (char *)"Hawkeye";
L2Class[10] = (char *)"Human Mystic";
L2Class[11] = (char *)"Human Wizard";
L2Class[12] = (char *)"Sorceror";
L2Class[13] = (char *)"Necromancer";
L2Class[14] = (char *)"Warlock";
L2Class[15] = (char *)"Cleric";
L2Class[16] = (char *)"Bishop";
L2Class[17] = (char *)"Prophet";
L2Class[18] = (char *)"Elven Fighter";
L2Class[19] = (char *)"Elven Knight";
L2Class[20] = (char *)"Temple Knight";
L2Class[21] = (char *)"Sword Singer";
L2Class[22] = (char *)"Elven Scout";
L2Class[23] = (char *)"Plains Walker";
L2Class[24] = (char *)"Silver Ranger";
L2Class[25] = (char *)"Elven Mystic";
L2Class[26] = (char *)"Elven Wizard";
L2Class[27] = (char *)"SpellSinger";
L2Class[28] = (char *)"Elemental Summoner";
L2Class[29] = (char *)"Elven Oracle";
L2Class[30] = (char *)"Elven Elder";
L2Class[31] = (char *)"Dark Fighter";
L2Class[32] = (char *)"Paulus Knight";
L2Class[33] = (char *)"Shillien Knight";
L2Class[34] = (char *)"BladeDancer";
L2Class[35] = (char *)"Assassin";
L2Class[36] = (char *)"Abyss Walker";
L2Class[37] = (char *)"Phantom Ranger";
L2Class[38] = (char *)"Dark Mystic";
L2Class[39] = (char *)"Dark Wizard";
L2Class[40] = (char *)"Spellhowler";
L2Class[41] = (char *)"Phantom Summoner";
L2Class[42] = (char *)"Shillien Oracle";
L2Class[43] = (char *)"Shillien Elder";
L2Class[44] = (char *)"Orc Fighter";
L2Class[45] = (char *)"Orc Raider";
L2Class[46] = (char *)"Destroyer";
L2Class[47] = (char *)"Orc Monk";
L2Class[48] = (char *)"Tyrant";
L2Class[49] = (char *)"Orc Mystic";
L2Class[50] = (char *)"Orc Shaman";
L2Class[51] = (char *)"Overlord";
L2Class[52] = (char *)"Warcryer";
L2Class[53] = (char *)"Dwarven Fighter";
L2Class[54] = (char *)"Scavenger";
L2Class[55] = (char *)"Bounty Hunter";
L2Class[56] = (char *)"Artisan";
L2Class[57] = (char *)"Warsmith";
//
// indexes [ 58 .. 87 ] unused
//
L2Class[88] = (char *)"Duelist";
L2Class[89] = (char *)"Dreadnought";
L2Class[90] = (char *)"Phoenix Knight";
L2Class[91] = (char *)"HellKnight";
L2Class[92] = (char *)"Sagittarius";
L2Class[93] = (char *)"Adventurer";
L2Class[94] = (char *)"Archmage";
L2Class[95] = (char *)"Soultaker";
L2Class[96] = (char *)"Arcana Lord";
L2Class[97] = (char *)"Cardinal";
L2Class[98] = (char *)"Hierophant";
L2Class[99] = (char *)"Eva's Templar";
L2Class[100] = (char *)"Sword Muse";
L2Class[101] = (char *)"Wind Rider";
L2Class[102] = (char *)"Moonlight Sentinel";
L2Class[103] = (char *)"Mystic Muse";
L2Class[104] = (char *)"Elemental Master";
L2Class[105] = (char *)"Eva's Saint";
L2Class[106] = (char *)"Shillien Templar";
L2Class[107] = (char *)"Spectral Dancer";
L2Class[108] = (char *)"Ghost Hunter";
L2Class[109] = (char *)"Ghost Sentinel";
L2Class[110] = (char *)"Storm Screamer";
L2Class[111] = (char *)"Spectral Master";
L2Class[112] = (char *)"Shillien Saint";
L2Class[113] = (char *)"Titan";
L2Class[114] = (char *)"Grand Khauatari";
L2Class[115] = (char *)"Dominator";
L2Class[116] = (char *)"Doomcryer";
L2Class[117] = (char *)"Fortune Seeker";
L2Class[118] = (char *)"Maestro";
//
// indexes [ 119 .. 122 ] unused
//
L2Class[123] = (char *)"Kamael Male Soldier";
L2Class[124] = (char *)"Kamael Female Soldier";
L2Class[125] = (char *)"Trooper";
L2Class[126] = (char *)"Warder";
L2Class[127] = (char *)"Berserker";
L2Class[128] = (char *)"Soulbreaker (Male)"; //"Male Soulbreaker";
L2Class[129] = (char *)"Soulbreaker (Female)"; //"Female Soulbreaker";
L2Class[130] = (char *)"Arbalester";
L2Class[131] = (char *)"Doombringer";
L2Class[132] = (char *)"Soulhound (Male)"; // male
L2Class[133] = (char *)"Soulhound (Female)"; // female
L2Class[134] = (char *)"Trickster"; // female
L2Class[135] = (char *)"Inspector";
L2Class[136] = (char *)"Judicator";

L2Sex[0] = (char *)"Male";
L2Sex[1] = (char *)"Female";

L2RacePic[0]  = (char *)"human_male";
L2RacePic[1]  = (char *)"human_female";
L2RacePic[2]  = (char *)"elf_male";
L2RacePic[3]  = (char *)"elf_female";
L2RacePic[4]  = (char *)"darkelf_male";
L2RacePic[5]  = (char *)"darkelf_female";
L2RacePic[6]  = (char *)"orc_male";
L2RacePic[7]  = (char *)"orc_female";
L2RacePic[8]  = (char *)"dwarf_male";
L2RacePic[9]  = (char *)"dwarf_female";
L2RacePic[10] = (char *)"kamael_male";
L2RacePic[11] = (char *)"kamael_female";

L2NPCRace[0] = (char *)"NPC race index 0 not used";
L2NPCRace[1] = (char *)"Undead";
L2NPCRace[2] = (char *)"Magic Creatures";
L2NPCRace[3] = (char *)"Beasts";
L2NPCRace[4] = (char *)"Animals";
L2NPCRace[5] = (char *)"Plants";
L2NPCRace[6] = (char *)"Humanoids";
L2NPCRace[7] = (char *)"Spirits";
L2NPCRace[8] = (char *)"Angels";
L2NPCRace[9] = (char *)"Demons";
L2NPCRace[10] = (char *)"Dragons";
L2NPCRace[11] = (char *)"Giants";
L2NPCRace[12] = (char *)"Bugs";
L2NPCRace[13] = (char *)"Fairies";
L2NPCRace[14] = (char *)"Humans";
L2NPCRace[15] = (char *)"Elves";
L2NPCRace[16] = (char *)"Dark Elves";
L2NPCRace[17] = (char *)"Orcs";
L2NPCRace[18] = (char *)"Dwarves";
L2NPCRace[19] = (char *)"Others";
L2NPCRace[20] = (char *)"Non-living Beings";
L2NPCRace[21] = (char *)"Siege Weapons";
L2NPCRace[22] = (char *)"Defending Army";
L2NPCRace[23] = (char *)"Mercenaries";
L2NPCRace[24] = (char *)"Unknown Creature";
}

const char *L2Data_getRace( int raceID )
{
	if( (raceID<0) || (raceID>=L2MaxRaces) ) return NULL;
	return L2Race[raceID];
}
const char *L2Data_getClass( int classID )
{
	if( (classID<0) || (classID>=L2MaxClasses) ) return NULL;
	return L2Class[classID];
}
const char *L2Data_getSex( int sexID )
{
	if( (sexID<0) || (sexID>=L2MaxSexes) ) return NULL;
	return L2Sex[sexID];
}
const char *L2Data_getRacePic( int raceID, int sexID )
{
	int picID = raceID*2 + sexID;
	if( (picID<0) || (picID>=L2MaxRacePics) ) return NULL;
	return L2RacePic[picID];
}
const char *L2Data_getNPCRace( int npcRaceID )
{
	if( (npcRaceID<0) || (npcRaceID>=L2MaxNPCRaces) ) return NULL;
	return L2NPCRace[npcRaceID];
}
