#include "stdafx.h"
#include "WorldObjectTree.h"
#ifdef _DEBUG
#include "Logger.h"
#endif

void WorldObjectTree::Init()
{
	world_object_tree = new BinTree();
#ifdef WOT_LOCK
	InitializeCriticalSection( &wot_cs );
#endif
}

void WorldObjectTree::Free()
{
#ifdef WOT_LOCK
	EnterCriticalSection( &wot_cs );
#endif
	delete world_object_tree;
	world_object_tree = NULL;
#ifdef WOT_LOCK
	LeaveCriticalSection( &wot_cs );
	DeleteCriticalSection( &wot_cs );
#endif
}

BOOL WorldObjectTree::GetInfoByObjectID( unsigned int objectID, WorldObjectTreeNode *pNode )
{
	//if( !world_object_tree ) return FALSE;
	pNode->~WorldObjectTreeNode();
#ifdef WOT_LOCK
	EnterCriticalSection( &wot_cs );
#endif
	WorldObjectTreeNode *node = (WorldObjectTreeNode *)world_object_tree->findNode( objectID );
#ifdef WOT_LOCK
	LeaveCriticalSection( &wot_cs );
#endif
	if( !node ) return FALSE;
	pNode->setFrom( node );
	return TRUE;
}

BOOL WorldObjectTree::AddObject( unsigned int objectID, WorldObjectTreeNode *pNode )
{
	//if( !world_object_tree ) return FALSE;
	WorldObjectTreeNode *wotNode = new WorldObjectTreeNode();
	wotNode->setFrom( pNode );
	wotNode->key = objectID;
#ifdef WOT_LOCK
	EnterCriticalSection( &wot_cs );
#endif

#ifdef _WOT_PC_DEBUG
	if( wotNode->getObjectType() == L2OT_PC )
		log_error( LOG_USERAI, "WOT: adding PC oid %u (idx %d)", objectID, wotNode->getArrayIdx() );
#endif

	bool ret = world_object_tree->addNode( wotNode );

#ifdef _WOT_PC_DEBUG
	if( wotNode->getObjectType() == L2OT_PC )
	{
		if( ret ) log_error_np( LOG_USERAI, " ... add ok\n" );
		else log_error_np( LOG_USERAI, " ... ERROR!\n" );
	}
#endif

#ifdef WOT_LOCK
	LeaveCriticalSection( &wot_cs );
#endif
	if( ret ) return TRUE;
	return FALSE;
}

BOOL WorldObjectTree::DelObject( unsigned int objectID )
{
	//if( !world_object_tree ) return FALSE;
#ifdef WOT_LOCK
	EnterCriticalSection( &wot_cs );
#endif

#ifdef _WOT_PC_DEBUG
	BOOL ret = FALSE;
	WorldObjectTreeNode *node = (WorldObjectTreeNode *)world_object_tree->findNode( objectID );
	if( node )
	{
		L2OBJECT_TYPE objectType = node->getObjectType();
		//if( objectType == L2OT_PC )
		//	log_error( LOG_USERAI, "WOT: deleting PC oid %u (idx %d)", node->key, node->getArrayIdx() );
		// detete!
		ret = world_object_tree->delNode( node );
		// again debug print
		if( objectType == L2OT_PC )
		{
			if( ret ) log_error_np( LOG_USERAI, " ... del ok\n" );
			else log_error_np( LOG_USERAI, " ... ERROR\n" );
		}
	}
#else
	BOOL ret = (BOOL)world_object_tree->delNode( objectID );
#endif

#ifdef WOT_LOCK
	LeaveCriticalSection( &wot_cs );
#endif
	return ret;
}

void WorldObjectTree::DeleteAll()
{
#ifdef WOT_LOCK
	EnterCriticalSection( &wot_cs );
#endif
	this->world_object_tree->deleteAll();
#ifdef WOT_LOCK
	LeaveCriticalSection( &wot_cs );
#endif
}
