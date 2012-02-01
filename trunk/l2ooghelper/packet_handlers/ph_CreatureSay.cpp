#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**
writeC(0x4a);
writeD(_objectId);
writeD(_textType);
writeS(_charName);
writeS(_text);     */
void L2Client::ph_CreatureSay( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	//
	unsigned int objectID = p->readUInt();
	unsigned int textType = p->readUInt();
	wchar_t charName[256];
	memset( charName, 0, sizeof(charName) );
	wcsncpy( charName, p->readUnicodeStringPtr(), 255 );
	charName[255] = 0;
	wchar_t *text = p->readUnicodeString();
	//
	switch( textType )
	{
	case L2_CHAT_MESSAGE::ALL:          pcls->addChatToTab( CHAT_ALL, text, charName ); break;
	case L2_CHAT_MESSAGE::SHOUT:        pcls->addChatToTab( CHAT_SHOUT, text, charName ); break;
	case L2_CHAT_MESSAGE::TRADE:        pcls->addChatToTab( CHAT_TRADE, text, charName ); break;
	case L2_CHAT_MESSAGE::TELL:         pcls->addChatToTab( CHAT_WHISPER, text, charName ); break;
	case L2_CHAT_MESSAGE::PARTY:        pcls->addChatToTab( CHAT_PARTY, text, charName ); break;
	case L2_CHAT_MESSAGE::CLAN:         pcls->addChatToTab( CHAT_CLAN, text, charName ); break;
	case L2_CHAT_MESSAGE::ALLIANCE:     pcls->addChatToTab( CHAT_ALLY, text, charName ); break;
	case L2_CHAT_MESSAGE::HERO_VOICE:   pcls->addChatToTab( CHAT_HERO, text, charName ); break;
	case L2_CHAT_MESSAGE::ANNOUNCEMENT: pcls->addChatAnnouncement( text ); break;
	}
	//
	pcls->scripter.call_onChat( objectID, textType, text, charName );
	free( text );
	text = NULL;
}
