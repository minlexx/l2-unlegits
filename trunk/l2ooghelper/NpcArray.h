#ifndef H_NPC_ARRAY
#define H_NPC_ARRAY

class NpcArray
{
public:
	NpcArray() { Init(); }
	~NpcArray() { Free(); }
public:
	static const int NPCA_MAX_NPCS = 512;
	L2Npc *npcs_array[NPCA_MAX_NPCS];
public:
	void Init();
	void Free();
public:
	int  AddNpcInfo( L2Npc *pNpcInfo );
	void UpdateNpcInfo( int idx, L2Npc *pNpcInfo );
	//void DelNPC( unsigned int objectID );
	void DelNPCByArrayIdx( int idx );
	void DeleteAll();
public:
	void Lock();
	void Unlock();
public:
	int FindNPCByObjectID( unsigned int objectID );
	int FindFreeIndex();
public:
	void SetUpdateCommand( HWND hWnd, UINT uMsg );
	unsigned int getCount() const { return npcArray_count; }
	void DisplayToConsole();
protected:
	void PostUpdateMessage();
protected:
	unsigned int     npcArray_count;
	CRITICAL_SECTION cs_npcs_array;
	HWND             npcArray_updateHWND;
	UINT             npcArray_updateMSG;
};

#endif
