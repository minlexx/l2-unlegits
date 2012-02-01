#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_ExNpcQuestHtmlMessage( class L2Client *pcls, L2GamePacket *p )
{
	p->readReset();
	p->readUChar();   // FE
	p->readUShort();  // 008D
	
	unsigned int   npcObjectID = p->readUInt();
	const wchar_t *html        = p->readUnicodeStringPtr();
	unsigned int   questID     = p->readUInt();

	// get NPC info
	L2Npc *pNpc = NULL;
	WorldObjectTreeNode node;
	if( pcls->world_tree.GetInfoByObjectID( npcObjectID, &node ) )
	{
		if( node.getObjectType() == L2OT_NPC )
		{
			int idx = node.getArrayIdx();
			pNpc = pcls->world_npcs.npcs_array[idx];
		}
	}

	// set copy of last NPC html to script engine to allow script to talk to NPCs
	pcls->getInterface()->set_last_NPC_HTML( html, pcls->usr.targetObjectID );

	// display UI dialog
	pcls->npcHtmlDlg->displayNPCHTML( html, pNpc, 0, questID );
}
