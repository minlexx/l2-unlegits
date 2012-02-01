#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**
Server: Len   19 [GameGuardQuery]
13 00
74
D9 3D 53 27  1D A5 72 2E  8B 03 17 20  A3 1E 5B C3  

Client: Len   19 [GameGuardReply]
13 00
CB
7F 97 F0 78  04 3C E6 D6  71 0C F6 89  DD 9E 06 70 
*/

void L2Client::ph_GameGuardQuery( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	unsigned int q1 = p->readUInt();
	unsigned int q2 = p->readUInt();
	unsigned int q3 = p->readUInt();
	unsigned int q4 = p->readUInt();
	// parse request
	if( q1 == 0x27533DD9 && q2 == 0x2E72A51D && q3 == 0x2017038B && q4 == 0xC35B1EA3 )
	{
		log_error( LOG_WARNING, "Received standard l2J GameGuardQuery, replying :)\n" );
		pcls->addChatToTab( CHAT_SYS, L"L2J GameGuardQuery" );
		// reply with well known answer
		pcls->send_GameGuardReply( 0x78F0977F, 0xD6E63C04, 0x89F60C71, 0x70069EDD );
		return;
	}
	//
	log_error( LOG_WARNING, "Received unknown GameGuardQuery 0x%08X 0x%08X 0x%08X 0x%08X!\n",
		q1, q2, q3, q4 );
	pcls->addChatToTab( CHAT_SYS, L"Unknown GameGuardQuery!!!!!" );
	pcls->send_GameGuardReply( rand(), rand(), rand(), rand() ); // :redlol:
}
