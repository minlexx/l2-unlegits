#include "stdafx.h"
#include "NpcArray.h"
#include "Logger.h"
#include "WorldObjectTree.h"
#include "RadarDllWnd.h"

L2Npc *npc_array[NPCA_MAX_NPCS];
CRITICAL_SECTION cs_npc_array;

//HWND npcArray_updateHWND;
//UINT npcArray_updateMSG;

// forward
//void NpcArray_PostUpdateMessage();

void NpcArray_Init()
{
	//npcArray_updateHWND = NULL;
	//npcArray_updateMSG = 0;
	InitializeCriticalSection( &cs_npc_array );
	int i;
	for( i=0; i<NPCA_MAX_NPCS; i++ ) npc_array[i] = new L2Npc();
}

void NpcArray_Free()
{
	NpcArray_Lock();
	int i;
	for( i=0; i<NPCA_MAX_NPCS; i++ )
	{
		if( npc_array[i] != NULL )
		{
			delete npc_array[i];
			npc_array[i] = NULL;
		}
	}
	NpcArray_Unlock();
	DeleteCriticalSection( &cs_npc_array );
}

void NpcArray_AddNpcInfo( L2Npc *pNpcInfo )
{
	NpcArray_Lock();
	int idx = -1;
	L2OBJECT_TYPE type;
	/*BOOL bRet = */WorldObjectTree_GetInfoByObjectID( pNpcInfo->objectID, &type, &idx );
	//
	if( (idx == -1) || (type != L2OT_NPC) )
	{
		// Add new npc info
		idx = NpcArray_FindFreeIndex();
		if( idx == -1 )
		{
			NpcArray_Unlock();
			return;
		}
		if( !npc_array[idx] ) npc_array[idx] = new L2Npc();
		if( !npc_array[idx] )
		{
			NpcArray_Unlock();
			return;
		}
		memcpy( npc_array[idx], pNpcInfo, sizeof(class L2Npc) );
		//log_error( LOG_OK, "NpcArray: Added new[%d]: [%S] [%S] (%d,%d,%d) tpml %u, oid %u\n",
		//	idx, name, title, x,y,z, templateID, objectID );
		// add to WorldObjectTree
		WorldObjectTree_AddObject( pNpcInfo->objectID, L2OT_NPC, idx );
		// add to radar window
		RadarWnd_AddNpc( pNpcInfo->objectID );
	}
	else if( (idx>=0) && (idx<NPCA_MAX_NPCS) && (type==L2OT_NPC) )
	{
		// update info on existing NPC
		if( !npc_array[idx] )
		{
			// try to allocate!
			npc_array[idx] = new L2Npc();
			if( !npc_array[idx] )
			{
				NpcArray_Unlock();
				return;
			}
		}
		memcpy( npc_array[idx], pNpcInfo, sizeof(class L2Npc) );
		//log_error( LOG_OK, "NpcArray: Updated[%d]: [%S] [%S] (%d,%d,%d) tpml %u, oid %u\n",
		//	idx, name, title, x,y,z, templateID, objectID );
		RadarWnd_UpdNpc( npc_array[idx]->objectID );
	}
	NpcArray_Unlock();
}

void NpcArray_DelNPC( unsigned int objectID )
{
	NpcArray_Lock();
	int idx = NpcArray_FindNPCByObjectID( objectID );
	if( idx == -1 )
	{
		NpcArray_Unlock();
		return;
	}
	if( npc_array[idx] )
	{
		npc_array[idx]->setUnused();
	}
	//log_error( LOG_OK, "NpcArray: deleted " );
	// delete also from world object tree
	WorldObjectTree_DelObject( objectID );
	NpcArray_Unlock();
	// del from radar window
	RadarWnd_DelNpc( objectID );
}

void NpcArray_DelNPCByIdx( int idx )
{
	if( idx<0 || (idx>=NPCA_MAX_NPCS) ) return;
	NpcArray_Lock();
	if( npc_array[idx] )
	{
		unsigned int objectID = npc_array[idx]->objectID;
		npc_array[idx]->setUnused();
		//log_error( LOG_OK, "NpcArray: deleted npc %u from index OK\n", objectID, idx );
		// delete from world object tree
		WorldObjectTree_DelObject( objectID );
		// del from radar window
	RadarWnd_DelNpc( objectID );
	}
	NpcArray_Unlock();
}

void NpcArray_Lock()
{
	EnterCriticalSection( &cs_npc_array );
}

void NpcArray_Unlock()
{
	LeaveCriticalSection( &cs_npc_array );
}

int NpcArray_FindNPCByObjectID( unsigned int objectID )
{
	int ret = -1;
	int i;
	NpcArray_Lock();
	for( i=0; i<NPCA_MAX_NPCS; i++ )
	{
		if( npc_array[i] == NULL ) continue;
		if( npc_array[i]->objectID == objectID )
		{
			ret = i;
			break;
		}
	}
	NpcArray_Unlock();
	return ret;
}

int NpcArray_FindFreeIndex()
{
	int i;
	NpcArray_Lock();
	for( i=0; i<NPCA_MAX_NPCS; i++ )
	{
		if( npc_array[i] == NULL )
		{
			npc_array[i] = new L2Npc(); // allocate new 
			npc_array[i]->setUnused(); // and set as unused
			NpcArray_Unlock();
			return i;
		}
		if( npc_array[i]->isUnused() )
		{
			NpcArray_Unlock();
			return i;
		}
	}
	NpcArray_Unlock();
	return -1;
}

//void NpcArray_SetUpdateCommand( HWND hWnd, UINT uMsg )
//{
//	npcArray_updateHWND = hWnd;
//	npcArray_updateMSG = uMsg;
//}

//void NpcArray_PostUpdateMessage()
//{
//	if( !npcArray_updateHWND ) return;
//	if( npcArray_updateMSG < WM_USER ) return;
//	PostMessage( npcArray_updateHWND, npcArray_updateMSG, 0, 0 );
//}

void NpcArray_DeleteAll()
{
	NpcArray_Lock();
	int i;
	for( i=0; i<NPCA_MAX_NPCS; i++ )
	{
		if( npc_array[i] ) npc_array[i]->setUnused();
	}
	//npcArray_count = 0;
	NpcArray_Unlock();
}
