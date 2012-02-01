#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_NpcHtmlMessage( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	unsigned int   npcObjectID = p->readUInt(); // DO NOT TRUST THIS VALUE
	const wchar_t *html = p->readUnicodeStringPtr(); // no malloc()
	unsigned int   itemID = p->readUInt();
	// parse end

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
	pcls->npcHtmlDlg->displayNPCHTML( html, pNpc, itemID, 0 );
}
