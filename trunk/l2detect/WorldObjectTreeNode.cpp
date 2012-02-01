#include "stdafx.h"
#include "WorldObjectTreeNode.h"

WorldObjectTreeNode::WorldObjectTreeNode()
{
	parent = left = right = NULL;
	key = 0;
	objectType = L2OT_NONE;
	arrayIdx = -1;
}

WorldObjectTreeNode::WorldObjectTreeNode( unsigned int keyInit, L2OBJECT_TYPE type, int indexInArray )
{
	parent = left = right = NULL;
	key = keyInit;
	objectType = type;
	arrayIdx = indexInArray;
}

void WorldObjectTreeNode::setObjectType( L2OBJECT_TYPE type )
{
	objectType = type;
}

void WorldObjectTreeNode::setArrayIdx( int ai )
{
	arrayIdx = ai;
}
