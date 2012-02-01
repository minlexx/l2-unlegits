#ifndef H_NPC_ARRAY
#define H_NPC_ARRAY

#define NPCA_MAX_NPCS 512

extern L2Npc *npc_array[NPCA_MAX_NPCS];

void NpcArray_Init();
void NpcArray_Free();
void NpcArray_DeleteAll();

void NpcArray_AddNpcInfo( L2Npc *pNpcInfo );
void NpcArray_DelNPC( unsigned int objectID );
void NpcArray_DelNPCByIdx( int idx );

void NpcArray_Lock();
void NpcArray_Unlock();

int NpcArray_FindNPCByObjectID( unsigned int objectID );
int NpcArray_FindFreeIndex();

//void NpcArray_SetUpdateCommand( HWND hWnd, UINT uMsg );

#endif
