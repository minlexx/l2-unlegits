#ifndef H_WORLD_OBJECT_TREE_NODE
#define H_WORLD_OBJECT_TREE_NODE
#include "BinTree.h"
#include "GroundItem.h"

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
	~WorldObjectTreeNode() { objectType = L2OT_NONE; arrayIdx = -1; pObj = NULL; };
public:
	void            setFrom( const WorldObjectTreeNode *other );
public:
	void            setArrayIdx( int ai ) { arrayIdx = ai; }
	void            setPlayer( L2Player *player );
	void            setNpc( L2Npc *npc );
	void            setGroundItem( GroundItem *item );
public:
	L2Player       *cast_L2Player() const;
	L2Npc          *cast_L2Npc() const;
	GroundItem     *cast_GroundItem() const;
public:
	int             getArrayIdx() const { return arrayIdx; }
	L2OBJECT_TYPE   getObjectType() const { return objectType; }
	unsigned int    getObjectID() const { return this->key; }
protected:
	L2OBJECT_TYPE   objectType;
	int             arrayIdx;
	L2Object       *pObj;
};

#endif
