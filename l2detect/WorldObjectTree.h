#ifndef H_WORLD_OBJECT_TREE
#define H_WORLD_OBJECT_TREE
#include "WorldObjectTreeNode.h"

void WorldObjectTree_Init();
void WorldObjectTree_Free();

BOOL WorldObjectTree_GetInfoByObjectID( unsigned int objectID, LPL2OBJECT_TYPE pObjectType, int *pIndexInArray );
BOOL WorldObjectTree_AddObject( unsigned int objectID, L2OBJECT_TYPE objectType, int indexInArray );
BOOL WorldObjectTree_DelObject( unsigned int objectID );

#endif
