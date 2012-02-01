#include "stdafx.h"
#include "L2Data_Recipes.h"

L2Data_RecipeInfo recipedata[ MAX_RECIPE_ID + 1 ];

bool L2Data_GetRecipeInfoForRecID( int recID, L2Data_RecipeInfo *pInfo )
{
	if( (recID>=1) && (recID<=MAX_RECIPE_ID) )
	{
		strcpy( pInfo->name,   recipedata[recID].name );
		pInfo->itemID        = recipedata[recID].itemID;
		pInfo->rec_itemID    = recipedata[recID].rec_itemID;
		pInfo->level         = recipedata[recID].level;
		pInfo->resultCount   = recipedata[recID].resultCount;
		pInfo->mpCost        = recipedata[recID].mpCost;
		pInfo->successRate   = recipedata[recID].successRate;
		return true;
	}
	return false;
}

void L2Data_InitRecipes()
{

// recipe id 0
strcpy( recipedata[0].name, "" );
recipedata[0].itemID = 0;
recipedata[0].rec_itemID = 0;
recipedata[0].level = 0;
recipedata[0].resultCount = 0;
recipedata[0].mpCost = 0;
recipedata[0].successRate = 0;

// recipe id 1
strcpy( recipedata[1].name, "mk_wooden_arrow\0" );
recipedata[1].itemID = 17;
recipedata[1].rec_itemID = 1666;
recipedata[1].level = 1;
recipedata[1].resultCount = 500;
recipedata[1].mpCost = 30;
recipedata[1].successRate = 100;

// recipe id 2
strcpy( recipedata[2].name, "mk_broad_sword\0" );
recipedata[2].itemID = 3;
recipedata[2].rec_itemID = 1786;
recipedata[2].level = 1;
recipedata[2].resultCount = 1;
recipedata[2].mpCost = 30;
recipedata[2].successRate = 100;

// recipe id 3
strcpy( recipedata[3].name, "mk_willow_staff\0" );
recipedata[3].itemID = 8;
recipedata[3].rec_itemID = 1787;
recipedata[3].level = 1;
recipedata[3].resultCount = 1;
recipedata[3].mpCost = 30;
recipedata[3].successRate = 100;

// recipe id 4
strcpy( recipedata[4].name, "mk_bow\0" );
recipedata[4].itemID = 14;
recipedata[4].rec_itemID = 1788;
recipedata[4].level = 1;
recipedata[4].resultCount = 1;
recipedata[4].mpCost = 30;
recipedata[4].successRate = 100;

// recipe id 5
strcpy( recipedata[5].name, "mk_cedar_staff\0" );
recipedata[5].itemID = 9;
recipedata[5].rec_itemID = 1789;
recipedata[5].level = 1;
recipedata[5].resultCount = 1;
recipedata[5].mpCost = 45;
recipedata[5].successRate = 100;

// recipe id 6
strcpy( recipedata[6].name, "mk_dirk\0" );
recipedata[6].itemID = 216;
recipedata[6].rec_itemID = 1790;
recipedata[6].level = 1;
recipedata[6].resultCount = 1;
recipedata[6].mpCost = 45;
recipedata[6].successRate = 100;

// recipe id 7
strcpy( recipedata[7].name, "mk_brandish\0" );
recipedata[7].itemID = 1333;
recipedata[7].rec_itemID = 1791;
recipedata[7].level = 1;
recipedata[7].resultCount = 1;
recipedata[7].mpCost = 45;
recipedata[7].successRate = 100;

// recipe id 8
strcpy( recipedata[8].name, "mk_short_spear\0" );
recipedata[8].itemID = 15;
recipedata[8].rec_itemID = 1792;
recipedata[8].level = 1;
recipedata[8].resultCount = 1;
recipedata[8].mpCost = 60;
recipedata[8].successRate = 100;

// recipe id 9
strcpy( recipedata[9].name, "mk_sword_of_reflexion\0" );
recipedata[9].itemID = 120;
recipedata[9].rec_itemID = 1793;
recipedata[9].level = 1;
recipedata[9].resultCount = 1;
recipedata[9].mpCost = 60;
recipedata[9].successRate = 100;

// recipe id 10
strcpy( recipedata[10].name, "mk_bow_of_forest\0" );
recipedata[10].itemID = 272;
recipedata[10].rec_itemID = 1794;
recipedata[10].level = 1;
recipedata[10].resultCount = 1;
recipedata[10].mpCost = 60;
recipedata[10].successRate = 100;

// recipe id 11
strcpy( recipedata[11].name, "mk_leather_shoes\0" );
recipedata[11].itemID = 37;
recipedata[11].rec_itemID = 1795;
recipedata[11].level = 1;
recipedata[11].resultCount = 1;
recipedata[11].mpCost = 45;
recipedata[11].successRate = 100;

// recipe id 12
strcpy( recipedata[12].name, "mk_leather_tunic\0" );
recipedata[12].itemID = 429;
recipedata[12].rec_itemID = 1796;
recipedata[12].level = 1;
recipedata[12].resultCount = 1;
recipedata[12].mpCost = 45;
recipedata[12].successRate = 100;

// recipe id 13
strcpy( recipedata[13].name, "mk_leather_hose\0" );
recipedata[13].itemID = 464;
recipedata[13].rec_itemID = 1797;
recipedata[13].level = 1;
recipedata[13].resultCount = 1;
recipedata[13].mpCost = 45;
recipedata[13].successRate = 100;

// recipe id 14
strcpy( recipedata[14].name, "mk_leather_helmet\0" );
recipedata[14].itemID = 44;
recipedata[14].rec_itemID = 1798;
recipedata[14].level = 1;
recipedata[14].resultCount = 1;
recipedata[14].mpCost = 60;
recipedata[14].successRate = 100;

// recipe id 15
strcpy( recipedata[15].name, "mk_leather_gloves\0" );
recipedata[15].itemID = 50;
recipedata[15].rec_itemID = 1799;
recipedata[15].level = 1;
recipedata[15].resultCount = 1;
recipedata[15].mpCost = 60;
recipedata[15].successRate = 100;

// recipe id 16
strcpy( recipedata[16].name, "mk_piece_bone_breastplate\0" );
recipedata[16].itemID = 25;
recipedata[16].rec_itemID = 1800;
recipedata[16].level = 1;
recipedata[16].resultCount = 1;
recipedata[16].mpCost = 63;
recipedata[16].successRate = 100;

// recipe id 17
strcpy( recipedata[17].name, "mk_piece_bone_gaiters\0" );
recipedata[17].itemID = 32;
recipedata[17].rec_itemID = 1801;
recipedata[17].level = 1;
recipedata[17].resultCount = 1;
recipedata[17].mpCost = 63;
recipedata[17].successRate = 100;

// recipe id 18
strcpy( recipedata[18].name, "mk_necklace_of_anguish\0" );
recipedata[18].itemID = 907;
recipedata[18].rec_itemID = 1802;
recipedata[18].level = 1;
recipedata[18].resultCount = 1;
recipedata[18].mpCost = 45;
recipedata[18].successRate = 100;

// recipe id 19
strcpy( recipedata[19].name, "mk_necklace_of_wisdom\0" );
recipedata[19].itemID = 908;
recipedata[19].rec_itemID = 1803;
recipedata[19].level = 1;
recipedata[19].resultCount = 1;
recipedata[19].mpCost = 60;
recipedata[19].successRate = 100;

// recipe id 20
strcpy( recipedata[20].name, "mk_soulshot_d\0" );
recipedata[20].itemID = 1463;
recipedata[20].rec_itemID = 1804;
recipedata[20].level = 2;
recipedata[20].resultCount = 468;
recipedata[20].mpCost = 72;
recipedata[20].successRate = 100;

// recipe id 21
strcpy( recipedata[21].name, "mk_soulshot_c\0" );
recipedata[21].itemID = 1464;
recipedata[21].rec_itemID = 1805;
recipedata[21].level = 4;
recipedata[21].resultCount = 952;
recipedata[21].mpCost = 84;
recipedata[21].successRate = 100;

// recipe id 22
strcpy( recipedata[22].name, "mk_soulshot_b\0" );
recipedata[22].itemID = 1465;
recipedata[22].rec_itemID = 1806;
recipedata[22].level = 6;
recipedata[22].resultCount = 450;
recipedata[22].mpCost = 100;
recipedata[22].successRate = 100;

// recipe id 23
strcpy( recipedata[23].name, "mk_soulshot_a\0" );
recipedata[23].itemID = 1466;
recipedata[23].rec_itemID = 1807;
recipedata[23].level = 7;
recipedata[23].resultCount = 300;
recipedata[23].mpCost = 120;
recipedata[23].successRate = 100;

// recipe id 24
strcpy( recipedata[24].name, "mk_soulshot_s\0" );
recipedata[24].itemID = 1467;
recipedata[24].rec_itemID = 1808;
recipedata[24].level = 8;
recipedata[24].resultCount = 350;
recipedata[24].mpCost = 150;
recipedata[24].successRate = 100;

// recipe id 25
strcpy( recipedata[25].name, "mk_leather\0" );
recipedata[25].itemID = 1882;
recipedata[25].rec_itemID = 1814;
recipedata[25].level = 1;
recipedata[25].resultCount = 1;
recipedata[25].mpCost = 10;
recipedata[25].successRate = 100;

// recipe id 26
strcpy( recipedata[26].name, "mk_cord\0" );
recipedata[26].itemID = 1884;
recipedata[26].rec_itemID = 1817;
recipedata[26].level = 2;
recipedata[26].resultCount = 20;
recipedata[26].mpCost = 20;
recipedata[26].successRate = 100;

// recipe id 27
strcpy( recipedata[27].name, "mk_oriharukon\0" );
recipedata[27].itemID = 1893;
recipedata[27].rec_itemID = 1825;
recipedata[27].level = 4;
recipedata[27].resultCount = 1;
recipedata[27].mpCost = 40;
recipedata[27].successRate = 100;

// recipe id 28
strcpy( recipedata[28].name, "mk_braided_hemp\0" );
recipedata[28].itemID = 1878;
recipedata[28].rec_itemID = 2135;
recipedata[28].level = 1;
recipedata[28].resultCount = 1;
recipedata[28].mpCost = 10;
recipedata[28].successRate = 100;

// recipe id 29
strcpy( recipedata[29].name, "mk_cokes\0" );
recipedata[29].itemID = 1879;
recipedata[29].rec_itemID = 2136;
recipedata[29].level = 1;
recipedata[29].resultCount = 1;
recipedata[29].mpCost = 10;
recipedata[29].successRate = 100;

// recipe id 30
strcpy( recipedata[30].name, "mk_steel\0" );
recipedata[30].itemID = 1880;
recipedata[30].rec_itemID = 2137;
recipedata[30].level = 1;
recipedata[30].resultCount = 1;
recipedata[30].mpCost = 10;
recipedata[30].successRate = 100;

// recipe id 31
strcpy( recipedata[31].name, "mk_coarse_bone_powder\0" );
recipedata[31].itemID = 1881;
recipedata[31].rec_itemID = 2138;
recipedata[31].level = 1;
recipedata[31].resultCount = 1;
recipedata[31].mpCost = 10;
recipedata[31].successRate = 100;

// recipe id 32
strcpy( recipedata[32].name, "mk_steel_mold\0" );
recipedata[32].itemID = 1883;
recipedata[32].rec_itemID = 2139;
recipedata[32].level = 2;
recipedata[32].resultCount = 1;
recipedata[32].mpCost = 20;
recipedata[32].successRate = 100;

// recipe id 33
strcpy( recipedata[33].name, "mk_high_grade_suede\0" );
recipedata[33].itemID = 1885;
recipedata[33].rec_itemID = 2140;
recipedata[33].level = 2;
recipedata[33].resultCount = 1;
recipedata[33].mpCost = 20;
recipedata[33].successRate = 100;

// recipe id 34
strcpy( recipedata[34].name, "mk_silver_mold\0" );
recipedata[34].itemID = 1886;
recipedata[34].rec_itemID = 2141;
recipedata[34].level = 2;
recipedata[34].resultCount = 1;
recipedata[34].mpCost = 20;
recipedata[34].successRate = 100;

// recipe id 35
strcpy( recipedata[35].name, "mk_varnish_of_purity\0" );
recipedata[35].itemID = 1887;
recipedata[35].rec_itemID = 2142;
recipedata[35].level = 2;
recipedata[35].resultCount = 1;
recipedata[35].mpCost = 20;
recipedata[35].successRate = 100;

// recipe id 36
strcpy( recipedata[36].name, "mk_synthesis_cokes\0" );
recipedata[36].itemID = 1888;
recipedata[36].rec_itemID = 2143;
recipedata[36].level = 2;
recipedata[36].resultCount = 1;
recipedata[36].mpCost = 20;
recipedata[36].successRate = 100;

// recipe id 37
strcpy( recipedata[37].name, "mk_compound_braid\0" );
recipedata[37].itemID = 1889;
recipedata[37].rec_itemID = 2144;
recipedata[37].level = 2;
recipedata[37].resultCount = 1;
recipedata[37].mpCost = 20;
recipedata[37].successRate = 100;

// recipe id 38
strcpy( recipedata[38].name, "mk_mithirl_alloy\0" );
recipedata[38].itemID = 1890;
recipedata[38].rec_itemID = 2145;
recipedata[38].level = 4;
recipedata[38].resultCount = 1;
recipedata[38].mpCost = 40;
recipedata[38].successRate = 100;

// recipe id 39
strcpy( recipedata[39].name, "mk_artisan's_frame\0" );
recipedata[39].itemID = 1891;
recipedata[39].rec_itemID = 2146;
recipedata[39].level = 4;
recipedata[39].resultCount = 1;
recipedata[39].mpCost = 40;
recipedata[39].successRate = 100;

// recipe id 40
strcpy( recipedata[40].name, "mk_blacksmith's_frame\0" );
recipedata[40].itemID = 1892;
recipedata[40].rec_itemID = 2147;
recipedata[40].level = 4;
recipedata[40].resultCount = 1;
recipedata[40].mpCost = 40;
recipedata[40].successRate = 100;

// recipe id 41
strcpy( recipedata[41].name, "mk_crafted_leather\0" );
recipedata[41].itemID = 1894;
recipedata[41].rec_itemID = 2148;
recipedata[41].level = 4;
recipedata[41].resultCount = 1;
recipedata[41].mpCost = 40;
recipedata[41].successRate = 100;

// recipe id 42
strcpy( recipedata[42].name, "mk_metallic_fiber\0" );
recipedata[42].itemID = 1895;
recipedata[42].rec_itemID = 2149;
recipedata[42].level = 4;
recipedata[42].resultCount = 20;
recipedata[42].mpCost = 40;
recipedata[42].successRate = 100;

// recipe id 43
strcpy( recipedata[43].name, "mk_blue_diamond_necklace\0" );
recipedata[43].itemID = 909;
recipedata[43].rec_itemID = 2150;
recipedata[43].level = 1;
recipedata[43].resultCount = 1;
recipedata[43].mpCost = 72;
recipedata[43].successRate = 100;

// recipe id 44
strcpy( recipedata[44].name, "mk_necklace_of_devotion\0" );
recipedata[44].itemID = 910;
recipedata[44].rec_itemID = 2151;
recipedata[44].level = 2;
recipedata[44].resultCount = 1;
recipedata[44].mpCost = 84;
recipedata[44].successRate = 100;

// recipe id 45
strcpy( recipedata[45].name, "mk_enchanted_necklace\0" );
recipedata[45].itemID = 911;
recipedata[45].rec_itemID = 2152;
recipedata[45].level = 2;
recipedata[45].resultCount = 1;
recipedata[45].mpCost = 96;
recipedata[45].successRate = 100;

// recipe id 46
strcpy( recipedata[46].name, "mk_tiger'seye_earing\0" );
recipedata[46].itemID = 849;
recipedata[46].rec_itemID = 2153;
recipedata[46].level = 3;
recipedata[46].resultCount = 1;
recipedata[46].mpCost = 108;
recipedata[46].successRate = 100;

// recipe id 47
strcpy( recipedata[47].name, "mk_elven_earing\0" );
recipedata[47].itemID = 850;
recipedata[47].rec_itemID = 2154;
recipedata[47].level = 3;
recipedata[47].resultCount = 1;
recipedata[47].mpCost = 120;
recipedata[47].successRate = 100;

// recipe id 48
strcpy( recipedata[48].name, "mk_elven_ring\0" );
recipedata[48].itemID = 881;
recipedata[48].rec_itemID = 2155;
recipedata[48].level = 3;
recipedata[48].resultCount = 1;
recipedata[48].mpCost = 120;
recipedata[48].successRate = 100;

// recipe id 49
strcpy( recipedata[49].name, "mk_elven_necklace\0" );
recipedata[49].itemID = 913;
recipedata[49].rec_itemID = 2156;
recipedata[49].level = 3;
recipedata[49].resultCount = 1;
recipedata[49].mpCost = 120;
recipedata[49].successRate = 100;

// recipe id 50
strcpy( recipedata[50].name, "mk_onyxbeast'seye_earing\0" );
recipedata[50].itemID = 851;
recipedata[50].rec_itemID = 2157;
recipedata[50].level = 4;
recipedata[50].resultCount = 1;
recipedata[50].mpCost = 129;
recipedata[50].successRate = 100;

// recipe id 51
strcpy( recipedata[51].name, "mk_mithril_ring\0" );
recipedata[51].itemID = 882;
recipedata[51].rec_itemID = 2158;
recipedata[51].level = 4;
recipedata[51].resultCount = 1;
recipedata[51].mpCost = 129;
recipedata[51].successRate = 100;

// recipe id 52
strcpy( recipedata[52].name, "mk_necklace_of_darkness\0" );
recipedata[52].itemID = 914;
recipedata[52].rec_itemID = 2159;
recipedata[52].level = 4;
recipedata[52].resultCount = 1;
recipedata[52].mpCost = 129;
recipedata[52].successRate = 100;

// recipe id 53
strcpy( recipedata[53].name, "mk_moonstone_earing\0" );
recipedata[53].itemID = 852;
recipedata[53].rec_itemID = 2160;
recipedata[53].level = 4;
recipedata[53].resultCount = 1;
recipedata[53].mpCost = 138;
recipedata[53].successRate = 100;

// recipe id 54
strcpy( recipedata[54].name, "mk_aquastone_ring\0" );
recipedata[54].itemID = 883;
recipedata[54].rec_itemID = 2161;
recipedata[54].level = 4;
recipedata[54].resultCount = 1;
recipedata[54].mpCost = 138;
recipedata[54].successRate = 100;

// recipe id 55
strcpy( recipedata[55].name, "mk_aquastone_necklace\0" );
recipedata[55].itemID = 915;
recipedata[55].rec_itemID = 2162;
recipedata[55].level = 4;
recipedata[55].resultCount = 1;
recipedata[55].mpCost = 138;
recipedata[55].successRate = 100;

// recipe id 56
strcpy( recipedata[56].name, "mk_earing_of_protection\0" );
recipedata[56].itemID = 853;
recipedata[56].rec_itemID = 2163;
recipedata[56].level = 5;
recipedata[56].resultCount = 1;
recipedata[56].mpCost = 147;
recipedata[56].successRate = 100;

// recipe id 57
strcpy( recipedata[57].name, "mk_ring_of_protection\0" );
recipedata[57].itemID = 884;
recipedata[57].rec_itemID = 2164;
recipedata[57].level = 5;
recipedata[57].resultCount = 1;
recipedata[57].mpCost = 147;
recipedata[57].successRate = 100;

// recipe id 58
strcpy( recipedata[58].name, "mk_necklace_of_protection\0" );
recipedata[58].itemID = 916;
recipedata[58].rec_itemID = 2165;
recipedata[58].level = 5;
recipedata[58].resultCount = 1;
recipedata[58].mpCost = 147;
recipedata[58].successRate = 100;

// recipe id 59
strcpy( recipedata[59].name, "mk_earing_of_binding\0" );
recipedata[59].itemID = 854;
recipedata[59].rec_itemID = 2166;
recipedata[59].level = 5;
recipedata[59].resultCount = 1;
recipedata[59].mpCost = 156;
recipedata[59].successRate = 100;

// recipe id 60
strcpy( recipedata[60].name, "mk_ring_of_ages\0" );
recipedata[60].itemID = 885;
recipedata[60].rec_itemID = 2167;
recipedata[60].level = 5;
recipedata[60].resultCount = 1;
recipedata[60].mpCost = 156;
recipedata[60].successRate = 100;

// recipe id 61
strcpy( recipedata[61].name, "mk_necklace_of_mermaid\0" );
recipedata[61].itemID = 917;
recipedata[61].rec_itemID = 2168;
recipedata[61].level = 5;
recipedata[61].resultCount = 1;
recipedata[61].mpCost = 156;
recipedata[61].successRate = 100;

// recipe id 62
strcpy( recipedata[62].name, "mk_necklace_of_binding\0" );
recipedata[62].itemID = 119;
recipedata[62].rec_itemID = 2169;
recipedata[62].level = 6;
recipedata[62].resultCount = 1;
recipedata[62].mpCost = 165;
recipedata[62].successRate = 100;

// recipe id 63
strcpy( recipedata[63].name, "mk_nassen's_earing\0" );
recipedata[63].itemID = 855;
recipedata[63].rec_itemID = 2170;
recipedata[63].level = 6;
recipedata[63].resultCount = 1;
recipedata[63].mpCost = 165;
recipedata[63].successRate = 100;

// recipe id 64
strcpy( recipedata[64].name, "mk_ring_of_sage\0" );
recipedata[64].itemID = 891;
recipedata[64].rec_itemID = 2171;
recipedata[64].level = 7;
recipedata[64].resultCount = 1;
recipedata[64].mpCost = 183;
recipedata[64].successRate = 100;

// recipe id 65
strcpy( recipedata[65].name, "mk_sage's_necklace\0" );
recipedata[65].itemID = 922;
recipedata[65].rec_itemID = 2172;
recipedata[65].level = 7;
recipedata[65].resultCount = 1;
recipedata[65].mpCost = 183;
recipedata[65].successRate = 100;

// recipe id 66
strcpy( recipedata[66].name, "mk_hard_leather_shirt\0" );
recipedata[66].itemID = 27;
recipedata[66].rec_itemID = 2173;
recipedata[66].level = 1;
recipedata[66].resultCount = 1;
recipedata[66].mpCost = 72;
recipedata[66].successRate = 100;

// recipe id 67
strcpy( recipedata[67].name, "mk_hard_leather_gaiters\0" );
recipedata[67].itemID = 33;
recipedata[67].rec_itemID = 2174;
recipedata[67].level = 1;
recipedata[67].resultCount = 1;
recipedata[67].mpCost = 72;
recipedata[67].successRate = 100;

// recipe id 68
strcpy( recipedata[68].name, "mk_boots\0" );
recipedata[68].itemID = 39;
recipedata[68].rec_itemID = 2175;
recipedata[68].level = 1;
recipedata[68].resultCount = 1;
recipedata[68].mpCost = 72;
recipedata[68].successRate = 100;

// recipe id 69
strcpy( recipedata[69].name, "mk_leather_boots\0" );
recipedata[69].itemID = 40;
recipedata[69].rec_itemID = 2176;
recipedata[69].level = 2;
recipedata[69].resultCount = 1;
recipedata[69].mpCost = 84;
recipedata[69].successRate = 100;

// recipe id 70
strcpy( recipedata[70].name, "mk_bone_helmet\0" );
recipedata[70].itemID = 45;
recipedata[70].rec_itemID = 2177;
recipedata[70].level = 2;
recipedata[70].resultCount = 1;
recipedata[70].mpCost = 84;
recipedata[70].successRate = 100;

// recipe id 71
strcpy( recipedata[71].name, "mk_dark_hose\0" );
recipedata[71].itemID = 467;
recipedata[71].rec_itemID = 2178;
recipedata[71].level = 2;
recipedata[71].resultCount = 1;
recipedata[71].mpCost = 96;
recipedata[71].successRate = 100;

// recipe id 72
strcpy( recipedata[72].name, "mk_excellence_leather_gloves\0" );
recipedata[72].itemID = 604;
recipedata[72].rec_itemID = 2179;
recipedata[72].level = 2;
recipedata[72].resultCount = 1;
recipedata[72].mpCost = 84;
recipedata[72].successRate = 100;

// recipe id 73
strcpy( recipedata[73].name, "mk_scale_mail\0" );
recipedata[73].itemID = 348;
recipedata[73].rec_itemID = 2180;
recipedata[73].level = 2;
recipedata[73].resultCount = 1;
recipedata[73].mpCost = 96;
recipedata[73].successRate = 100;

// recipe id 74
strcpy( recipedata[74].name, "mk_white_tunic\0" );
recipedata[74].itemID = 434;
recipedata[74].rec_itemID = 2181;
recipedata[74].level = 2;
recipedata[74].resultCount = 1;
recipedata[74].mpCost = 96;
recipedata[74].successRate = 100;

// recipe id 75
strcpy( recipedata[75].name, "mk_scale_gaiters\0" );
recipedata[75].itemID = 377;
recipedata[75].rec_itemID = 2182;
recipedata[75].level = 2;
recipedata[75].resultCount = 1;
recipedata[75].mpCost = 96;
recipedata[75].successRate = 100;

// recipe id 76
strcpy( recipedata[76].name, "mk_mithril_banded_mail\0" );
recipedata[76].itemID = 393;
recipedata[76].rec_itemID = 2183;
recipedata[76].level = 3;
recipedata[76].resultCount = 1;
recipedata[76].mpCost = 108;
recipedata[76].successRate = 100;

// recipe id 77
strcpy( recipedata[77].name, "mk_mithril_banded_gaiters\0" );
recipedata[77].itemID = 415;
recipedata[77].rec_itemID = 2184;
recipedata[77].level = 3;
recipedata[77].resultCount = 1;
recipedata[77].mpCost = 108;
recipedata[77].successRate = 100;

// recipe id 78
strcpy( recipedata[78].name, "mk_iron_boots\0" );
recipedata[78].itemID = 553;
recipedata[78].rec_itemID = 2185;
recipedata[78].level = 3;
recipedata[78].resultCount = 1;
recipedata[78].mpCost = 108;
recipedata[78].successRate = 100;

// recipe id 79
strcpy( recipedata[79].name, "mk_brigandine\0" );
recipedata[79].itemID = 352;
recipedata[79].rec_itemID = 2186;
recipedata[79].level = 3;
recipedata[79].resultCount = 1;
recipedata[79].mpCost = 120;
recipedata[79].successRate = 100;

// recipe id 80
strcpy( recipedata[80].name, "mk_manticor_skin_shirt\0" );
recipedata[80].itemID = 395;
recipedata[80].rec_itemID = 2187;
recipedata[80].level = 3;
recipedata[80].resultCount = 1;
recipedata[80].mpCost = 120;
recipedata[80].successRate = 100;

// recipe id 81
strcpy( recipedata[81].name, "mk_manticor_skin_gaiters\0" );
recipedata[81].itemID = 417;
recipedata[81].rec_itemID = 2188;
recipedata[81].level = 3;
recipedata[81].resultCount = 1;
recipedata[81].mpCost = 120;
recipedata[81].successRate = 100;

// recipe id 82
strcpy( recipedata[82].name, "mk_mithril_tunic\0" );
recipedata[82].itemID = 437;
recipedata[82].rec_itemID = 2189;
recipedata[82].level = 3;
recipedata[82].resultCount = 1;
recipedata[82].mpCost = 120;
recipedata[82].successRate = 100;

// recipe id 83
strcpy( recipedata[83].name, "mk_mithril_hose\0" );
recipedata[83].itemID = 470;
recipedata[83].rec_itemID = 2190;
recipedata[83].level = 3;
recipedata[83].resultCount = 1;
recipedata[83].mpCost = 120;
recipedata[83].successRate = 100;

// recipe id 84
strcpy( recipedata[84].name, "mk_gauntlet_of_repose_of_the_soul\0" );
recipedata[84].itemID = 606;
recipedata[84].rec_itemID = 2191;
recipedata[84].level = 3;
recipedata[84].resultCount = 1;
recipedata[84].mpCost = 120;
recipedata[84].successRate = 100;

// recipe id 85
strcpy( recipedata[85].name, "mk_kite_shield\0" );
recipedata[85].itemID = 629;
recipedata[85].rec_itemID = 2192;
recipedata[85].level = 3;
recipedata[85].resultCount = 1;
recipedata[85].mpCost = 120;
recipedata[85].successRate = 100;

// recipe id 86
strcpy( recipedata[86].name, "mk_boots_of_power\0" );
recipedata[86].itemID = 1124;
recipedata[86].rec_itemID = 2193;
recipedata[86].level = 3;
recipedata[86].resultCount = 1;
recipedata[86].mpCost = 120;
recipedata[86].successRate = 100;

// recipe id 87
strcpy( recipedata[87].name, "mk_mithril_glove\0" );
recipedata[87].itemID = 61;
recipedata[87].rec_itemID = 2194;
recipedata[87].level = 4;
recipedata[87].resultCount = 1;
recipedata[87].mpCost = 129;
recipedata[87].successRate = 100;

// recipe id 88
strcpy( recipedata[88].name, "mk_half_plate\0" );
recipedata[88].itemID = 353;
recipedata[88].rec_itemID = 2195;
recipedata[88].level = 4;
recipedata[88].resultCount = 1;
recipedata[88].mpCost = 129;
recipedata[88].successRate = 100;

// recipe id 89
strcpy( recipedata[89].name, "mk_plate_gaiters\0" );
recipedata[89].itemID = 380;
recipedata[89].rec_itemID = 2196;
recipedata[89].level = 4;
recipedata[89].resultCount = 1;
recipedata[89].mpCost = 129;
recipedata[89].successRate = 100;

// recipe id 90
strcpy( recipedata[90].name, "mk_slamander_skin_mail\0" );
recipedata[90].itemID = 396;
recipedata[90].rec_itemID = 2197;
recipedata[90].level = 4;
recipedata[90].resultCount = 1;
recipedata[90].mpCost = 129;
recipedata[90].successRate = 100;

// recipe id 91
strcpy( recipedata[91].name, "mk_sage's_rag\0" );
recipedata[91].itemID = 438;
recipedata[91].rec_itemID = 2198;
recipedata[91].level = 4;
recipedata[91].resultCount = 1;
recipedata[91].mpCost = 129;
recipedata[91].successRate = 100;

// recipe id 92
strcpy( recipedata[92].name, "mk_karmian_hose\0" );
recipedata[92].itemID = 471;
recipedata[92].rec_itemID = 2199;
recipedata[92].level = 4;
recipedata[92].resultCount = 1;
recipedata[92].mpCost = 138;
recipedata[92].successRate = 100;

// recipe id 93
strcpy( recipedata[93].name, "mk_chain_helmet\0" );
recipedata[93].itemID = 497;
recipedata[93].rec_itemID = 2200;
recipedata[93].level = 4;
recipedata[93].resultCount = 1;
recipedata[93].mpCost = 129;
recipedata[93].successRate = 100;

// recipe id 94
strcpy( recipedata[94].name, "mk_square_shield\0" );
recipedata[94].itemID = 630;
recipedata[94].rec_itemID = 2201;
recipedata[94].level = 4;
recipedata[94].resultCount = 1;
recipedata[94].mpCost = 129;
recipedata[94].successRate = 100;

// recipe id 95
strcpy( recipedata[95].name, "mk_assault_boots\0" );
recipedata[95].itemID = 1125;
recipedata[95].rec_itemID = 2202;
recipedata[95].level = 4;
recipedata[95].resultCount = 1;
recipedata[95].mpCost = 129;
recipedata[95].successRate = 100;

// recipe id 96
strcpy( recipedata[96].name, "mk_mithril_boots\0" );
recipedata[96].itemID = 62;
recipedata[96].rec_itemID = 2203;
recipedata[96].level = 4;
recipedata[96].resultCount = 1;
recipedata[96].mpCost = 138;
recipedata[96].successRate = 100;

// recipe id 97
strcpy( recipedata[97].name, "mk_chain_mail_shirt\0" );
recipedata[97].itemID = 354;
recipedata[97].rec_itemID = 2204;
recipedata[97].level = 4;
recipedata[97].resultCount = 1;
recipedata[97].mpCost = 138;
recipedata[97].successRate = 100;

// recipe id 98
strcpy( recipedata[98].name, "mk_chain_gaiters\0" );
recipedata[98].itemID = 381;
recipedata[98].rec_itemID = 2205;
recipedata[98].level = 4;
recipedata[98].resultCount = 1;
recipedata[98].mpCost = 138;
recipedata[98].successRate = 100;

// recipe id 99
strcpy( recipedata[99].name, "mk_tempered_mithril_shirt\0" );
recipedata[99].itemID = 397;
recipedata[99].rec_itemID = 2206;
recipedata[99].level = 4;
recipedata[99].resultCount = 1;
recipedata[99].mpCost = 138;
recipedata[99].successRate = 100;

// recipe id 100
strcpy( recipedata[100].name, "mk_karmian_tunic\0" );
recipedata[100].itemID = 439;
recipedata[100].rec_itemID = 2207;
recipedata[100].level = 4;
recipedata[100].resultCount = 1;
recipedata[100].mpCost = 138;
recipedata[100].successRate = 100;

// recipe id 101
strcpy( recipedata[101].name, "mk_ogre_power_gauntlet\0" );
recipedata[101].itemID = 607;
recipedata[101].rec_itemID = 2208;
recipedata[101].level = 4;
recipedata[101].resultCount = 1;
recipedata[101].mpCost = 129;
recipedata[101].successRate = 100;

// recipe id 102
strcpy( recipedata[102].name, "mk_eldarake\0" );
recipedata[102].itemID = 631;
recipedata[102].rec_itemID = 2209;
recipedata[102].level = 4;
recipedata[102].resultCount = 1;
recipedata[102].mpCost = 138;
recipedata[102].successRate = 100;

// recipe id 103
strcpy( recipedata[103].name, "mk_metal_plate_helmet\0" );
recipedata[103].itemID = 498;
recipedata[103].rec_itemID = 2210;
recipedata[103].level = 4;
recipedata[103].resultCount = 1;
recipedata[103].mpCost = 141;
recipedata[103].successRate = 100;

// recipe id 104
strcpy( recipedata[104].name, "mk_plate_leather\0" );
recipedata[104].itemID = 398;
recipedata[104].rec_itemID = 2211;
recipedata[104].level = 5;
recipedata[104].resultCount = 1;
recipedata[104].mpCost = 144;
recipedata[104].successRate = 100;

// recipe id 105
strcpy( recipedata[105].name, "mk_plate_leather_gaiters\0" );
recipedata[105].itemID = 418;
recipedata[105].rec_itemID = 2212;
recipedata[105].level = 5;
recipedata[105].resultCount = 1;
recipedata[105].mpCost = 144;
recipedata[105].successRate = 100;

// recipe id 106
strcpy( recipedata[106].name, "mk_dwarven_chain_mail_shirt\0" );
recipedata[106].itemID = 355;
recipedata[106].rec_itemID = 2213;
recipedata[106].level = 5;
recipedata[106].resultCount = 1;
recipedata[106].mpCost = 147;
recipedata[106].successRate = 100;

// recipe id 107
strcpy( recipedata[107].name, "mk_dwarven_chain_gaiters\0" );
recipedata[107].itemID = 382;
recipedata[107].rec_itemID = 2214;
recipedata[107].level = 5;
recipedata[107].resultCount = 1;
recipedata[107].mpCost = 147;
recipedata[107].successRate = 100;

// recipe id 108
strcpy( recipedata[108].name, "mk_robe_of_seal\0" );
recipedata[108].itemID = 440;
recipedata[108].rec_itemID = 2215;
recipedata[108].level = 5;
recipedata[108].resultCount = 1;
recipedata[108].mpCost = 147;
recipedata[108].successRate = 100;

// recipe id 109
strcpy( recipedata[109].name, "mk_great_helmet\0" );
recipedata[109].itemID = 500;
recipedata[109].rec_itemID = 2216;
recipedata[109].level = 5;
recipedata[109].resultCount = 1;
recipedata[109].mpCost = 147;
recipedata[109].successRate = 100;

// recipe id 110
strcpy( recipedata[110].name, "mk_knight_shield\0" );
recipedata[110].itemID = 632;
recipedata[110].rec_itemID = 2217;
recipedata[110].level = 5;
recipedata[110].resultCount = 1;
recipedata[110].mpCost = 147;
recipedata[110].successRate = 100;

// recipe id 111
strcpy( recipedata[111].name, "mk_paagrio_hand\0" );
recipedata[111].itemID = 1120;
recipedata[111].rec_itemID = 2218;
recipedata[111].level = 5;
recipedata[111].resultCount = 1;
recipedata[111].mpCost = 147;
recipedata[111].successRate = 100;

// recipe id 112
strcpy( recipedata[112].name, "mk_crimson_boots\0" );
recipedata[112].itemID = 1126;
recipedata[112].rec_itemID = 2219;
recipedata[112].level = 5;
recipedata[112].resultCount = 1;
recipedata[112].mpCost = 147;
recipedata[112].successRate = 100;

// recipe id 113
strcpy( recipedata[113].name, "mk_rind_leather_mail\0" );
recipedata[113].itemID = 399;
recipedata[113].rec_itemID = 2220;
recipedata[113].level = 5;
recipedata[113].resultCount = 1;
recipedata[113].mpCost = 147;
recipedata[113].successRate = 100;

// recipe id 114
strcpy( recipedata[114].name, "mk_rind_leather_gaiters\0" );
recipedata[114].itemID = 419;
recipedata[114].rec_itemID = 2221;
recipedata[114].level = 5;
recipedata[114].resultCount = 1;
recipedata[114].mpCost = 147;
recipedata[114].successRate = 100;

// recipe id 115
strcpy( recipedata[115].name, "mk_composite_armor\0" );
recipedata[115].itemID = 60;
recipedata[115].rec_itemID = 2222;
recipedata[115].level = 5;
recipedata[115].resultCount = 1;
recipedata[115].mpCost = 156;
recipedata[115].successRate = 100;

// recipe id 116
strcpy( recipedata[116].name, "mk_tower_shield\0" );
recipedata[116].itemID = 103;
recipedata[116].rec_itemID = 2223;
recipedata[116].level = 5;
recipedata[116].resultCount = 1;
recipedata[116].mpCost = 156;
recipedata[116].successRate = 100;

// recipe id 117
strcpy( recipedata[117].name, "mk_demon's_tunic\0" );
recipedata[117].itemID = 441;
recipedata[117].rec_itemID = 2224;
recipedata[117].level = 5;
recipedata[117].resultCount = 1;
recipedata[117].mpCost = 156;
recipedata[117].successRate = 100;

// recipe id 118
strcpy( recipedata[118].name, "mk_demon's_hose\0" );
recipedata[118].itemID = 472;
recipedata[118].rec_itemID = 2225;
recipedata[118].level = 5;
recipedata[118].resultCount = 1;
recipedata[118].mpCost = 156;
recipedata[118].successRate = 100;

// recipe id 119
strcpy( recipedata[119].name, "mk_mithril_gauntlet\0" );
recipedata[119].itemID = 608;
recipedata[119].rec_itemID = 2226;
recipedata[119].level = 5;
recipedata[119].resultCount = 1;
recipedata[119].mpCost = 156;
recipedata[119].successRate = 100;

// recipe id 120
strcpy( recipedata[120].name, "mk_forgotten_boots\0" );
recipedata[120].itemID = 1127;
recipedata[120].rec_itemID = 2227;
recipedata[120].level = 5;
recipedata[120].resultCount = 1;
recipedata[120].mpCost = 156;
recipedata[120].successRate = 100;

// recipe id 121
strcpy( recipedata[121].name, "mk_shining_circlet\0" );
recipedata[121].itemID = 1149;
recipedata[121].rec_itemID = 2228;
recipedata[121].level = 5;
recipedata[121].resultCount = 1;
recipedata[121].mpCost = 156;
recipedata[121].successRate = 100;

// recipe id 122
strcpy( recipedata[122].name, "mk_theca_leather_mail\0" );
recipedata[122].itemID = 400;
recipedata[122].rec_itemID = 2229;
recipedata[122].level = 5;
recipedata[122].resultCount = 1;
recipedata[122].mpCost = 159;
recipedata[122].successRate = 100;

// recipe id 123
strcpy( recipedata[123].name, "mk_theca_leather_gaiters\0" );
recipedata[123].itemID = 420;
recipedata[123].rec_itemID = 2230;
recipedata[123].level = 5;
recipedata[123].resultCount = 1;
recipedata[123].mpCost = 159;
recipedata[123].successRate = 100;

// recipe id 124
strcpy( recipedata[124].name, "mk_full_plate_armor\0" );
recipedata[124].itemID = 356;
recipedata[124].rec_itemID = 2231;
recipedata[124].level = 6;
recipedata[124].resultCount = 1;
recipedata[124].mpCost = 165;
recipedata[124].successRate = 100;

// recipe id 125
strcpy( recipedata[125].name, "mk_drake_leather_mail\0" );
recipedata[125].itemID = 401;
recipedata[125].rec_itemID = 2232;
recipedata[125].level = 6;
recipedata[125].resultCount = 1;
recipedata[125].mpCost = 165;
recipedata[125].successRate = 100;

// recipe id 126
strcpy( recipedata[126].name, "mk_divine_tunic\0" );
recipedata[126].itemID = 442;
recipedata[126].rec_itemID = 2233;
recipedata[126].level = 6;
recipedata[126].resultCount = 1;
recipedata[126].mpCost = 165;
recipedata[126].successRate = 100;

// recipe id 127
strcpy( recipedata[127].name, "mk_divine_hose\0" );
recipedata[127].itemID = 473;
recipedata[127].rec_itemID = 2234;
recipedata[127].level = 6;
recipedata[127].resultCount = 1;
recipedata[127].mpCost = 165;
recipedata[127].successRate = 100;

// recipe id 128
strcpy( recipedata[128].name, "mk_mithril_helmet\0" );
recipedata[128].itemID = 499;
recipedata[128].rec_itemID = 2235;
recipedata[128].level = 6;
recipedata[128].resultCount = 1;
recipedata[128].mpCost = 165;
recipedata[128].successRate = 100;

// recipe id 129
strcpy( recipedata[129].name, "mk_cap_of_mana\0" );
recipedata[129].itemID = 529;
recipedata[129].rec_itemID = 2236;
recipedata[129].level = 6;
recipedata[129].resultCount = 1;
recipedata[129].mpCost = 165;
recipedata[129].successRate = 100;

// recipe id 130
strcpy( recipedata[130].name, "mk_paradia_hood\0" );
recipedata[130].itemID = 531;
recipedata[130].rec_itemID = 2237;
recipedata[130].level = 6;
recipedata[130].resultCount = 1;
recipedata[130].mpCost = 165;
recipedata[130].successRate = 100;

// recipe id 131
strcpy( recipedata[131].name, "mk_hood_of_sola_eclipse\0" );
recipedata[131].itemID = 533;
recipedata[131].rec_itemID = 2238;
recipedata[131].level = 6;
recipedata[131].resultCount = 1;
recipedata[131].mpCost = 165;
recipedata[131].successRate = 100;

// recipe id 132
strcpy( recipedata[132].name, "mk_hood_of_summons\0" );
recipedata[132].itemID = 535;
recipedata[132].rec_itemID = 2239;
recipedata[132].level = 6;
recipedata[132].resultCount = 1;
recipedata[132].mpCost = 165;
recipedata[132].successRate = 100;

// recipe id 133
strcpy( recipedata[133].name, "mk_elemental_hood\0" );
recipedata[133].itemID = 537;
recipedata[133].rec_itemID = 2240;
recipedata[133].level = 6;
recipedata[133].resultCount = 1;
recipedata[133].mpCost = 165;
recipedata[133].successRate = 100;

// recipe id 134
strcpy( recipedata[134].name, "mk_hood_of_grace\0" );
recipedata[134].itemID = 539;
recipedata[134].rec_itemID = 2241;
recipedata[134].level = 6;
recipedata[134].resultCount = 1;
recipedata[134].mpCost = 165;
recipedata[134].successRate = 100;

// recipe id 135
strcpy( recipedata[135].name, "mk_phoenix's_hood\0" );
recipedata[135].itemID = 541;
recipedata[135].rec_itemID = 2242;
recipedata[135].level = 6;
recipedata[135].resultCount = 1;
recipedata[135].mpCost = 165;
recipedata[135].successRate = 100;

// recipe id 136
strcpy( recipedata[136].name, "mk_hood_of_assistance\0" );
recipedata[136].itemID = 543;
recipedata[136].rec_itemID = 2243;
recipedata[136].level = 6;
recipedata[136].resultCount = 1;
recipedata[136].mpCost = 165;
recipedata[136].successRate = 100;

// recipe id 137
strcpy( recipedata[137].name, "mk_flame_bonnet\0" );
recipedata[137].itemID = 545;
recipedata[137].rec_itemID = 2244;
recipedata[137].level = 6;
recipedata[137].resultCount = 1;
recipedata[137].mpCost = 165;
recipedata[137].successRate = 100;

// recipe id 138
strcpy( recipedata[138].name, "mk_pata_i\0" );
recipedata[138].itemID = 264;
recipedata[138].rec_itemID = 5008;
recipedata[138].level = 6;
recipedata[138].resultCount = 1;
recipedata[138].mpCost = 171;
recipedata[138].successRate = 60;

// recipe id 139
strcpy( recipedata[139].name, "mk_bonnet_o'_avadon\0" );
recipedata[139].itemID = 549;
recipedata[139].rec_itemID = 2246;
recipedata[139].level = 6;
recipedata[139].resultCount = 1;
recipedata[139].mpCost = 165;
recipedata[139].successRate = 100;

// recipe id 140
strcpy( recipedata[140].name, "mk_helmet_of_pledge\0" );
recipedata[140].itemID = 551;
recipedata[140].rec_itemID = 2247;
recipedata[140].level = 6;
recipedata[140].resultCount = 1;
recipedata[140].mpCost = 165;
recipedata[140].successRate = 100;

// recipe id 141
strcpy( recipedata[141].name, "mk_gauntlet_of_ghost\0" );
recipedata[141].itemID = 609;
recipedata[141].rec_itemID = 2248;
recipedata[141].level = 6;
recipedata[141].resultCount = 1;
recipedata[141].mpCost = 165;
recipedata[141].successRate = 100;

// recipe id 142
strcpy( recipedata[142].name, "mk_adamantite_boots\0" );
recipedata[142].itemID = 1128;
recipedata[142].rec_itemID = 2249;
recipedata[142].level = 6;
recipedata[142].resultCount = 1;
recipedata[142].mpCost = 165;
recipedata[142].successRate = 100;

// recipe id 143
strcpy( recipedata[143].name, "mk_bone_arrow\0" );
recipedata[143].itemID = 1341;
recipedata[143].rec_itemID = 2250;
recipedata[143].level = 2;
recipedata[143].resultCount = 650;
recipedata[143].mpCost = 80;
recipedata[143].successRate = 100;

// recipe id 144
strcpy( recipedata[144].name, "mk_fine_steel_arrow\0" );
recipedata[144].itemID = 1342;
recipedata[144].rec_itemID = 2251;
recipedata[144].level = 3;
recipedata[144].resultCount = 600;
recipedata[144].mpCost = 100;
recipedata[144].successRate = 100;

// recipe id 145
strcpy( recipedata[145].name, "mk_iron_hammer\0" );
recipedata[145].itemID = 87;
recipedata[145].rec_itemID = 2252;
recipedata[145].level = 1;
recipedata[145].resultCount = 1;
recipedata[145].mpCost = 72;
recipedata[145].successRate = 100;

// recipe id 146
strcpy( recipedata[146].name, "mk_sword_breaker\0" );
recipedata[146].itemID = 219;
recipedata[146].rec_itemID = 2253;
recipedata[146].level = 1;
recipedata[146].resultCount = 1;
recipedata[146].mpCost = 72;
recipedata[146].successRate = 100;

// recipe id 147
strcpy( recipedata[147].name, "mk_composition_bow\0" );
recipedata[147].itemID = 273;
recipedata[147].rec_itemID = 2254;
recipedata[147].level = 1;
recipedata[147].resultCount = 1;
recipedata[147].mpCost = 72;
recipedata[147].successRate = 100;

// recipe id 148
strcpy( recipedata[148].name, "mk_saber\0" );
recipedata[148].itemID = 123;
recipedata[148].rec_itemID = 2255;
recipedata[148].level = 2;
recipedata[148].resultCount = 1;
recipedata[148].mpCost = 84;
recipedata[148].successRate = 100;

// recipe id 149
strcpy( recipedata[149].name, "mk_assassin_knife\0" );
recipedata[149].itemID = 221;
recipedata[149].rec_itemID = 2256;
recipedata[149].level = 2;
recipedata[149].resultCount = 1;
recipedata[149].mpCost = 84;
recipedata[149].successRate = 100;

// recipe id 150
strcpy( recipedata[150].name, "mk_trident\0" );
recipedata[150].itemID = 291;
recipedata[150].rec_itemID = 2257;
recipedata[150].level = 2;
recipedata[150].resultCount = 1;
recipedata[150].mpCost = 84;
recipedata[150].successRate = 100;

// recipe id 151
strcpy( recipedata[151].name, "mk_temptation_of_abyss\0" );
recipedata[151].itemID = 313;
recipedata[151].rec_itemID = 2258;
recipedata[151].level = 2;
recipedata[151].resultCount = 1;
recipedata[151].mpCost = 84;
recipedata[151].successRate = 100;

// recipe id 152
strcpy( recipedata[152].name, "mk_spinebone_sword\0" );
recipedata[152].itemID = 125;
recipedata[152].rec_itemID = 2259;
recipedata[152].level = 2;
recipedata[152].resultCount = 1;
recipedata[152].mpCost = 96;
recipedata[152].successRate = 100;

// recipe id 153
strcpy( recipedata[153].name, "mk_mace_of_judgment\0" );
recipedata[153].itemID = 180;
recipedata[153].rec_itemID = 2260;
recipedata[153].level = 2;
recipedata[153].resultCount = 1;
recipedata[153].mpCost = 96;
recipedata[153].successRate = 100;

// recipe id 154
strcpy( recipedata[154].name, "mk_conjure_staff\0" );
recipedata[154].itemID = 184;
recipedata[154].rec_itemID = 2261;
recipedata[154].level = 2;
recipedata[154].resultCount = 1;
recipedata[154].mpCost = 96;
recipedata[154].successRate = 100;

// recipe id 155
strcpy( recipedata[155].name, "mk_elven_bow\0" );
recipedata[155].itemID = 276;
recipedata[155].rec_itemID = 2262;
recipedata[155].level = 2;
recipedata[155].resultCount = 1;
recipedata[155].mpCost = 96;
recipedata[155].successRate = 100;

// recipe id 156
strcpy( recipedata[156].name, "mk_dwarven_trident\0" );
recipedata[156].itemID = 295;
recipedata[156].rec_itemID = 2263;
recipedata[156].level = 2;
recipedata[156].resultCount = 1;
recipedata[156].mpCost = 96;
recipedata[156].successRate = 100;

// recipe id 157
strcpy( recipedata[157].name, "mk_two-handed_sword\0" );
recipedata[157].itemID = 124;
recipedata[157].rec_itemID = 2264;
recipedata[157].level = 3;
recipedata[157].resultCount = 1;
recipedata[157].mpCost = 108;
recipedata[157].successRate = 100;

// recipe id 158
strcpy( recipedata[158].name, "mk_spike_club\0" );
recipedata[158].itemID = 157;
recipedata[158].rec_itemID = 2265;
recipedata[158].level = 3;
recipedata[158].resultCount = 1;
recipedata[158].mpCost = 108;
recipedata[158].successRate = 100;

// recipe id 159
strcpy( recipedata[159].name, "mk_knife_o'_silenus\0" );
recipedata[159].itemID = 241;
recipedata[159].rec_itemID = 2266;
recipedata[159].level = 3;
recipedata[159].resultCount = 1;
recipedata[159].mpCost = 108;
recipedata[159].successRate = 100;

// recipe id 160
strcpy( recipedata[160].name, "mk_gastraphetes\0" );
recipedata[160].itemID = 278;
recipedata[160].rec_itemID = 2267;
recipedata[160].level = 3;
recipedata[160].resultCount = 1;
recipedata[160].mpCost = 108;
recipedata[160].successRate = 100;

// recipe id 161
strcpy( recipedata[161].name, "mk_tome_of_blood\0" );
recipedata[161].itemID = 317;
recipedata[161].rec_itemID = 2268;
recipedata[161].level = 3;
recipedata[161].resultCount = 1;
recipedata[161].mpCost = 108;
recipedata[161].successRate = 100;

// recipe id 162
strcpy( recipedata[162].name, "mk_morning_star\0" );
recipedata[162].itemID = 88;
recipedata[162].rec_itemID = 2269;
recipedata[162].level = 3;
recipedata[162].resultCount = 1;
recipedata[162].mpCost = 120;
recipedata[162].successRate = 100;

// recipe id 163
strcpy( recipedata[163].name, "mk_goathead_staff\0" );
recipedata[163].itemID = 90;
recipedata[163].rec_itemID = 2270;
recipedata[163].level = 3;
recipedata[163].resultCount = 1;
recipedata[163].mpCost = 120;
recipedata[163].successRate = 100;

// recipe id 164
strcpy( recipedata[164].name, "mk_winged_spear\0" );
recipedata[164].itemID = 93;
recipedata[164].rec_itemID = 2271;
recipedata[164].level = 3;
recipedata[164].resultCount = 1;
recipedata[164].mpCost = 120;
recipedata[164].successRate = 100;

// recipe id 165
strcpy( recipedata[165].name, "mk_sword_of_revolution\0" );
recipedata[165].itemID = 129;
recipedata[165].rec_itemID = 2272;
recipedata[165].level = 3;
recipedata[165].resultCount = 1;
recipedata[165].mpCost = 120;
recipedata[165].successRate = 100;

// recipe id 166
strcpy( recipedata[166].name, "mk_tarbar\0" );
recipedata[166].itemID = 158;
recipedata[166].rec_itemID = 2273;
recipedata[166].level = 3;
recipedata[166].resultCount = 1;
recipedata[166].mpCost = 120;
recipedata[166].successRate = 100;

// recipe id 167
strcpy( recipedata[167].name, "mk_skull_breaker\0" );
recipedata[167].itemID = 169;
recipedata[167].rec_itemID = 2274;
recipedata[167].level = 3;
recipedata[167].resultCount = 1;
recipedata[167].mpCost = 120;
recipedata[167].successRate = 100;

// recipe id 168
strcpy( recipedata[168].name, "mk_heavy_bone_club\0" );
recipedata[168].itemID = 172;
recipedata[168].rec_itemID = 2275;
recipedata[168].level = 3;
recipedata[168].resultCount = 1;
recipedata[168].mpCost = 120;
recipedata[168].successRate = 100;

// recipe id 169
strcpy( recipedata[169].name, "mk_maingauche\0" );
recipedata[169].itemID = 224;
recipedata[169].rec_itemID = 2276;
recipedata[169].level = 3;
recipedata[169].resultCount = 1;
recipedata[169].mpCost = 120;
recipedata[169].successRate = 100;

// recipe id 170
strcpy( recipedata[170].name, "mk_bich'hwa\0" );
recipedata[170].itemID = 261;
recipedata[170].rec_itemID = 2277;
recipedata[170].level = 3;
recipedata[170].resultCount = 1;
recipedata[170].mpCost = 120;
recipedata[170].successRate = 100;

// recipe id 171
strcpy( recipedata[171].name, "mk_strengthening_long_bow\0" );
recipedata[171].itemID = 279;
recipedata[171].rec_itemID = 2278;
recipedata[171].level = 3;
recipedata[171].resultCount = 1;
recipedata[171].mpCost = 120;
recipedata[171].successRate = 100;

// recipe id 172
strcpy( recipedata[172].name, "mk_hammer_in_flames\0" );
recipedata[172].itemID = 294;
recipedata[172].rec_itemID = 2279;
recipedata[172].level = 3;
recipedata[172].resultCount = 1;
recipedata[172].mpCost = 120;
recipedata[172].successRate = 100;

// recipe id 173
strcpy( recipedata[173].name, "mk_crucifix_of_blood\0" );
recipedata[173].itemID = 318;
recipedata[173].rec_itemID = 2280;
recipedata[173].level = 3;
recipedata[173].resultCount = 1;
recipedata[173].mpCost = 120;
recipedata[173].successRate = 100;

// recipe id 174
strcpy( recipedata[174].name, "mk_eye_of_infinity\0" );
recipedata[174].itemID = 319;
recipedata[174].rec_itemID = 2281;
recipedata[174].level = 3;
recipedata[174].resultCount = 1;
recipedata[174].mpCost = 120;
recipedata[174].successRate = 100;

// recipe id 175
strcpy( recipedata[175].name, "mk_cursed_maingauche\0" );
recipedata[175].itemID = 1660;
recipedata[175].rec_itemID = 2282;
recipedata[175].level = 3;
recipedata[175].resultCount = 1;
recipedata[175].mpCost = 120;
recipedata[175].successRate = 100;

// recipe id 176
strcpy( recipedata[176].name, "mk_blue_crystal_skull\0" );
recipedata[176].itemID = 320;
recipedata[176].rec_itemID = 2283;
recipedata[176].level = 4;
recipedata[176].resultCount = 1;
recipedata[176].mpCost = 123;
recipedata[176].successRate = 100;

// recipe id 177
strcpy( recipedata[177].name, "mk_demon_fangs\0" );
recipedata[177].itemID = 321;
recipedata[177].rec_itemID = 2284;
recipedata[177].level = 4;
recipedata[177].resultCount = 1;
recipedata[177].mpCost = 123;
recipedata[177].successRate = 100;

// recipe id 178
strcpy( recipedata[178].name, "mk_claymore\0" );
recipedata[178].itemID = 70;
recipedata[178].rec_itemID = 2285;
recipedata[178].level = 4;
recipedata[178].resultCount = 1;
recipedata[178].mpCost = 129;
recipedata[178].successRate = 100;

// recipe id 179
strcpy( recipedata[179].name, "mk_bonebreaker\0" );
recipedata[179].itemID = 159;
recipedata[179].rec_itemID = 2286;
recipedata[179].level = 4;
recipedata[179].resultCount = 1;
recipedata[179].mpCost = 129;
recipedata[179].successRate = 100;

// recipe id 180
strcpy( recipedata[180].name, "mk_atuba_hammer\0" );
recipedata[180].itemID = 187;
recipedata[180].rec_itemID = 2287;
recipedata[180].level = 4;
recipedata[180].resultCount = 1;
recipedata[180].mpCost = 129;
recipedata[180].successRate = 100;

// recipe id 181
strcpy( recipedata[181].name, "mk_ghost_staff\0" );
recipedata[181].itemID = 188;
recipedata[181].rec_itemID = 2288;
recipedata[181].level = 4;
recipedata[181].resultCount = 1;
recipedata[181].mpCost = 129;
recipedata[181].successRate = 100;

// recipe id 182
strcpy( recipedata[182].name, "mk_life_stick\0" );
recipedata[182].itemID = 189;
recipedata[182].rec_itemID = 2289;
recipedata[182].level = 4;
recipedata[182].resultCount = 1;
recipedata[182].mpCost = 129;
recipedata[182].successRate = 100;

// recipe id 183
strcpy( recipedata[183].name, "mk_mithril_dagger\0" );
recipedata[183].itemID = 225;
recipedata[183].rec_itemID = 2290;
recipedata[183].level = 4;
recipedata[183].resultCount = 1;
recipedata[183].mpCost = 129;
recipedata[183].successRate = 100;

// recipe id 184
strcpy( recipedata[184].name, "mk_scallop_jamadhr\0" );
recipedata[184].itemID = 262;
recipedata[184].rec_itemID = 2291;
recipedata[184].level = 4;
recipedata[184].resultCount = 1;
recipedata[184].mpCost = 129;
recipedata[184].successRate = 100;

// recipe id 185
strcpy( recipedata[185].name, "mk_cyclone_bow\0" );
recipedata[185].itemID = 280;
recipedata[185].rec_itemID = 2292;
recipedata[185].level = 4;
recipedata[185].resultCount = 1;
recipedata[185].mpCost = 129;
recipedata[185].successRate = 100;

// recipe id 186
strcpy( recipedata[186].name, "mk_glaive\0" );
recipedata[186].itemID = 297;
recipedata[186].rec_itemID = 2293;
recipedata[186].level = 4;
recipedata[186].resultCount = 1;
recipedata[186].mpCost = 129;
recipedata[186].successRate = 100;

// recipe id 187
strcpy( recipedata[187].name, "mk_vajra_wands\0" );
recipedata[187].itemID = 322;
recipedata[187].rec_itemID = 2294;
recipedata[187].level = 4;
recipedata[187].resultCount = 1;
recipedata[187].mpCost = 129;
recipedata[187].successRate = 100;

// recipe id 188
strcpy( recipedata[188].name, "mk_ancient_reagent\0" );
recipedata[188].itemID = 323;
recipedata[188].rec_itemID = 2295;
recipedata[188].level = 4;
recipedata[188].resultCount = 1;
recipedata[188].mpCost = 129;
recipedata[188].successRate = 100;

// recipe id 189
strcpy( recipedata[189].name, "mk_atuba_mace\0" );
recipedata[189].itemID = 190;
recipedata[189].rec_itemID = 2296;
recipedata[189].level = 4;
recipedata[189].resultCount = 1;
recipedata[189].mpCost = 129;
recipedata[189].successRate = 100;

// recipe id 190
strcpy( recipedata[190].name, "mk_flamberge\0" );
recipedata[190].itemID = 71;
recipedata[190].rec_itemID = 2297;
recipedata[190].level = 4;
recipedata[190].resultCount = 1;
recipedata[190].mpCost = 138;
recipedata[190].successRate = 100;

// recipe id 191
strcpy( recipedata[191].name, "mk_stormbringer\0" );
recipedata[191].itemID = 72;
recipedata[191].rec_itemID = 2298;
recipedata[191].level = 4;
recipedata[191].resultCount = 1;
recipedata[191].mpCost = 138;
recipedata[191].successRate = 100;

// recipe id 192
strcpy( recipedata[192].name, "mk_big_hammer\0" );
recipedata[192].itemID = 89;
recipedata[192].rec_itemID = 2299;
recipedata[192].level = 4;
recipedata[192].resultCount = 1;
recipedata[192].mpCost = 138;
recipedata[192].successRate = 100;

// recipe id 193
strcpy( recipedata[193].name, "mk_scythe\0" );
recipedata[193].itemID = 96;
recipedata[193].rec_itemID = 2300;
recipedata[193].level = 4;
recipedata[193].resultCount = 1;
recipedata[193].mpCost = 138;
recipedata[193].successRate = 100;

// recipe id 194
strcpy( recipedata[194].name, "mk_battle_axe\0" );
recipedata[194].itemID = 160;
recipedata[194].rec_itemID = 2301;
recipedata[194].level = 4;
recipedata[194].resultCount = 1;
recipedata[194].mpCost = 138;
recipedata[194].successRate = 100;

// recipe id 195
strcpy( recipedata[195].name, "mk_war_pick\0" );
recipedata[195].itemID = 161;
recipedata[195].rec_itemID = 2302;
recipedata[195].level = 4;
recipedata[195].resultCount = 1;
recipedata[195].mpCost = 138;
recipedata[195].successRate = 100;

// recipe id 196
strcpy( recipedata[196].name, "mk_skull_graver\0" );
recipedata[196].itemID = 173;
recipedata[196].rec_itemID = 2303;
recipedata[196].level = 4;
recipedata[196].resultCount = 1;
recipedata[196].mpCost = 138;
recipedata[196].successRate = 100;

// recipe id 197
strcpy( recipedata[197].name, "mk_heavy_doom_hammer\0" );
recipedata[197].itemID = 191;
recipedata[197].rec_itemID = 2304;
recipedata[197].level = 4;
recipedata[197].resultCount = 1;
recipedata[197].mpCost = 138;
recipedata[197].successRate = 100;

// recipe id 198
strcpy( recipedata[198].name, "mk_crystal_staff\0" );
recipedata[198].itemID = 192;
recipedata[198].rec_itemID = 2305;
recipedata[198].level = 4;
recipedata[198].resultCount = 1;
recipedata[198].mpCost = 138;
recipedata[198].successRate = 100;

// recipe id 199
strcpy( recipedata[199].name, "mk_stick_of_faith\0" );
recipedata[199].itemID = 193;
recipedata[199].rec_itemID = 2306;
recipedata[199].level = 4;
recipedata[199].resultCount = 1;
recipedata[199].mpCost = 138;
recipedata[199].successRate = 100;

// recipe id 200
strcpy( recipedata[200].name, "mk_heavy_doom_axe\0" );
recipedata[200].itemID = 194;
recipedata[200].rec_itemID = 2307;
recipedata[200].level = 4;
recipedata[200].resultCount = 1;
recipedata[200].mpCost = 138;
recipedata[200].successRate = 100;

// recipe id 201
strcpy( recipedata[201].name, "mk_cursed_dagger\0" );
recipedata[201].itemID = 226;
recipedata[201].rec_itemID = 2308;
recipedata[201].level = 4;
recipedata[201].resultCount = 1;
recipedata[201].mpCost = 138;
recipedata[201].successRate = 100;

// recipe id 202
strcpy( recipedata[202].name, "mk_needle_wolverine\0" );
recipedata[202].itemID = 230;
recipedata[202].rec_itemID = 2309;
recipedata[202].level = 4;
recipedata[202].resultCount = 1;
recipedata[202].mpCost = 138;
recipedata[202].successRate = 100;

// recipe id 203
strcpy( recipedata[203].name, "mk_darkelven_dagger\0" );
recipedata[203].itemID = 232;
recipedata[203].rec_itemID = 2310;
recipedata[203].level = 4;
recipedata[203].resultCount = 1;
recipedata[203].mpCost = 138;
recipedata[203].successRate = 100;

// recipe id 204
strcpy( recipedata[204].name, "mk_chakram\0" );
recipedata[204].itemID = 263;
recipedata[204].rec_itemID = 2311;
recipedata[204].level = 4;
recipedata[204].resultCount = 1;
recipedata[204].mpCost = 138;
recipedata[204].successRate = 100;

// recipe id 205
strcpy( recipedata[205].name, "mk_crystallized_ice_bow\0" );
recipedata[205].itemID = 281;
recipedata[205].rec_itemID = 2312;
recipedata[205].level = 4;
recipedata[205].resultCount = 1;
recipedata[205].mpCost = 138;
recipedata[205].successRate = 100;

// recipe id 206
strcpy( recipedata[206].name, "mk_orcish_glaive\0" );
recipedata[206].itemID = 298;
recipedata[206].rec_itemID = 2313;
recipedata[206].level = 4;
recipedata[206].resultCount = 1;
recipedata[206].mpCost = 138;
recipedata[206].successRate = 100;

// recipe id 207
strcpy( recipedata[207].name, "mk_body_slasher\0" );
recipedata[207].itemID = 302;
recipedata[207].rec_itemID = 2314;
recipedata[207].level = 4;
recipedata[207].resultCount = 1;
recipedata[207].mpCost = 138;
recipedata[207].successRate = 100;

// recipe id 208
strcpy( recipedata[208].name, "mk_shamshir\0" );
recipedata[208].itemID = 73;
recipedata[208].rec_itemID = 2315;
recipedata[208].level = 5;
recipedata[208].resultCount = 1;
recipedata[208].mpCost = 147;
recipedata[208].successRate = 100;

// recipe id 209
strcpy( recipedata[209].name, "mk_katana\0" );
recipedata[209].itemID = 74;
recipedata[209].rec_itemID = 2316;
recipedata[209].level = 5;
recipedata[209].resultCount = 1;
recipedata[209].mpCost = 147;
recipedata[209].successRate = 100;

// recipe id 210
strcpy( recipedata[210].name, "mk_bech_de_corbin\0" );
recipedata[210].itemID = 94;
recipedata[210].rec_itemID = 2317;
recipedata[210].level = 5;
recipedata[210].resultCount = 1;
recipedata[210].mpCost = 147;
recipedata[210].successRate = 100;

// recipe id 211
strcpy( recipedata[211].name, "mk_spirits_sword\0" );
recipedata[211].itemID = 131;
recipedata[211].rec_itemID = 2318;
recipedata[211].level = 5;
recipedata[211].resultCount = 1;
recipedata[211].mpCost = 147;
recipedata[211].successRate = 100;

// recipe id 212
strcpy( recipedata[212].name, "mk_raid_sword\0" );
recipedata[212].itemID = 133;
recipedata[212].rec_itemID = 2319;
recipedata[212].level = 5;
recipedata[212].resultCount = 1;
recipedata[212].mpCost = 147;
recipedata[212].successRate = 100;

// recipe id 213
strcpy( recipedata[213].name, "mk_cursed_staff\0" );
recipedata[213].itemID = 195;
recipedata[213].rec_itemID = 2320;
recipedata[213].level = 5;
recipedata[213].resultCount = 1;
recipedata[213].mpCost = 147;
recipedata[213].successRate = 100;

// recipe id 214
strcpy( recipedata[214].name, "mk_stiletto\0" );
recipedata[214].itemID = 227;
recipedata[214].rec_itemID = 2321;
recipedata[214].level = 5;
recipedata[214].resultCount = 1;
recipedata[214].mpCost = 147;
recipedata[214].successRate = 100;

// recipe id 215
strcpy( recipedata[215].name, "mk_dagger_of_magicflame\0" );
recipedata[215].itemID = 242;
recipedata[215].rec_itemID = 2322;
recipedata[215].level = 5;
recipedata[215].resultCount = 1;
recipedata[215].mpCost = 147;
recipedata[215].successRate = 100;

// recipe id 216
strcpy( recipedata[216].name, "mk_elemental_bow\0" );
recipedata[216].itemID = 282;
recipedata[216].rec_itemID = 2323;
recipedata[216].level = 5;
recipedata[216].resultCount = 1;
recipedata[216].mpCost = 147;
recipedata[216].successRate = 100;

// recipe id 217
strcpy( recipedata[217].name, "mk_noble_elven_bow\0" );
recipedata[217].itemID = 285;
recipedata[217].rec_itemID = 2324;
recipedata[217].level = 5;
recipedata[217].resultCount = 1;
recipedata[217].mpCost = 147;
recipedata[217].successRate = 100;

// recipe id 218
strcpy( recipedata[218].name, "mk_tears_of_fairy\0" );
recipedata[218].itemID = 324;
recipedata[218].rec_itemID = 2325;
recipedata[218].level = 5;
recipedata[218].resultCount = 1;
recipedata[218].mpCost = 147;
recipedata[218].successRate = 100;

// recipe id 219
strcpy( recipedata[219].name, "mk_horn_of_glory\0" );
recipedata[219].itemID = 325;
recipedata[219].rec_itemID = 2326;
recipedata[219].level = 5;
recipedata[219].resultCount = 1;
recipedata[219].mpCost = 147;
recipedata[219].successRate = 100;

// recipe id 220
strcpy( recipedata[220].name, "mk_caliburs\0" );
recipedata[220].itemID = 75;
recipedata[220].rec_itemID = 2327;
recipedata[220].level = 5;
recipedata[220].resultCount = 1;
recipedata[220].mpCost = 156;
recipedata[220].successRate = 100;

// recipe id 221
strcpy( recipedata[221].name, "mk_sword_of_delusion\0" );
recipedata[221].itemID = 76;
recipedata[221].rec_itemID = 2328;
recipedata[221].level = 5;
recipedata[221].resultCount = 1;
recipedata[221].mpCost = 156;
recipedata[221].successRate = 100;

// recipe id 222
strcpy( recipedata[222].name, "mk_tsurugi\0" );
recipedata[222].itemID = 77;
recipedata[222].rec_itemID = 2329;
recipedata[222].level = 5;
recipedata[222].resultCount = 1;
recipedata[222].mpCost = 156;
recipedata[222].successRate = 100;

// recipe id 223
strcpy( recipedata[223].name, "mk_homunkulus's_sword\0" );
recipedata[223].itemID = 84;
recipedata[223].rec_itemID = 2330;
recipedata[223].level = 5;
recipedata[223].resultCount = 1;
recipedata[223].mpCost = 156;
recipedata[223].successRate = 100;

// recipe id 224
strcpy( recipedata[224].name, "mk_poleaxe\0" );
recipedata[224].itemID = 95;
recipedata[224].rec_itemID = 2331;
recipedata[224].level = 5;
recipedata[224].resultCount = 1;
recipedata[224].mpCost = 156;
recipedata[224].successRate = 100;

// recipe id 225
strcpy( recipedata[225].name, "mk_sword_of_limit\0" );
recipedata[225].itemID = 132;
recipedata[225].rec_itemID = 2332;
recipedata[225].level = 5;
recipedata[225].resultCount = 1;
recipedata[225].mpCost = 156;
recipedata[225].successRate = 100;

// recipe id 226
strcpy( recipedata[226].name, "mk_sword_of_nightmare\0" );
recipedata[226].itemID = 134;
recipedata[226].rec_itemID = 2333;
recipedata[226].level = 5;
recipedata[226].resultCount = 1;
recipedata[226].mpCost = 156;
recipedata[226].successRate = 100;

// recipe id 227
strcpy( recipedata[227].name, "mk_deathbreath_sword\0" );
recipedata[227].itemID = 145;
recipedata[227].rec_itemID = 2334;
recipedata[227].level = 5;
recipedata[227].resultCount = 1;
recipedata[227].mpCost = 156;
recipedata[227].successRate = 100;

// recipe id 228
strcpy( recipedata[228].name, "mk_war_axe\0" );
recipedata[228].itemID = 162;
recipedata[228].rec_itemID = 2335;
recipedata[228].level = 5;
recipedata[228].resultCount = 1;
recipedata[228].mpCost = 156;
recipedata[228].successRate = 100;

// recipe id 229
strcpy( recipedata[229].name, "mk_nirvana_axe\0" );
recipedata[229].itemID = 174;
recipedata[229].rec_itemID = 2336;
recipedata[229].level = 5;
recipedata[229].resultCount = 1;
recipedata[229].mpCost = 156;
recipedata[229].successRate = 100;

// recipe id 230
strcpy( recipedata[230].name, "mk_stick_of_eternity\0" );
recipedata[230].itemID = 196;
recipedata[230].rec_itemID = 2337;
recipedata[230].level = 5;
recipedata[230].resultCount = 1;
recipedata[230].mpCost = 156;
recipedata[230].successRate = 100;

// recipe id 231
strcpy( recipedata[231].name, "mk_paradia_staff\0" );
recipedata[231].itemID = 197;
recipedata[231].rec_itemID = 2338;
recipedata[231].level = 5;
recipedata[231].resultCount = 1;
recipedata[231].mpCost = 156;
recipedata[231].successRate = 100;

// recipe id 232
strcpy( recipedata[232].name, "mk_inferno_staff\0" );
recipedata[232].itemID = 198;
recipedata[232].rec_itemID = 2339;
recipedata[232].level = 5;
recipedata[232].resultCount = 1;
recipedata[232].mpCost = 156;
recipedata[232].successRate = 100;

// recipe id 233
strcpy( recipedata[233].name, "mk_paagrio_hammer\0" );
recipedata[233].itemID = 199;
recipedata[233].rec_itemID = 2340;
recipedata[233].level = 5;
recipedata[233].resultCount = 1;
recipedata[233].mpCost = 156;
recipedata[233].successRate = 100;

// recipe id 234
strcpy( recipedata[234].name, "mk_sage's_staff\0" );
recipedata[234].itemID = 200;
recipedata[234].rec_itemID = 2341;
recipedata[234].level = 5;
recipedata[234].resultCount = 1;
recipedata[234].mpCost = 156;
recipedata[234].successRate = 100;

// recipe id 235
strcpy( recipedata[235].name, "mk_club_of_nature\0" );
recipedata[235].itemID = 201;
recipedata[235].rec_itemID = 2342;
recipedata[235].level = 5;
recipedata[235].resultCount = 1;
recipedata[235].mpCost = 156;
recipedata[235].successRate = 100;

// recipe id 236
strcpy( recipedata[236].name, "mk_mace_of_underworld\0" );
recipedata[236].itemID = 202;
recipedata[236].rec_itemID = 2343;
recipedata[236].level = 5;
recipedata[236].resultCount = 1;
recipedata[236].mpCost = 156;
recipedata[236].successRate = 100;

// recipe id 237
strcpy( recipedata[237].name, "mk_grace_dagger\0" );
recipedata[237].itemID = 231;
recipedata[237].rec_itemID = 2344;
recipedata[237].level = 5;
recipedata[237].resultCount = 1;
recipedata[237].mpCost = 156;
recipedata[237].successRate = 100;

// recipe id 238
strcpy( recipedata[238].name, "mk_dark_screamer\0" );
recipedata[238].itemID = 233;
recipedata[238].rec_itemID = 2345;
recipedata[238].level = 5;
recipedata[238].resultCount = 1;
recipedata[238].mpCost = 156;
recipedata[238].successRate = 100;

// recipe id 239
strcpy( recipedata[239].name, "mk_fist_blade\0" );
recipedata[239].itemID = 265;
recipedata[239].rec_itemID = 2346;
recipedata[239].level = 5;
recipedata[239].resultCount = 1;
recipedata[239].mpCost = 156;
recipedata[239].successRate = 100;

// recipe id 240
strcpy( recipedata[240].name, "mk_akat_long_bow\0" );
recipedata[240].itemID = 283;
recipedata[240].rec_itemID = 2347;
recipedata[240].level = 5;
recipedata[240].resultCount = 1;
recipedata[240].mpCost = 156;
recipedata[240].successRate = 100;

// recipe id 241
strcpy( recipedata[241].name, "mk_heathen's_book\0" );
recipedata[241].itemID = 326;
recipedata[241].rec_itemID = 2348;
recipedata[241].level = 5;
recipedata[241].resultCount = 1;
recipedata[241].mpCost = 156;
recipedata[241].successRate = 100;

// recipe id 242
strcpy( recipedata[242].name, "mk_hex_doll\0" );
recipedata[242].itemID = 327;
recipedata[242].rec_itemID = 2349;
recipedata[242].level = 5;
recipedata[242].resultCount = 1;
recipedata[242].mpCost = 156;
recipedata[242].successRate = 100;

// recipe id 243
strcpy( recipedata[243].name, "mk_paagrio_axe\0" );
recipedata[243].itemID = 203;
recipedata[243].rec_itemID = 2350;
recipedata[243].level = 5;
recipedata[243].resultCount = 1;
recipedata[243].mpCost = 159;
recipedata[243].successRate = 100;

// recipe id 244
strcpy( recipedata[244].name, "mk_scorpion\0" );
recipedata[244].itemID = 301;
recipedata[244].rec_itemID = 2351;
recipedata[244].level = 5;
recipedata[244].resultCount = 1;
recipedata[244].mpCost = 159;
recipedata[244].successRate = 100;

// recipe id 245
strcpy( recipedata[245].name, "mk_widow_maker\0" );
recipedata[245].itemID = 303;
recipedata[245].rec_itemID = 2352;
recipedata[245].level = 5;
recipedata[245].resultCount = 1;
recipedata[245].mpCost = 159;
recipedata[245].successRate = 100;

// recipe id 246
strcpy( recipedata[246].name, "mk_samurai_longsword\0" );
recipedata[246].itemID = 135;
recipedata[246].rec_itemID = 2353;
recipedata[246].level = 6;
recipedata[246].resultCount = 1;
recipedata[246].mpCost = 165;
recipedata[246].successRate = 100;

// recipe id 247
strcpy( recipedata[247].name, "mk_deadman's_staff\0" );
recipedata[247].itemID = 204;
recipedata[247].rec_itemID = 2354;
recipedata[247].level = 6;
recipedata[247].resultCount = 1;
recipedata[247].mpCost = 165;
recipedata[247].successRate = 100;

// recipe id 248
strcpy( recipedata[248].name, "mk_ghoul's_staff\0" );
recipedata[248].itemID = 205;
recipedata[248].rec_itemID = 2355;
recipedata[248].level = 6;
recipedata[248].resultCount = 1;
recipedata[248].mpCost = 165;
recipedata[248].successRate = 100;

// recipe id 249
strcpy( recipedata[249].name, "mk_demon's_staff\0" );
recipedata[249].itemID = 206;
recipedata[249].rec_itemID = 2356;
recipedata[249].level = 6;
recipedata[249].resultCount = 1;
recipedata[249].mpCost = 165;
recipedata[249].successRate = 100;

// recipe id 250
strcpy( recipedata[250].name, "mk_crystal_dagger\0" );
recipedata[250].itemID = 228;
recipedata[250].rec_itemID = 2357;
recipedata[250].level = 6;
recipedata[250].resultCount = 1;
recipedata[250].mpCost = 165;
recipedata[250].successRate = 100;

// recipe id 251
strcpy( recipedata[251].name, "mk_great_pata\0" );
recipedata[251].itemID = 266;
recipedata[251].rec_itemID = 2358;
recipedata[251].level = 6;
recipedata[251].resultCount = 1;
recipedata[251].mpCost = 165;
recipedata[251].successRate = 100;

// recipe id 252
strcpy( recipedata[252].name, "mk_eminence_bow\0" );
recipedata[252].itemID = 286;
recipedata[252].rec_itemID = 2359;
recipedata[252].level = 6;
recipedata[252].resultCount = 1;
recipedata[252].mpCost = 165;
recipedata[252].successRate = 100;

// recipe id 253
strcpy( recipedata[253].name, "mk_orcish_poleaxe\0" );
recipedata[253].itemID = 299;
recipedata[253].rec_itemID = 2360;
recipedata[253].level = 6;
recipedata[253].resultCount = 1;
recipedata[253].mpCost = 165;
recipedata[253].successRate = 100;

// recipe id 254
strcpy( recipedata[254].name, "mk_candle_of_wisdom\0" );
recipedata[254].itemID = 328;
recipedata[254].rec_itemID = 2361;
recipedata[254].level = 6;
recipedata[254].resultCount = 1;
recipedata[254].mpCost = 165;
recipedata[254].successRate = 100;

// recipe id 255
strcpy( recipedata[255].name, "mk_blessed_branch\0" );
recipedata[255].itemID = 329;
recipedata[255].rec_itemID = 2362;
recipedata[255].level = 6;
recipedata[255].resultCount = 1;
recipedata[255].mpCost = 165;
recipedata[255].successRate = 100;

// recipe id 256
strcpy( recipedata[256].name, "mk_phoenix's_feather\0" );
recipedata[256].itemID = 330;
recipedata[256].rec_itemID = 2363;
recipedata[256].level = 6;
recipedata[256].resultCount = 1;
recipedata[256].mpCost = 165;
recipedata[256].successRate = 100;

// recipe id 257
strcpy( recipedata[257].name, "mk_cerberus's_eye\0" );
recipedata[257].itemID = 331;
recipedata[257].rec_itemID = 2364;
recipedata[257].level = 6;
recipedata[257].resultCount = 1;
recipedata[257].mpCost = 165;
recipedata[257].successRate = 100;

// recipe id 258
strcpy( recipedata[258].name, "mk_scroll_of_destruction\0" );
recipedata[258].itemID = 332;
recipedata[258].rec_itemID = 2365;
recipedata[258].level = 6;
recipedata[258].resultCount = 1;
recipedata[258].mpCost = 165;
recipedata[258].successRate = 100;

// recipe id 259
strcpy( recipedata[259].name, "mk_claws_of_blackdragon\0" );
recipedata[259].itemID = 333;
recipedata[259].rec_itemID = 2366;
recipedata[259].level = 6;
recipedata[259].resultCount = 1;
recipedata[259].mpCost = 165;
recipedata[259].successRate = 100;

// recipe id 260
strcpy( recipedata[260].name, "mk_three_eye_crow's_feather\0" );
recipedata[260].itemID = 334;
recipedata[260].rec_itemID = 2367;
recipedata[260].level = 6;
recipedata[260].resultCount = 1;
recipedata[260].mpCost = 165;
recipedata[260].successRate = 100;

// recipe id 261
strcpy( recipedata[261].name, "mk_ring_of_binding\0" );
recipedata[261].itemID = 886;
recipedata[261].rec_itemID = 2970;
recipedata[261].level = 6;
recipedata[261].resultCount = 1;
recipedata[261].mpCost = 165;
recipedata[261].successRate = 100;

// recipe id 262
strcpy( recipedata[262].name, "mk_mithril_scale_gaiters\0" );
recipedata[262].itemID = 2377;
recipedata[262].rec_itemID = 2971;
recipedata[262].level = 3;
recipedata[262].resultCount = 1;
recipedata[262].mpCost = 120;
recipedata[262].successRate = 100;

// recipe id 263
strcpy( recipedata[263].name, "mk_brigandine_gaiters\0" );
recipedata[263].itemID = 2378;
recipedata[263].rec_itemID = 2972;
recipedata[263].level = 3;
recipedata[263].resultCount = 1;
recipedata[263].mpCost = 120;
recipedata[263].successRate = 100;

// recipe id 264
strcpy( recipedata[264].name, "mk_elven_mithril_tunic\0" );
recipedata[264].itemID = 2396;
recipedata[264].rec_itemID = 2973;
recipedata[264].level = 3;
recipedata[264].resultCount = 1;
recipedata[264].mpCost = 120;
recipedata[264].successRate = 100;

// recipe id 265
strcpy( recipedata[265].name, "mk_elven_mithril_hose\0" );
recipedata[265].itemID = 2401;
recipedata[265].rec_itemID = 2974;
recipedata[265].level = 3;
recipedata[265].resultCount = 1;
recipedata[265].mpCost = 120;
recipedata[265].successRate = 100;

// recipe id 266
strcpy( recipedata[266].name, "mk_brigandine_helmet\0" );
recipedata[266].itemID = 2411;
recipedata[266].rec_itemID = 2975;
recipedata[266].level = 3;
recipedata[266].resultCount = 1;
recipedata[266].mpCost = 120;
recipedata[266].successRate = 100;

// recipe id 267
strcpy( recipedata[267].name, "mk_manticor_skin_boots\0" );
recipedata[267].itemID = 2424;
recipedata[267].rec_itemID = 2976;
recipedata[267].level = 3;
recipedata[267].resultCount = 1;
recipedata[267].mpCost = 120;
recipedata[267].successRate = 100;

// recipe id 268
strcpy( recipedata[268].name, "mk_brigandine_boots\0" );
recipedata[268].itemID = 2425;
recipedata[268].rec_itemID = 2977;
recipedata[268].level = 3;
recipedata[268].resultCount = 1;
recipedata[268].mpCost = 120;
recipedata[268].successRate = 100;

// recipe id 269
strcpy( recipedata[269].name, "mk_elven_mithril_boots\0" );
recipedata[269].itemID = 2426;
recipedata[269].rec_itemID = 2978;
recipedata[269].level = 3;
recipedata[269].resultCount = 1;
recipedata[269].mpCost = 120;
recipedata[269].successRate = 100;

// recipe id 270
strcpy( recipedata[270].name, "mk_manticor_skin_gloves\0" );
recipedata[270].itemID = 2448;
recipedata[270].rec_itemID = 2979;
recipedata[270].level = 3;
recipedata[270].resultCount = 1;
recipedata[270].mpCost = 120;
recipedata[270].successRate = 100;

// recipe id 271
strcpy( recipedata[271].name, "mk_brigandine_gauntlet\0" );
recipedata[271].itemID = 2449;
recipedata[271].rec_itemID = 2980;
recipedata[271].level = 3;
recipedata[271].resultCount = 1;
recipedata[271].mpCost = 120;
recipedata[271].successRate = 100;

// recipe id 272
strcpy( recipedata[272].name, "mk_elven_mithril_gloves\0" );
recipedata[272].itemID = 2450;
recipedata[272].rec_itemID = 2981;
recipedata[272].level = 3;
recipedata[272].resultCount = 1;
recipedata[272].mpCost = 120;
recipedata[272].successRate = 100;

// recipe id 273
strcpy( recipedata[273].name, "mk_brigandine_shield\0" );
recipedata[273].itemID = 2493;
recipedata[273].rec_itemID = 2982;
recipedata[273].level = 3;
recipedata[273].resultCount = 1;
recipedata[273].mpCost = 120;
recipedata[273].successRate = 100;

// recipe id 274
strcpy( recipedata[274].name, "mk_plate_helmet\0" );
recipedata[274].itemID = 2412;
recipedata[274].rec_itemID = 2983;
recipedata[274].level = 4;
recipedata[274].resultCount = 1;
recipedata[274].mpCost = 129;
recipedata[274].successRate = 100;

// recipe id 275
strcpy( recipedata[275].name, "mk_slamander_skin_boots\0" );
recipedata[275].itemID = 2427;
recipedata[275].rec_itemID = 2984;
recipedata[275].level = 4;
recipedata[275].resultCount = 1;
recipedata[275].mpCost = 129;
recipedata[275].successRate = 100;

// recipe id 276
strcpy( recipedata[276].name, "mk_plate_boots\0" );
recipedata[276].itemID = 2428;
recipedata[276].rec_itemID = 2985;
recipedata[276].level = 4;
recipedata[276].resultCount = 1;
recipedata[276].mpCost = 129;
recipedata[276].successRate = 100;

// recipe id 277
strcpy( recipedata[277].name, "mk_sage's_worn_gloves\0" );
recipedata[277].itemID = 2451;
recipedata[277].rec_itemID = 2986;
recipedata[277].level = 4;
recipedata[277].resultCount = 1;
recipedata[277].mpCost = 129;
recipedata[277].successRate = 100;

// recipe id 278
strcpy( recipedata[278].name, "mk_plate_shield\0" );
recipedata[278].itemID = 2494;
recipedata[278].rec_itemID = 2987;
recipedata[278].level = 4;
recipedata[278].resultCount = 1;
recipedata[278].mpCost = 129;
recipedata[278].successRate = 100;

// recipe id 279
strcpy( recipedata[279].name, "mk_tempered_mithril_gaiters\0" );
recipedata[279].itemID = 2387;
recipedata[279].rec_itemID = 2988;
recipedata[279].level = 4;
recipedata[279].resultCount = 1;
recipedata[279].mpCost = 138;
recipedata[279].successRate = 100;

// recipe id 280
strcpy( recipedata[280].name, "mk_chain_hood\0" );
recipedata[280].itemID = 2413;
recipedata[280].rec_itemID = 2989;
recipedata[280].level = 4;
recipedata[280].resultCount = 1;
recipedata[280].mpCost = 138;
recipedata[280].successRate = 100;

// recipe id 281
strcpy( recipedata[281].name, "mk_chain_boots\0" );
recipedata[281].itemID = 2429;
recipedata[281].rec_itemID = 2990;
recipedata[281].level = 4;
recipedata[281].resultCount = 1;
recipedata[281].mpCost = 138;
recipedata[281].successRate = 100;

// recipe id 282
strcpy( recipedata[282].name, "mk_karmian_boots\0" );
recipedata[282].itemID = 2430;
recipedata[282].rec_itemID = 2991;
recipedata[282].level = 4;
recipedata[282].resultCount = 1;
recipedata[282].mpCost = 138;
recipedata[282].successRate = 100;

// recipe id 283
strcpy( recipedata[283].name, "mk_plate_leather_boots\0" );
recipedata[283].itemID = 2431;
recipedata[283].rec_itemID = 2992;
recipedata[283].level = 4;
recipedata[283].resultCount = 1;
recipedata[283].mpCost = 138;
recipedata[283].successRate = 100;

// recipe id 284
strcpy( recipedata[284].name, "mk_dwarven_chain_boots\0" );
recipedata[284].itemID = 2432;
recipedata[284].rec_itemID = 2993;
recipedata[284].level = 4;
recipedata[284].resultCount = 1;
recipedata[284].mpCost = 138;
recipedata[284].successRate = 100;

// recipe id 285
strcpy( recipedata[285].name, "mk_boots_of_seal\0" );
recipedata[285].itemID = 2433;
recipedata[285].rec_itemID = 2994;
recipedata[285].level = 4;
recipedata[285].resultCount = 1;
recipedata[285].mpCost = 138;
recipedata[285].successRate = 100;

// recipe id 286
strcpy( recipedata[286].name, "mk_reinforce_mithril_gloves\0" );
recipedata[286].itemID = 2452;
recipedata[286].rec_itemID = 2995;
recipedata[286].level = 4;
recipedata[286].resultCount = 1;
recipedata[286].mpCost = 138;
recipedata[286].successRate = 100;

// recipe id 287
strcpy( recipedata[287].name, "mk_chain_gloves\0" );
recipedata[287].itemID = 2453;
recipedata[287].rec_itemID = 2996;
recipedata[287].level = 4;
recipedata[287].resultCount = 1;
recipedata[287].mpCost = 138;
recipedata[287].successRate = 100;

// recipe id 288
strcpy( recipedata[288].name, "mk_karmian_gloves\0" );
recipedata[288].itemID = 2454;
recipedata[288].rec_itemID = 2997;
recipedata[288].level = 4;
recipedata[288].resultCount = 1;
recipedata[288].mpCost = 138;
recipedata[288].successRate = 100;

// recipe id 289
strcpy( recipedata[289].name, "mk_chain_shield\0" );
recipedata[289].itemID = 2495;
recipedata[289].rec_itemID = 2998;
recipedata[289].level = 4;
recipedata[289].resultCount = 1;
recipedata[289].mpCost = 138;
recipedata[289].successRate = 100;

// recipe id 290
strcpy( recipedata[290].name, "mk_plate_leather_gloves\0" );
recipedata[290].itemID = 2455;
recipedata[290].rec_itemID = 2999;
recipedata[290].level = 5;
recipedata[290].resultCount = 1;
recipedata[290].mpCost = 144;
recipedata[290].successRate = 100;

// recipe id 291
strcpy( recipedata[291].name, "mk_dwarven_chain_shield\0" );
recipedata[291].itemID = 2496;
recipedata[291].rec_itemID = 3000;
recipedata[291].level = 5;
recipedata[291].resultCount = 1;
recipedata[291].mpCost = 144;
recipedata[291].successRate = 100;

// recipe id 292
strcpy( recipedata[292].name, "mk_rind_leather_boots\0" );
recipedata[292].itemID = 2434;
recipedata[292].rec_itemID = 3001;
recipedata[292].level = 5;
recipedata[292].resultCount = 1;
recipedata[292].mpCost = 147;
recipedata[292].successRate = 100;

// recipe id 293
strcpy( recipedata[293].name, "mk_dwarven_chain_gloves\0" );
recipedata[293].itemID = 2456;
recipedata[293].rec_itemID = 3002;
recipedata[293].level = 5;
recipedata[293].resultCount = 1;
recipedata[293].mpCost = 147;
recipedata[293].successRate = 100;

// recipe id 294
strcpy( recipedata[294].name, "mk_gloves_of_seal\0" );
recipedata[294].itemID = 2457;
recipedata[294].rec_itemID = 3003;
recipedata[294].level = 5;
recipedata[294].resultCount = 1;
recipedata[294].mpCost = 147;
recipedata[294].successRate = 100;

// recipe id 295
strcpy( recipedata[295].name, "mk_rind_leather_gloves\0" );
recipedata[295].itemID = 2458;
recipedata[295].rec_itemID = 3004;
recipedata[295].level = 5;
recipedata[295].resultCount = 1;
recipedata[295].mpCost = 147;
recipedata[295].successRate = 100;

// recipe id 296
strcpy( recipedata[296].name, "mk_demon's_boots\0" );
recipedata[296].itemID = 2435;
recipedata[296].rec_itemID = 3005;
recipedata[296].level = 5;
recipedata[296].resultCount = 1;
recipedata[296].mpCost = 156;
recipedata[296].successRate = 100;

// recipe id 297
strcpy( recipedata[297].name, "mk_demon's_gloves\0" );
recipedata[297].itemID = 2459;
recipedata[297].rec_itemID = 3006;
recipedata[297].level = 5;
recipedata[297].resultCount = 1;
recipedata[297].mpCost = 156;
recipedata[297].successRate = 100;

// recipe id 298
strcpy( recipedata[298].name, "mk_theca_leather_boots\0" );
recipedata[298].itemID = 2436;
recipedata[298].rec_itemID = 3007;
recipedata[298].level = 5;
recipedata[298].resultCount = 1;
recipedata[298].mpCost = 159;
recipedata[298].successRate = 100;

// recipe id 299
strcpy( recipedata[299].name, "mk_theca_leather_gloves\0" );
recipedata[299].itemID = 2460;
recipedata[299].rec_itemID = 3008;
recipedata[299].level = 5;
recipedata[299].resultCount = 1;
recipedata[299].mpCost = 159;
recipedata[299].successRate = 100;

// recipe id 300
strcpy( recipedata[300].name, "mk_composite_boots\0" );
recipedata[300].itemID = 64;
recipedata[300].rec_itemID = 3009;
recipedata[300].level = 5;
recipedata[300].resultCount = 1;
recipedata[300].mpCost = 156;
recipedata[300].successRate = 100;

// recipe id 301
strcpy( recipedata[301].name, "mk_composite_helmet\0" );
recipedata[301].itemID = 517;
recipedata[301].rec_itemID = 3010;
recipedata[301].level = 5;
recipedata[301].resultCount = 1;
recipedata[301].mpCost = 156;
recipedata[301].successRate = 100;

// recipe id 302
strcpy( recipedata[302].name, "mk_divine_boots\0" );
recipedata[302].itemID = 603;
recipedata[302].rec_itemID = 3011;
recipedata[302].level = 6;
recipedata[302].resultCount = 1;
recipedata[302].mpCost = 165;
recipedata[302].successRate = 100;

// recipe id 303
strcpy( recipedata[303].name, "mk_full_plate_helmet\0" );
recipedata[303].itemID = 2414;
recipedata[303].rec_itemID = 3012;
recipedata[303].level = 6;
recipedata[303].resultCount = 1;
recipedata[303].mpCost = 165;
recipedata[303].successRate = 100;

// recipe id 304
strcpy( recipedata[304].name, "mk_drake_leather_boots\0" );
recipedata[304].itemID = 2437;
recipedata[304].rec_itemID = 3013;
recipedata[304].level = 6;
recipedata[304].resultCount = 1;
recipedata[304].mpCost = 165;
recipedata[304].successRate = 100;

// recipe id 305
strcpy( recipedata[305].name, "mk_full_plate_boots\0" );
recipedata[305].itemID = 2438;
recipedata[305].rec_itemID = 3014;
recipedata[305].level = 6;
recipedata[305].resultCount = 1;
recipedata[305].mpCost = 165;
recipedata[305].successRate = 100;

// recipe id 306
strcpy( recipedata[306].name, "mk_drake_leather_gloves\0" );
recipedata[306].itemID = 2461;
recipedata[306].rec_itemID = 3015;
recipedata[306].level = 6;
recipedata[306].resultCount = 1;
recipedata[306].mpCost = 165;
recipedata[306].successRate = 100;

// recipe id 307
strcpy( recipedata[307].name, "mk_full_plate_gauntlet\0" );
recipedata[307].itemID = 2462;
recipedata[307].rec_itemID = 3016;
recipedata[307].level = 6;
recipedata[307].resultCount = 1;
recipedata[307].mpCost = 165;
recipedata[307].successRate = 100;

// recipe id 308
strcpy( recipedata[308].name, "mk_blessed_gloves\0" );
recipedata[308].itemID = 2463;
recipedata[308].rec_itemID = 3017;
recipedata[308].level = 6;
recipedata[308].resultCount = 1;
recipedata[308].mpCost = 165;
recipedata[308].successRate = 100;

// recipe id 309
strcpy( recipedata[309].name, "mk_divine_gloves\0" );
recipedata[309].itemID = 2468;
recipedata[309].rec_itemID = 3018;
recipedata[309].level = 6;
recipedata[309].resultCount = 1;
recipedata[309].mpCost = 165;
recipedata[309].successRate = 100;

// recipe id 310
strcpy( recipedata[310].name, "mk_full_plate_shield\0" );
recipedata[310].itemID = 2497;
recipedata[310].rec_itemID = 3019;
recipedata[310].level = 6;
recipedata[310].resultCount = 1;
recipedata[310].mpCost = 165;
recipedata[310].successRate = 100;

// recipe id 311
strcpy( recipedata[311].name, "mk_elven_long_sword\0" );
recipedata[311].itemID = 2499;
recipedata[311].rec_itemID = 3020;
recipedata[311].level = 4;
recipedata[311].resultCount = 1;
recipedata[311].mpCost = 129;
recipedata[311].successRate = 100;

// recipe id 312
strcpy( recipedata[312].name, "mk_dwarven_warhammer\0" );
recipedata[312].itemID = 2502;
recipedata[312].rec_itemID = 3021;
recipedata[312].level = 5;
recipedata[312].resultCount = 1;
recipedata[312].mpCost = 147;
recipedata[312].successRate = 100;

// recipe id 313
strcpy( recipedata[313].name, "mk_yaksa_mace\0" );
recipedata[313].itemID = 2503;
recipedata[313].rec_itemID = 3022;
recipedata[313].level = 6;
recipedata[313].resultCount = 1;
recipedata[313].mpCost = 165;
recipedata[313].successRate = 100;

// recipe id 314
strcpy( recipedata[314].name, "mk_titan_key\0" );
recipedata[314].itemID = 3030;
recipedata[314].rec_itemID = 3023;
recipedata[314].level = 1;
recipedata[314].resultCount = 1;
recipedata[314].mpCost = 100;
recipedata[314].successRate = 100;

// recipe id 315
strcpy( recipedata[315].name, "mk_journeyman_ring\0" );
recipedata[315].itemID = 3139;
recipedata[315].rec_itemID = 3024;
recipedata[315].level = 1;
recipedata[315].resultCount = 1;
recipedata[315].mpCost = 100;
recipedata[315].successRate = 100;

// recipe id 316
strcpy( recipedata[316].name, "mk_amber_bead\0" );
recipedata[316].itemID = 3136;
recipedata[316].rec_itemID = 3025;
recipedata[316].level = 2;
recipedata[316].resultCount = 5;
recipedata[316].mpCost = 100;
recipedata[316].successRate = 100;

// recipe id 317
strcpy( recipedata[317].name, "mk_spiritshot_d\0" );
recipedata[317].itemID = 2510;
recipedata[317].rec_itemID = 3032;
recipedata[317].level = 2;
recipedata[317].resultCount = 100;
recipedata[317].mpCost = 60;
recipedata[317].successRate = 100;

// recipe id 318
strcpy( recipedata[318].name, "mk_spiritshot_c\0" );
recipedata[318].itemID = 2511;
recipedata[318].rec_itemID = 3033;
recipedata[318].level = 4;
recipedata[318].resultCount = 200;
recipedata[318].mpCost = 120;
recipedata[318].successRate = 100;

// recipe id 319
strcpy( recipedata[319].name, "mk_spiritshot_b\0" );
recipedata[319].itemID = 2512;
recipedata[319].rec_itemID = 3034;
recipedata[319].level = 6;
recipedata[319].resultCount = 150;
recipedata[319].mpCost = 180;
recipedata[319].successRate = 100;

// recipe id 320
strcpy( recipedata[320].name, "mk_spiritshot_a\0" );
recipedata[320].itemID = 2513;
recipedata[320].rec_itemID = 3035;
recipedata[320].level = 7;
recipedata[320].resultCount = 200;
recipedata[320].mpCost = 240;
recipedata[320].successRate = 100;

// recipe id 321
strcpy( recipedata[321].name, "mk_spiritshot_s\0" );
recipedata[321].itemID = 2514;
recipedata[321].rec_itemID = 3036;
recipedata[321].level = 8;
recipedata[321].resultCount = 200;
recipedata[321].mpCost = 300;
recipedata[321].successRate = 100;

// recipe id 322
strcpy( recipedata[322].name, "q_mk_titans_powerstone\0" );
recipedata[322].itemID = 3846;
recipedata[322].rec_itemID = 3838;
recipedata[322].level = 2;
recipedata[322].resultCount = 1;
recipedata[322].mpCost = 100;
recipedata[322].successRate = 25;

// recipe id 323
strcpy( recipedata[323].name, "mk_blessed_spiritshot_d\0" );
recipedata[323].itemID = 3948;
recipedata[323].rec_itemID = 3953;
recipedata[323].level = 2;
recipedata[323].resultCount = 100;
recipedata[323].mpCost = 60;
recipedata[323].successRate = 100;

// recipe id 324
strcpy( recipedata[324].name, "mk_blessed_spiritshot_c\0" );
recipedata[324].itemID = 3949;
recipedata[324].rec_itemID = 3954;
recipedata[324].level = 4;
recipedata[324].resultCount = 200;
recipedata[324].mpCost = 120;
recipedata[324].successRate = 100;

// recipe id 325
strcpy( recipedata[325].name, "mk_blessed_spiritshot_b\0" );
recipedata[325].itemID = 3950;
recipedata[325].rec_itemID = 3955;
recipedata[325].level = 6;
recipedata[325].resultCount = 100;
recipedata[325].mpCost = 180;
recipedata[325].successRate = 100;

// recipe id 326
strcpy( recipedata[326].name, "mk_blessed_spiritshot_a\0" );
recipedata[326].itemID = 3951;
recipedata[326].rec_itemID = 3956;
recipedata[326].level = 7;
recipedata[326].resultCount = 200;
recipedata[326].mpCost = 240;
recipedata[326].successRate = 100;

// recipe id 327
strcpy( recipedata[327].name, "mk_blessed_spiritshot_s\0" );
recipedata[327].itemID = 3952;
recipedata[327].rec_itemID = 3957;
recipedata[327].level = 8;
recipedata[327].resultCount = 200;
recipedata[327].mpCost = 300;
recipedata[327].successRate = 100;

// recipe id 328
strcpy( recipedata[328].name, "mk_adamantite_earing_i\0" );
recipedata[328].itemID = 856;
recipedata[328].rec_itemID = 4937;
recipedata[328].level = 6;
recipedata[328].resultCount = 1;
recipedata[328].mpCost = 174;
recipedata[328].successRate = 70;

// recipe id 329
strcpy( recipedata[329].name, "mk_adamantite_earing_ii\0" );
recipedata[329].itemID = 856;
recipedata[329].rec_itemID = 4126;
recipedata[329].level = 6;
recipedata[329].resultCount = 1;
recipedata[329].mpCost = 174;
recipedata[329].successRate = 100;

// recipe id 330
strcpy( recipedata[330].name, "mk_adamantite_ring_i\0" );
recipedata[330].itemID = 887;
recipedata[330].rec_itemID = 4938;
recipedata[330].level = 6;
recipedata[330].resultCount = 1;
recipedata[330].mpCost = 174;
recipedata[330].successRate = 70;

// recipe id 331
strcpy( recipedata[331].name, "mk_adamantite_ring_ii\0" );
recipedata[331].itemID = 887;
recipedata[331].rec_itemID = 4127;
recipedata[331].level = 6;
recipedata[331].resultCount = 1;
recipedata[331].mpCost = 174;
recipedata[331].successRate = 100;

// recipe id 332
strcpy( recipedata[332].name, "mk_adamantite_necklace_i\0" );
recipedata[332].itemID = 918;
recipedata[332].rec_itemID = 4939;
recipedata[332].level = 6;
recipedata[332].resultCount = 1;
recipedata[332].mpCost = 174;
recipedata[332].successRate = 70;

// recipe id 333
strcpy( recipedata[333].name, "mk_adamantite_necklace_ii\0" );
recipedata[333].itemID = 918;
recipedata[333].rec_itemID = 4128;
recipedata[333].level = 6;
recipedata[333].resultCount = 1;
recipedata[333].mpCost = 174;
recipedata[333].successRate = 100;

// recipe id 334
strcpy( recipedata[334].name, "mk_earing_of_black_ore_i\0" );
recipedata[334].itemID = 864;
recipedata[334].rec_itemID = 4973;
recipedata[334].level = 7;
recipedata[334].resultCount = 1;
recipedata[334].mpCost = 183;
recipedata[334].successRate = 70;

// recipe id 335
strcpy( recipedata[335].name, "mk_earing_of_black_ore_ii\0" );
recipedata[335].itemID = 864;
recipedata[335].rec_itemID = 4129;
recipedata[335].level = 7;
recipedata[335].resultCount = 1;
recipedata[335].mpCost = 183;
recipedata[335].successRate = 100;

// recipe id 336
strcpy( recipedata[336].name, "mk_ring_of_black_ore_i\0" );
recipedata[336].itemID = 895;
recipedata[336].rec_itemID = 4974;
recipedata[336].level = 7;
recipedata[336].resultCount = 1;
recipedata[336].mpCost = 183;
recipedata[336].successRate = 70;

// recipe id 337
strcpy( recipedata[337].name, "mk_ring_of_black_ore_ii\0" );
recipedata[337].itemID = 895;
recipedata[337].rec_itemID = 4130;
recipedata[337].level = 7;
recipedata[337].resultCount = 1;
recipedata[337].mpCost = 183;
recipedata[337].successRate = 100;

// recipe id 338
strcpy( recipedata[338].name, "mk_necklace_of_black_ore_i\0" );
recipedata[338].itemID = 926;
recipedata[338].rec_itemID = 4975;
recipedata[338].level = 7;
recipedata[338].resultCount = 1;
recipedata[338].mpCost = 183;
recipedata[338].successRate = 70;

// recipe id 339
strcpy( recipedata[339].name, "mk_necklace_of_black_ore_ii\0" );
recipedata[339].itemID = 926;
recipedata[339].rec_itemID = 4131;
recipedata[339].level = 7;
recipedata[339].resultCount = 1;
recipedata[339].mpCost = 183;
recipedata[339].successRate = 100;

// recipe id 340
strcpy( recipedata[340].name, "mk_composite_shield\0" );
recipedata[340].itemID = 107;
recipedata[340].rec_itemID = 4132;
recipedata[340].level = 5;
recipedata[340].resultCount = 1;
recipedata[340].mpCost = 156;
recipedata[340].successRate = 100;

// recipe id 341
strcpy( recipedata[341].name, "mk_shrnoen's_breastplate_i\0" );
recipedata[341].itemID = 357;
recipedata[341].rec_itemID = 4940;
recipedata[341].level = 6;
recipedata[341].resultCount = 1;
recipedata[341].mpCost = 174;
recipedata[341].successRate = 60;

// recipe id 342
strcpy( recipedata[342].name, "mk_shrnoen's_breastplate_ii\0" );
recipedata[342].itemID = 357;
recipedata[342].rec_itemID = 4133;
recipedata[342].level = 6;
recipedata[342].resultCount = 1;
recipedata[342].mpCost = 174;
recipedata[342].successRate = 100;

// recipe id 343
strcpy( recipedata[343].name, "mk_shrnoen's_gaiters_i\0" );
recipedata[343].itemID = 383;
recipedata[343].rec_itemID = 4941;
recipedata[343].level = 6;
recipedata[343].resultCount = 1;
recipedata[343].mpCost = 174;
recipedata[343].successRate = 60;

// recipe id 344
strcpy( recipedata[344].name, "mk_shrnoen's_gaiters_ii\0" );
recipedata[344].itemID = 383;
recipedata[344].rec_itemID = 4134;
recipedata[344].level = 6;
recipedata[344].resultCount = 1;
recipedata[344].mpCost = 174;
recipedata[344].successRate = 100;

// recipe id 345
strcpy( recipedata[345].name, "mk_implosion_boots_i\0" );
recipedata[345].itemID = 564;
recipedata[345].rec_itemID = 4976;
recipedata[345].level = 7;
recipedata[345].resultCount = 1;
recipedata[345].mpCost = 183;
recipedata[345].successRate = 60;

// recipe id 346
strcpy( recipedata[346].name, "mk_implosion_boots_ii\0" );
recipedata[346].itemID = 564;
recipedata[346].rec_itemID = 4135;
recipedata[346].level = 7;
recipedata[346].resultCount = 1;
recipedata[346].mpCost = 183;
recipedata[346].successRate = 100;

// recipe id 347
strcpy( recipedata[347].name, "mk_silent_boots_i\0" );
recipedata[347].itemID = 572;
recipedata[347].rec_itemID = 4942;
recipedata[347].level = 6;
recipedata[347].resultCount = 1;
recipedata[347].mpCost = 174;
recipedata[347].successRate = 60;

// recipe id 348
strcpy( recipedata[348].name, "mk_silent_boots_ii\0" );
recipedata[348].itemID = 572;
recipedata[348].rec_itemID = 4136;
recipedata[348].level = 6;
recipedata[348].resultCount = 1;
recipedata[348].mpCost = 174;
recipedata[348].successRate = 100;

// recipe id 349
strcpy( recipedata[349].name, "mk_guardian's_boots_i\0" );
recipedata[349].itemID = 578;
recipedata[349].rec_itemID = 4943;
recipedata[349].level = 6;
recipedata[349].resultCount = 1;
recipedata[349].mpCost = 174;
recipedata[349].successRate = 60;

// recipe id 350
strcpy( recipedata[350].name, "mk_guardian's_boots_ii\0" );
recipedata[350].itemID = 578;
recipedata[350].rec_itemID = 4137;
recipedata[350].level = 6;
recipedata[350].resultCount = 1;
recipedata[350].mpCost = 174;
recipedata[350].successRate = 100;

// recipe id 351
strcpy( recipedata[351].name, "mk_paradia_boots_i\0" );
recipedata[351].itemID = 582;
recipedata[351].rec_itemID = 4977;
recipedata[351].level = 7;
recipedata[351].resultCount = 1;
recipedata[351].mpCost = 183;
recipedata[351].successRate = 60;

// recipe id 352
strcpy( recipedata[352].name, "mk_paradia_boots_ii\0" );
recipedata[352].itemID = 582;
recipedata[352].rec_itemID = 4138;
recipedata[352].level = 7;
recipedata[352].resultCount = 1;
recipedata[352].mpCost = 183;
recipedata[352].successRate = 100;

// recipe id 353
strcpy( recipedata[353].name, "mk_elemental_boots_i\0" );
recipedata[353].itemID = 588;
recipedata[353].rec_itemID = 4978;
recipedata[353].level = 7;
recipedata[353].resultCount = 1;
recipedata[353].mpCost = 183;
recipedata[353].successRate = 60;

// recipe id 354
strcpy( recipedata[354].name, "mk_elemental_boots_ii\0" );
recipedata[354].itemID = 588;
recipedata[354].rec_itemID = 4139;
recipedata[354].level = 7;
recipedata[354].resultCount = 1;
recipedata[354].mpCost = 183;
recipedata[354].successRate = 100;

// recipe id 355
strcpy( recipedata[355].name, "mk_boots_of_grace_i\0" );
recipedata[355].itemID = 590;
recipedata[355].rec_itemID = 4979;
recipedata[355].level = 7;
recipedata[355].resultCount = 1;
recipedata[355].mpCost = 183;
recipedata[355].successRate = 60;

// recipe id 356
strcpy( recipedata[356].name, "mk_boots_of_grace_ii\0" );
recipedata[356].itemID = 590;
recipedata[356].rec_itemID = 4140;
recipedata[356].level = 7;
recipedata[356].resultCount = 1;
recipedata[356].mpCost = 183;
recipedata[356].successRate = 100;

// recipe id 357
strcpy( recipedata[357].name, "mk_avadon_breastplate_i\0" );
recipedata[357].itemID = 2376;
recipedata[357].rec_itemID = 4944;
recipedata[357].level = 6;
recipedata[357].resultCount = 1;
recipedata[357].mpCost = 174;
recipedata[357].successRate = 60;

// recipe id 358
strcpy( recipedata[358].name, "mk_avadon_breastplate_ii\0" );
recipedata[358].itemID = 2376;
recipedata[358].rec_itemID = 4141;
recipedata[358].level = 6;
recipedata[358].resultCount = 1;
recipedata[358].mpCost = 174;
recipedata[358].successRate = 100;

// recipe id 359
strcpy( recipedata[359].name, "mk_avadon_gaiters_i\0" );
recipedata[359].itemID = 2379;
recipedata[359].rec_itemID = 4945;
recipedata[359].level = 6;
recipedata[359].resultCount = 1;
recipedata[359].mpCost = 174;
recipedata[359].successRate = 60;

// recipe id 360
strcpy( recipedata[360].name, "mk_avadon_gaiters_ii\0" );
recipedata[360].itemID = 2379;
recipedata[360].rec_itemID = 4142;
recipedata[360].level = 6;
recipedata[360].resultCount = 1;
recipedata[360].mpCost = 174;
recipedata[360].successRate = 100;

// recipe id 361
strcpy( recipedata[361].name, "mk_shrnoen's_leather_shirts_i\0" );
recipedata[361].itemID = 2384;
recipedata[361].rec_itemID = 4946;
recipedata[361].level = 6;
recipedata[361].resultCount = 1;
recipedata[361].mpCost = 174;
recipedata[361].successRate = 60;

// recipe id 362
strcpy( recipedata[362].name, "mk_shrnoen's_leather_shirts_ii\0" );
recipedata[362].itemID = 2384;
recipedata[362].rec_itemID = 4143;
recipedata[362].level = 6;
recipedata[362].resultCount = 1;
recipedata[362].mpCost = 174;
recipedata[362].successRate = 100;

// recipe id 363
strcpy( recipedata[363].name, "mk_shrnoen's_leather_gaiters_i\0" );
recipedata[363].itemID = 2388;
recipedata[363].rec_itemID = 4947;
recipedata[363].level = 6;
recipedata[363].resultCount = 1;
recipedata[363].mpCost = 174;
recipedata[363].successRate = 60;

// recipe id 364
strcpy( recipedata[364].name, "mk_shrnoen's_leather_gaiters_ii\0" );
recipedata[364].itemID = 2388;
recipedata[364].rec_itemID = 4144;
recipedata[364].level = 6;
recipedata[364].resultCount = 1;
recipedata[364].mpCost = 174;
recipedata[364].successRate = 100;

// recipe id 365
strcpy( recipedata[365].name, "mk_avadon_leather_mail_i\0" );
recipedata[365].itemID = 2390;
recipedata[365].rec_itemID = 4948;
recipedata[365].level = 6;
recipedata[365].resultCount = 1;
recipedata[365].mpCost = 174;
recipedata[365].successRate = 60;

// recipe id 366
strcpy( recipedata[366].name, "mk_avadon_leather_mail_ii\0" );
recipedata[366].itemID = 2390;
recipedata[366].rec_itemID = 4145;
recipedata[366].level = 6;
recipedata[366].resultCount = 1;
recipedata[366].mpCost = 174;
recipedata[366].successRate = 100;

// recipe id 367
strcpy( recipedata[367].name, "mk_tunic_of_shrnoen_i\0" );
recipedata[367].itemID = 2397;
recipedata[367].rec_itemID = 4949;
recipedata[367].level = 6;
recipedata[367].resultCount = 1;
recipedata[367].mpCost = 174;
recipedata[367].successRate = 60;

// recipe id 368
strcpy( recipedata[368].name, "mk_tunic_of_shrnoen_ii\0" );
recipedata[368].itemID = 2397;
recipedata[368].rec_itemID = 4146;
recipedata[368].level = 6;
recipedata[368].resultCount = 1;
recipedata[368].mpCost = 174;
recipedata[368].successRate = 100;

// recipe id 369
strcpy( recipedata[369].name, "mk_hose_of_shrnoen_i\0" );
recipedata[369].itemID = 2402;
recipedata[369].rec_itemID = 4950;
recipedata[369].level = 6;
recipedata[369].resultCount = 1;
recipedata[369].mpCost = 174;
recipedata[369].successRate = 60;

// recipe id 370
strcpy( recipedata[370].name, "mk_hose_of_shrnoen_ii\0" );
recipedata[370].itemID = 2402;
recipedata[370].rec_itemID = 4147;
recipedata[370].level = 6;
recipedata[370].resultCount = 1;
recipedata[370].mpCost = 174;
recipedata[370].successRate = 100;

// recipe id 371
strcpy( recipedata[371].name, "mk_avadon_robe_i\0" );
recipedata[371].itemID = 2406;
recipedata[371].rec_itemID = 4951;
recipedata[371].level = 6;
recipedata[371].resultCount = 1;
recipedata[371].mpCost = 174;
recipedata[371].successRate = 60;

// recipe id 372
strcpy( recipedata[372].name, "mk_avadon_robe_ii\0" );
recipedata[372].itemID = 2406;
recipedata[372].rec_itemID = 4148;
recipedata[372].level = 6;
recipedata[372].resultCount = 1;
recipedata[372].mpCost = 174;
recipedata[372].successRate = 100;

// recipe id 373
strcpy( recipedata[373].name, "mk_avadon_circlet_i\0" );
recipedata[373].itemID = 2415;
recipedata[373].rec_itemID = 4952;
recipedata[373].level = 6;
recipedata[373].resultCount = 1;
recipedata[373].mpCost = 174;
recipedata[373].successRate = 60;

// recipe id 374
strcpy( recipedata[374].name, "mk_avadon_circlet_ii\0" );
recipedata[374].itemID = 2415;
recipedata[374].rec_itemID = 4149;
recipedata[374].level = 6;
recipedata[374].resultCount = 1;
recipedata[374].mpCost = 174;
recipedata[374].successRate = 100;

// recipe id 375
strcpy( recipedata[375].name, "mk_avadon_gloves_i\0" );
recipedata[375].itemID = 2464;
recipedata[375].rec_itemID = 4953;
recipedata[375].level = 6;
recipedata[375].resultCount = 1;
recipedata[375].mpCost = 174;
recipedata[375].successRate = 60;

// recipe id 376
strcpy( recipedata[376].name, "mk_avadon_gloves_ii\0" );
recipedata[376].itemID = 2464;
recipedata[376].rec_itemID = 4150;
recipedata[376].level = 6;
recipedata[376].resultCount = 1;
recipedata[376].mpCost = 174;
recipedata[376].successRate = 100;

// recipe id 377
strcpy( recipedata[377].name, "mk_silent_chain_gloves_i\0" );
recipedata[377].itemID = 2465;
recipedata[377].rec_itemID = 4954;
recipedata[377].level = 6;
recipedata[377].resultCount = 1;
recipedata[377].mpCost = 174;
recipedata[377].successRate = 60;

// recipe id 378
strcpy( recipedata[378].name, "mk_silent_chain_gloves_ii\0" );
recipedata[378].itemID = 2465;
recipedata[378].rec_itemID = 4151;
recipedata[378].level = 6;
recipedata[378].resultCount = 1;
recipedata[378].mpCost = 174;
recipedata[378].successRate = 100;

// recipe id 379
strcpy( recipedata[379].name, "mk_guardian's_gloves_i\0" );
recipedata[379].itemID = 2466;
recipedata[379].rec_itemID = 4955;
recipedata[379].level = 6;
recipedata[379].resultCount = 1;
recipedata[379].mpCost = 174;
recipedata[379].successRate = 60;

// recipe id 380
strcpy( recipedata[380].name, "mk_guardian's_gloves_ii\0" );
recipedata[380].itemID = 2466;
recipedata[380].rec_itemID = 4152;
recipedata[380].level = 6;
recipedata[380].resultCount = 1;
recipedata[380].mpCost = 174;
recipedata[380].successRate = 100;

// recipe id 381
strcpy( recipedata[381].name, "mk_gloves_of_blessing_i\0" );
recipedata[381].itemID = 2467;
recipedata[381].rec_itemID = 4956;
recipedata[381].level = 6;
recipedata[381].resultCount = 1;
recipedata[381].mpCost = 174;
recipedata[381].successRate = 60;

// recipe id 382
strcpy( recipedata[382].name, "mk_gloves_of_blessing_ii\0" );
recipedata[382].itemID = 2467;
recipedata[382].rec_itemID = 4153;
recipedata[382].level = 6;
recipedata[382].resultCount = 1;
recipedata[382].mpCost = 174;
recipedata[382].successRate = 100;

// recipe id 383
strcpy( recipedata[383].name, "mk_doom_shield_i\0" );
recipedata[383].itemID = 110;
recipedata[383].rec_itemID = 4980;
recipedata[383].level = 7;
recipedata[383].resultCount = 1;
recipedata[383].mpCost = 183;
recipedata[383].successRate = 60;

// recipe id 384
strcpy( recipedata[384].name, "mk_doom_shield_ii\0" );
recipedata[384].itemID = 110;
recipedata[384].rec_itemID = 4154;
recipedata[384].level = 7;
recipedata[384].resultCount = 1;
recipedata[384].mpCost = 183;
recipedata[384].successRate = 100;

// recipe id 385
strcpy( recipedata[385].name, "mk_blue_wolve's_breastplate_i\0" );
recipedata[385].itemID = 358;
recipedata[385].rec_itemID = 4981;
recipedata[385].level = 7;
recipedata[385].resultCount = 1;
recipedata[385].mpCost = 183;
recipedata[385].successRate = 60;

// recipe id 386
strcpy( recipedata[386].name, "mk_blue_wolve's_breastplate_ii\0" );
recipedata[386].itemID = 358;
recipedata[386].rec_itemID = 4155;
recipedata[386].level = 7;
recipedata[386].resultCount = 1;
recipedata[386].mpCost = 183;
recipedata[386].successRate = 100;

// recipe id 387
strcpy( recipedata[387].name, "mk_boots_of_blessing_i\0" );
recipedata[387].itemID = 595;
recipedata[387].rec_itemID = 4957;
recipedata[387].level = 6;
recipedata[387].resultCount = 1;
recipedata[387].mpCost = 174;
recipedata[387].successRate = 60;

// recipe id 388
strcpy( recipedata[388].name, "mk_boots_of_blessing_ii\0" );
recipedata[388].itemID = 595;
recipedata[388].rec_itemID = 4156;
recipedata[388].level = 6;
recipedata[388].resultCount = 1;
recipedata[388].mpCost = 174;
recipedata[388].successRate = 100;

// recipe id 389
strcpy( recipedata[389].name, "mk_blue_wolve's_gaiters_i\0" );
recipedata[389].itemID = 2380;
recipedata[389].rec_itemID = 4982;
recipedata[389].level = 7;
recipedata[389].resultCount = 1;
recipedata[389].mpCost = 183;
recipedata[389].successRate = 60;

// recipe id 390
strcpy( recipedata[390].name, "mk_blue_wolve's_gaiters_ii\0" );
recipedata[390].itemID = 2380;
recipedata[390].rec_itemID = 4157;
recipedata[390].level = 7;
recipedata[390].resultCount = 1;
recipedata[390].mpCost = 183;
recipedata[390].successRate = 100;

// recipe id 391
strcpy( recipedata[391].name, "mk_doom_plate_armor_i\0" );
recipedata[391].itemID = 2381;
recipedata[391].rec_itemID = 4983;
recipedata[391].level = 7;
recipedata[391].resultCount = 1;
recipedata[391].mpCost = 183;
recipedata[391].successRate = 60;

// recipe id 392
strcpy( recipedata[392].name, "mk_doom_plate_armor_ii\0" );
recipedata[392].itemID = 2381;
recipedata[392].rec_itemID = 4158;
recipedata[392].level = 7;
recipedata[392].resultCount = 1;
recipedata[392].mpCost = 183;
recipedata[392].successRate = 100;

// recipe id 393
strcpy( recipedata[393].name, "mk_blue_wolve's_leather_mail_i\0" );
recipedata[393].itemID = 2391;
recipedata[393].rec_itemID = 4984;
recipedata[393].level = 7;
recipedata[393].resultCount = 1;
recipedata[393].mpCost = 183;
recipedata[393].successRate = 60;

// recipe id 394
strcpy( recipedata[394].name, "mk_blue_wolve's_leather_mail_ii\0" );
recipedata[394].itemID = 2391;
recipedata[394].rec_itemID = 4159;
recipedata[394].level = 7;
recipedata[394].resultCount = 1;
recipedata[394].mpCost = 183;
recipedata[394].successRate = 100;

// recipe id 395
strcpy( recipedata[395].name, "mk_leather_mail_of_doom_i\0" );
recipedata[395].itemID = 2392;
recipedata[395].rec_itemID = 4985;
recipedata[395].level = 7;
recipedata[395].resultCount = 1;
recipedata[395].mpCost = 183;
recipedata[395].successRate = 60;

// recipe id 396
strcpy( recipedata[396].name, "mk_leather_mail_of_doom_ii\0" );
recipedata[396].itemID = 2392;
recipedata[396].rec_itemID = 4160;
recipedata[396].level = 7;
recipedata[396].resultCount = 1;
recipedata[396].mpCost = 183;
recipedata[396].successRate = 100;

// recipe id 397
strcpy( recipedata[397].name, "mk_blue_wolve's_tunic_i\0" );
recipedata[397].itemID = 2398;
recipedata[397].rec_itemID = 4986;
recipedata[397].level = 7;
recipedata[397].resultCount = 1;
recipedata[397].mpCost = 183;
recipedata[397].successRate = 60;

// recipe id 398
strcpy( recipedata[398].name, "mk_blue_wolve's_tunic_ii\0" );
recipedata[398].itemID = 2398;
recipedata[398].rec_itemID = 4161;
recipedata[398].level = 7;
recipedata[398].resultCount = 1;
recipedata[398].mpCost = 183;
recipedata[398].successRate = 100;

// recipe id 399
strcpy( recipedata[399].name, "mk_tunic_of_doom_i\0" );
recipedata[399].itemID = 2399;
recipedata[399].rec_itemID = 4987;
recipedata[399].level = 7;
recipedata[399].resultCount = 1;
recipedata[399].mpCost = 183;
recipedata[399].successRate = 60;

// recipe id 400
strcpy( recipedata[400].name, "mk_tunic_of_doom_ii\0" );
recipedata[400].itemID = 2399;
recipedata[400].rec_itemID = 4162;
recipedata[400].level = 7;
recipedata[400].resultCount = 1;
recipedata[400].mpCost = 183;
recipedata[400].successRate = 100;

// recipe id 401
strcpy( recipedata[401].name, "mk_blue_wolve's_hose_i\0" );
recipedata[401].itemID = 2403;
recipedata[401].rec_itemID = 4988;
recipedata[401].level = 7;
recipedata[401].resultCount = 1;
recipedata[401].mpCost = 183;
recipedata[401].successRate = 60;

// recipe id 402
strcpy( recipedata[402].name, "mk_blue_wolve's_hose_ii\0" );
recipedata[402].itemID = 2403;
recipedata[402].rec_itemID = 4163;
recipedata[402].level = 7;
recipedata[402].resultCount = 1;
recipedata[402].mpCost = 183;
recipedata[402].successRate = 100;

// recipe id 403
strcpy( recipedata[403].name, "mk_hose_of_doom_i\0" );
recipedata[403].itemID = 2404;
recipedata[403].rec_itemID = 4989;
recipedata[403].level = 7;
recipedata[403].resultCount = 1;
recipedata[403].mpCost = 183;
recipedata[403].successRate = 60;

// recipe id 404
strcpy( recipedata[404].name, "mk_hose_of_doom_ii\0" );
recipedata[404].itemID = 2404;
recipedata[404].rec_itemID = 4164;
recipedata[404].level = 7;
recipedata[404].resultCount = 1;
recipedata[404].mpCost = 183;
recipedata[404].successRate = 100;

// recipe id 405
strcpy( recipedata[405].name, "mk_blue_wolve's_helmet_i\0" );
recipedata[405].itemID = 2416;
recipedata[405].rec_itemID = 4990;
recipedata[405].level = 7;
recipedata[405].resultCount = 1;
recipedata[405].mpCost = 183;
recipedata[405].successRate = 60;

// recipe id 406
strcpy( recipedata[406].name, "mk_blue_wolve's_helmet_ii\0" );
recipedata[406].itemID = 2416;
recipedata[406].rec_itemID = 4165;
recipedata[406].level = 7;
recipedata[406].resultCount = 1;
recipedata[406].mpCost = 183;
recipedata[406].successRate = 100;

// recipe id 407
strcpy( recipedata[407].name, "mk_doom_helmet_i\0" );
recipedata[407].itemID = 2417;
recipedata[407].rec_itemID = 4991;
recipedata[407].level = 7;
recipedata[407].resultCount = 1;
recipedata[407].mpCost = 183;
recipedata[407].successRate = 60;

// recipe id 408
strcpy( recipedata[408].name, "mk_doom_helmet_ii\0" );
recipedata[408].itemID = 2417;
recipedata[408].rec_itemID = 4166;
recipedata[408].level = 7;
recipedata[408].resultCount = 1;
recipedata[408].mpCost = 183;
recipedata[408].successRate = 100;

// recipe id 409
strcpy( recipedata[409].name, "mk_blue_wolve's_boots_i\0" );
recipedata[409].itemID = 2439;
recipedata[409].rec_itemID = 4992;
recipedata[409].level = 7;
recipedata[409].resultCount = 1;
recipedata[409].mpCost = 183;
recipedata[409].successRate = 60;

// recipe id 410
strcpy( recipedata[410].name, "mk_blue_wolve's_boots_ii\0" );
recipedata[410].itemID = 2439;
recipedata[410].rec_itemID = 4167;
recipedata[410].level = 7;
recipedata[410].resultCount = 1;
recipedata[410].mpCost = 183;
recipedata[410].successRate = 100;

// recipe id 411
strcpy( recipedata[411].name, "mk_doom_gloves_i\0" );
recipedata[411].itemID = 2475;
recipedata[411].rec_itemID = 4993;
recipedata[411].level = 7;
recipedata[411].resultCount = 1;
recipedata[411].mpCost = 183;
recipedata[411].successRate = 60;

// recipe id 412
strcpy( recipedata[412].name, "mk_doom_gloves_ii\0" );
recipedata[412].itemID = 2475;
recipedata[412].rec_itemID = 4168;
recipedata[412].level = 7;
recipedata[412].resultCount = 1;
recipedata[412].mpCost = 183;
recipedata[412].successRate = 100;

// recipe id 413
strcpy( recipedata[413].name, "mk_elemental_gloves_i\0" );
recipedata[413].itemID = 2480;
recipedata[413].rec_itemID = 4994;
recipedata[413].level = 7;
recipedata[413].resultCount = 1;
recipedata[413].mpCost = 183;
recipedata[413].successRate = 60;

// recipe id 414
strcpy( recipedata[414].name, "mk_elemental_gloves_ii\0" );
recipedata[414].itemID = 2480;
recipedata[414].rec_itemID = 4169;
recipedata[414].level = 7;
recipedata[414].resultCount = 1;
recipedata[414].mpCost = 183;
recipedata[414].successRate = 100;

// recipe id 415
strcpy( recipedata[415].name, "mk_gloves_of_grace_i\0" );
recipedata[415].itemID = 2481;
recipedata[415].rec_itemID = 4995;
recipedata[415].level = 7;
recipedata[415].resultCount = 1;
recipedata[415].mpCost = 183;
recipedata[415].successRate = 60;

// recipe id 416
strcpy( recipedata[416].name, "mk_gloves_of_grace_ii\0" );
recipedata[416].itemID = 2481;
recipedata[416].rec_itemID = 4170;
recipedata[416].level = 7;
recipedata[416].resultCount = 1;
recipedata[416].mpCost = 183;
recipedata[416].successRate = 100;

// recipe id 417
strcpy( recipedata[417].name, "mk_implosion_gauntlet_i\0" );
recipedata[417].itemID = 2485;
recipedata[417].rec_itemID = 4996;
recipedata[417].level = 7;
recipedata[417].resultCount = 1;
recipedata[417].mpCost = 183;
recipedata[417].successRate = 60;

// recipe id 418
strcpy( recipedata[418].name, "mk_implosion_gauntlet_ii\0" );
recipedata[418].itemID = 2485;
recipedata[418].rec_itemID = 4171;
recipedata[418].level = 7;
recipedata[418].resultCount = 1;
recipedata[418].mpCost = 183;
recipedata[418].successRate = 100;

// recipe id 419
strcpy( recipedata[419].name, "mk_paradia_gloves_i\0" );
recipedata[419].itemID = 2486;
recipedata[419].rec_itemID = 4997;
recipedata[419].level = 7;
recipedata[419].resultCount = 1;
recipedata[419].mpCost = 183;
recipedata[419].successRate = 60;

// recipe id 420
strcpy( recipedata[420].name, "mk_paradia_gloves_ii\0" );
recipedata[420].itemID = 2486;
recipedata[420].rec_itemID = 4172;
recipedata[420].level = 7;
recipedata[420].resultCount = 1;
recipedata[420].mpCost = 183;
recipedata[420].successRate = 100;

// recipe id 421
strcpy( recipedata[421].name, "mk_blue_wolve's_gloves_i\0" );
recipedata[421].itemID = 2487;
recipedata[421].rec_itemID = 4998;
recipedata[421].level = 7;
recipedata[421].resultCount = 1;
recipedata[421].mpCost = 183;
recipedata[421].successRate = 60;

// recipe id 422
strcpy( recipedata[422].name, "mk_blue_wolve's_gloves_ii\0" );
recipedata[422].itemID = 2487;
recipedata[422].rec_itemID = 4173;
recipedata[422].level = 7;
recipedata[422].resultCount = 1;
recipedata[422].mpCost = 183;
recipedata[422].successRate = 100;

// recipe id 423
strcpy( recipedata[423].name, "mk_shrnoen's_boots_i\0" );
recipedata[423].itemID = 554;
recipedata[423].rec_itemID = 4958;
recipedata[423].level = 6;
recipedata[423].resultCount = 1;
recipedata[423].mpCost = 174;
recipedata[423].successRate = 60;

// recipe id 424
strcpy( recipedata[424].name, "mk_shrnoen's_boots_ii\0" );
recipedata[424].itemID = 554;
recipedata[424].rec_itemID = 4174;
recipedata[424].level = 6;
recipedata[424].resultCount = 1;
recipedata[424].mpCost = 174;
recipedata[424].successRate = 100;

// recipe id 425
strcpy( recipedata[425].name, "mk_avadon_boots_i\0" );
recipedata[425].itemID = 600;
recipedata[425].rec_itemID = 4959;
recipedata[425].level = 6;
recipedata[425].resultCount = 1;
recipedata[425].mpCost = 174;
recipedata[425].successRate = 60;

// recipe id 426
strcpy( recipedata[426].name, "mk_avadon_boots_ii\0" );
recipedata[426].itemID = 600;
recipedata[426].rec_itemID = 4175;
recipedata[426].level = 6;
recipedata[426].resultCount = 1;
recipedata[426].mpCost = 174;
recipedata[426].successRate = 100;

// recipe id 427
strcpy( recipedata[427].name, "mk_doom_boots_i\0" );
recipedata[427].itemID = 601;
recipedata[427].rec_itemID = 4999;
recipedata[427].level = 7;
recipedata[427].resultCount = 1;
recipedata[427].mpCost = 183;
recipedata[427].successRate = 60;

// recipe id 428
strcpy( recipedata[428].name, "mk_doom_boots_ii\0" );
recipedata[428].itemID = 601;
recipedata[428].rec_itemID = 4176;
recipedata[428].level = 7;
recipedata[428].resultCount = 1;
recipedata[428].mpCost = 183;
recipedata[428].successRate = 100;

// recipe id 429
strcpy( recipedata[429].name, "mk_shrnoen's_gauntlet_i\0" );
recipedata[429].itemID = 612;
recipedata[429].rec_itemID = 4960;
recipedata[429].level = 6;
recipedata[429].resultCount = 1;
recipedata[429].mpCost = 174;
recipedata[429].successRate = 60;

// recipe id 430
strcpy( recipedata[430].name, "mk_shrnoen's_gauntlet_ii\0" );
recipedata[430].itemID = 612;
recipedata[430].rec_itemID = 4177;
recipedata[430].level = 6;
recipedata[430].resultCount = 1;
recipedata[430].mpCost = 174;
recipedata[430].successRate = 100;

// recipe id 431
strcpy( recipedata[431].name, "mk_shrnoen's_shield_i\0" );
recipedata[431].itemID = 633;
recipedata[431].rec_itemID = 4961;
recipedata[431].level = 6;
recipedata[431].resultCount = 1;
recipedata[431].mpCost = 174;
recipedata[431].successRate = 60;

// recipe id 432
strcpy( recipedata[432].name, "mk_shrnoen's_shield_ii\0" );
recipedata[432].itemID = 633;
recipedata[432].rec_itemID = 4178;
recipedata[432].level = 6;
recipedata[432].resultCount = 1;
recipedata[432].mpCost = 174;
recipedata[432].successRate = 100;

// recipe id 433
strcpy( recipedata[433].name, "mk_shrnoen's_helmet_i\0" );
recipedata[433].itemID = 503;
recipedata[433].rec_itemID = 4962;
recipedata[433].level = 6;
recipedata[433].resultCount = 1;
recipedata[433].mpCost = 174;
recipedata[433].successRate = 60;

// recipe id 434
strcpy( recipedata[434].name, "mk_shrnoen's_helmet_ii\0" );
recipedata[434].itemID = 503;
recipedata[434].rec_itemID = 4179;
recipedata[434].level = 6;
recipedata[434].resultCount = 1;
recipedata[434].mpCost = 174;
recipedata[434].successRate = 100;

// recipe id 435
strcpy( recipedata[435].name, "mk_silver_arrow\0" );
recipedata[435].itemID = 1343;
recipedata[435].rec_itemID = 4180;
recipedata[435].level = 4;
recipedata[435].resultCount = 550;
recipedata[435].mpCost = 120;
recipedata[435].successRate = 100;

// recipe id 436
strcpy( recipedata[436].name, "mk_pata_i\0" );
recipedata[436].itemID = 264;
recipedata[436].rec_itemID = 5008;
recipedata[436].level = 6;
recipedata[436].resultCount = 1;
recipedata[436].mpCost = 171;
recipedata[436].successRate = 60;

// recipe id 437
strcpy( recipedata[437].name, "mk_pata_ii\0" );
recipedata[437].itemID = 264;
recipedata[437].rec_itemID = 4181;
recipedata[437].level = 6;
recipedata[437].resultCount = 1;
recipedata[437].mpCost = 171;
recipedata[437].successRate = 100;

// recipe id 438
strcpy( recipedata[438].name, "mk_great_sword_i\0" );
recipedata[438].itemID = 78;
recipedata[438].rec_itemID = 4963;
recipedata[438].level = 6;
recipedata[438].resultCount = 1;
recipedata[438].mpCost = 174;
recipedata[438].successRate = 60;

// recipe id 439
strcpy( recipedata[439].name, "mk_great_sword_ii\0" );
recipedata[439].itemID = 78;
recipedata[439].rec_itemID = 4182;
recipedata[439].level = 6;
recipedata[439].resultCount = 1;
recipedata[439].mpCost = 174;
recipedata[439].successRate = 100;

// recipe id 440
strcpy( recipedata[440].name, "mk_heavy_war_axe_i\0" );
recipedata[440].itemID = 91;
recipedata[440].rec_itemID = 4964;
recipedata[440].level = 6;
recipedata[440].resultCount = 1;
recipedata[440].mpCost = 174;
recipedata[440].successRate = 60;

// recipe id 441
strcpy( recipedata[441].name, "mk_heavy_war_axe_ii\0" );
recipedata[441].itemID = 91;
recipedata[441].rec_itemID = 4183;
recipedata[441].level = 6;
recipedata[441].resultCount = 1;
recipedata[441].mpCost = 174;
recipedata[441].successRate = 100;

// recipe id 442
strcpy( recipedata[442].name, "mk_sprite's_staff_i\0" );
recipedata[442].itemID = 92;
recipedata[442].rec_itemID = 4965;
recipedata[442].level = 6;
recipedata[442].resultCount = 1;
recipedata[442].mpCost = 174;
recipedata[442].successRate = 60;

// recipe id 443
strcpy( recipedata[443].name, "mk_sprite's_staff_ii\0" );
recipedata[443].itemID = 92;
recipedata[443].rec_itemID = 4184;
recipedata[443].level = 6;
recipedata[443].resultCount = 1;
recipedata[443].mpCost = 174;
recipedata[443].successRate = 100;

// recipe id 444
strcpy( recipedata[444].name, "mk_kshanberk_i\0" );
recipedata[444].itemID = 142;
recipedata[444].rec_itemID = 4966;
recipedata[444].level = 6;
recipedata[444].resultCount = 1;
recipedata[444].mpCost = 174;
recipedata[444].successRate = 60;

// recipe id 445
strcpy( recipedata[445].name, "mk_kshanberk_ii\0" );
recipedata[445].itemID = 142;
recipedata[445].rec_itemID = 4185;
recipedata[445].level = 6;
recipedata[445].resultCount = 1;
recipedata[445].mpCost = 174;
recipedata[445].successRate = 100;

// recipe id 446
strcpy( recipedata[446].name, "mk_sword_of_valhalla_i\0" );
recipedata[446].itemID = 148;
recipedata[446].rec_itemID = 4967;
recipedata[446].level = 6;
recipedata[446].resultCount = 1;
recipedata[446].mpCost = 174;
recipedata[446].successRate = 60;

// recipe id 447
strcpy( recipedata[447].name, "mk_sword_of_valhalla_ii\0" );
recipedata[447].itemID = 148;
recipedata[447].rec_itemID = 4186;
recipedata[447].level = 6;
recipedata[447].resultCount = 1;
recipedata[447].mpCost = 174;
recipedata[447].successRate = 100;

// recipe id 448
strcpy( recipedata[448].name, "mk_kris_i\0" );
recipedata[448].itemID = 229;
recipedata[448].rec_itemID = 4968;
recipedata[448].level = 6;
recipedata[448].resultCount = 1;
recipedata[448].mpCost = 174;
recipedata[448].successRate = 60;

// recipe id 449
strcpy( recipedata[449].name, "mk_kris_ii\0" );
recipedata[449].itemID = 229;
recipedata[449].rec_itemID = 4187;
recipedata[449].level = 6;
recipedata[449].resultCount = 1;
recipedata[449].mpCost = 174;
recipedata[449].successRate = 100;

// recipe id 450
strcpy( recipedata[450].name, "mk_hell_knife_i\0" );
recipedata[450].itemID = 243;
recipedata[450].rec_itemID = 4969;
recipedata[450].level = 6;
recipedata[450].resultCount = 1;
recipedata[450].mpCost = 174;
recipedata[450].successRate = 60;

// recipe id 451
strcpy( recipedata[451].name, "mk_hell_knife_ii\0" );
recipedata[451].itemID = 243;
recipedata[451].rec_itemID = 4188;
recipedata[451].level = 6;
recipedata[451].resultCount = 1;
recipedata[451].mpCost = 174;
recipedata[451].successRate = 100;

// recipe id 452
strcpy( recipedata[452].name, "mk_arthro_nail_i\0" );
recipedata[452].itemID = 267;
recipedata[452].rec_itemID = 4970;
recipedata[452].level = 6;
recipedata[452].resultCount = 1;
recipedata[452].mpCost = 174;
recipedata[452].successRate = 60;

// recipe id 453
strcpy( recipedata[453].name, "mk_arthro_nail_ii\0" );
recipedata[453].itemID = 267;
recipedata[453].rec_itemID = 4189;
recipedata[453].level = 6;
recipedata[453].resultCount = 1;
recipedata[453].mpCost = 174;
recipedata[453].successRate = 100;

// recipe id 454
strcpy( recipedata[454].name, "mk_dark_elven_long_bow_i\0" );
recipedata[454].itemID = 284;
recipedata[454].rec_itemID = 4971;
recipedata[454].level = 6;
recipedata[454].resultCount = 1;
recipedata[454].mpCost = 174;
recipedata[454].successRate = 60;

// recipe id 455
strcpy( recipedata[455].name, "mk_dark_elven_long_bow_ii\0" );
recipedata[455].itemID = 284;
recipedata[455].rec_itemID = 4190;
recipedata[455].level = 6;
recipedata[455].resultCount = 1;
recipedata[455].mpCost = 174;
recipedata[455].successRate = 100;

// recipe id 456
strcpy( recipedata[456].name, "mk_great_axe_i\0" );
recipedata[456].itemID = 300;
recipedata[456].rec_itemID = 4972;
recipedata[456].level = 6;
recipedata[456].resultCount = 1;
recipedata[456].mpCost = 174;
recipedata[456].successRate = 60;

// recipe id 457
strcpy( recipedata[457].name, "mk_great_axe_ii\0" );
recipedata[457].itemID = 300;
recipedata[457].rec_itemID = 4191;
recipedata[457].level = 6;
recipedata[457].resultCount = 1;
recipedata[457].mpCost = 174;
recipedata[457].successRate = 100;

// recipe id 458
strcpy( recipedata[458].name, "mk_sword_of_damascus_i\0" );
recipedata[458].itemID = 79;
recipedata[458].rec_itemID = 5000;
recipedata[458].level = 7;
recipedata[458].resultCount = 1;
recipedata[458].mpCost = 183;
recipedata[458].successRate = 60;

// recipe id 459
strcpy( recipedata[459].name, "mk_sword_of_damascus_ii\0" );
recipedata[459].itemID = 79;
recipedata[459].rec_itemID = 4192;
recipedata[459].level = 7;
recipedata[459].resultCount = 1;
recipedata[459].mpCost = 183;
recipedata[459].successRate = 100;

// recipe id 460
strcpy( recipedata[460].name, "mk_lancia_i\0" );
recipedata[460].itemID = 97;
recipedata[460].rec_itemID = 5001;
recipedata[460].level = 7;
recipedata[460].resultCount = 1;
recipedata[460].mpCost = 183;
recipedata[460].successRate = 60;

// recipe id 461
strcpy( recipedata[461].name, "mk_lancia_ii\0" );
recipedata[461].itemID = 97;
recipedata[461].rec_itemID = 4193;
recipedata[461].level = 7;
recipedata[461].resultCount = 1;
recipedata[461].mpCost = 183;
recipedata[461].successRate = 100;

// recipe id 462
strcpy( recipedata[462].name, "mk_deadman's_glory_i\0" );
recipedata[462].itemID = 171;
recipedata[462].rec_itemID = 5002;
recipedata[462].level = 7;
recipedata[462].resultCount = 1;
recipedata[462].mpCost = 183;
recipedata[462].successRate = 60;

// recipe id 463
strcpy( recipedata[463].name, "mk_deadman's_glory_ii\0" );
recipedata[463].itemID = 171;
recipedata[463].rec_itemID = 4194;
recipedata[463].level = 7;
recipedata[463].resultCount = 1;
recipedata[463].mpCost = 183;
recipedata[463].successRate = 100;

// recipe id 464
strcpy( recipedata[464].name, "mk_art_of_battle_axe_i\0" );
recipedata[464].itemID = 175;
recipedata[464].rec_itemID = 5003;
recipedata[464].level = 7;
recipedata[464].resultCount = 1;
recipedata[464].mpCost = 183;
recipedata[464].successRate = 60;

// recipe id 465
strcpy( recipedata[465].name, "mk_art_of_battle_axe_ii\0" );
recipedata[465].itemID = 175;
recipedata[465].rec_itemID = 4195;
recipedata[465].level = 7;
recipedata[465].resultCount = 1;
recipedata[465].mpCost = 183;
recipedata[465].successRate = 100;

// recipe id 466
strcpy( recipedata[466].name, "mk_staff_of_evil_sprit_i\0" );
recipedata[466].itemID = 210;
recipedata[466].rec_itemID = 5004;
recipedata[466].level = 7;
recipedata[466].resultCount = 1;
recipedata[466].mpCost = 183;
recipedata[466].successRate = 60;

// recipe id 467
strcpy( recipedata[467].name, "mk_staff_of_evil_sprit_ii\0" );
recipedata[467].itemID = 210;
recipedata[467].rec_itemID = 4196;
recipedata[467].level = 7;
recipedata[467].resultCount = 1;
recipedata[467].mpCost = 183;
recipedata[467].successRate = 100;

// recipe id 468
strcpy( recipedata[468].name, "mk_demon's_sword_i\0" );
recipedata[468].itemID = 234;
recipedata[468].rec_itemID = 5005;
recipedata[468].level = 7;
recipedata[468].resultCount = 1;
recipedata[468].mpCost = 183;
recipedata[468].successRate = 60;

// recipe id 469
strcpy( recipedata[469].name, "mk_demon's_sword_ii\0" );
recipedata[469].itemID = 234;
recipedata[469].rec_itemID = 4197;
recipedata[469].level = 7;
recipedata[469].resultCount = 1;
recipedata[469].mpCost = 183;
recipedata[469].successRate = 100;

// recipe id 470
strcpy( recipedata[470].name, "mk_bellion_cestus_i\0" );
recipedata[470].itemID = 268;
recipedata[470].rec_itemID = 5006;
recipedata[470].level = 7;
recipedata[470].resultCount = 1;
recipedata[470].mpCost = 183;
recipedata[470].successRate = 60;

// recipe id 471
strcpy( recipedata[471].name, "mk_bellion_cestus_ii\0" );
recipedata[471].itemID = 268;
recipedata[471].rec_itemID = 4198;
recipedata[471].level = 7;
recipedata[471].resultCount = 1;
recipedata[471].mpCost = 183;
recipedata[471].successRate = 100;

// recipe id 472
strcpy( recipedata[472].name, "mk_hazard_bow_i\0" );
recipedata[472].itemID = 287;
recipedata[472].rec_itemID = 5007;
recipedata[472].level = 7;
recipedata[472].resultCount = 1;
recipedata[472].mpCost = 183;
recipedata[472].successRate = 60;

// recipe id 473
strcpy( recipedata[473].name, "mk_hazard_bow_ii\0" );
recipedata[473].itemID = 287;
recipedata[473].rec_itemID = 4199;
recipedata[473].level = 7;
recipedata[473].resultCount = 1;
recipedata[473].mpCost = 183;
recipedata[473].successRate = 100;

// recipe id 474
strcpy( recipedata[474].name, "mk_maestro_holder\0" );
recipedata[474].itemID = 4045;
recipedata[474].rec_itemID = 4122;
recipedata[474].level = 6;
recipedata[474].resultCount = 1;
recipedata[474].mpCost = 60;
recipedata[474].successRate = 100;

// recipe id 475
strcpy( recipedata[475].name, "mk_maestro_anvil_lock\0" );
recipedata[475].itemID = 4046;
recipedata[475].rec_itemID = 4123;
recipedata[475].level = 6;
recipedata[475].resultCount = 1;
recipedata[475].mpCost = 60;
recipedata[475].successRate = 100;

// recipe id 476
strcpy( recipedata[476].name, "mk_craftsman_mold\0" );
recipedata[476].itemID = 4047;
recipedata[476].rec_itemID = 4124;
recipedata[476].level = 6;
recipedata[476].resultCount = 1;
recipedata[476].mpCost = 60;
recipedata[476].successRate = 100;

// recipe id 477
strcpy( recipedata[477].name, "mk_maestro_mold\0" );
recipedata[477].itemID = 4048;
recipedata[477].rec_itemID = 4125;
recipedata[477].level = 6;
recipedata[477].resultCount = 1;
recipedata[477].mpCost = 60;
recipedata[477].successRate = 100;

// recipe id 478
strcpy( recipedata[478].name, "mk_avadon_shield_i\0" );
recipedata[478].itemID = 673;
recipedata[478].rec_itemID = 4936;
recipedata[478].level = 6;
recipedata[478].resultCount = 1;
recipedata[478].mpCost = 174;
recipedata[478].successRate = 60;

// recipe id 479
strcpy( recipedata[479].name, "mk_avadon_shield_ii\0" );
recipedata[479].itemID = 673;
recipedata[479].rec_itemID = 4441;
recipedata[479].level = 6;
recipedata[479].resultCount = 1;
recipedata[479].mpCost = 174;
recipedata[479].successRate = 100;

// recipe id 480
strcpy( recipedata[480].name, "mk_comp_soulshot_d\0" );
recipedata[480].itemID = 5135;
recipedata[480].rec_itemID = 5153;
recipedata[480].level = 2;
recipedata[480].resultCount = 1;
recipedata[480].mpCost = 90;
recipedata[480].successRate = 100;

// recipe id 481
strcpy( recipedata[481].name, "mk_comp_soulshot_c\0" );
recipedata[481].itemID = 5136;
recipedata[481].rec_itemID = 5154;
recipedata[481].level = 4;
recipedata[481].resultCount = 1;
recipedata[481].mpCost = 180;
recipedata[481].successRate = 100;

// recipe id 482
strcpy( recipedata[482].name, "mk_comp_soulshot_b\0" );
recipedata[482].itemID = 5137;
recipedata[482].rec_itemID = 5155;
recipedata[482].level = 6;
recipedata[482].resultCount = 1;
recipedata[482].mpCost = 270;
recipedata[482].successRate = 100;

// recipe id 483
strcpy( recipedata[483].name, "mk_comp_soulshot_a\0" );
recipedata[483].itemID = 5138;
recipedata[483].rec_itemID = 5156;
recipedata[483].level = 7;
recipedata[483].resultCount = 1;
recipedata[483].mpCost = 360;
recipedata[483].successRate = 100;

// recipe id 484
strcpy( recipedata[484].name, "mk_comp_soulshot_s\0" );
recipedata[484].itemID = 5139;
recipedata[484].rec_itemID = 5157;
recipedata[484].level = 8;
recipedata[484].resultCount = 1;
recipedata[484].mpCost = 450;
recipedata[484].successRate = 100;

// recipe id 485
strcpy( recipedata[485].name, "mk_comp_spiritshot_d\0" );
recipedata[485].itemID = 5141;
recipedata[485].rec_itemID = 5158;
recipedata[485].level = 2;
recipedata[485].resultCount = 1;
recipedata[485].mpCost = 180;
recipedata[485].successRate = 100;

// recipe id 486
strcpy( recipedata[486].name, "mk_comp_spiritshot_c\0" );
recipedata[486].itemID = 5142;
recipedata[486].rec_itemID = 5159;
recipedata[486].level = 4;
recipedata[486].resultCount = 1;
recipedata[486].mpCost = 360;
recipedata[486].successRate = 100;

// recipe id 487
strcpy( recipedata[487].name, "mk_comp_spiritshot_b\0" );
recipedata[487].itemID = 5143;
recipedata[487].rec_itemID = 5160;
recipedata[487].level = 6;
recipedata[487].resultCount = 1;
recipedata[487].mpCost = 540;
recipedata[487].successRate = 100;

// recipe id 488
strcpy( recipedata[488].name, "mk_comp_spiritshot_a\0" );
recipedata[488].itemID = 5144;
recipedata[488].rec_itemID = 5161;
recipedata[488].level = 7;
recipedata[488].resultCount = 1;
recipedata[488].mpCost = 720;
recipedata[488].successRate = 100;

// recipe id 489
strcpy( recipedata[489].name, "mk_comp_spiritshot_s\0" );
recipedata[489].itemID = 5145;
recipedata[489].rec_itemID = 5162;
recipedata[489].level = 8;
recipedata[489].resultCount = 1;
recipedata[489].mpCost = 900;
recipedata[489].successRate = 100;

// recipe id 490
strcpy( recipedata[490].name, "mk_comp_bspiritshot_d\0" );
recipedata[490].itemID = 5147;
recipedata[490].rec_itemID = 5163;
recipedata[490].level = 2;
recipedata[490].resultCount = 1;
recipedata[490].mpCost = 180;
recipedata[490].successRate = 100;

// recipe id 491
strcpy( recipedata[491].name, "mk_comp_bspiritshot_c\0" );
recipedata[491].itemID = 5148;
recipedata[491].rec_itemID = 5164;
recipedata[491].level = 4;
recipedata[491].resultCount = 1;
recipedata[491].mpCost = 360;
recipedata[491].successRate = 100;

// recipe id 492
strcpy( recipedata[492].name, "mk_comp_bspiritshot_b\0" );
recipedata[492].itemID = 5149;
recipedata[492].rec_itemID = 5165;
recipedata[492].level = 6;
recipedata[492].resultCount = 1;
recipedata[492].mpCost = 540;
recipedata[492].successRate = 100;

// recipe id 493
strcpy( recipedata[493].name, "mk_comp_bspiritshot_a\0" );
recipedata[493].itemID = 5150;
recipedata[493].rec_itemID = 5166;
recipedata[493].level = 7;
recipedata[493].resultCount = 1;
recipedata[493].mpCost = 720;
recipedata[493].successRate = 100;

// recipe id 494
strcpy( recipedata[494].name, "mk_comp_bspiritshot_s\0" );
recipedata[494].itemID = 5151;
recipedata[494].rec_itemID = 5167;
recipedata[494].level = 8;
recipedata[494].resultCount = 1;
recipedata[494].mpCost = 900;
recipedata[494].successRate = 100;

// recipe id 495
strcpy( recipedata[495].name, "mk_knuckle_dust\0" );
recipedata[495].itemID = 4233;
recipedata[495].rec_itemID = 4440;
recipedata[495].level = 5;
recipedata[495].resultCount = 1;
recipedata[495].mpCost = 147;
recipedata[495].successRate = 100;

// recipe id 496
strcpy( recipedata[496].name, "mk_mithril_arrow\0" );
recipedata[496].itemID = 1344;
recipedata[496].rec_itemID = 5230;
recipedata[496].level = 4;
recipedata[496].resultCount = 500;
recipedata[496].mpCost = 130;
recipedata[496].successRate = 100;

// recipe id 497
strcpy( recipedata[497].name, "mk_reinforcing_agent\0" );
recipedata[497].itemID = 5220;
recipedata[497].rec_itemID = 5231;
recipedata[497].level = 4;
recipedata[497].resultCount = 1;
recipedata[497].mpCost = 40;
recipedata[497].successRate = 100;

// recipe id 498
strcpy( recipedata[498].name, "mk_adv_comp_soulshot_d\0" );
recipedata[498].itemID = 5251;
recipedata[498].rec_itemID = 5268;
recipedata[498].level = 2;
recipedata[498].resultCount = 1;
recipedata[498].mpCost = 90;
recipedata[498].successRate = 100;

// recipe id 499
strcpy( recipedata[499].name, "mk_adv_comp_soulshot_c\0" );
recipedata[499].itemID = 5252;
recipedata[499].rec_itemID = 5269;
recipedata[499].level = 4;
recipedata[499].resultCount = 1;
recipedata[499].mpCost = 180;
recipedata[499].successRate = 100;

// recipe id 500
strcpy( recipedata[500].name, "mk_adv_comp_soulshot_b\0" );
recipedata[500].itemID = 5253;
recipedata[500].rec_itemID = 5270;
recipedata[500].level = 6;
recipedata[500].resultCount = 1;
recipedata[500].mpCost = 270;
recipedata[500].successRate = 100;

// recipe id 501
strcpy( recipedata[501].name, "mk_adv_comp_soulshot_a\0" );
recipedata[501].itemID = 5254;
recipedata[501].rec_itemID = 5271;
recipedata[501].level = 7;
recipedata[501].resultCount = 1;
recipedata[501].mpCost = 360;
recipedata[501].successRate = 100;

// recipe id 502
strcpy( recipedata[502].name, "mk_adv_comp_soulshot_s\0" );
recipedata[502].itemID = 5255;
recipedata[502].rec_itemID = 5272;
recipedata[502].level = 8;
recipedata[502].resultCount = 1;
recipedata[502].mpCost = 450;
recipedata[502].successRate = 100;

// recipe id 503
strcpy( recipedata[503].name, "mk_adv_comp_spiritshot_d\0" );
recipedata[503].itemID = 5257;
recipedata[503].rec_itemID = 5273;
recipedata[503].level = 2;
recipedata[503].resultCount = 1;
recipedata[503].mpCost = 180;
recipedata[503].successRate = 100;

// recipe id 504
strcpy( recipedata[504].name, "mk_adv_comp_spiritshot_c\0" );
recipedata[504].itemID = 5258;
recipedata[504].rec_itemID = 5274;
recipedata[504].level = 4;
recipedata[504].resultCount = 1;
recipedata[504].mpCost = 360;
recipedata[504].successRate = 100;

// recipe id 505
strcpy( recipedata[505].name, "mk_adv_comp_spiritshot_b\0" );
recipedata[505].itemID = 5259;
recipedata[505].rec_itemID = 5275;
recipedata[505].level = 6;
recipedata[505].resultCount = 1;
recipedata[505].mpCost = 540;
recipedata[505].successRate = 100;

// recipe id 506
strcpy( recipedata[506].name, "mk_adv_comp_spiritshot_a\0" );
recipedata[506].itemID = 5260;
recipedata[506].rec_itemID = 5276;
recipedata[506].level = 7;
recipedata[506].resultCount = 1;
recipedata[506].mpCost = 720;
recipedata[506].successRate = 100;

// recipe id 507
strcpy( recipedata[507].name, "mk_adv_comp_spiritshot_s\0" );
recipedata[507].itemID = 5261;
recipedata[507].rec_itemID = 5277;
recipedata[507].level = 8;
recipedata[507].resultCount = 1;
recipedata[507].mpCost = 900;
recipedata[507].successRate = 100;

// recipe id 508
strcpy( recipedata[508].name, "mk_adv_comp_bspiritshot_d\0" );
recipedata[508].itemID = 5263;
recipedata[508].rec_itemID = 5278;
recipedata[508].level = 2;
recipedata[508].resultCount = 1;
recipedata[508].mpCost = 180;
recipedata[508].successRate = 100;

// recipe id 509
strcpy( recipedata[509].name, "mk_adv_comp_bspiritshot_c\0" );
recipedata[509].itemID = 5264;
recipedata[509].rec_itemID = 5279;
recipedata[509].level = 4;
recipedata[509].resultCount = 1;
recipedata[509].mpCost = 360;
recipedata[509].successRate = 100;

// recipe id 510
strcpy( recipedata[510].name, "mk_adv_comp_bspiritshot_b\0" );
recipedata[510].itemID = 5265;
recipedata[510].rec_itemID = 5280;
recipedata[510].level = 6;
recipedata[510].resultCount = 1;
recipedata[510].mpCost = 540;
recipedata[510].successRate = 100;

// recipe id 511
strcpy( recipedata[511].name, "mk_adv_comp_bspiritshot_a\0" );
recipedata[511].itemID = 5266;
recipedata[511].rec_itemID = 5281;
recipedata[511].level = 7;
recipedata[511].resultCount = 1;
recipedata[511].mpCost = 720;
recipedata[511].successRate = 100;

// recipe id 512
strcpy( recipedata[512].name, "mk_adv_comp_bspiritshot_s\0" );
recipedata[512].itemID = 5267;
recipedata[512].rec_itemID = 5282;
recipedata[512].level = 8;
recipedata[512].resultCount = 1;
recipedata[512].mpCost = 900;
recipedata[512].successRate = 100;

// recipe id 513
strcpy( recipedata[513].name, "mk_sealed_dark_crystal_leather_mail_i\0" );
recipedata[513].itemID = 5297;
recipedata[513].rec_itemID = 5332;
recipedata[513].level = 7;
recipedata[513].resultCount = 1;
recipedata[513].mpCost = 192;
recipedata[513].successRate = 60;

// recipe id 514
strcpy( recipedata[514].name, "mk_sealed_dark_crystal_leather_mail_ii\0" );
recipedata[514].itemID = 5297;
recipedata[514].rec_itemID = 5333;
recipedata[514].level = 7;
recipedata[514].resultCount = 1;
recipedata[514].mpCost = 192;
recipedata[514].successRate = 100;

// recipe id 515
strcpy( recipedata[515].name, "mk_sealed_tallum_leather_mail_i\0" );
recipedata[515].itemID = 5301;
recipedata[515].rec_itemID = 5334;
recipedata[515].level = 7;
recipedata[515].resultCount = 1;
recipedata[515].mpCost = 192;
recipedata[515].successRate = 60;

// recipe id 516
strcpy( recipedata[516].name, "mk_sealed_tallum_leather_mail_ii\0" );
recipedata[516].itemID = 5301;
recipedata[516].rec_itemID = 5335;
recipedata[516].level = 7;
recipedata[516].resultCount = 1;
recipedata[516].mpCost = 192;
recipedata[516].successRate = 100;

// recipe id 517
strcpy( recipedata[517].name, "mk_sealed_leather_mail_of_nightmare_i\0" );
recipedata[517].itemID = 5320;
recipedata[517].rec_itemID = 5336;
recipedata[517].level = 8;
recipedata[517].resultCount = 1;
recipedata[517].mpCost = 201;
recipedata[517].successRate = 60;

// recipe id 518
strcpy( recipedata[518].name, "mk_sealed_leather_mail_of_nightmare_ii\0" );
recipedata[518].itemID = 5320;
recipedata[518].rec_itemID = 5337;
recipedata[518].level = 8;
recipedata[518].resultCount = 1;
recipedata[518].mpCost = 201;
recipedata[518].successRate = 100;

// recipe id 519
strcpy( recipedata[519].name, "mk_sealed_majestic_leather_mail_i\0" );
recipedata[519].itemID = 5323;
recipedata[519].rec_itemID = 5338;
recipedata[519].level = 8;
recipedata[519].resultCount = 1;
recipedata[519].mpCost = 201;
recipedata[519].successRate = 60;

// recipe id 520
strcpy( recipedata[520].name, "mk_sealed_majestic_leather_mail_ii\0" );
recipedata[520].itemID = 5323;
recipedata[520].rec_itemID = 5339;
recipedata[520].level = 8;
recipedata[520].resultCount = 1;
recipedata[520].mpCost = 201;
recipedata[520].successRate = 100;

// recipe id 521
strcpy( recipedata[521].name, "mk_sealed_legging_of_dark_crystal_i\0" );
recipedata[521].itemID = 5298;
recipedata[521].rec_itemID = 5340;
recipedata[521].level = 7;
recipedata[521].resultCount = 1;
recipedata[521].mpCost = 192;
recipedata[521].successRate = 60;

// recipe id 522
strcpy( recipedata[522].name, "mk_sealed_legging_of_dark_crystal_ii\0" );
recipedata[522].itemID = 5298;
recipedata[522].rec_itemID = 5341;
recipedata[522].level = 7;
recipedata[522].resultCount = 1;
recipedata[522].mpCost = 192;
recipedata[522].successRate = 100;

// recipe id 523
strcpy( recipedata[523].name, "mk_sealed_tallum_tunic_i\0" );
recipedata[523].itemID = 5304;
recipedata[523].rec_itemID = 5346;
recipedata[523].level = 7;
recipedata[523].resultCount = 1;
recipedata[523].mpCost = 192;
recipedata[523].successRate = 60;

// recipe id 524
strcpy( recipedata[524].name, "mk_sealed_tallum_tunic_ii\0" );
recipedata[524].itemID = 5304;
recipedata[524].rec_itemID = 5347;
recipedata[524].level = 7;
recipedata[524].resultCount = 1;
recipedata[524].mpCost = 192;
recipedata[524].successRate = 100;

// recipe id 525
strcpy( recipedata[525].name, "mk_sealed_dark_crystal_robe_i\0" );
recipedata[525].itemID = 5308;
recipedata[525].rec_itemID = 5348;
recipedata[525].level = 7;
recipedata[525].resultCount = 1;
recipedata[525].mpCost = 192;
recipedata[525].successRate = 60;

// recipe id 526
strcpy( recipedata[526].name, "mk_sealed_dark_crystal_robe_ii\0" );
recipedata[526].itemID = 5308;
recipedata[526].rec_itemID = 5349;
recipedata[526].level = 7;
recipedata[526].resultCount = 1;
recipedata[526].mpCost = 192;
recipedata[526].successRate = 100;

// recipe id 527
strcpy( recipedata[527].name, "mk_sealed_robe_of_nightmare_i\0" );
recipedata[527].itemID = 5326;
recipedata[527].rec_itemID = 5350;
recipedata[527].level = 8;
recipedata[527].resultCount = 1;
recipedata[527].mpCost = 201;
recipedata[527].successRate = 60;

// recipe id 528
strcpy( recipedata[528].name, "mk_sealed_robe_of_nightmare_ii\0" );
recipedata[528].itemID = 5326;
recipedata[528].rec_itemID = 5351;
recipedata[528].level = 8;
recipedata[528].resultCount = 1;
recipedata[528].mpCost = 201;
recipedata[528].successRate = 100;

// recipe id 529
strcpy( recipedata[529].name, "mk_sealed_majestic_robe_i\0" );
recipedata[529].itemID = 5329;
recipedata[529].rec_itemID = 5352;
recipedata[529].level = 8;
recipedata[529].resultCount = 1;
recipedata[529].mpCost = 201;
recipedata[529].successRate = 60;

// recipe id 530
strcpy( recipedata[530].name, "mk_sealed_majestic_robe_ii\0" );
recipedata[530].itemID = 5329;
recipedata[530].rec_itemID = 5353;
recipedata[530].level = 8;
recipedata[530].resultCount = 1;
recipedata[530].mpCost = 201;
recipedata[530].successRate = 100;

// recipe id 531
strcpy( recipedata[531].name, "mk_sealed_tallum_hose_i\0" );
recipedata[531].itemID = 5305;
recipedata[531].rec_itemID = 5354;
recipedata[531].level = 7;
recipedata[531].resultCount = 1;
recipedata[531].mpCost = 192;
recipedata[531].successRate = 60;

// recipe id 532
strcpy( recipedata[532].name, "mk_sealed_tallum_hose_ii\0" );
recipedata[532].itemID = 5305;
recipedata[532].rec_itemID = 5355;
recipedata[532].level = 7;
recipedata[532].resultCount = 1;
recipedata[532].mpCost = 192;
recipedata[532].successRate = 100;

// recipe id 533
strcpy( recipedata[533].name, "mk_sealed_dark_crystal_shield_i\0" );
recipedata[533].itemID = 5292;
recipedata[533].rec_itemID = 5364;
recipedata[533].level = 7;
recipedata[533].resultCount = 1;
recipedata[533].mpCost = 192;
recipedata[533].successRate = 60;

// recipe id 534
strcpy( recipedata[534].name, "mk_sealed_dark_crystal_shield_ii\0" );
recipedata[534].itemID = 5292;
recipedata[534].rec_itemID = 5365;
recipedata[534].level = 7;
recipedata[534].resultCount = 1;
recipedata[534].mpCost = 192;
recipedata[534].successRate = 100;

// recipe id 535
strcpy( recipedata[535].name, "mk_sealed_shield_of_nightmare_i\0" );
recipedata[535].itemID = 5315;
recipedata[535].rec_itemID = 5366;
recipedata[535].level = 8;
recipedata[535].resultCount = 1;
recipedata[535].mpCost = 201;
recipedata[535].successRate = 60;

// recipe id 536
strcpy( recipedata[536].name, "mk_sealed_shield_of_nightmare_ii\0" );
recipedata[536].itemID = 5315;
recipedata[536].rec_itemID = 5367;
recipedata[536].level = 8;
recipedata[536].resultCount = 1;
recipedata[536].mpCost = 201;
recipedata[536].successRate = 100;

// recipe id 537
strcpy( recipedata[537].name, "mk_sealed_dark_crystal_boots_i\0" );
recipedata[537].itemID = 5291;
recipedata[537].rec_itemID = 5368;
recipedata[537].level = 7;
recipedata[537].resultCount = 1;
recipedata[537].mpCost = 192;
recipedata[537].successRate = 60;

// recipe id 538
strcpy( recipedata[538].name, "mk_sealed_dark_crystal_boots_ii\0" );
recipedata[538].itemID = 5291;
recipedata[538].rec_itemID = 5369;
recipedata[538].level = 7;
recipedata[538].resultCount = 1;
recipedata[538].mpCost = 192;
recipedata[538].successRate = 100;

// recipe id 539
strcpy( recipedata[539].name, "mk_sealed_tallum_boots_i\0" );
recipedata[539].itemID = 5296;
recipedata[539].rec_itemID = 5370;
recipedata[539].level = 7;
recipedata[539].resultCount = 1;
recipedata[539].mpCost = 192;
recipedata[539].successRate = 60;

// recipe id 540
strcpy( recipedata[540].name, "mk_sealed_tallum_boots_ii\0" );
recipedata[540].itemID = 5296;
recipedata[540].rec_itemID = 5371;
recipedata[540].level = 7;
recipedata[540].resultCount = 1;
recipedata[540].mpCost = 192;
recipedata[540].successRate = 100;

// recipe id 541
strcpy( recipedata[541].name, "mk_sealed_boots_of_nightmare_i\0" );
recipedata[541].itemID = 5314;
recipedata[541].rec_itemID = 5380;
recipedata[541].level = 8;
recipedata[541].resultCount = 1;
recipedata[541].mpCost = 201;
recipedata[541].successRate = 60;

// recipe id 542
strcpy( recipedata[542].name, "mk_sealed_boots_of_nightmare_ii\0" );
recipedata[542].itemID = 5314;
recipedata[542].rec_itemID = 5381;
recipedata[542].level = 8;
recipedata[542].resultCount = 1;
recipedata[542].mpCost = 201;
recipedata[542].successRate = 100;

// recipe id 543
strcpy( recipedata[543].name, "mk_sealed_magestic_boots_i\0" );
recipedata[543].itemID = 5319;
recipedata[543].rec_itemID = 5382;
recipedata[543].level = 8;
recipedata[543].resultCount = 1;
recipedata[543].mpCost = 201;
recipedata[543].successRate = 60;

// recipe id 544
strcpy( recipedata[544].name, "mk_sealed_magestic_boots_ii\0" );
recipedata[544].itemID = 5319;
recipedata[544].rec_itemID = 5383;
recipedata[544].level = 8;
recipedata[544].resultCount = 1;
recipedata[544].mpCost = 201;
recipedata[544].successRate = 100;

// recipe id 545
strcpy( recipedata[545].name, "mk_sealed_dark_crystal_gloves_i\0" );
recipedata[545].itemID = 5290;
recipedata[545].rec_itemID = 5392;
recipedata[545].level = 7;
recipedata[545].resultCount = 1;
recipedata[545].mpCost = 192;
recipedata[545].successRate = 60;

// recipe id 546
strcpy( recipedata[546].name, "mk_sealed_dark_crystal_gloves_ii\0" );
recipedata[546].itemID = 5290;
recipedata[546].rec_itemID = 5393;
recipedata[546].level = 7;
recipedata[546].resultCount = 1;
recipedata[546].mpCost = 192;
recipedata[546].successRate = 100;

// recipe id 547
strcpy( recipedata[547].name, "mk_sealed_tallum_gloves_i\0" );
recipedata[547].itemID = 5295;
recipedata[547].rec_itemID = 5394;
recipedata[547].level = 7;
recipedata[547].resultCount = 1;
recipedata[547].mpCost = 192;
recipedata[547].successRate = 60;

// recipe id 548
strcpy( recipedata[548].name, "mk_sealed_tallum_gloves_ii\0" );
recipedata[548].itemID = 5295;
recipedata[548].rec_itemID = 5395;
recipedata[548].level = 7;
recipedata[548].resultCount = 1;
recipedata[548].mpCost = 192;
recipedata[548].successRate = 100;

// recipe id 549
strcpy( recipedata[549].name, "mk_sealed_gloves_of_nightmare_i\0" );
recipedata[549].itemID = 5313;
recipedata[549].rec_itemID = 5404;
recipedata[549].level = 8;
recipedata[549].resultCount = 1;
recipedata[549].mpCost = 201;
recipedata[549].successRate = 60;

// recipe id 550
strcpy( recipedata[550].name, "mk_sealed_gloves_of_nightmare_ii\0" );
recipedata[550].itemID = 5313;
recipedata[550].rec_itemID = 5405;
recipedata[550].level = 8;
recipedata[550].resultCount = 1;
recipedata[550].mpCost = 201;
recipedata[550].successRate = 100;

// recipe id 551
strcpy( recipedata[551].name, "mk_sealed_magestic_gloves_i\0" );
recipedata[551].itemID = 5318;
recipedata[551].rec_itemID = 5406;
recipedata[551].level = 8;
recipedata[551].resultCount = 1;
recipedata[551].mpCost = 201;
recipedata[551].successRate = 60;

// recipe id 552
strcpy( recipedata[552].name, "mk_sealed_magestic_gloves_ii\0" );
recipedata[552].itemID = 5318;
recipedata[552].rec_itemID = 5407;
recipedata[552].level = 8;
recipedata[552].resultCount = 1;
recipedata[552].mpCost = 201;
recipedata[552].successRate = 100;

// recipe id 553
strcpy( recipedata[553].name, "mk_sealed_dark_crystal_breastplate_i\0" );
recipedata[553].itemID = 5287;
recipedata[553].rec_itemID = 5416;
recipedata[553].level = 7;
recipedata[553].resultCount = 1;
recipedata[553].mpCost = 192;
recipedata[553].successRate = 60;

// recipe id 554
strcpy( recipedata[554].name, "mk_sealed_dark_crystal_breastplate_ii\0" );
recipedata[554].itemID = 5287;
recipedata[554].rec_itemID = 5417;
recipedata[554].level = 7;
recipedata[554].resultCount = 1;
recipedata[554].mpCost = 192;
recipedata[554].successRate = 100;

// recipe id 555
strcpy( recipedata[555].name, "mk_sealed_tallum_plate_armor_i\0" );
recipedata[555].itemID = 5293;
recipedata[555].rec_itemID = 5418;
recipedata[555].level = 7;
recipedata[555].resultCount = 1;
recipedata[555].mpCost = 192;
recipedata[555].successRate = 60;

// recipe id 556
strcpy( recipedata[556].name, "mk_sealed_tallum_plate_armor_ii\0" );
recipedata[556].itemID = 5293;
recipedata[556].rec_itemID = 5419;
recipedata[556].level = 7;
recipedata[556].resultCount = 1;
recipedata[556].mpCost = 192;
recipedata[556].successRate = 100;

// recipe id 557
strcpy( recipedata[557].name, "mk_sealed_armor_of_nightmare_i\0" );
recipedata[557].itemID = 5311;
recipedata[557].rec_itemID = 5420;
recipedata[557].level = 8;
recipedata[557].resultCount = 1;
recipedata[557].mpCost = 201;
recipedata[557].successRate = 60;

// recipe id 558
strcpy( recipedata[558].name, "mk_sealed_armor_of_nightmare_ii\0" );
recipedata[558].itemID = 5311;
recipedata[558].rec_itemID = 5421;
recipedata[558].level = 8;
recipedata[558].resultCount = 1;
recipedata[558].mpCost = 201;
recipedata[558].successRate = 100;

// recipe id 559
strcpy( recipedata[559].name, "mk_sealed_majestic_platte_armor_i\0" );
recipedata[559].itemID = 5316;
recipedata[559].rec_itemID = 5422;
recipedata[559].level = 8;
recipedata[559].resultCount = 1;
recipedata[559].mpCost = 201;
recipedata[559].successRate = 60;

// recipe id 560
strcpy( recipedata[560].name, "mk_sealed_majestic_platte_armor_ii\0" );
recipedata[560].itemID = 5316;
recipedata[560].rec_itemID = 5423;
recipedata[560].level = 8;
recipedata[560].resultCount = 1;
recipedata[560].mpCost = 201;
recipedata[560].successRate = 100;

// recipe id 561
strcpy( recipedata[561].name, "mk_sealed_dark_crystal_gaiters_i\0" );
recipedata[561].itemID = 5288;
recipedata[561].rec_itemID = 5424;
recipedata[561].level = 7;
recipedata[561].resultCount = 1;
recipedata[561].mpCost = 192;
recipedata[561].successRate = 60;

// recipe id 562
strcpy( recipedata[562].name, "mk_sealed_dark_crystal_gaiters_ii\0" );
recipedata[562].itemID = 5288;
recipedata[562].rec_itemID = 5425;
recipedata[562].level = 7;
recipedata[562].resultCount = 1;
recipedata[562].mpCost = 192;
recipedata[562].successRate = 100;

// recipe id 563
strcpy( recipedata[563].name, "mk_sealed_dark_crystal_helmet_i\0" );
recipedata[563].itemID = 5289;
recipedata[563].rec_itemID = 5426;
recipedata[563].level = 7;
recipedata[563].resultCount = 1;
recipedata[563].mpCost = 192;
recipedata[563].successRate = 60;

// recipe id 564
strcpy( recipedata[564].name, "mk_sealed_dark_crystal_helmet_ii\0" );
recipedata[564].itemID = 5289;
recipedata[564].rec_itemID = 5427;
recipedata[564].level = 7;
recipedata[564].resultCount = 1;
recipedata[564].mpCost = 192;
recipedata[564].successRate = 100;

// recipe id 565
strcpy( recipedata[565].name, "mk_sealed_tallum_bonnet_i\0" );
recipedata[565].itemID = 5294;
recipedata[565].rec_itemID = 5428;
recipedata[565].level = 7;
recipedata[565].resultCount = 1;
recipedata[565].mpCost = 192;
recipedata[565].successRate = 60;

// recipe id 566
strcpy( recipedata[566].name, "mk_sealed_tallum_bonnet_ii\0" );
recipedata[566].itemID = 5294;
recipedata[566].rec_itemID = 5429;
recipedata[566].level = 7;
recipedata[566].resultCount = 1;
recipedata[566].mpCost = 192;
recipedata[566].successRate = 100;

// recipe id 567
strcpy( recipedata[567].name, "mk_sealed_helm_of_nightmare_i\0" );
recipedata[567].itemID = 5312;
recipedata[567].rec_itemID = 5430;
recipedata[567].level = 8;
recipedata[567].resultCount = 1;
recipedata[567].mpCost = 201;
recipedata[567].successRate = 60;

// recipe id 568
strcpy( recipedata[568].name, "mk_sealed_helm_of_nightmare_ii\0" );
recipedata[568].itemID = 5312;
recipedata[568].rec_itemID = 5431;
recipedata[568].level = 8;
recipedata[568].resultCount = 1;
recipedata[568].mpCost = 201;
recipedata[568].successRate = 100;

// recipe id 569
strcpy( recipedata[569].name, "mk_sealed_magestic_circlet_i\0" );
recipedata[569].itemID = 5317;
recipedata[569].rec_itemID = 5432;
recipedata[569].level = 8;
recipedata[569].resultCount = 1;
recipedata[569].mpCost = 201;
recipedata[569].successRate = 60;

// recipe id 570
strcpy( recipedata[570].name, "mk_sealed_magestic_circlet_ii\0" );
recipedata[570].itemID = 5317;
recipedata[570].rec_itemID = 5433;
recipedata[570].level = 8;
recipedata[570].resultCount = 1;
recipedata[570].mpCost = 201;
recipedata[570].successRate = 100;

// recipe id 571
strcpy( recipedata[571].name, "mk_dragon_slayer_i\0" );
recipedata[571].itemID = 81;
recipedata[571].rec_itemID = 5434;
recipedata[571].level = 8;
recipedata[571].resultCount = 1;
recipedata[571].mpCost = 201;
recipedata[571].successRate = 60;

// recipe id 572
strcpy( recipedata[572].name, "mk_dragon_slayer_ii\0" );
recipedata[572].itemID = 81;
recipedata[572].rec_itemID = 5435;
recipedata[572].level = 8;
recipedata[572].resultCount = 1;
recipedata[572].mpCost = 201;
recipedata[572].successRate = 100;

// recipe id 573
strcpy( recipedata[573].name, "mk_berserker_blade\0" );
recipedata[573].itemID = 5286;
recipedata[573].rec_itemID = 5436;
recipedata[573].level = 6;
recipedata[573].resultCount = 1;
recipedata[573].mpCost = 165;
recipedata[573].successRate = 100;

// recipe id 574
strcpy( recipedata[574].name, "mk_heavy_sword\0" );
recipedata[574].itemID = 5285;
recipedata[574].rec_itemID = 5437;
recipedata[574].level = 2;
recipedata[574].resultCount = 1;
recipedata[574].mpCost = 84;
recipedata[574].successRate = 100;

// recipe id 575
strcpy( recipedata[575].name, "mk_meteor_shower_i\0" );
recipedata[575].itemID = 2504;
recipedata[575].rec_itemID = 5438;
recipedata[575].level = 7;
recipedata[575].resultCount = 1;
recipedata[575].mpCost = 192;
recipedata[575].successRate = 60;

// recipe id 576
strcpy( recipedata[576].name, "mk_meteor_shower_ii\0" );
recipedata[576].itemID = 2504;
recipedata[576].rec_itemID = 5439;
recipedata[576].level = 7;
recipedata[576].resultCount = 1;
recipedata[576].mpCost = 192;
recipedata[576].successRate = 100;

// recipe id 577
strcpy( recipedata[577].name, "mk_elysian_i\0" );
recipedata[577].itemID = 164;
recipedata[577].rec_itemID = 5440;
recipedata[577].level = 8;
recipedata[577].resultCount = 1;
recipedata[577].mpCost = 201;
recipedata[577].successRate = 60;

// recipe id 578
strcpy( recipedata[578].name, "mk_elysian_ii\0" );
recipedata[578].itemID = 164;
recipedata[578].rec_itemID = 5441;
recipedata[578].level = 8;
recipedata[578].resultCount = 1;
recipedata[578].mpCost = 201;
recipedata[578].successRate = 100;

// recipe id 579
strcpy( recipedata[579].name, "mk_soul_bow_i\0" );
recipedata[579].itemID = 289;
recipedata[579].rec_itemID = 5442;
recipedata[579].level = 8;
recipedata[579].resultCount = 1;
recipedata[579].mpCost = 201;
recipedata[579].successRate = 60;

// recipe id 580
strcpy( recipedata[580].name, "mk_soul_bow_ii\0" );
recipedata[580].itemID = 289;
recipedata[580].rec_itemID = 5443;
recipedata[580].level = 8;
recipedata[580].resultCount = 1;
recipedata[580].mpCost = 201;
recipedata[580].successRate = 100;

// recipe id 581
strcpy( recipedata[581].name, "mk_carnium_bow_i\0" );
recipedata[581].itemID = 288;
recipedata[581].rec_itemID = 5444;
recipedata[581].level = 7;
recipedata[581].resultCount = 1;
recipedata[581].mpCost = 192;
recipedata[581].successRate = 60;

// recipe id 582
strcpy( recipedata[582].name, "mk_carnium_bow_ii\0" );
recipedata[582].itemID = 288;
recipedata[582].rec_itemID = 5445;
recipedata[582].level = 7;
recipedata[582].resultCount = 1;
recipedata[582].mpCost = 192;
recipedata[582].successRate = 100;

// recipe id 583
strcpy( recipedata[583].name, "mk_bloody_orchid_i\0" );
recipedata[583].itemID = 235;
recipedata[583].rec_itemID = 5446;
recipedata[583].level = 7;
recipedata[583].resultCount = 1;
recipedata[583].mpCost = 192;
recipedata[583].successRate = 60;

// recipe id 584
strcpy( recipedata[584].name, "mk_bloody_orchid_ii\0" );
recipedata[584].itemID = 235;
recipedata[584].rec_itemID = 5447;
recipedata[584].level = 7;
recipedata[584].resultCount = 1;
recipedata[584].mpCost = 192;
recipedata[584].successRate = 100;

// recipe id 585
strcpy( recipedata[585].name, "mk_soul_separator_i\0" );
recipedata[585].itemID = 236;
recipedata[585].rec_itemID = 5448;
recipedata[585].level = 8;
recipedata[585].resultCount = 1;
recipedata[585].mpCost = 201;
recipedata[585].successRate = 60;

// recipe id 586
strcpy( recipedata[586].name, "mk_soul_separator_ii\0" );
recipedata[586].itemID = 236;
recipedata[586].rec_itemID = 5449;
recipedata[586].level = 8;
recipedata[586].resultCount = 1;
recipedata[586].mpCost = 201;
recipedata[586].successRate = 100;

// recipe id 587
strcpy( recipedata[587].name, "mk_dragon_grinder_i\0" );
recipedata[587].itemID = 270;
recipedata[587].rec_itemID = 5450;
recipedata[587].level = 8;
recipedata[587].resultCount = 1;
recipedata[587].mpCost = 201;
recipedata[587].successRate = 60;

// recipe id 588
strcpy( recipedata[588].name, "mk_dragon_grinder_ii\0" );
recipedata[588].itemID = 270;
recipedata[588].rec_itemID = 5451;
recipedata[588].level = 8;
recipedata[588].resultCount = 1;
recipedata[588].mpCost = 201;
recipedata[588].successRate = 100;

// recipe id 589
strcpy( recipedata[589].name, "mk_blood_tornado_i\0" );
recipedata[589].itemID = 269;
recipedata[589].rec_itemID = 5452;
recipedata[589].level = 7;
recipedata[589].resultCount = 1;
recipedata[589].mpCost = 192;
recipedata[589].successRate = 60;

// recipe id 590
strcpy( recipedata[590].name, "mk_blood_tornado_ii\0" );
recipedata[590].itemID = 269;
recipedata[590].rec_itemID = 5453;
recipedata[590].level = 7;
recipedata[590].resultCount = 1;
recipedata[590].mpCost = 192;
recipedata[590].successRate = 100;

// recipe id 591
strcpy( recipedata[591].name, "mk_tallum_glaive_i\0" );
recipedata[591].itemID = 305;
recipedata[591].rec_itemID = 5456;
recipedata[591].level = 8;
recipedata[591].resultCount = 1;
recipedata[591].mpCost = 201;
recipedata[591].successRate = 60;

// recipe id 592
strcpy( recipedata[592].name, "mk_tallum_glaive_ii\0" );
recipedata[592].itemID = 305;
recipedata[592].rec_itemID = 5457;
recipedata[592].level = 8;
recipedata[592].resultCount = 1;
recipedata[592].mpCost = 201;
recipedata[592].successRate = 100;

// recipe id 593
strcpy( recipedata[593].name, "mk_halbard_i\0" );
recipedata[593].itemID = 98;
recipedata[593].rec_itemID = 5458;
recipedata[593].level = 7;
recipedata[593].resultCount = 1;
recipedata[593].mpCost = 192;
recipedata[593].successRate = 60;

// recipe id 594
strcpy( recipedata[594].name, "mk_halbard_ii\0" );
recipedata[594].itemID = 98;
recipedata[594].rec_itemID = 5459;
recipedata[594].level = 7;
recipedata[594].resultCount = 1;
recipedata[594].mpCost = 192;
recipedata[594].successRate = 100;

// recipe id 595
strcpy( recipedata[595].name, "mk_dasparion's_staff_i\0" );
recipedata[595].itemID = 212;
recipedata[595].rec_itemID = 5460;
recipedata[595].level = 7;
recipedata[595].resultCount = 1;
recipedata[595].mpCost = 192;
recipedata[595].successRate = 60;

// recipe id 596
strcpy( recipedata[596].name, "mk_dasparion's_staff_ii\0" );
recipedata[596].itemID = 212;
recipedata[596].rec_itemID = 5461;
recipedata[596].level = 7;
recipedata[596].resultCount = 1;
recipedata[596].mpCost = 192;
recipedata[596].successRate = 100;

// recipe id 597
strcpy( recipedata[597].name, "mk_worldtree's_branch_i\0" );
recipedata[597].itemID = 213;
recipedata[597].rec_itemID = 5462;
recipedata[597].level = 8;
recipedata[597].resultCount = 1;
recipedata[597].mpCost = 201;
recipedata[597].successRate = 60;

// recipe id 598
strcpy( recipedata[598].name, "mk_worldtree's_branch_ii\0" );
recipedata[598].itemID = 213;
recipedata[598].rec_itemID = 5463;
recipedata[598].level = 8;
recipedata[598].resultCount = 1;
recipedata[598].mpCost = 201;
recipedata[598].successRate = 100;

// recipe id 599
strcpy( recipedata[599].name, "mk_dark_legion's_edge_i\0" );
recipedata[599].itemID = 2500;
recipedata[599].rec_itemID = 5464;
recipedata[599].level = 8;
recipedata[599].resultCount = 1;
recipedata[599].mpCost = 201;
recipedata[599].successRate = 60;

// recipe id 600
strcpy( recipedata[600].name, "mk_dark_legion's_edge_ii\0" );
recipedata[600].itemID = 2500;
recipedata[600].rec_itemID = 5465;
recipedata[600].level = 8;
recipedata[600].resultCount = 1;
recipedata[600].mpCost = 201;
recipedata[600].successRate = 100;

// recipe id 601
strcpy( recipedata[601].name, "mk_sword_of_miracle_i\0" );
recipedata[601].itemID = 151;
recipedata[601].rec_itemID = 5466;
recipedata[601].level = 8;
recipedata[601].resultCount = 1;
recipedata[601].mpCost = 201;
recipedata[601].successRate = 60;

// recipe id 602
strcpy( recipedata[602].name, "mk_sword_of_miracle_ii\0" );
recipedata[602].itemID = 151;
recipedata[602].rec_itemID = 5467;
recipedata[602].level = 8;
recipedata[602].resultCount = 1;
recipedata[602].mpCost = 201;
recipedata[602].successRate = 100;

// recipe id 603
strcpy( recipedata[603].name, "mk_elemental_sword_i\0" );
recipedata[603].itemID = 150;
recipedata[603].rec_itemID = 5468;
recipedata[603].level = 7;
recipedata[603].resultCount = 1;
recipedata[603].mpCost = 192;
recipedata[603].successRate = 60;

// recipe id 604
strcpy( recipedata[604].name, "mk_elemental_sword_ii\0" );
recipedata[604].itemID = 150;
recipedata[604].rec_itemID = 5469;
recipedata[604].level = 7;
recipedata[604].resultCount = 1;
recipedata[604].mpCost = 192;
recipedata[604].successRate = 100;

// recipe id 605
strcpy( recipedata[605].name, "mk_tallum_blade_i\0" );
recipedata[605].itemID = 80;
recipedata[605].rec_itemID = 5470;
recipedata[605].level = 7;
recipedata[605].resultCount = 1;
recipedata[605].mpCost = 192;
recipedata[605].successRate = 60;

// recipe id 606
strcpy( recipedata[606].name, "mk_tallum_blade_ii\0" );
recipedata[606].itemID = 80;
recipedata[606].rec_itemID = 5471;
recipedata[606].level = 7;
recipedata[606].resultCount = 1;
recipedata[606].mpCost = 192;
recipedata[606].successRate = 100;

// recipe id 607
strcpy( recipedata[607].name, "mk_iron_thread\0" );
recipedata[607].itemID = 5549;
recipedata[607].rec_itemID = 5472;
recipedata[607].level = 4;
recipedata[607].resultCount = 1;
recipedata[607].mpCost = 40;
recipedata[607].successRate = 100;

// recipe id 608
strcpy( recipedata[608].name, "mk_reinforcing_plate\0" );
recipedata[608].itemID = 5550;
recipedata[608].rec_itemID = 5473;
recipedata[608].level = 4;
recipedata[608].resultCount = 1;
recipedata[608].mpCost = 40;
recipedata[608].successRate = 100;

// recipe id 609
strcpy( recipedata[609].name, "mk_reorins_mold\0" );
recipedata[609].itemID = 5551;
recipedata[609].rec_itemID = 5474;
recipedata[609].level = 5;
recipedata[609].resultCount = 1;
recipedata[609].mpCost = 50;
recipedata[609].successRate = 100;

// recipe id 610
strcpy( recipedata[610].name, "mk_warsmith_mold\0" );
recipedata[610].itemID = 5552;
recipedata[610].rec_itemID = 5475;
recipedata[610].level = 6;
recipedata[610].resultCount = 1;
recipedata[610].mpCost = 60;
recipedata[610].successRate = 100;

// recipe id 611
strcpy( recipedata[611].name, "mk_arcsmith_anvil\0" );
recipedata[611].itemID = 5553;
recipedata[611].rec_itemID = 5476;
recipedata[611].level = 6;
recipedata[611].resultCount = 1;
recipedata[611].mpCost = 60;
recipedata[611].successRate = 100;

// recipe id 612
strcpy( recipedata[612].name, "mk_warsmith_holder\0" );
recipedata[612].itemID = 5554;
recipedata[612].rec_itemID = 5477;
recipedata[612].level = 6;
recipedata[612].resultCount = 1;
recipedata[612].mpCost = 60;
recipedata[612].successRate = 100;

// recipe id 613
strcpy( recipedata[613].name, "mk_sealed_phoenix's_earing_i\0" );
recipedata[613].itemID = 6324;
recipedata[613].rec_itemID = 6331;
recipedata[613].level = 7;
recipedata[613].resultCount = 1;
recipedata[613].mpCost = 192;
recipedata[613].successRate = 70;

// recipe id 614
strcpy( recipedata[614].name, "mk_sealed_phoenix's_earing_ii\0" );
recipedata[614].itemID = 6324;
recipedata[614].rec_itemID = 6332;
recipedata[614].level = 7;
recipedata[614].resultCount = 1;
recipedata[614].mpCost = 192;
recipedata[614].successRate = 100;

// recipe id 615
strcpy( recipedata[615].name, "mk_sealed_majestic_earing_i\0" );
recipedata[615].itemID = 6327;
recipedata[615].rec_itemID = 6337;
recipedata[615].level = 8;
recipedata[615].resultCount = 1;
recipedata[615].mpCost = 201;
recipedata[615].successRate = 70;

// recipe id 616
strcpy( recipedata[616].name, "mk_sealed_majestic_earing_ii\0" );
recipedata[616].itemID = 6327;
recipedata[616].rec_itemID = 6338;
recipedata[616].level = 8;
recipedata[616].resultCount = 1;
recipedata[616].mpCost = 201;
recipedata[616].successRate = 100;

// recipe id 617
strcpy( recipedata[617].name, "mk_sealed_phoenix's_necklace_i\0" );
recipedata[617].itemID = 6323;
recipedata[617].rec_itemID = 6329;
recipedata[617].level = 7;
recipedata[617].resultCount = 1;
recipedata[617].mpCost = 192;
recipedata[617].successRate = 70;

// recipe id 618
strcpy( recipedata[618].name, "mk_sealed_phoenix's_necklace_ii\0" );
recipedata[618].itemID = 6323;
recipedata[618].rec_itemID = 6330;
recipedata[618].level = 7;
recipedata[618].resultCount = 1;
recipedata[618].mpCost = 192;
recipedata[618].successRate = 100;

// recipe id 619
strcpy( recipedata[619].name, "mk_sealed_majestic_necklace_i\0" );
recipedata[619].itemID = 6326;
recipedata[619].rec_itemID = 6335;
recipedata[619].level = 8;
recipedata[619].resultCount = 1;
recipedata[619].mpCost = 201;
recipedata[619].successRate = 70;

// recipe id 620
strcpy( recipedata[620].name, "mk_sealed_majestic_necklace_ii\0" );
recipedata[620].itemID = 6326;
recipedata[620].rec_itemID = 6336;
recipedata[620].level = 8;
recipedata[620].resultCount = 1;
recipedata[620].mpCost = 201;
recipedata[620].successRate = 100;

// recipe id 621
strcpy( recipedata[621].name, "mk_sealed_phoenix's_ring_i\0" );
recipedata[621].itemID = 6325;
recipedata[621].rec_itemID = 6333;
recipedata[621].level = 7;
recipedata[621].resultCount = 1;
recipedata[621].mpCost = 192;
recipedata[621].successRate = 70;

// recipe id 622
strcpy( recipedata[622].name, "mk_sealed_phoenix's_ring_ii\0" );
recipedata[622].itemID = 6325;
recipedata[622].rec_itemID = 6334;
recipedata[622].level = 7;
recipedata[622].resultCount = 1;
recipedata[622].mpCost = 192;
recipedata[622].successRate = 100;

// recipe id 623
strcpy( recipedata[623].name, "mk_sealed_majestic_ring_i\0" );
recipedata[623].itemID = 6328;
recipedata[623].rec_itemID = 6339;
recipedata[623].level = 8;
recipedata[623].resultCount = 1;
recipedata[623].mpCost = 201;
recipedata[623].successRate = 70;

// recipe id 624
strcpy( recipedata[624].name, "mk_sealed_majestic_ring_ii\0" );
recipedata[624].itemID = 6328;
recipedata[624].rec_itemID = 6340;
recipedata[624].level = 8;
recipedata[624].resultCount = 1;
recipedata[624].mpCost = 201;
recipedata[624].successRate = 100;

// recipe id 625
strcpy( recipedata[625].name, "mk_shining_arrow\0" );
recipedata[625].itemID = 1345;
recipedata[625].rec_itemID = 6901;
recipedata[625].level = 5;
recipedata[625].resultCount = 500;
recipedata[625].mpCost = 150;
recipedata[625].successRate = 100;

// recipe id 626
strcpy( recipedata[626].name, "mk_forgotten_blade_i\0" );
recipedata[626].itemID = 6364;
recipedata[626].rec_itemID = 6881;
recipedata[626].level = 9;
recipedata[626].resultCount = 1;
recipedata[626].mpCost = 225;
recipedata[626].successRate = 60;

// recipe id 627
strcpy( recipedata[627].name, "mk_forgotten_blade_ii\0" );
recipedata[627].itemID = 6364;
recipedata[627].rec_itemID = 6882;
recipedata[627].level = 9;
recipedata[627].resultCount = 1;
recipedata[627].mpCost = 225;
recipedata[627].successRate = 100;

// recipe id 628
strcpy( recipedata[628].name, "mk_basalt_battlehammer_i\0" );
recipedata[628].itemID = 6365;
recipedata[628].rec_itemID = 6883;
recipedata[628].level = 9;
recipedata[628].resultCount = 1;
recipedata[628].mpCost = 225;
recipedata[628].successRate = 60;

// recipe id 629
strcpy( recipedata[629].name, "mk_basalt_battlehammer_ii\0" );
recipedata[629].itemID = 6365;
recipedata[629].rec_itemID = 6884;
recipedata[629].level = 9;
recipedata[629].resultCount = 1;
recipedata[629].mpCost = 225;
recipedata[629].successRate = 100;

// recipe id 630
strcpy( recipedata[630].name, "mk_imperial_staff_i\0" );
recipedata[630].itemID = 6366;
recipedata[630].rec_itemID = 6885;
recipedata[630].level = 9;
recipedata[630].resultCount = 1;
recipedata[630].mpCost = 225;
recipedata[630].successRate = 60;

// recipe id 631
strcpy( recipedata[631].name, "mk_imperial_staff_ii\0" );
recipedata[631].itemID = 6366;
recipedata[631].rec_itemID = 6886;
recipedata[631].level = 9;
recipedata[631].resultCount = 1;
recipedata[631].mpCost = 225;
recipedata[631].successRate = 100;

// recipe id 632
strcpy( recipedata[632].name, "mk_angel_slayer_i\0" );
recipedata[632].itemID = 6367;
recipedata[632].rec_itemID = 6887;
recipedata[632].level = 9;
recipedata[632].resultCount = 1;
recipedata[632].mpCost = 225;
recipedata[632].successRate = 60;

// recipe id 633
strcpy( recipedata[633].name, "mk_angel_slayer_ii\0" );
recipedata[633].itemID = 6367;
recipedata[633].rec_itemID = 6888;
recipedata[633].level = 9;
recipedata[633].resultCount = 1;
recipedata[633].mpCost = 225;
recipedata[633].successRate = 100;

// recipe id 634
strcpy( recipedata[634].name, "mk_shining_bow_i\0" );
recipedata[634].itemID = 6368;
recipedata[634].rec_itemID = 6889;
recipedata[634].level = 9;
recipedata[634].resultCount = 1;
recipedata[634].mpCost = 225;
recipedata[634].successRate = 60;

// recipe id 635
strcpy( recipedata[635].name, "mk_shining_bow_ii\0" );
recipedata[635].itemID = 6368;
recipedata[635].rec_itemID = 6890;
recipedata[635].level = 9;
recipedata[635].resultCount = 1;
recipedata[635].mpCost = 225;
recipedata[635].successRate = 100;

// recipe id 636
strcpy( recipedata[636].name, "mk_dragon_hunter_axe_i\0" );
recipedata[636].itemID = 6369;
recipedata[636].rec_itemID = 6891;
recipedata[636].level = 9;
recipedata[636].resultCount = 1;
recipedata[636].mpCost = 225;
recipedata[636].successRate = 60;

// recipe id 637
strcpy( recipedata[637].name, "mk_dragon_hunter_axe_ii\0" );
recipedata[637].itemID = 6369;
recipedata[637].rec_itemID = 6892;
recipedata[637].level = 9;
recipedata[637].resultCount = 1;
recipedata[637].mpCost = 225;
recipedata[637].successRate = 100;

// recipe id 638
strcpy( recipedata[638].name, "mk_saint_spear_i\0" );
recipedata[638].itemID = 6370;
recipedata[638].rec_itemID = 6893;
recipedata[638].level = 9;
recipedata[638].resultCount = 1;
recipedata[638].mpCost = 225;
recipedata[638].successRate = 60;

// recipe id 639
strcpy( recipedata[639].name, "mk_saint_spear_ii\0" );
recipedata[639].itemID = 6370;
recipedata[639].rec_itemID = 6894;
recipedata[639].level = 9;
recipedata[639].resultCount = 1;
recipedata[639].mpCost = 225;
recipedata[639].successRate = 100;

// recipe id 640
strcpy( recipedata[640].name, "mk_demon_splinter_i\0" );
recipedata[640].itemID = 6371;
recipedata[640].rec_itemID = 6895;
recipedata[640].level = 9;
recipedata[640].resultCount = 1;
recipedata[640].mpCost = 225;
recipedata[640].successRate = 60;

// recipe id 641
strcpy( recipedata[641].name, "mk_demon_splinter_ii\0" );
recipedata[641].itemID = 6371;
recipedata[641].rec_itemID = 6896;
recipedata[641].level = 9;
recipedata[641].resultCount = 1;
recipedata[641].mpCost = 225;
recipedata[641].successRate = 100;

// recipe id 642
strcpy( recipedata[642].name, "mk_heavens_divider_i\0" );
recipedata[642].itemID = 6372;
recipedata[642].rec_itemID = 6897;
recipedata[642].level = 9;
recipedata[642].resultCount = 1;
recipedata[642].mpCost = 225;
recipedata[642].successRate = 60;

// recipe id 643
strcpy( recipedata[643].name, "mk_heavens_divider_ii\0" );
recipedata[643].itemID = 6372;
recipedata[643].rec_itemID = 6898;
recipedata[643].level = 9;
recipedata[643].resultCount = 1;
recipedata[643].mpCost = 225;
recipedata[643].successRate = 100;

// recipe id 644
strcpy( recipedata[644].name, "mk_arcana_mace_i\0" );
recipedata[644].itemID = 6579;
recipedata[644].rec_itemID = 6899;
recipedata[644].level = 9;
recipedata[644].resultCount = 1;
recipedata[644].mpCost = 225;
recipedata[644].successRate = 60;

// recipe id 645
strcpy( recipedata[645].name, "mk_arcana_mace_ii\0" );
recipedata[645].itemID = 6579;
recipedata[645].rec_itemID = 6900;
recipedata[645].level = 9;
recipedata[645].resultCount = 1;
recipedata[645].mpCost = 225;
recipedata[645].successRate = 100;

// recipe id 646
strcpy( recipedata[646].name, "mk_sealed_sanddragon's_earing_i\0" );
recipedata[646].itemID = 6724;
recipedata[646].rec_itemID = 6847;
recipedata[646].level = 9;
recipedata[646].resultCount = 1;
recipedata[646].mpCost = 225;
recipedata[646].successRate = 70;

// recipe id 647
strcpy( recipedata[647].name, "mk_sealed_sanddragon's_earing_ii\0" );
recipedata[647].itemID = 6724;
recipedata[647].rec_itemID = 6848;
recipedata[647].level = 9;
recipedata[647].resultCount = 1;
recipedata[647].mpCost = 225;
recipedata[647].successRate = 100;

// recipe id 648
strcpy( recipedata[648].name, "mk_sealed_ring_of_aurakyria_i\0" );
recipedata[648].itemID = 6725;
recipedata[648].rec_itemID = 6849;
recipedata[648].level = 9;
recipedata[648].resultCount = 1;
recipedata[648].mpCost = 225;
recipedata[648].successRate = 70;

// recipe id 649
strcpy( recipedata[649].name, "mk_sealed_ring_of_aurakyria_ii\0" );
recipedata[649].itemID = 6725;
recipedata[649].rec_itemID = 6850;
recipedata[649].level = 9;
recipedata[649].resultCount = 1;
recipedata[649].mpCost = 225;
recipedata[649].successRate = 100;

// recipe id 650
strcpy( recipedata[650].name, "mk_sealed_dragon_necklace_i\0" );
recipedata[650].itemID = 6726;
recipedata[650].rec_itemID = 6851;
recipedata[650].level = 9;
recipedata[650].resultCount = 1;
recipedata[650].mpCost = 225;
recipedata[650].successRate = 70;

// recipe id 651
strcpy( recipedata[651].name, "mk_sealed_dragon_necklace_ii\0" );
recipedata[651].itemID = 6726;
recipedata[651].rec_itemID = 6852;
recipedata[651].level = 9;
recipedata[651].resultCount = 1;
recipedata[651].mpCost = 225;
recipedata[651].successRate = 100;

// recipe id 652
strcpy( recipedata[652].name, "mk_sealed_imperial_crusader_armor_i\0" );
recipedata[652].itemID = 6674;
recipedata[652].rec_itemID = 6853;
recipedata[652].level = 9;
recipedata[652].resultCount = 1;
recipedata[652].mpCost = 225;
recipedata[652].successRate = 60;

// recipe id 653
strcpy( recipedata[653].name, "mk_sealed_imperial_crusader_armor_ii\0" );
recipedata[653].itemID = 6674;
recipedata[653].rec_itemID = 6854;
recipedata[653].level = 9;
recipedata[653].resultCount = 1;
recipedata[653].mpCost = 225;
recipedata[653].successRate = 100;

// recipe id 654
strcpy( recipedata[654].name, "mk_sealed_imperial_crusader_gaiters_i\0" );
recipedata[654].itemID = 6675;
recipedata[654].rec_itemID = 6855;
recipedata[654].level = 9;
recipedata[654].resultCount = 1;
recipedata[654].mpCost = 225;
recipedata[654].successRate = 60;

// recipe id 655
strcpy( recipedata[655].name, "mk_sealed_imperial_crusader_gaiters_ii\0" );
recipedata[655].itemID = 6675;
recipedata[655].rec_itemID = 6856;
recipedata[655].level = 9;
recipedata[655].resultCount = 1;
recipedata[655].mpCost = 225;
recipedata[655].successRate = 100;

// recipe id 656
strcpy( recipedata[656].name, "mk_sealed_imperial_crusader_gauntlet_i\0" );
recipedata[656].itemID = 6676;
recipedata[656].rec_itemID = 6857;
recipedata[656].level = 9;
recipedata[656].resultCount = 1;
recipedata[656].mpCost = 225;
recipedata[656].successRate = 60;

// recipe id 657
strcpy( recipedata[657].name, "mk_sealed_imperial_crusader_gauntlet_ii\0" );
recipedata[657].itemID = 6676;
recipedata[657].rec_itemID = 6858;
recipedata[657].level = 9;
recipedata[657].resultCount = 1;
recipedata[657].mpCost = 225;
recipedata[657].successRate = 100;

// recipe id 658
strcpy( recipedata[658].name, "mk_sealed_imperial_crusader_boots_i\0" );
recipedata[658].itemID = 6677;
recipedata[658].rec_itemID = 6859;
recipedata[658].level = 9;
recipedata[658].resultCount = 1;
recipedata[658].mpCost = 225;
recipedata[658].successRate = 60;

// recipe id 659
strcpy( recipedata[659].name, "mk_sealed_imperial_crusader_boots_ii\0" );
recipedata[659].itemID = 6677;
recipedata[659].rec_itemID = 6860;
recipedata[659].level = 9;
recipedata[659].resultCount = 1;
recipedata[659].mpCost = 225;
recipedata[659].successRate = 100;

// recipe id 660
strcpy( recipedata[660].name, "mk_sealed_imperial_crusader_shield_i\0" );
recipedata[660].itemID = 6678;
recipedata[660].rec_itemID = 6861;
recipedata[660].level = 9;
recipedata[660].resultCount = 1;
recipedata[660].mpCost = 225;
recipedata[660].successRate = 60;

// recipe id 661
strcpy( recipedata[661].name, "mk_sealed_imperial_crusader_shield_ii\0" );
recipedata[661].itemID = 6678;
recipedata[661].rec_itemID = 6862;
recipedata[661].level = 9;
recipedata[661].resultCount = 1;
recipedata[661].mpCost = 225;
recipedata[661].successRate = 100;

// recipe id 662
strcpy( recipedata[662].name, "mk_sealed_imperial_crusader_helmet_i\0" );
recipedata[662].itemID = 6679;
recipedata[662].rec_itemID = 6863;
recipedata[662].level = 9;
recipedata[662].resultCount = 1;
recipedata[662].mpCost = 225;
recipedata[662].successRate = 60;

// recipe id 663
strcpy( recipedata[663].name, "mk_sealed_imperial_crusader_helmet_ii\0" );
recipedata[663].itemID = 6679;
recipedata[663].rec_itemID = 6864;
recipedata[663].level = 9;
recipedata[663].resultCount = 1;
recipedata[663].mpCost = 225;
recipedata[663].successRate = 100;

// recipe id 664
strcpy( recipedata[664].name, "mk_sealed_draconic_leather_armor_i\0" );
recipedata[664].itemID = 6680;
recipedata[664].rec_itemID = 6865;
recipedata[664].level = 9;
recipedata[664].resultCount = 1;
recipedata[664].mpCost = 225;
recipedata[664].successRate = 60;

// recipe id 665
strcpy( recipedata[665].name, "mk_sealed_draconic_leather_armor_ii\0" );
recipedata[665].itemID = 6680;
recipedata[665].rec_itemID = 6866;
recipedata[665].level = 9;
recipedata[665].resultCount = 1;
recipedata[665].mpCost = 225;
recipedata[665].successRate = 100;

// recipe id 666
strcpy( recipedata[666].name, "mk_sealed_draconic_leather_gloves_i\0" );
recipedata[666].itemID = 6681;
recipedata[666].rec_itemID = 6867;
recipedata[666].level = 9;
recipedata[666].resultCount = 1;
recipedata[666].mpCost = 225;
recipedata[666].successRate = 60;

// recipe id 667
strcpy( recipedata[667].name, "mk_sealed_draconic_leather_gloves_ii\0" );
recipedata[667].itemID = 6681;
recipedata[667].rec_itemID = 6868;
recipedata[667].level = 9;
recipedata[667].resultCount = 1;
recipedata[667].mpCost = 225;
recipedata[667].successRate = 100;

// recipe id 668
strcpy( recipedata[668].name, "mk_sealed_draconic_leather_boots_i\0" );
recipedata[668].itemID = 6682;
recipedata[668].rec_itemID = 6869;
recipedata[668].level = 9;
recipedata[668].resultCount = 1;
recipedata[668].mpCost = 225;
recipedata[668].successRate = 60;

// recipe id 669
strcpy( recipedata[669].name, "mk_sealed_draconic_leather_boots_ii\0" );
recipedata[669].itemID = 6682;
recipedata[669].rec_itemID = 6870;
recipedata[669].level = 9;
recipedata[669].resultCount = 1;
recipedata[669].mpCost = 225;
recipedata[669].successRate = 100;

// recipe id 670
strcpy( recipedata[670].name, "mk_sealed_draconic_leather_helmet_i\0" );
recipedata[670].itemID = 6683;
recipedata[670].rec_itemID = 6871;
recipedata[670].level = 9;
recipedata[670].resultCount = 1;
recipedata[670].mpCost = 225;
recipedata[670].successRate = 60;

// recipe id 671
strcpy( recipedata[671].name, "mk_sealed_draconic_leather_helmet_ii\0" );
recipedata[671].itemID = 6683;
recipedata[671].rec_itemID = 6872;
recipedata[671].level = 9;
recipedata[671].resultCount = 1;
recipedata[671].mpCost = 225;
recipedata[671].successRate = 100;

// recipe id 672
strcpy( recipedata[672].name, "mk_sealed_major_arcana_robe_i\0" );
recipedata[672].itemID = 6684;
recipedata[672].rec_itemID = 6873;
recipedata[672].level = 9;
recipedata[672].resultCount = 1;
recipedata[672].mpCost = 225;
recipedata[672].successRate = 60;

// recipe id 673
strcpy( recipedata[673].name, "mk_sealed_major_arcana_robe_ii\0" );
recipedata[673].itemID = 6684;
recipedata[673].rec_itemID = 6874;
recipedata[673].level = 9;
recipedata[673].resultCount = 1;
recipedata[673].mpCost = 225;
recipedata[673].successRate = 100;

// recipe id 674
strcpy( recipedata[674].name, "mk_sealed_major_arcana_gloves_i\0" );
recipedata[674].itemID = 6685;
recipedata[674].rec_itemID = 6875;
recipedata[674].level = 9;
recipedata[674].resultCount = 1;
recipedata[674].mpCost = 225;
recipedata[674].successRate = 60;

// recipe id 675
strcpy( recipedata[675].name, "mk_sealed_major_arcana_gloves_ii\0" );
recipedata[675].itemID = 6685;
recipedata[675].rec_itemID = 6876;
recipedata[675].level = 9;
recipedata[675].resultCount = 1;
recipedata[675].mpCost = 225;
recipedata[675].successRate = 100;

// recipe id 676
strcpy( recipedata[676].name, "mk_sealed_major_arcana_boots_i\0" );
recipedata[676].itemID = 6686;
recipedata[676].rec_itemID = 6877;
recipedata[676].level = 9;
recipedata[676].resultCount = 1;
recipedata[676].mpCost = 225;
recipedata[676].successRate = 60;

// recipe id 677
strcpy( recipedata[677].name, "mk_sealed_major_arcana_boots_ii\0" );
recipedata[677].itemID = 6686;
recipedata[677].rec_itemID = 6878;
recipedata[677].level = 9;
recipedata[677].resultCount = 1;
recipedata[677].mpCost = 225;
recipedata[677].successRate = 100;

// recipe id 678
strcpy( recipedata[678].name, "mk_sealed_major_arcana_hood_i\0" );
recipedata[678].itemID = 6687;
recipedata[678].rec_itemID = 6879;
recipedata[678].level = 9;
recipedata[678].resultCount = 1;
recipedata[678].mpCost = 225;
recipedata[678].successRate = 60;

// recipe id 679
strcpy( recipedata[679].name, "mk_sealed_major_arcana_hood_ii\0" );
recipedata[679].itemID = 6687;
recipedata[679].rec_itemID = 6880;
recipedata[679].level = 9;
recipedata[679].resultCount = 1;
recipedata[679].mpCost = 225;
recipedata[679].successRate = 100;

// recipe id 680
strcpy( recipedata[680].name, "mk_fish_oil_average\0" );
recipedata[680].itemID = 6909;
recipedata[680].rec_itemID = 6920;
recipedata[680].level = 2;
recipedata[680].resultCount = 1;
recipedata[680].mpCost = 75;
recipedata[680].successRate = 100;

// recipe id 681
strcpy( recipedata[681].name, "mk_fish_oil_high\0" );
recipedata[681].itemID = 6910;
recipedata[681].rec_itemID = 6921;
recipedata[681].level = 3;
recipedata[681].resultCount = 1;
recipedata[681].mpCost = 99;
recipedata[681].successRate = 100;

// recipe id 682
strcpy( recipedata[682].name, "mk_party_mask\0" );
recipedata[682].itemID = 5808;
recipedata[682].rec_itemID = 6922;
recipedata[682].level = 4;
recipedata[682].resultCount = 1;
recipedata[682].mpCost = 138;
recipedata[682].successRate = 100;

// recipe id 683
strcpy( recipedata[683].name, "mk_hair_pin_of_lady\0" );
recipedata[683].itemID = 6844;
recipedata[683].rec_itemID = 6923;
recipedata[683].level = 4;
recipedata[683].resultCount = 1;
recipedata[683].mpCost = 138;
recipedata[683].successRate = 100;

// recipe id 684
strcpy( recipedata[684].name, "mk_eye_bandage_of_pirate\0" );
recipedata[684].itemID = 6845;
recipedata[684].rec_itemID = 6924;
recipedata[684].level = 4;
recipedata[684].resultCount = 1;
recipedata[684].mpCost = 138;
recipedata[684].successRate = 100;

// recipe id 685
strcpy( recipedata[685].name, "mk_monocle\0" );
recipedata[685].itemID = 6846;
recipedata[685].rec_itemID = 6925;
recipedata[685].level = 4;
recipedata[685].resultCount = 1;
recipedata[685].mpCost = 138;
recipedata[685].successRate = 100;

// recipe id 686
strcpy( recipedata[686].name, "mk_lesser_healing_potion\0" );
recipedata[686].itemID = 1060;
recipedata[686].rec_itemID = 6926;
recipedata[686].level = 1;
recipedata[686].resultCount = 1;
recipedata[686].mpCost = 30;
recipedata[686].successRate = 100;

// recipe id 687
strcpy( recipedata[687].name, "mk_healing_potion\0" );
recipedata[687].itemID = 1061;
recipedata[687].rec_itemID = 6927;
recipedata[687].level = 2;
recipedata[687].resultCount = 1;
recipedata[687].mpCost = 90;
recipedata[687].successRate = 100;

// recipe id 688
strcpy( recipedata[688].name, "mk_greater_healing_potion\0" );
recipedata[688].itemID = 1539;
recipedata[688].rec_itemID = 6928;
recipedata[688].level = 5;
recipedata[688].resultCount = 1;
recipedata[688].mpCost = 150;
recipedata[688].successRate = 100;

// recipe id 689
strcpy( recipedata[689].name, "mk_antidote\0" );
recipedata[689].itemID = 1831;
recipedata[689].rec_itemID = 6929;
recipedata[689].level = 1;
recipedata[689].resultCount = 1;
recipedata[689].mpCost = 45;
recipedata[689].successRate = 100;

// recipe id 690
strcpy( recipedata[690].name, "mk_advanced_antidote\0" );
recipedata[690].itemID = 1832;
recipedata[690].rec_itemID = 6930;
recipedata[690].level = 2;
recipedata[690].resultCount = 1;
recipedata[690].mpCost = 90;
recipedata[690].successRate = 100;

// recipe id 691
strcpy( recipedata[691].name, "mk_bandage\0" );
recipedata[691].itemID = 1833;
recipedata[691].rec_itemID = 6931;
recipedata[691].level = 1;
recipedata[691].resultCount = 1;
recipedata[691].mpCost = 45;
recipedata[691].successRate = 100;

// recipe id 692
strcpy( recipedata[692].name, "mk_emergency_dressing\0" );
recipedata[692].itemID = 1834;
recipedata[692].rec_itemID = 6932;
recipedata[692].level = 2;
recipedata[692].resultCount = 1;
recipedata[692].mpCost = 90;
recipedata[692].successRate = 100;

// recipe id 693
strcpy( recipedata[693].name, "mk_quick_step_potion\0" );
recipedata[693].itemID = 734;
recipedata[693].rec_itemID = 6933;
recipedata[693].level = 1;
recipedata[693].resultCount = 1;
recipedata[693].mpCost = 60;
recipedata[693].successRate = 100;

// recipe id 694
strcpy( recipedata[694].name, "mk_swift_attack_potion\0" );
recipedata[694].itemID = 735;
recipedata[694].rec_itemID = 6934;
recipedata[694].level = 1;
recipedata[694].resultCount = 1;
recipedata[694].mpCost = 60;
recipedata[694].successRate = 100;

// recipe id 695
strcpy( recipedata[695].name, "mk_adv_quick_step_potion\0" );
recipedata[695].itemID = 1374;
recipedata[695].rec_itemID = 6935;
recipedata[695].level = 3;
recipedata[695].resultCount = 1;
recipedata[695].mpCost = 120;
recipedata[695].successRate = 100;

// recipe id 696
strcpy( recipedata[696].name, "mk_adv_swift_attack_potion\0" );
recipedata[696].itemID = 1375;
recipedata[696].rec_itemID = 6936;
recipedata[696].level = 3;
recipedata[696].resultCount = 1;
recipedata[696].mpCost = 120;
recipedata[696].successRate = 100;

// recipe id 697
strcpy( recipedata[697].name, "mk_potion_of_acumen2\0" );
recipedata[697].itemID = 6035;
recipedata[697].rec_itemID = 6937;
recipedata[697].level = 1;
recipedata[697].resultCount = 1;
recipedata[697].mpCost = 60;
recipedata[697].successRate = 100;

// recipe id 698
strcpy( recipedata[698].name, "mk_potion_of_acumen3\0" );
recipedata[698].itemID = 6036;
recipedata[698].rec_itemID = 6938;
recipedata[698].level = 3;
recipedata[698].resultCount = 1;
recipedata[698].mpCost = 120;
recipedata[698].successRate = 100;

// recipe id 699
strcpy( recipedata[699].name, "mk_bighead_potion\0" );
recipedata[699].itemID = 5234;
recipedata[699].rec_itemID = 6939;
recipedata[699].level = 2;
recipedata[699].resultCount = 1;
recipedata[699].mpCost = 96;
recipedata[699].successRate = 100;

// recipe id 700
strcpy( recipedata[700].name, "mk_masktype_a_potion\0" );
recipedata[700].itemID = 5235;
recipedata[700].rec_itemID = 6940;
recipedata[700].level = 3;
recipedata[700].resultCount = 1;
recipedata[700].mpCost = 114;
recipedata[700].successRate = 100;

// recipe id 701
strcpy( recipedata[701].name, "mk_masktype_b_potion\0" );
recipedata[701].itemID = 5236;
recipedata[701].rec_itemID = 6941;
recipedata[701].level = 3;
recipedata[701].resultCount = 1;
recipedata[701].mpCost = 114;
recipedata[701].successRate = 100;

// recipe id 702
strcpy( recipedata[702].name, "mk_masktype_c_potion\0" );
recipedata[702].itemID = 5237;
recipedata[702].rec_itemID = 6942;
recipedata[702].level = 3;
recipedata[702].resultCount = 1;
recipedata[702].mpCost = 114;
recipedata[702].successRate = 100;

// recipe id 703
strcpy( recipedata[703].name, "mk_haircolor_a_potion\0" );
recipedata[703].itemID = 5238;
recipedata[703].rec_itemID = 6943;
recipedata[703].level = 3;
recipedata[703].resultCount = 1;
recipedata[703].mpCost = 114;
recipedata[703].successRate = 100;

// recipe id 704
strcpy( recipedata[704].name, "mk_haircolor_b_potion\0" );
recipedata[704].itemID = 5239;
recipedata[704].rec_itemID = 6944;
recipedata[704].level = 3;
recipedata[704].resultCount = 1;
recipedata[704].mpCost = 114;
recipedata[704].successRate = 100;

// recipe id 705
strcpy( recipedata[705].name, "mk_haircolor_c_potion\0" );
recipedata[705].itemID = 5240;
recipedata[705].rec_itemID = 6945;
recipedata[705].level = 3;
recipedata[705].resultCount = 1;
recipedata[705].mpCost = 114;
recipedata[705].successRate = 100;

// recipe id 706
strcpy( recipedata[706].name, "mk_haircolor_d_potion\0" );
recipedata[706].itemID = 5241;
recipedata[706].rec_itemID = 6946;
recipedata[706].level = 3;
recipedata[706].resultCount = 1;
recipedata[706].mpCost = 114;
recipedata[706].successRate = 100;

// recipe id 707
strcpy( recipedata[707].name, "mk_hairstyle_a_potion\0" );
recipedata[707].itemID = 5242;
recipedata[707].rec_itemID = 6947;
recipedata[707].level = 3;
recipedata[707].resultCount = 1;
recipedata[707].mpCost = 114;
recipedata[707].successRate = 100;

// recipe id 708
strcpy( recipedata[708].name, "mk_hairstyle_b_potion\0" );
recipedata[708].itemID = 5243;
recipedata[708].rec_itemID = 6948;
recipedata[708].level = 3;
recipedata[708].resultCount = 1;
recipedata[708].mpCost = 114;
recipedata[708].successRate = 100;

// recipe id 709
strcpy( recipedata[709].name, "mk_hairstyle_c_potion\0" );
recipedata[709].itemID = 5244;
recipedata[709].rec_itemID = 6949;
recipedata[709].level = 3;
recipedata[709].resultCount = 1;
recipedata[709].mpCost = 114;
recipedata[709].successRate = 100;

// recipe id 710
strcpy( recipedata[710].name, "mk_hairstyle_d_potion\0" );
recipedata[710].itemID = 5245;
recipedata[710].rec_itemID = 6950;
recipedata[710].level = 3;
recipedata[710].resultCount = 1;
recipedata[710].mpCost = 114;
recipedata[710].successRate = 100;

// recipe id 711
strcpy( recipedata[711].name, "mk_hairstyle_e_potion\0" );
recipedata[711].itemID = 5246;
recipedata[711].rec_itemID = 6951;
recipedata[711].level = 3;
recipedata[711].resultCount = 1;
recipedata[711].mpCost = 114;
recipedata[711].successRate = 100;

// recipe id 712
strcpy( recipedata[712].name, "mk_hairstyle_f_potion\0" );
recipedata[712].itemID = 5247;
recipedata[712].rec_itemID = 6952;
recipedata[712].level = 3;
recipedata[712].resultCount = 1;
recipedata[712].mpCost = 114;
recipedata[712].successRate = 100;

// recipe id 713
strcpy( recipedata[713].name, "mk_hairstyle_g_potion\0" );
recipedata[713].itemID = 5248;
recipedata[713].rec_itemID = 6953;
recipedata[713].level = 3;
recipedata[713].resultCount = 1;
recipedata[713].mpCost = 114;
recipedata[713].successRate = 100;

// recipe id 714
strcpy( recipedata[714].name, "mk_dye_s1c1_d\0" );
recipedata[714].itemID = 4469;
recipedata[714].rec_itemID = 6954;
recipedata[714].level = 4;
recipedata[714].resultCount = 1;
recipedata[714].mpCost = 126;
recipedata[714].successRate = 100;

// recipe id 715
strcpy( recipedata[715].name, "mk_dye_s1d1_d\0" );
recipedata[715].itemID = 4470;
recipedata[715].rec_itemID = 6955;
recipedata[715].level = 4;
recipedata[715].resultCount = 1;
recipedata[715].mpCost = 126;
recipedata[715].successRate = 100;

// recipe id 716
strcpy( recipedata[716].name, "mk_dye_c1s1_d\0" );
recipedata[716].itemID = 4471;
recipedata[716].rec_itemID = 6956;
recipedata[716].level = 4;
recipedata[716].resultCount = 1;
recipedata[716].mpCost = 126;
recipedata[716].successRate = 100;

// recipe id 717
strcpy( recipedata[717].name, "mk_dye_c1d1_d\0" );
recipedata[717].itemID = 4472;
recipedata[717].rec_itemID = 6957;
recipedata[717].level = 4;
recipedata[717].resultCount = 1;
recipedata[717].mpCost = 126;
recipedata[717].successRate = 100;

// recipe id 718
strcpy( recipedata[718].name, "mk_dye_d1s1_d\0" );
recipedata[718].itemID = 4473;
recipedata[718].rec_itemID = 6958;
recipedata[718].level = 4;
recipedata[718].resultCount = 1;
recipedata[718].mpCost = 126;
recipedata[718].successRate = 100;

// recipe id 719
strcpy( recipedata[719].name, "mk_dye_d1c1_d\0" );
recipedata[719].itemID = 4474;
recipedata[719].rec_itemID = 6959;
recipedata[719].level = 4;
recipedata[719].resultCount = 1;
recipedata[719].mpCost = 126;
recipedata[719].successRate = 100;

// recipe id 720
strcpy( recipedata[720].name, "mk_dye_i1m1_d\0" );
recipedata[720].itemID = 4475;
recipedata[720].rec_itemID = 6960;
recipedata[720].level = 4;
recipedata[720].resultCount = 1;
recipedata[720].mpCost = 126;
recipedata[720].successRate = 100;

// recipe id 721
strcpy( recipedata[721].name, "mk_dye_i1w1_d\0" );
recipedata[721].itemID = 4476;
recipedata[721].rec_itemID = 6961;
recipedata[721].level = 4;
recipedata[721].resultCount = 1;
recipedata[721].mpCost = 126;
recipedata[721].successRate = 100;

// recipe id 722
strcpy( recipedata[722].name, "mk_dye_m1i1_d\0" );
recipedata[722].itemID = 4477;
recipedata[722].rec_itemID = 6962;
recipedata[722].level = 4;
recipedata[722].resultCount = 1;
recipedata[722].mpCost = 126;
recipedata[722].successRate = 100;

// recipe id 723
strcpy( recipedata[723].name, "mk_dye_m1w1_d\0" );
recipedata[723].itemID = 4478;
recipedata[723].rec_itemID = 6963;
recipedata[723].level = 4;
recipedata[723].resultCount = 1;
recipedata[723].mpCost = 126;
recipedata[723].successRate = 100;

// recipe id 724
strcpy( recipedata[724].name, "mk_dye_w1i1_d\0" );
recipedata[724].itemID = 4479;
recipedata[724].rec_itemID = 6964;
recipedata[724].level = 4;
recipedata[724].resultCount = 1;
recipedata[724].mpCost = 126;
recipedata[724].successRate = 100;

// recipe id 725
strcpy( recipedata[725].name, "mk_dye_w1m1_d\0" );
recipedata[725].itemID = 4480;
recipedata[725].rec_itemID = 6965;
recipedata[725].level = 4;
recipedata[725].resultCount = 1;
recipedata[725].mpCost = 126;
recipedata[725].successRate = 100;

// recipe id 726
strcpy( recipedata[726].name, "mk_dye_s1c1_c\0" );
recipedata[726].itemID = 4553;
recipedata[726].rec_itemID = 6966;
recipedata[726].level = 4;
recipedata[726].resultCount = 1;
recipedata[726].mpCost = 138;
recipedata[726].successRate = 100;

// recipe id 727
strcpy( recipedata[727].name, "mk_dye_s1d1_c\0" );
recipedata[727].itemID = 4554;
recipedata[727].rec_itemID = 6967;
recipedata[727].level = 4;
recipedata[727].resultCount = 1;
recipedata[727].mpCost = 138;
recipedata[727].successRate = 100;

// recipe id 728
strcpy( recipedata[728].name, "mk_dye_c1s1_c\0" );
recipedata[728].itemID = 4555;
recipedata[728].rec_itemID = 6968;
recipedata[728].level = 4;
recipedata[728].resultCount = 1;
recipedata[728].mpCost = 138;
recipedata[728].successRate = 100;

// recipe id 729
strcpy( recipedata[729].name, "mk_dye_c1c1_c\0" );
recipedata[729].itemID = 4556;
recipedata[729].rec_itemID = 6969;
recipedata[729].level = 4;
recipedata[729].resultCount = 1;
recipedata[729].mpCost = 138;
recipedata[729].successRate = 100;

// recipe id 730
strcpy( recipedata[730].name, "mk_dye_d1s1_c\0" );
recipedata[730].itemID = 4557;
recipedata[730].rec_itemID = 6970;
recipedata[730].level = 4;
recipedata[730].resultCount = 1;
recipedata[730].mpCost = 138;
recipedata[730].successRate = 100;

// recipe id 731
strcpy( recipedata[731].name, "mk_dye_d1c1_c\0" );
recipedata[731].itemID = 4558;
recipedata[731].rec_itemID = 6971;
recipedata[731].level = 4;
recipedata[731].resultCount = 1;
recipedata[731].mpCost = 138;
recipedata[731].successRate = 100;

// recipe id 732
strcpy( recipedata[732].name, "mk_dye_i1m1_c\0" );
recipedata[732].itemID = 4559;
recipedata[732].rec_itemID = 6972;
recipedata[732].level = 4;
recipedata[732].resultCount = 1;
recipedata[732].mpCost = 138;
recipedata[732].successRate = 100;

// recipe id 733
strcpy( recipedata[733].name, "mk_dye_i1w1_c\0" );
recipedata[733].itemID = 4560;
recipedata[733].rec_itemID = 6973;
recipedata[733].level = 4;
recipedata[733].resultCount = 1;
recipedata[733].mpCost = 138;
recipedata[733].successRate = 100;

// recipe id 734
strcpy( recipedata[734].name, "mk_dye_m1i1_c\0" );
recipedata[734].itemID = 4561;
recipedata[734].rec_itemID = 6974;
recipedata[734].level = 4;
recipedata[734].resultCount = 1;
recipedata[734].mpCost = 138;
recipedata[734].successRate = 100;

// recipe id 735
strcpy( recipedata[735].name, "mk_dye_m1w1_c\0" );
recipedata[735].itemID = 4562;
recipedata[735].rec_itemID = 6975;
recipedata[735].level = 4;
recipedata[735].resultCount = 1;
recipedata[735].mpCost = 138;
recipedata[735].successRate = 100;

// recipe id 736
strcpy( recipedata[736].name, "mk_dye_w1i1_c\0" );
recipedata[736].itemID = 4563;
recipedata[736].rec_itemID = 6976;
recipedata[736].level = 4;
recipedata[736].resultCount = 1;
recipedata[736].mpCost = 138;
recipedata[736].successRate = 100;

// recipe id 737
strcpy( recipedata[737].name, "mk_dye_w1m1_c\0" );
recipedata[737].itemID = 4564;
recipedata[737].rec_itemID = 6977;
recipedata[737].level = 4;
recipedata[737].resultCount = 1;
recipedata[737].mpCost = 138;
recipedata[737].successRate = 100;

// recipe id 738
strcpy( recipedata[738].name, "mk_dye_s2c2_c\0" );
recipedata[738].itemID = 4589;
recipedata[738].rec_itemID = 6978;
recipedata[738].level = 5;
recipedata[738].resultCount = 1;
recipedata[738].mpCost = 147;
recipedata[738].successRate = 100;

// recipe id 739
strcpy( recipedata[739].name, "mk_dye_s2d2_c\0" );
recipedata[739].itemID = 4590;
recipedata[739].rec_itemID = 6979;
recipedata[739].level = 5;
recipedata[739].resultCount = 1;
recipedata[739].mpCost = 147;
recipedata[739].successRate = 100;

// recipe id 740
strcpy( recipedata[740].name, "mk_dye_c2s2_c\0" );
recipedata[740].itemID = 4591;
recipedata[740].rec_itemID = 6980;
recipedata[740].level = 5;
recipedata[740].resultCount = 1;
recipedata[740].mpCost = 147;
recipedata[740].successRate = 100;

// recipe id 741
strcpy( recipedata[741].name, "mk_dye_c2c2_c\0" );
recipedata[741].itemID = 4592;
recipedata[741].rec_itemID = 6981;
recipedata[741].level = 5;
recipedata[741].resultCount = 1;
recipedata[741].mpCost = 147;
recipedata[741].successRate = 100;

// recipe id 742
strcpy( recipedata[742].name, "mk_dye_d2s2_c\0" );
recipedata[742].itemID = 4593;
recipedata[742].rec_itemID = 6982;
recipedata[742].level = 5;
recipedata[742].resultCount = 1;
recipedata[742].mpCost = 147;
recipedata[742].successRate = 100;

// recipe id 743
strcpy( recipedata[743].name, "mk_dye_d2c2_c\0" );
recipedata[743].itemID = 4594;
recipedata[743].rec_itemID = 6983;
recipedata[743].level = 5;
recipedata[743].resultCount = 1;
recipedata[743].mpCost = 147;
recipedata[743].successRate = 100;

// recipe id 744
strcpy( recipedata[744].name, "mk_dye_i2m2_c\0" );
recipedata[744].itemID = 4595;
recipedata[744].rec_itemID = 6984;
recipedata[744].level = 5;
recipedata[744].resultCount = 1;
recipedata[744].mpCost = 147;
recipedata[744].successRate = 100;

// recipe id 745
strcpy( recipedata[745].name, "mk_dye_i2w2_c\0" );
recipedata[745].itemID = 4596;
recipedata[745].rec_itemID = 6985;
recipedata[745].level = 5;
recipedata[745].resultCount = 1;
recipedata[745].mpCost = 147;
recipedata[745].successRate = 100;

// recipe id 746
strcpy( recipedata[746].name, "mk_dye_m2i2_c\0" );
recipedata[746].itemID = 4597;
recipedata[746].rec_itemID = 6986;
recipedata[746].level = 5;
recipedata[746].resultCount = 1;
recipedata[746].mpCost = 147;
recipedata[746].successRate = 100;

// recipe id 747
strcpy( recipedata[747].name, "mk_dye_m2w2_c\0" );
recipedata[747].itemID = 4598;
recipedata[747].rec_itemID = 6987;
recipedata[747].level = 5;
recipedata[747].resultCount = 1;
recipedata[747].mpCost = 147;
recipedata[747].successRate = 100;

// recipe id 748
strcpy( recipedata[748].name, "mk_dye_w2i2_c\0" );
recipedata[748].itemID = 4599;
recipedata[748].rec_itemID = 6988;
recipedata[748].level = 5;
recipedata[748].resultCount = 1;
recipedata[748].mpCost = 147;
recipedata[748].successRate = 100;

// recipe id 749
strcpy( recipedata[749].name, "mk_dye_w2m2_c\0" );
recipedata[749].itemID = 4600;
recipedata[749].rec_itemID = 6989;
recipedata[749].level = 5;
recipedata[749].resultCount = 1;
recipedata[749].mpCost = 147;
recipedata[749].successRate = 100;

// recipe id 750
strcpy( recipedata[750].name, "mk_dye_s3c3_c\0" );
recipedata[750].itemID = 4601;
recipedata[750].rec_itemID = 6990;
recipedata[750].level = 5;
recipedata[750].resultCount = 1;
recipedata[750].mpCost = 156;
recipedata[750].successRate = 100;

// recipe id 751
strcpy( recipedata[751].name, "mk_dye_s3d3_c\0" );
recipedata[751].itemID = 4602;
recipedata[751].rec_itemID = 6991;
recipedata[751].level = 5;
recipedata[751].resultCount = 1;
recipedata[751].mpCost = 156;
recipedata[751].successRate = 100;

// recipe id 752
strcpy( recipedata[752].name, "mk_dye_c3s3_c\0" );
recipedata[752].itemID = 4603;
recipedata[752].rec_itemID = 6992;
recipedata[752].level = 5;
recipedata[752].resultCount = 1;
recipedata[752].mpCost = 156;
recipedata[752].successRate = 100;

// recipe id 753
strcpy( recipedata[753].name, "mk_dye_c3c3_c\0" );
recipedata[753].itemID = 4604;
recipedata[753].rec_itemID = 6993;
recipedata[753].level = 5;
recipedata[753].resultCount = 1;
recipedata[753].mpCost = 156;
recipedata[753].successRate = 100;

// recipe id 754
strcpy( recipedata[754].name, "mk_dye_d3s3_c\0" );
recipedata[754].itemID = 4605;
recipedata[754].rec_itemID = 6994;
recipedata[754].level = 5;
recipedata[754].resultCount = 1;
recipedata[754].mpCost = 156;
recipedata[754].successRate = 100;

// recipe id 755
strcpy( recipedata[755].name, "mk_dye_d3c3_c\0" );
recipedata[755].itemID = 4606;
recipedata[755].rec_itemID = 6995;
recipedata[755].level = 5;
recipedata[755].resultCount = 1;
recipedata[755].mpCost = 156;
recipedata[755].successRate = 100;

// recipe id 756
strcpy( recipedata[756].name, "mk_dye_i3m3_c\0" );
recipedata[756].itemID = 4607;
recipedata[756].rec_itemID = 6996;
recipedata[756].level = 5;
recipedata[756].resultCount = 1;
recipedata[756].mpCost = 156;
recipedata[756].successRate = 100;

// recipe id 757
strcpy( recipedata[757].name, "mk_dye_i3w3_c\0" );
recipedata[757].itemID = 4608;
recipedata[757].rec_itemID = 6997;
recipedata[757].level = 5;
recipedata[757].resultCount = 1;
recipedata[757].mpCost = 156;
recipedata[757].successRate = 100;

// recipe id 758
strcpy( recipedata[758].name, "mk_dye_m3i3_c\0" );
recipedata[758].itemID = 4609;
recipedata[758].rec_itemID = 6998;
recipedata[758].level = 5;
recipedata[758].resultCount = 1;
recipedata[758].mpCost = 156;
recipedata[758].successRate = 100;

// recipe id 759
strcpy( recipedata[759].name, "mk_dye_m3w3_c\0" );
recipedata[759].itemID = 4610;
recipedata[759].rec_itemID = 6999;
recipedata[759].level = 5;
recipedata[759].resultCount = 1;
recipedata[759].mpCost = 156;
recipedata[759].successRate = 100;

// recipe id 760
strcpy( recipedata[760].name, "mk_dye_w3i3_c\0" );
recipedata[760].itemID = 4611;
recipedata[760].rec_itemID = 7000;
recipedata[760].level = 5;
recipedata[760].resultCount = 1;
recipedata[760].mpCost = 156;
recipedata[760].successRate = 100;

// recipe id 761
strcpy( recipedata[761].name, "mk_dye_w3m3_c\0" );
recipedata[761].itemID = 4612;
recipedata[761].rec_itemID = 7001;
recipedata[761].level = 5;
recipedata[761].resultCount = 1;
recipedata[761].mpCost = 156;
recipedata[761].successRate = 100;

// recipe id 762
strcpy( recipedata[762].name, "mk_dye_s4c4_c\0" );
recipedata[762].itemID = 4613;
recipedata[762].rec_itemID = 7002;
recipedata[762].level = 6;
recipedata[762].resultCount = 1;
recipedata[762].mpCost = 168;
recipedata[762].successRate = 100;

// recipe id 763
strcpy( recipedata[763].name, "mk_dye_s4d4_c\0" );
recipedata[763].itemID = 4614;
recipedata[763].rec_itemID = 7003;
recipedata[763].level = 6;
recipedata[763].resultCount = 1;
recipedata[763].mpCost = 168;
recipedata[763].successRate = 100;

// recipe id 764
strcpy( recipedata[764].name, "mk_dye_c4s4_c\0" );
recipedata[764].itemID = 4615;
recipedata[764].rec_itemID = 7004;
recipedata[764].level = 6;
recipedata[764].resultCount = 1;
recipedata[764].mpCost = 168;
recipedata[764].successRate = 100;

// recipe id 765
strcpy( recipedata[765].name, "mk_dye_c4c4_c\0" );
recipedata[765].itemID = 4616;
recipedata[765].rec_itemID = 7005;
recipedata[765].level = 6;
recipedata[765].resultCount = 1;
recipedata[765].mpCost = 168;
recipedata[765].successRate = 100;

// recipe id 766
strcpy( recipedata[766].name, "mk_dye_d4s4_c\0" );
recipedata[766].itemID = 4617;
recipedata[766].rec_itemID = 7006;
recipedata[766].level = 6;
recipedata[766].resultCount = 1;
recipedata[766].mpCost = 168;
recipedata[766].successRate = 100;

// recipe id 767
strcpy( recipedata[767].name, "mk_dye_d4c4_c\0" );
recipedata[767].itemID = 4618;
recipedata[767].rec_itemID = 7007;
recipedata[767].level = 6;
recipedata[767].resultCount = 1;
recipedata[767].mpCost = 168;
recipedata[767].successRate = 100;

// recipe id 768
strcpy( recipedata[768].name, "mk_dye_i4m4_c\0" );
recipedata[768].itemID = 4619;
recipedata[768].rec_itemID = 7008;
recipedata[768].level = 6;
recipedata[768].resultCount = 1;
recipedata[768].mpCost = 168;
recipedata[768].successRate = 100;

// recipe id 769
strcpy( recipedata[769].name, "mk_dye_i4w4_c\0" );
recipedata[769].itemID = 4620;
recipedata[769].rec_itemID = 7009;
recipedata[769].level = 6;
recipedata[769].resultCount = 1;
recipedata[769].mpCost = 168;
recipedata[769].successRate = 100;

// recipe id 770
strcpy( recipedata[770].name, "mk_dye_m4i4_c\0" );
recipedata[770].itemID = 4621;
recipedata[770].rec_itemID = 7010;
recipedata[770].level = 6;
recipedata[770].resultCount = 1;
recipedata[770].mpCost = 168;
recipedata[770].successRate = 100;

// recipe id 771
strcpy( recipedata[771].name, "mk_dye_m4w4_c\0" );
recipedata[771].itemID = 4622;
recipedata[771].rec_itemID = 7011;
recipedata[771].level = 6;
recipedata[771].resultCount = 1;
recipedata[771].mpCost = 168;
recipedata[771].successRate = 100;

// recipe id 772
strcpy( recipedata[772].name, "mk_dye_w4i4_c\0" );
recipedata[772].itemID = 4623;
recipedata[772].rec_itemID = 7012;
recipedata[772].level = 6;
recipedata[772].resultCount = 1;
recipedata[772].mpCost = 168;
recipedata[772].successRate = 100;

// recipe id 773
strcpy( recipedata[773].name, "mk_dye_w4m4_c\0" );
recipedata[773].itemID = 4624;
recipedata[773].rec_itemID = 7013;
recipedata[773].level = 6;
recipedata[773].resultCount = 1;
recipedata[773].mpCost = 168;
recipedata[773].successRate = 100;

// recipe id 774
strcpy( recipedata[774].name, "mk_draconic_bow_i\0" );
recipedata[774].itemID = 7575;
recipedata[774].rec_itemID = 7580;
recipedata[774].level = 9;
recipedata[774].resultCount = 1;
recipedata[774].mpCost = 225;
recipedata[774].successRate = 60;

// recipe id 775
strcpy( recipedata[775].name, "mk_draconic_bow_ii\0" );
recipedata[775].itemID = 7575;
recipedata[775].rec_itemID = 7581;
recipedata[775].level = 9;
recipedata[775].resultCount = 1;
recipedata[775].mpCost = 225;
recipedata[775].successRate = 100;

// recipe id 776
strcpy( recipedata[776].name, "mk_racoon_ear\0" );
recipedata[776].itemID = 7680;
recipedata[776].rec_itemID = 7689;
recipedata[776].level = 4;
recipedata[776].resultCount = 1;
recipedata[776].mpCost = 138;
recipedata[776].successRate = 100;

// recipe id 777
strcpy( recipedata[777].name, "mk_outlaw_eyepatch\0" );
recipedata[777].itemID = 7681;
recipedata[777].rec_itemID = 7690;
recipedata[777].level = 4;
recipedata[777].resultCount = 1;
recipedata[777].mpCost = 138;
recipedata[777].successRate = 100;

// recipe id 778
strcpy( recipedata[778].name, "mk_maidens_hairpin\0" );
recipedata[778].itemID = 7682;
recipedata[778].rec_itemID = 7691;
recipedata[778].level = 4;
recipedata[778].resultCount = 1;
recipedata[778].mpCost = 138;
recipedata[778].successRate = 100;

// recipe id 779
strcpy( recipedata[779].name, "mk_rabbit_ear\0" );
recipedata[779].itemID = 7683;
recipedata[779].rec_itemID = 7692;
recipedata[779].level = 4;
recipedata[779].resultCount = 1;
recipedata[779].mpCost = 138;
recipedata[779].successRate = 100;

// recipe id 780
strcpy( recipedata[780].name, "mk_cat_ear\0" );
recipedata[780].itemID = 6843;
recipedata[780].rec_itemID = 7693;
recipedata[780].level = 4;
recipedata[780].resultCount = 1;
recipedata[780].mpCost = 138;
recipedata[780].successRate = 100;

// recipe id 781
strcpy( recipedata[781].name, "mk_forget_me_not_hairpin\0" );
recipedata[781].itemID = 7695;
recipedata[781].rec_itemID = 7699;
recipedata[781].level = 4;
recipedata[781].resultCount = 1;
recipedata[781].mpCost = 138;
recipedata[781].successRate = 100;

// recipe id 782
strcpy( recipedata[782].name, "mk_daisy_hairpin\0" );
recipedata[782].itemID = 7696;
recipedata[782].rec_itemID = 7700;
recipedata[782].level = 4;
recipedata[782].resultCount = 1;
recipedata[782].mpCost = 138;
recipedata[782].successRate = 100;

// recipe id 783
strcpy( recipedata[783].name, "mk_iron_sword\0" );
recipedata[783].itemID = 7880;
recipedata[783].rec_itemID = 8294;
recipedata[783].level = 2;
recipedata[783].resultCount = 1;
recipedata[783].mpCost = 84;
recipedata[783].successRate = 100;

// recipe id 784
strcpy( recipedata[784].name, "mk_sword_of_priest\0" );
recipedata[784].itemID = 7885;
recipedata[784].rec_itemID = 8301;
recipedata[784].level = 2;
recipedata[784].resultCount = 1;
recipedata[784].mpCost = 84;
recipedata[784].successRate = 100;

// recipe id 785
strcpy( recipedata[785].name, "mk_giants_sword\0" );
recipedata[785].itemID = 7881;
recipedata[785].rec_itemID = 8295;
recipedata[785].level = 3;
recipedata[785].resultCount = 1;
recipedata[785].mpCost = 120;
recipedata[785].successRate = 100;

// recipe id 786
strcpy( recipedata[786].name, "mk_sword_of_magic_fog\0" );
recipedata[786].itemID = 7886;
recipedata[786].rec_itemID = 8302;
recipedata[786].level = 3;
recipedata[786].resultCount = 1;
recipedata[786].mpCost = 120;
recipedata[786].successRate = 100;

// recipe id 787
strcpy( recipedata[787].name, "mk_mace_of_priest\0" );
recipedata[787].itemID = 7890;
recipedata[787].rec_itemID = 8307;
recipedata[787].level = 3;
recipedata[787].resultCount = 1;
recipedata[787].mpCost = 120;
recipedata[787].successRate = 100;

// recipe id 788
strcpy( recipedata[788].name, "mk_giants_hammer\0" );
recipedata[788].itemID = 7896;
recipedata[788].rec_itemID = 8317;
recipedata[788].level = 3;
recipedata[788].resultCount = 1;
recipedata[788].mpCost = 120;
recipedata[788].successRate = 100;

// recipe id 789
strcpy( recipedata[789].name, "mk_sword_of_paagrio\0" );
recipedata[789].itemID = 7882;
recipedata[789].rec_itemID = 8296;
recipedata[789].level = 5;
recipedata[789].resultCount = 1;
recipedata[789].mpCost = 156;
recipedata[789].successRate = 100;

// recipe id 790
strcpy( recipedata[790].name, "mk_horn_of_karik\0" );
recipedata[790].itemID = 7898;
recipedata[790].rec_itemID = 8319;
recipedata[790].level = 5;
recipedata[790].resultCount = 1;
recipedata[790].mpCost = 156;
recipedata[790].successRate = 100;

// recipe id 791
strcpy( recipedata[791].name, "mk_mystery_sword\0" );
recipedata[791].itemID = 7887;
recipedata[791].rec_itemID = 8303;
recipedata[791].level = 4;
recipedata[791].resultCount = 1;
recipedata[791].mpCost = 138;
recipedata[791].successRate = 100;

// recipe id 792
strcpy( recipedata[792].name, "mk_sword_of_eclipse\0" );
recipedata[792].itemID = 7888;
recipedata[792].rec_itemID = 8304;
recipedata[792].level = 6;
recipedata[792].resultCount = 1;
recipedata[792].mpCost = 165;
recipedata[792].successRate = 100;

// recipe id 793
strcpy( recipedata[793].name, "mk_eclipse_axe\0" );
recipedata[793].itemID = 7891;
recipedata[793].rec_itemID = 8308;
recipedata[793].level = 6;
recipedata[793].resultCount = 1;
recipedata[793].mpCost = 165;
recipedata[793].successRate = 100;

// recipe id 794
strcpy( recipedata[794].name, "mk_spell_breaker_i\0" );
recipedata[794].itemID = 7892;
recipedata[794].rec_itemID = 8310;
recipedata[794].level = 6;
recipedata[794].resultCount = 1;
recipedata[794].mpCost = 174;
recipedata[794].successRate = 60;

// recipe id 795
strcpy( recipedata[795].name, "mk_spell_breaker_ii\0" );
recipedata[795].itemID = 7892;
recipedata[795].rec_itemID = 8309;
recipedata[795].level = 6;
recipedata[795].resultCount = 1;
recipedata[795].mpCost = 174;
recipedata[795].successRate = 100;

// recipe id 796
strcpy( recipedata[796].name, "mk_ice_storm_hammer_i\0" );
recipedata[796].itemID = 7900;
recipedata[796].rec_itemID = 8322;
recipedata[796].level = 6;
recipedata[796].resultCount = 1;
recipedata[796].mpCost = 174;
recipedata[796].successRate = 60;

// recipe id 797
strcpy( recipedata[797].name, "mk_ice_storm_hammer_ii\0" );
recipedata[797].itemID = 7900;
recipedata[797].rec_itemID = 8321;
recipedata[797].level = 6;
recipedata[797].resultCount = 1;
recipedata[797].mpCost = 174;
recipedata[797].successRate = 100;

// recipe id 798
strcpy( recipedata[798].name, "mk_tears_of_wizard_i\0" );
recipedata[798].itemID = 7889;
recipedata[798].rec_itemID = 8306;
recipedata[798].level = 7;
recipedata[798].resultCount = 1;
recipedata[798].mpCost = 183;
recipedata[798].successRate = 60;

// recipe id 799
strcpy( recipedata[799].name, "mk_tears_of_wizard_ii\0" );
recipedata[799].itemID = 7889;
recipedata[799].rec_itemID = 8305;
recipedata[799].level = 7;
recipedata[799].resultCount = 1;
recipedata[799].mpCost = 183;
recipedata[799].successRate = 100;

// recipe id 800
strcpy( recipedata[800].name, "mk_bone_of_kaim_vanul_i\0" );
recipedata[800].itemID = 7893;
recipedata[800].rec_itemID = 8312;
recipedata[800].level = 7;
recipedata[800].resultCount = 1;
recipedata[800].mpCost = 183;
recipedata[800].successRate = 60;

// recipe id 801
strcpy( recipedata[801].name, "mk_bone_of_kaim_vanul_ii\0" );
recipedata[801].itemID = 7893;
recipedata[801].rec_itemID = 8311;
recipedata[801].level = 7;
recipedata[801].resultCount = 1;
recipedata[801].mpCost = 183;
recipedata[801].successRate = 100;

// recipe id 802
strcpy( recipedata[802].name, "mk_star_buster_i\0" );
recipedata[802].itemID = 7901;
recipedata[802].rec_itemID = 8324;
recipedata[802].level = 7;
recipedata[802].resultCount = 1;
recipedata[802].mpCost = 183;
recipedata[802].successRate = 60;

// recipe id 803
strcpy( recipedata[803].name, "mk_star_buster_ii\0" );
recipedata[803].itemID = 7901;
recipedata[803].rec_itemID = 8323;
recipedata[803].level = 7;
recipedata[803].resultCount = 1;
recipedata[803].mpCost = 183;
recipedata[803].successRate = 100;

// recipe id 804
strcpy( recipedata[804].name, "mk_eye_of_soul_i\0" );
recipedata[804].itemID = 7894;
recipedata[804].rec_itemID = 8314;
recipedata[804].level = 7;
recipedata[804].resultCount = 1;
recipedata[804].mpCost = 192;
recipedata[804].successRate = 60;

// recipe id 805
strcpy( recipedata[805].name, "mk_eye_of_soul_ii\0" );
recipedata[805].itemID = 7894;
recipedata[805].rec_itemID = 8313;
recipedata[805].level = 7;
recipedata[805].resultCount = 1;
recipedata[805].mpCost = 192;
recipedata[805].successRate = 100;

// recipe id 806
strcpy( recipedata[806].name, "mk_dragon_flame_head_i\0" );
recipedata[806].itemID = 7895;
recipedata[806].rec_itemID = 8316;
recipedata[806].level = 8;
recipedata[806].resultCount = 1;
recipedata[806].mpCost = 201;
recipedata[806].successRate = 60;

// recipe id 807
strcpy( recipedata[807].name, "mk_dragon_flame_head_ii\0" );
recipedata[807].itemID = 7895;
recipedata[807].rec_itemID = 8315;
recipedata[807].level = 8;
recipedata[807].resultCount = 1;
recipedata[807].mpCost = 201;
recipedata[807].successRate = 100;

// recipe id 808
strcpy( recipedata[808].name, "mk_doom_crusher_i\0" );
recipedata[808].itemID = 7902;
recipedata[808].rec_itemID = 8326;
recipedata[808].level = 8;
recipedata[808].resultCount = 1;
recipedata[808].mpCost = 201;
recipedata[808].successRate = 60;

// recipe id 809
strcpy( recipedata[809].name, "mk_doom_crusher_ii\0" );
recipedata[809].itemID = 7902;
recipedata[809].rec_itemID = 8325;
recipedata[809].level = 8;
recipedata[809].resultCount = 1;
recipedata[809].mpCost = 201;
recipedata[809].successRate = 100;

// recipe id 810
strcpy( recipedata[810].name, "mk_guardians_sword_i\0" );
recipedata[810].itemID = 7883;
recipedata[810].rec_itemID = 8298;
recipedata[810].level = 7;
recipedata[810].resultCount = 1;
recipedata[810].mpCost = 183;
recipedata[810].successRate = 60;

// recipe id 811
strcpy( recipedata[811].name, "mk_guardians_sword_ii\0" );
recipedata[811].itemID = 7883;
recipedata[811].rec_itemID = 8297;
recipedata[811].level = 7;
recipedata[811].resultCount = 1;
recipedata[811].mpCost = 183;
recipedata[811].successRate = 100;

// recipe id 812
strcpy( recipedata[812].name, "mk_hammer_of_destroyer_i\0" );
recipedata[812].itemID = 7899;
recipedata[812].rec_itemID = 8487;
recipedata[812].level = 7;
recipedata[812].resultCount = 1;
recipedata[812].mpCost = 192;
recipedata[812].successRate = 60;

// recipe id 813
strcpy( recipedata[813].name, "mk_hammer_of_destroyer_ii\0" );
recipedata[813].itemID = 7899;
recipedata[813].rec_itemID = 8320;
recipedata[813].level = 7;
recipedata[813].resultCount = 1;
recipedata[813].mpCost = 192;
recipedata[813].successRate = 100;

// recipe id 814
strcpy( recipedata[814].name, "mk_inferno_master_i\0" );
recipedata[814].itemID = 7884;
recipedata[814].rec_itemID = 8300;
recipedata[814].level = 7;
recipedata[814].resultCount = 1;
recipedata[814].mpCost = 192;
recipedata[814].successRate = 60;

// recipe id 815
strcpy( recipedata[815].name, "mk_inferno_master_ii\0" );
recipedata[815].itemID = 7884;
recipedata[815].rec_itemID = 8299;
recipedata[815].level = 7;
recipedata[815].resultCount = 1;
recipedata[815].mpCost = 192;
recipedata[815].successRate = 100;

// recipe id 816
strcpy( recipedata[816].name, "mk_dwarven_hammer\0" );
recipedata[816].itemID = 7897;
recipedata[816].rec_itemID = 8318;
recipedata[816].level = 6;
recipedata[816].resultCount = 1;
recipedata[816].mpCost = 165;
recipedata[816].successRate = 100;

// recipe id 817
strcpy( recipedata[817].name, "mk_horn_of_reddevil\0" );
recipedata[817].itemID = 8187;
recipedata[817].rec_itemID = 8370;
recipedata[817].level = 4;
recipedata[817].resultCount = 1;
recipedata[817].mpCost = 138;
recipedata[817].successRate = 100;

// recipe id 818
strcpy( recipedata[818].name, "mk_wing_of_little_angel\0" );
recipedata[818].itemID = 8188;
recipedata[818].rec_itemID = 8371;
recipedata[818].level = 4;
recipedata[818].resultCount = 1;
recipedata[818].mpCost = 138;
recipedata[818].successRate = 100;

// recipe id 819
strcpy( recipedata[819].name, "mk_feeler_of_fairy\0" );
recipedata[819].itemID = 8189;
recipedata[819].rec_itemID = 8372;
recipedata[819].level = 4;
recipedata[819].resultCount = 1;
recipedata[819].mpCost = 138;
recipedata[819].successRate = 100;

// recipe id 820
strcpy( recipedata[820].name, "mk_party_hat_i\0" );
recipedata[820].itemID = 8184;
recipedata[820].rec_itemID = 8365;
recipedata[820].level = 6;
recipedata[820].resultCount = 1;
recipedata[820].mpCost = 168;
recipedata[820].successRate = 70;

// recipe id 821
strcpy( recipedata[821].name, "mk_party_hat_ii\0" );
recipedata[821].itemID = 8184;
recipedata[821].rec_itemID = 8364;
recipedata[821].level = 6;
recipedata[821].resultCount = 1;
recipedata[821].mpCost = 168;
recipedata[821].successRate = 100;

// recipe id 822
strcpy( recipedata[822].name, "mk_chaperon_of_dresser_i\0" );
recipedata[822].itemID = 8185;
recipedata[822].rec_itemID = 8367;
recipedata[822].level = 6;
recipedata[822].resultCount = 1;
recipedata[822].mpCost = 168;
recipedata[822].successRate = 70;

// recipe id 823
strcpy( recipedata[823].name, "mk_chaperon_of_dresser_ii\0" );
recipedata[823].itemID = 8185;
recipedata[823].rec_itemID = 8366;
recipedata[823].level = 6;
recipedata[823].resultCount = 1;
recipedata[823].mpCost = 168;
recipedata[823].successRate = 100;

// recipe id 824
strcpy( recipedata[824].name, "mk_goggle_of_artisan_i\0" );
recipedata[824].itemID = 8186;
recipedata[824].rec_itemID = 8369;
recipedata[824].level = 6;
recipedata[824].resultCount = 1;
recipedata[824].mpCost = 168;
recipedata[824].successRate = 70;

// recipe id 825
strcpy( recipedata[825].name, "mk_goggle_of_artisan_ii\0" );
recipedata[825].itemID = 8186;
recipedata[825].rec_itemID = 8368;
recipedata[825].level = 6;
recipedata[825].resultCount = 1;
recipedata[825].mpCost = 168;
recipedata[825].successRate = 100;

// recipe id 826
strcpy( recipedata[826].name, "mk_elixir_of_life_none\0" );
recipedata[826].itemID = 8622;
recipedata[826].rec_itemID = 8640;
recipedata[826].level = 1;
recipedata[826].resultCount = 10;
recipedata[826].mpCost = 10;
recipedata[826].successRate = 100;

// recipe id 827
strcpy( recipedata[827].name, "mk_elixir_of_life_d\0" );
recipedata[827].itemID = 8623;
recipedata[827].rec_itemID = 8641;
recipedata[827].level = 2;
recipedata[827].resultCount = 10;
recipedata[827].mpCost = 30;
recipedata[827].successRate = 100;

// recipe id 828
strcpy( recipedata[828].name, "mk_elixir_of_life_c\0" );
recipedata[828].itemID = 8624;
recipedata[828].rec_itemID = 8642;
recipedata[828].level = 3;
recipedata[828].resultCount = 10;
recipedata[828].mpCost = 50;
recipedata[828].successRate = 100;

// recipe id 829
strcpy( recipedata[829].name, "mk_elixir_of_life_b\0" );
recipedata[829].itemID = 8625;
recipedata[829].rec_itemID = 8643;
recipedata[829].level = 4;
recipedata[829].resultCount = 10;
recipedata[829].mpCost = 80;
recipedata[829].successRate = 100;

// recipe id 830
strcpy( recipedata[830].name, "mk_elixir_of_life_a\0" );
recipedata[830].itemID = 8626;
recipedata[830].rec_itemID = 8644;
recipedata[830].level = 5;
recipedata[830].resultCount = 10;
recipedata[830].mpCost = 110;
recipedata[830].successRate = 100;

// recipe id 831
strcpy( recipedata[831].name, "mk_elixir_of_life_s\0" );
recipedata[831].itemID = 8627;
recipedata[831].rec_itemID = 8645;
recipedata[831].level = 6;
recipedata[831].resultCount = 10;
recipedata[831].mpCost = 150;
recipedata[831].successRate = 100;

// recipe id 832
strcpy( recipedata[832].name, "mk_elixir_of_mental_none\0" );
recipedata[832].itemID = 8628;
recipedata[832].rec_itemID = 8646;
recipedata[832].level = 1;
recipedata[832].resultCount = 10;
recipedata[832].mpCost = 10;
recipedata[832].successRate = 100;

// recipe id 833
strcpy( recipedata[833].name, "mk_elixir_of_mental_d\0" );
recipedata[833].itemID = 8629;
recipedata[833].rec_itemID = 8647;
recipedata[833].level = 2;
recipedata[833].resultCount = 10;
recipedata[833].mpCost = 30;
recipedata[833].successRate = 100;

// recipe id 834
strcpy( recipedata[834].name, "mk_elixir_of_mental_c\0" );
recipedata[834].itemID = 8630;
recipedata[834].rec_itemID = 8648;
recipedata[834].level = 3;
recipedata[834].resultCount = 10;
recipedata[834].mpCost = 50;
recipedata[834].successRate = 100;

// recipe id 835
strcpy( recipedata[835].name, "mk_elixir_of_mental_b\0" );
recipedata[835].itemID = 8631;
recipedata[835].rec_itemID = 8649;
recipedata[835].level = 4;
recipedata[835].resultCount = 10;
recipedata[835].mpCost = 80;
recipedata[835].successRate = 100;

// recipe id 836
strcpy( recipedata[836].name, "mk_elixir_of_mental_a\0" );
recipedata[836].itemID = 8632;
recipedata[836].rec_itemID = 8650;
recipedata[836].level = 5;
recipedata[836].resultCount = 10;
recipedata[836].mpCost = 110;
recipedata[836].successRate = 100;

// recipe id 837
strcpy( recipedata[837].name, "mk_elixir_of_mental_s\0" );
recipedata[837].itemID = 8633;
recipedata[837].rec_itemID = 8651;
recipedata[837].level = 6;
recipedata[837].resultCount = 10;
recipedata[837].mpCost = 150;
recipedata[837].successRate = 100;

// recipe id 838
strcpy( recipedata[838].name, "mk_elixir_of_combative_none\0" );
recipedata[838].itemID = 8634;
recipedata[838].rec_itemID = 8652;
recipedata[838].level = 1;
recipedata[838].resultCount = 10;
recipedata[838].mpCost = 10;
recipedata[838].successRate = 100;

// recipe id 839
strcpy( recipedata[839].name, "mk_elixir_of_combative_d\0" );
recipedata[839].itemID = 8635;
recipedata[839].rec_itemID = 8653;
recipedata[839].level = 2;
recipedata[839].resultCount = 10;
recipedata[839].mpCost = 30;
recipedata[839].successRate = 100;

// recipe id 840
strcpy( recipedata[840].name, "mk_elixir_of_combative_c\0" );
recipedata[840].itemID = 8636;
recipedata[840].rec_itemID = 8654;
recipedata[840].level = 3;
recipedata[840].resultCount = 10;
recipedata[840].mpCost = 50;
recipedata[840].successRate = 100;

// recipe id 841
strcpy( recipedata[841].name, "mk_elixir_of_combative_b\0" );
recipedata[841].itemID = 8637;
recipedata[841].rec_itemID = 8655;
recipedata[841].level = 4;
recipedata[841].resultCount = 10;
recipedata[841].mpCost = 80;
recipedata[841].successRate = 100;

// recipe id 842
strcpy( recipedata[842].name, "mk_elixir_of_combative_a\0" );
recipedata[842].itemID = 8638;
recipedata[842].rec_itemID = 8656;
recipedata[842].level = 5;
recipedata[842].resultCount = 10;
recipedata[842].mpCost = 110;
recipedata[842].successRate = 100;

// recipe id 843
strcpy( recipedata[843].name, "mk_elixir_of_combative_s\0" );
recipedata[843].itemID = 8639;
recipedata[843].rec_itemID = 8657;
recipedata[843].level = 6;
recipedata[843].resultCount = 10;
recipedata[843].mpCost = 150;
recipedata[843].successRate = 100;

// recipe id 844
strcpy( recipedata[844].name, "mk_sirr_blade_i\0" );
recipedata[844].itemID = 8678;
recipedata[844].rec_itemID = 8690;
recipedata[844].level = 8;
recipedata[844].resultCount = 1;
recipedata[844].mpCost = 210;
recipedata[844].successRate = 60;

// recipe id 845
strcpy( recipedata[845].name, "mk_sirr_blade_ii\0" );
recipedata[845].itemID = 8678;
recipedata[845].rec_itemID = 8691;
recipedata[845].level = 8;
recipedata[845].resultCount = 1;
recipedata[845].mpCost = 210;
recipedata[845].successRate = 100;

// recipe id 846
strcpy( recipedata[846].name, "mk_sword_of_ipos_i\0" );
recipedata[846].itemID = 8679;
recipedata[846].rec_itemID = 8692;
recipedata[846].level = 8;
recipedata[846].resultCount = 1;
recipedata[846].mpCost = 210;
recipedata[846].successRate = 60;

// recipe id 847
strcpy( recipedata[847].name, "mk_sword_of_ipos_ii\0" );
recipedata[847].itemID = 8679;
recipedata[847].rec_itemID = 8693;
recipedata[847].level = 8;
recipedata[847].resultCount = 1;
recipedata[847].mpCost = 210;
recipedata[847].successRate = 100;

// recipe id 848
strcpy( recipedata[848].name, "mk_barakiel_axe_i\0" );
recipedata[848].itemID = 8680;
recipedata[848].rec_itemID = 8694;
recipedata[848].level = 8;
recipedata[848].resultCount = 1;
recipedata[848].mpCost = 210;
recipedata[848].successRate = 60;

// recipe id 849
strcpy( recipedata[849].name, "mk_barakiel_axe_ii\0" );
recipedata[849].itemID = 8680;
recipedata[849].rec_itemID = 8695;
recipedata[849].level = 8;
recipedata[849].resultCount = 1;
recipedata[849].mpCost = 210;
recipedata[849].successRate = 100;

// recipe id 850
strcpy( recipedata[850].name, "mk_tuning_fork_of_behemoth_i\0" );
recipedata[850].itemID = 8681;
recipedata[850].rec_itemID = 8696;
recipedata[850].level = 8;
recipedata[850].resultCount = 1;
recipedata[850].mpCost = 210;
recipedata[850].successRate = 60;

// recipe id 851
strcpy( recipedata[851].name, "mk_tuning_fork_of_behemoth_ii\0" );
recipedata[851].itemID = 8681;
recipedata[851].rec_itemID = 8697;
recipedata[851].level = 8;
recipedata[851].resultCount = 1;
recipedata[851].mpCost = 210;
recipedata[851].successRate = 100;

// recipe id 852
strcpy( recipedata[852].name, "mk_naga_storm_i\0" );
recipedata[852].itemID = 8682;
recipedata[852].rec_itemID = 8698;
recipedata[852].level = 8;
recipedata[852].resultCount = 1;
recipedata[852].mpCost = 210;
recipedata[852].successRate = 60;

// recipe id 853
strcpy( recipedata[853].name, "mk_naga_storm_ii\0" );
recipedata[853].itemID = 8682;
recipedata[853].rec_itemID = 8699;
recipedata[853].level = 8;
recipedata[853].resultCount = 1;
recipedata[853].mpCost = 210;
recipedata[853].successRate = 100;

// recipe id 854
strcpy( recipedata[854].name, "mk_tiphon_spear_i\0" );
recipedata[854].itemID = 8683;
recipedata[854].rec_itemID = 8700;
recipedata[854].level = 8;
recipedata[854].resultCount = 1;
recipedata[854].mpCost = 210;
recipedata[854].successRate = 60;

// recipe id 855
strcpy( recipedata[855].name, "mk_tiphon_spear_ii\0" );
recipedata[855].itemID = 8683;
recipedata[855].rec_itemID = 8701;
recipedata[855].level = 8;
recipedata[855].resultCount = 1;
recipedata[855].mpCost = 210;
recipedata[855].successRate = 100;

// recipe id 856
strcpy( recipedata[856].name, "mk_shyid_bow_i\0" );
recipedata[856].itemID = 8684;
recipedata[856].rec_itemID = 8702;
recipedata[856].level = 8;
recipedata[856].resultCount = 1;
recipedata[856].mpCost = 210;
recipedata[856].successRate = 60;

// recipe id 857
strcpy( recipedata[857].name, "mk_shyid_bow_ii\0" );
recipedata[857].itemID = 8684;
recipedata[857].rec_itemID = 8703;
recipedata[857].level = 8;
recipedata[857].resultCount = 1;
recipedata[857].mpCost = 210;
recipedata[857].successRate = 100;

// recipe id 858
strcpy( recipedata[858].name, "mk_sobekk_hurricane_i\0" );
recipedata[858].itemID = 8685;
recipedata[858].rec_itemID = 8704;
recipedata[858].level = 8;
recipedata[858].resultCount = 1;
recipedata[858].mpCost = 210;
recipedata[858].successRate = 60;

// recipe id 859
strcpy( recipedata[859].name, "mk_sobekk_hurricane_ii\0" );
recipedata[859].itemID = 8685;
recipedata[859].rec_itemID = 8705;
recipedata[859].level = 8;
recipedata[859].resultCount = 1;
recipedata[859].mpCost = 210;
recipedata[859].successRate = 100;

// recipe id 860
strcpy( recipedata[860].name, "mk_tongue_of_themis_i\0" );
recipedata[860].itemID = 8686;
recipedata[860].rec_itemID = 8706;
recipedata[860].level = 8;
recipedata[860].resultCount = 1;
recipedata[860].mpCost = 210;
recipedata[860].successRate = 60;

// recipe id 861
strcpy( recipedata[861].name, "mk_tongue_of_themis_ii\0" );
recipedata[861].itemID = 8686;
recipedata[861].rec_itemID = 8707;
recipedata[861].level = 8;
recipedata[861].resultCount = 1;
recipedata[861].mpCost = 210;
recipedata[861].successRate = 100;

// recipe id 862
strcpy( recipedata[862].name, "mk_hand_of_cabrio_i\0" );
recipedata[862].itemID = 8687;
recipedata[862].rec_itemID = 8708;
recipedata[862].level = 8;
recipedata[862].resultCount = 1;
recipedata[862].mpCost = 210;
recipedata[862].successRate = 60;

// recipe id 863
strcpy( recipedata[863].name, "mk_hand_of_cabrio_ii\0" );
recipedata[863].itemID = 8687;
recipedata[863].rec_itemID = 8709;
recipedata[863].level = 8;
recipedata[863].resultCount = 1;
recipedata[863].mpCost = 210;
recipedata[863].successRate = 100;

// recipe id 864
strcpy( recipedata[864].name, "mk_crystal_of_deamon_i\0" );
recipedata[864].itemID = 8688;
recipedata[864].rec_itemID = 8710;
recipedata[864].level = 8;
recipedata[864].resultCount = 1;
recipedata[864].mpCost = 210;
recipedata[864].successRate = 60;

// recipe id 865
strcpy( recipedata[865].name, "mk_crystal_of_deamon_ii\0" );
recipedata[865].itemID = 8688;
recipedata[865].rec_itemID = 8711;
recipedata[865].level = 8;
recipedata[865].resultCount = 1;
recipedata[865].mpCost = 210;
recipedata[865].successRate = 100;

// recipe id 866
strcpy( recipedata[866].name, "mk_magic_sign_event_1\0" );
recipedata[866].itemID = 9171;
recipedata[866].rec_itemID = 9162;
recipedata[866].level = 1;
recipedata[866].resultCount = 1;
recipedata[866].mpCost = 50;
recipedata[866].successRate = 100;

// recipe id 867
strcpy( recipedata[867].name, "mk_magic_sign_event_2\0" );
recipedata[867].itemID = 9172;
recipedata[867].rec_itemID = 9163;
recipedata[867].level = 1;
recipedata[867].resultCount = 1;
recipedata[867].mpCost = 50;
recipedata[867].successRate = 100;

// recipe id 868
strcpy( recipedata[868].name, "mk_magic_sign_event_3\0" );
recipedata[868].itemID = 9173;
recipedata[868].rec_itemID = 9164;
recipedata[868].level = 1;
recipedata[868].resultCount = 1;
recipedata[868].mpCost = 50;
recipedata[868].successRate = 100;

// recipe id 869
strcpy( recipedata[869].name, "mk_magic_sign_event_4\0" );
recipedata[869].itemID = 9174;
recipedata[869].rec_itemID = 9165;
recipedata[869].level = 1;
recipedata[869].resultCount = 1;
recipedata[869].mpCost = 50;
recipedata[869].successRate = 100;

// recipe id 870
strcpy( recipedata[870].name, "mk_magic_sign_event_5\0" );
recipedata[870].itemID = 9175;
recipedata[870].rec_itemID = 9166;
recipedata[870].level = 1;
recipedata[870].resultCount = 1;
recipedata[870].mpCost = 50;
recipedata[870].successRate = 100;

// recipe id 871
strcpy( recipedata[871].name, "mk_magic_sign_event_6\0" );
recipedata[871].itemID = 9176;
recipedata[871].rec_itemID = 9167;
recipedata[871].level = 1;
recipedata[871].resultCount = 1;
recipedata[871].mpCost = 50;
recipedata[871].successRate = 100;

// recipe id 872
strcpy( recipedata[872].name, "mk_dynasty_blade_i\0" );
recipedata[872].itemID = 9442;
recipedata[872].rec_itemID = 9967;
recipedata[872].level = 9;
recipedata[872].resultCount = 1;
recipedata[872].mpCost = 240;
recipedata[872].successRate = 60;

// recipe id 873
strcpy( recipedata[873].name, "mk_dynasty_two_hand_sword_i\0" );
recipedata[873].itemID = 9443;
recipedata[873].rec_itemID = 9968;
recipedata[873].level = 9;
recipedata[873].resultCount = 1;
recipedata[873].mpCost = 240;
recipedata[873].successRate = 60;

// recipe id 874
strcpy( recipedata[874].name, "mk_dynasty_magic_sword_i\0" );
recipedata[874].itemID = 9444;
recipedata[874].rec_itemID = 9969;
recipedata[874].level = 9;
recipedata[874].resultCount = 1;
recipedata[874].mpCost = 240;
recipedata[874].successRate = 60;

// recipe id 875
strcpy( recipedata[875].name, "mk_dynasty_bow_i\0" );
recipedata[875].itemID = 9445;
recipedata[875].rec_itemID = 9970;
recipedata[875].level = 9;
recipedata[875].resultCount = 1;
recipedata[875].mpCost = 240;
recipedata[875].successRate = 60;

// recipe id 876
strcpy( recipedata[876].name, "mk_dynasty_dagger_i\0" );
recipedata[876].itemID = 9446;
recipedata[876].rec_itemID = 9971;
recipedata[876].level = 9;
recipedata[876].resultCount = 1;
recipedata[876].mpCost = 240;
recipedata[876].successRate = 60;

// recipe id 877
strcpy( recipedata[877].name, "mk_dynasty_spear_i\0" );
recipedata[877].itemID = 9447;
recipedata[877].rec_itemID = 9972;
recipedata[877].level = 9;
recipedata[877].resultCount = 1;
recipedata[877].mpCost = 240;
recipedata[877].successRate = 60;

// recipe id 878
strcpy( recipedata[878].name, "mk_dynasty_hammer_i\0" );
recipedata[878].itemID = 9448;
recipedata[878].rec_itemID = 9973;
recipedata[878].level = 9;
recipedata[878].resultCount = 1;
recipedata[878].mpCost = 240;
recipedata[878].successRate = 60;

// recipe id 879
strcpy( recipedata[879].name, "mk_dynasty_staff_i\0" );
recipedata[879].itemID = 9449;
recipedata[879].rec_itemID = 9974;
recipedata[879].level = 9;
recipedata[879].resultCount = 1;
recipedata[879].mpCost = 240;
recipedata[879].successRate = 60;

// recipe id 880
strcpy( recipedata[880].name, "mk_dynasty_jamadhr_i\0" );
recipedata[880].itemID = 9450;
recipedata[880].rec_itemID = 9975;
recipedata[880].level = 9;
recipedata[880].resultCount = 1;
recipedata[880].mpCost = 240;
recipedata[880].successRate = 60;

// recipe id 881
strcpy( recipedata[881].name, "mk_sealed_dynasty_blast_plate_i\0" );
recipedata[881].itemID = 9514;
recipedata[881].rec_itemID = 9482;
recipedata[881].level = 9;
recipedata[881].resultCount = 1;
recipedata[881].mpCost = 240;
recipedata[881].successRate = 60;

// recipe id 882
strcpy( recipedata[882].name, "mk_sealed_dynasty_gaiter_i\0" );
recipedata[882].itemID = 9515;
recipedata[882].rec_itemID = 9483;
recipedata[882].level = 9;
recipedata[882].resultCount = 1;
recipedata[882].mpCost = 240;
recipedata[882].successRate = 60;

// recipe id 883
strcpy( recipedata[883].name, "mk_sealed_dynasty_helmet_i\0" );
recipedata[883].itemID = 9516;
recipedata[883].rec_itemID = 9484;
recipedata[883].level = 9;
recipedata[883].resultCount = 1;
recipedata[883].mpCost = 240;
recipedata[883].successRate = 60;

// recipe id 884
strcpy( recipedata[884].name, "mk_sealed_dynasty_gauntlet_i\0" );
recipedata[884].itemID = 9517;
recipedata[884].rec_itemID = 9485;
recipedata[884].level = 9;
recipedata[884].resultCount = 1;
recipedata[884].mpCost = 240;
recipedata[884].successRate = 60;

// recipe id 885
strcpy( recipedata[885].name, "mk_sealed_dynasty_boots_i\0" );
recipedata[885].itemID = 9518;
recipedata[885].rec_itemID = 9486;
recipedata[885].level = 9;
recipedata[885].resultCount = 1;
recipedata[885].mpCost = 240;
recipedata[885].successRate = 60;

// recipe id 886
strcpy( recipedata[886].name, "mk_sealed_dynasty_leather_mail_i\0" );
recipedata[886].itemID = 9519;
recipedata[886].rec_itemID = 9487;
recipedata[886].level = 9;
recipedata[886].resultCount = 1;
recipedata[886].mpCost = 240;
recipedata[886].successRate = 60;

// recipe id 887
strcpy( recipedata[887].name, "mk_sealed_dynasty_leather_legging_i\0" );
recipedata[887].itemID = 9520;
recipedata[887].rec_itemID = 9488;
recipedata[887].level = 9;
recipedata[887].resultCount = 1;
recipedata[887].mpCost = 240;
recipedata[887].successRate = 60;

// recipe id 888
strcpy( recipedata[888].name, "mk_sealed_dynasty_leather_helmet_i\0" );
recipedata[888].itemID = 9521;
recipedata[888].rec_itemID = 9489;
recipedata[888].level = 9;
recipedata[888].resultCount = 1;
recipedata[888].mpCost = 240;
recipedata[888].successRate = 60;

// recipe id 889
strcpy( recipedata[889].name, "mk_sealed_dynasty_leather_gloves_i\0" );
recipedata[889].itemID = 9522;
recipedata[889].rec_itemID = 9490;
recipedata[889].level = 9;
recipedata[889].resultCount = 1;
recipedata[889].mpCost = 240;
recipedata[889].successRate = 60;

// recipe id 890
strcpy( recipedata[890].name, "mk_sealed_dynasty_leather_boots_i\0" );
recipedata[890].itemID = 9523;
recipedata[890].rec_itemID = 9491;
recipedata[890].level = 9;
recipedata[890].resultCount = 1;
recipedata[890].mpCost = 240;
recipedata[890].successRate = 60;

// recipe id 891
strcpy( recipedata[891].name, "mk_sealed_dynasty_tunic_i\0" );
recipedata[891].itemID = 9524;
recipedata[891].rec_itemID = 9492;
recipedata[891].level = 9;
recipedata[891].resultCount = 1;
recipedata[891].mpCost = 240;
recipedata[891].successRate = 60;

// recipe id 892
strcpy( recipedata[892].name, "mk_sealed_dynasty_hose_i\0" );
recipedata[892].itemID = 9525;
recipedata[892].rec_itemID = 9493;
recipedata[892].level = 9;
recipedata[892].resultCount = 1;
recipedata[892].mpCost = 240;
recipedata[892].successRate = 60;

// recipe id 893
strcpy( recipedata[893].name, "mk_sealed_dynasty_circlet_i\0" );
recipedata[893].itemID = 9526;
recipedata[893].rec_itemID = 9494;
recipedata[893].level = 9;
recipedata[893].resultCount = 1;
recipedata[893].mpCost = 240;
recipedata[893].successRate = 60;

// recipe id 894
strcpy( recipedata[894].name, "mk_sealed_dynasty_gloves_i\0" );
recipedata[894].itemID = 9527;
recipedata[894].rec_itemID = 9495;
recipedata[894].level = 9;
recipedata[894].resultCount = 1;
recipedata[894].mpCost = 240;
recipedata[894].successRate = 60;

// recipe id 895
strcpy( recipedata[895].name, "mk_sealed_dynasty_shoes_i\0" );
recipedata[895].itemID = 9528;
recipedata[895].rec_itemID = 9496;
recipedata[895].level = 9;
recipedata[895].resultCount = 1;
recipedata[895].mpCost = 240;
recipedata[895].successRate = 60;

// recipe id 896
strcpy( recipedata[896].name, "mk_sealed_dynasty_shield_i\0" );
recipedata[896].itemID = 9529;
recipedata[896].rec_itemID = 9497;
recipedata[896].level = 9;
recipedata[896].resultCount = 1;
recipedata[896].mpCost = 240;
recipedata[896].successRate = 60;

// recipe id 897
strcpy( recipedata[897].name, "mk_sealed_dynasty_earring_i\0" );
recipedata[897].itemID = 9452;
recipedata[897].rec_itemID = 9985;
recipedata[897].level = 9;
recipedata[897].resultCount = 1;
recipedata[897].mpCost = 240;
recipedata[897].successRate = 70;

// recipe id 898
strcpy( recipedata[898].name, "mk_sealed_dynasty_neckalce_i\0" );
recipedata[898].itemID = 9453;
recipedata[898].rec_itemID = 9986;
recipedata[898].level = 9;
recipedata[898].resultCount = 1;
recipedata[898].mpCost = 240;
recipedata[898].successRate = 70;

// recipe id 899
strcpy( recipedata[899].name, "mk_sealed_dynasty_ring_i\0" );
recipedata[899].itemID = 9454;
recipedata[899].rec_itemID = 9987;
recipedata[899].level = 9;
recipedata[899].resultCount = 1;
recipedata[899].mpCost = 240;
recipedata[899].successRate = 70;

// recipe id 900
strcpy( recipedata[900].name, "mk_icarus_sowsword_i\0" );
recipedata[900].itemID = 10215;
recipedata[900].rec_itemID = 10373;
recipedata[900].level = 9;
recipedata[900].resultCount = 1;
recipedata[900].mpCost = 240;
recipedata[900].successRate = 60;

// recipe id 901
strcpy( recipedata[901].name, "mk_icarus_disperser_i\0" );
recipedata[901].itemID = 10216;
recipedata[901].rec_itemID = 10374;
recipedata[901].level = 9;
recipedata[901].resultCount = 1;
recipedata[901].mpCost = 240;
recipedata[901].successRate = 60;

// recipe id 902
strcpy( recipedata[902].name, "mk_icarus_spirits_i\0" );
recipedata[902].itemID = 10217;
recipedata[902].rec_itemID = 10375;
recipedata[902].level = 9;
recipedata[902].resultCount = 1;
recipedata[902].mpCost = 240;
recipedata[902].successRate = 60;

// recipe id 903
strcpy( recipedata[903].name, "mk_icarus_heavy_arms_i\0" );
recipedata[903].itemID = 10218;
recipedata[903].rec_itemID = 10376;
recipedata[903].level = 9;
recipedata[903].resultCount = 1;
recipedata[903].mpCost = 240;
recipedata[903].successRate = 60;

// recipe id 904
strcpy( recipedata[904].name, "mk_icarus_trident_i\0" );
recipedata[904].itemID = 10219;
recipedata[904].rec_itemID = 10377;
recipedata[904].level = 9;
recipedata[904].resultCount = 1;
recipedata[904].mpCost = 240;
recipedata[904].successRate = 60;

// recipe id 905
strcpy( recipedata[905].name, "mk_icarus_chopper_i\0" );
recipedata[905].itemID = 10220;
recipedata[905].rec_itemID = 10378;
recipedata[905].level = 9;
recipedata[905].resultCount = 1;
recipedata[905].mpCost = 240;
recipedata[905].successRate = 60;

// recipe id 906
strcpy( recipedata[906].name, "mk_icarus_knuckle_i\0" );
recipedata[906].itemID = 10221;
recipedata[906].rec_itemID = 10379;
recipedata[906].level = 9;
recipedata[906].resultCount = 1;
recipedata[906].mpCost = 240;
recipedata[906].successRate = 60;

// recipe id 907
strcpy( recipedata[907].name, "mk_icarus_wand_i\0" );
recipedata[907].itemID = 10222;
recipedata[907].rec_itemID = 10380;
recipedata[907].level = 9;
recipedata[907].resultCount = 1;
recipedata[907].mpCost = 240;
recipedata[907].successRate = 60;

// recipe id 908
strcpy( recipedata[908].name, "mk_icarus_accipiter_i\0" );
recipedata[908].itemID = 10223;
recipedata[908].rec_itemID = 10381;
recipedata[908].level = 9;
recipedata[908].resultCount = 1;
recipedata[908].mpCost = 240;
recipedata[908].successRate = 60;

// recipe id 909
strcpy( recipedata[909].name, "mk_dynasty_twohand_staff_i\0" );
recipedata[909].itemID = 10252;
recipedata[909].rec_itemID = 10544;
recipedata[909].level = 9;
recipedata[909].resultCount = 1;
recipedata[909].mpCost = 240;
recipedata[909].successRate = 60;

// recipe id 910
strcpy( recipedata[910].name, "mk_dynasty_crusher_i\0" );
recipedata[910].itemID = 10253;
recipedata[910].rec_itemID = 10545;
recipedata[910].level = 9;
recipedata[910].resultCount = 1;
recipedata[910].mpCost = 240;
recipedata[910].successRate = 60;

} // void RecipeData_Init();

