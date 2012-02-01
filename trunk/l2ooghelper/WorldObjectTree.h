#ifndef H_WORLD_OBJECT_TREE
#define H_WORLD_OBJECT_TREE
#include "WorldObjectTreeNode.h"

#define WOT_LOCK

class WorldObjectTree
{
public:
	WorldObjectTree() { world_object_tree = NULL; Init(); }
	~WorldObjectTree() { Free(); }
public:
	void Init();
	void Free();
public:
	BOOL GetInfoByObjectID( unsigned int objectID, WorldObjectTreeNode *pNode );
	BOOL AddObject( unsigned int objectID, WorldObjectTreeNode *pNode );
	BOOL DelObject( unsigned int objectID );
	void DeleteAll();

#ifdef WOT_LOCK
	void Lock() { EnterCriticalSection( &wot_cs ); }
	void Unlock() { LeaveCriticalSection( &wot_cs ); }
#endif

protected:
	BinTree *world_object_tree;
#ifdef WOT_LOCK
	CRITICAL_SECTION wot_cs;
#endif
};

#endif
