#include "stdafx.h"
#include "Logger.h"
#include "NpcArray.h"

void NpcArray::Init()
{
	npcArray_updateHWND = NULL;
	npcArray_updateMSG = 0;
	npcArray_count = 0;
	InitializeCriticalSection( &cs_npcs_array );
	int i;
	for( i=0; i<NPCA_MAX_NPCS; i++ ) npcs_array[i] = new L2Npc();
}

void NpcArray::Free()
{
	Lock();
	int i;
	for( i=0; i<NPCA_MAX_NPCS; i++ )
	{
		if( npcs_array[i] != NULL )
		{
			delete npcs_array[i];
			npcs_array[i] = NULL;
		}
	}
	npcArray_count = 0;
	Unlock();
	DeleteCriticalSection( &cs_npcs_array );
}

int NpcArray::AddNpcInfo( L2Npc *pNpcInfo )
{
	Lock();
	// Add new npc info
	int idx = FindFreeIndex();
	if( idx == -1 )
	{
		Unlock();
		return -1;
	}
	if( !npcs_array[idx] ) npcs_array[idx] = new L2Npc();
	if( !npcs_array[idx] )
	{
		Unlock();
		return -1;
	}
	memcpy( npcs_array[idx], pNpcInfo, sizeof(class L2Npc) );
	npcArray_count++;
	Unlock();
	PostUpdateMessage();
	//log_error( LOG_OK, "NpcArray: Added new[%d]: [%S] [%S] (%d,%d,%d) tpml %u, oid %u\n",
	//	idx,
	//	name, title, x,y,z, templateID, objectID );
	return idx;
}

void NpcArray::UpdateNpcInfo( int idx, L2Npc *pNpcInfo )
{
	Lock();
	// update info on existing NPC
	if( !npcs_array[idx] )
	{
		log_error( LOG_ERROR, "NpcArray_UpdateNpcInfo(): idx[%d] is free!\n", idx );
		// try to allocate!
		npcs_array[idx] = new L2Npc();
		if( !npcs_array[idx] )
		{
			Unlock();
			return;
		}
	}
	if( npcs_array[idx]->isUnused() ) log_error( LOG_ERROR, "NpcArray_UpdateNpcInfo(): idx[%d] is unused!\n", idx );
	// save old move vectors (since by default unused NpcInfo is not moving)
	int xDst = npcs_array[idx]->xDst;
	int yDst = npcs_array[idx]->yDst;
	int zDst = npcs_array[idx]->zDst;
	//
	memcpy( npcs_array[idx], pNpcInfo, sizeof(class L2Npc) );
	// restore old move vectors (since by default unused NpcInfo is not moving)
	npcs_array[idx]->xDst = xDst;
	npcs_array[idx]->yDst = yDst;
	npcs_array[idx]->zDst = zDst;
	//
	Unlock();
	PostUpdateMessage();
	//log_error( LOG_OK, "NpcArray: Updated[%d]: [%S] [%S] (%d,%d,%d) tpml %u, oid %u\n",
	//	idx,
	//	name, title, x,y,z, templateID, objectID );
}

/*void NpcArray::DelNPC( unsigned int objectID )
{
	Lock();
	int idx = FindNPCByObjectID( objectID );
	if( idx == -1 )
	{
		Unlock();
		return;
	}
	if( npcs_array[idx] )
	{
		npcs_array[idx]->setUnused();
	}
	//log_error( LOG_OK, "NpcArray: deleted " );
	Unlock();
}*/

void NpcArray::DelNPCByArrayIdx( int idx )
{
	if( idx<0 || (idx>=NPCA_MAX_NPCS) ) return;
	Lock();
	if( npcs_array[idx] )
	{
		npcs_array[idx]->setUnused();
		npcArray_count--;
		//log_error( LOG_OK, "NpcArray: deleted npc %u from index OK\n", objectID, idx );
	}
	Unlock();
	PostUpdateMessage();
}

void NpcArray::DeleteAll()
{
	Lock();
	int i;
	for( i=0; i<NPCA_MAX_NPCS; i++ )
	{
		npcs_array[i]->setUnused();
	}
	npcArray_count = 0;
	Unlock();
	PostUpdateMessage();
}

void NpcArray::Lock()
{
	EnterCriticalSection( &cs_npcs_array );
}

void NpcArray::Unlock()
{
	LeaveCriticalSection( &cs_npcs_array );
}

int NpcArray::FindNPCByObjectID( unsigned int objectID )
{
	int ret = -1;
	int i;
	Lock();
	for( i=0; i<NPCA_MAX_NPCS; i++ )
	{
		if( npcs_array[i] == NULL ) continue;
		if( npcs_array[i]->objectID == objectID )
		{
			ret = i;
			break;
		}
	}
	Unlock();
	return ret;
}

int NpcArray::FindFreeIndex()
{
	int i;
	Lock();
	for( i=0; i<NPCA_MAX_NPCS; i++ )
	{
		if( npcs_array[i] == NULL )
		{
			npcs_array[i] = new L2Npc(); // allocate new 
			npcs_array[i]->setUnused(); // and set as unused
			Unlock();
			return i;
		}
		if( npcs_array[i]->isUnused() )
		{
			Unlock();
			return i;
		}
	}
	Unlock();
	log_error( LOG_ERROR, "NpcArray_FindFreeIndex(): canot find free index, limit is %d!\n", NPCA_MAX_NPCS );
	return -1;
}

void NpcArray::SetUpdateCommand( HWND hWnd, UINT uMsg )
{
	npcArray_updateHWND = hWnd;
	npcArray_updateMSG = uMsg;
}

void NpcArray::PostUpdateMessage()
{
	if( !npcArray_updateHWND ) return;
	if( npcArray_updateMSG < WM_USER ) return;
	PostMessage( npcArray_updateHWND, npcArray_updateMSG, 0, 0 );
}

void NpcArray::DisplayToConsole()
{
	int i = 0;
	unsigned int nDisplayed = 0;
	log_error( LOG_USERAI, "=== NPCs: %d ===\n", npcArray_count );
	if( npcArray_count > 0 )
	{
		Lock();
		for( i=0; i<NPCA_MAX_NPCS; i++ )
		{
			if( npcs_array[i]->isUnused() )
				log_error( LOG_USERAI, "NPC: %2d: <unused>\n", i );
			else
			{
				log_error( LOG_USERAI, "NPC: %2d: %u [%S] [%S]\n", i,
					npcs_array[i]->templateID,
					npcs_array[i]->charName, npcs_array[i]->charTitle );
				nDisplayed++;
			}
			if( nDisplayed >= npcArray_count ) break;
		}
		Unlock();
	}
	log_error( LOG_USERAI, "=== NPCs End ===\n" );
}
