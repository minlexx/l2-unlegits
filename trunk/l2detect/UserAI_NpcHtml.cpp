#include "stdafx.h"
#include "UserAI.h"
#include "Logger.h"

/**
NpcHtmlMessage::writeImpl()
{
	writeC(0x19);
	writeD(_npcObjId);
	writeS(_html);
	writeD(_itemId);
}

NpcQuestHtmlMessage::writeImpl()
{
	writeC(0xfe);
	writeH(0x8d);
	writeD(_npcObjId);
	writeS(_html);
	writeD(_questId);
}
**/

bool UserAI::UAI_process_NpcHtml( unsigned char *bytes, unsigned int len )
{
	if( !bytes || (len<1) ) return false;
	if( this->m_last_npc_html )
	{
		free( this->m_last_npc_html );
		this->m_last_npc_html = NULL;
	}
	L2GamePacket *p = new L2GamePacket( bytes, len );
	if( !p ) return false;
	unsigned char opcode = p->getPacketType(); // 0x19 - NpcHtmlMessage, 0xFE:0x008D - ExNpcQuestHtmlMessage
	if( opcode == 0xFE ) // ExNpcQuestHtmlMessage
		p->readH(); // read 0x008D
	this->m_last_npc_html_npcOid = p->readUInt();              // read npcObjID
	this->m_last_npc_html = p->readUnicodeString();            // read html
	// don't read itemID / questID
	delete p;
	// log last NpcHtml
	if( this->m_last_npc_html )
	{
		FILE *f = fopen( "NpcHtml.txt", "wt" );
		if( f )
		{
			fwprintf( f, L"%s", this->m_last_npc_html );
			fclose( f );
		}
	}
	//
	return true;
}

wchar_t *UserAI::get_last_NPC_HTML( unsigned int *npc_oid )
{
	if( npc_oid ) (*npc_oid ) = this->m_last_npc_html_npcOid;
	return m_last_npc_html;
}

void UserAI::clear_last_NPC_HTML()
{
	if( m_last_npc_html )
	{
		free( m_last_npc_html );
		m_last_npc_html = 0;
	}
	m_last_npc_html_npcOid = 0;
}
