#include "stdafx.h"
#include "WorldObjectTree.h"
#ifdef _DEBUG
#include "Logger.h"
#endif

//#define WOT_LOCK

BinTree *world_object_tree = NULL;
#ifdef WOT_LOCK
CRITICAL_SECTION wot_cs;
#endif

void WorldObjectTree_Init()
{
	world_object_tree = new BinTree();
#ifdef WOT_LOCK
	InitializeCriticalSection( &wot_cs );
#endif
}

void WorldObjectTree_Free()
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

BOOL WorldObjectTree_GetInfoByObjectID( unsigned int objectID, LPL2OBJECT_TYPE pObjectType, int *pIndexInArray )
{
	//if( !world_object_tree ) return FALSE;
	(*pObjectType) = L2OT_NONE;
	(*pIndexInArray) = -1;
#ifdef WOT_LOCK
	EnterCriticalSection( &wot_cs );
#endif
	WorldObjectTreeNode *node = (WorldObjectTreeNode *)world_object_tree->findNode( objectID );
#ifdef WOT_LOCK
	LeaveCriticalSection( &wot_cs );
#endif
	if( !node ) return FALSE;
	(*pObjectType) = node->objectType;
	(*pIndexInArray) = node->arrayIdx;
	return TRUE;
}

BOOL WorldObjectTree_AddObject( unsigned int objectID, L2OBJECT_TYPE objectType, int indexInArray )
{
	if( indexInArray < 0 ) return FALSE;
	if( !world_object_tree ) return FALSE;
	WorldObjectTreeNode *wotNode = new WorldObjectTreeNode( objectID, objectType, indexInArray );
#ifdef WOT_LOCK
	EnterCriticalSection( &wot_cs );
#endif

#ifdef _DEBUG
	if( objectType == L2OT_PC )
		log_error( LOG_DEBUG, "WOT: adding PC oid %u (idx %d)", objectID, indexInArray );
#endif

	bool ret = world_object_tree->addNode( wotNode );

#ifdef _DEBUG
	if( objectType == L2OT_PC )
	{
		if( ret ) log_error_np( LOG_DEBUG, " ... add ok\n" );
		else log_error_np( LOG_DEBUG, " ... ERROR!\n" );
	}
#endif

#ifdef WOT_LOCK
	LeaveCriticalSection( &wot_cs );
#endif
	if( ret ) return TRUE;
	return FALSE;
}

BOOL WorldObjectTree_DelObject( unsigned int objectID )
{
	//if( !world_object_tree ) return FALSE;
#ifdef WOT_LOCK
	EnterCriticalSection( &wot_cs );
#endif

#ifdef _DEBUG
	BOOL ret = FALSE;
	WorldObjectTreeNode *node = (WorldObjectTreeNode *)world_object_tree->findNode( objectID );
	if( node )
	{
		L2OBJECT_TYPE objectType = node->objectType;
		if( objectType == L2OT_PC )
			log_error( LOG_DEBUG, "WOT: deleting PC oid %u (idx %d)", node->key, node->arrayIdx );

		ret = world_object_tree->delNode( node );

		if( objectType == L2OT_PC )
		{
			if( ret ) log_error_np( LOG_DEBUG, " ... del ok\n" );
			else log_error_np( LOG_DEBUG, " ... ERROR\n" );
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
