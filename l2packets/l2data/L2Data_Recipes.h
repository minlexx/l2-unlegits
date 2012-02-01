#ifndef H_RECIPEDATA
#define H_RECIPEDATA

class L2Data_RecipeInfo
{
public:
	L2Data_RecipeInfo() { name[0] = 0; itemID = rec_itemID = 0; level = resultCount = mpCost = successRate; }
public:
	char         name[128];
	unsigned int itemID;
	unsigned int rec_itemID;
	int          level;
	int          resultCount;
	int          mpCost;
	int          successRate;
};

#define MAX_RECIPE_ID 910

void L2Data_InitRecipes();
bool L2Data_GetRecipeInfoForRecID( int recID, L2Data_RecipeInfo *pInfo );

#endif
