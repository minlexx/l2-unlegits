#include "stdafx.h"
#include "WorldObjectTreeNode.h"

WorldObjectTreeNode::WorldObjectTreeNode()
{
	parent = left = right = NULL;
	key = 0;
	objectType = L2OT_NONE;
	arrayIdx = -1;
	pObj = NULL;
}

void WorldObjectTreeNode::setFrom( const WorldObjectTreeNode *other )
{
	objectType = other->objectType;
	arrayIdx = other->arrayIdx;
	pObj = other->pObj;
}

void WorldObjectTreeNode::setPlayer( L2Player *player )
{
	objectType = L2OT_PC;
	pObj = dynamic_cast<L2Object *>(player);
}

void WorldObjectTreeNode::setNpc( L2Npc *npc )
{
	objectType = L2OT_NPC;
	pObj = dynamic_cast<L2Object *>(npc);
}

void WorldObjectTreeNode::setGroundItem( GroundItem *item )
{
	objectType = L2OT_ITEM;
	pObj = dynamic_cast<L2Object *>(item);
}

L2Player   *WorldObjectTreeNode::cast_L2Player() const
{
	if( this->objectType != L2OT_PC ) return NULL;
	return (L2Player *)(this->pObj);
}

L2Npc      *WorldObjectTreeNode::cast_L2Npc() const
{
	if( this->objectType != L2OT_NPC ) return NULL;
	return (L2Npc *)this->pObj;
}

GroundItem *WorldObjectTreeNode::cast_GroundItem() const
{
	if( this->objectType != L2OT_ITEM ) return NULL;
	return (GroundItem *)this->pObj;
}
