#ifndef H_WORLD_OBJECT_TREE_NODE
#define H_WORLD_OBJECT_TREE_NODE
#include "BinTree.h"

typedef enum eL2OBJECT_TYPE
{
	L2OT_NONE = 0,
	L2OT_NPC,
	L2OT_PC,
	L2OT_ITEM
} L2OBJECT_TYPE, *LPL2OBJECT_TYPE;

class WorldObjectTreeNode : public BinTreeNode
{
public:
	WorldObjectTreeNode();
	WorldObjectTreeNode( unsigned int keyInit, L2OBJECT_TYPE type, int indexInArray );
	~WorldObjectTreeNode() {};
public:
	void setObjectType( L2OBJECT_TYPE type );
	void setArrayIdx( int ai );
public:
	L2OBJECT_TYPE objectType;
	int           arrayIdx;
};

#endif
