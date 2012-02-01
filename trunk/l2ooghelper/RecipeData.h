#ifndef H_RECIPEDATA
#define H_RECIPEDATA

class RecipeInfo
{
public:
	RecipeInfo() { name[0] = 0; itemID = rec_itemID = 0; level = resultCount = mpCost = successRate; }
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

void RecipeData_Init();
bool GetRecipeInfoForRecID( int recID, RecipeInfo *pInfo );

#endif
