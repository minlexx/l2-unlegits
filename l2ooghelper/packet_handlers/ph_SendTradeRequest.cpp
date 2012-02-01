#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/**  Trade receiver recieves:
Server: Len    7 [SendTradeRequest]
07 00
70             // pcode
CE 09 00 10    // object ID of person who wants to trade to you   **/
void L2Client::ph_SendTradeRequest( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType();
	unsigned int objectID = p->readUInt();
	
	// get requester name
	WorldObjectTreeNode wotNode;
	if( !pcls->world_tree.GetInfoByObjectID( objectID, &wotNode ) )
	{
		log_error( LOG_ERROR, "SendTradeRequest: trade requester [%u] not found in knownlist!\n", objectID );
		return;
	}

	// create question string
	TCHAR szQuestion[256] = {0};
	wsprintf( szQuestion, TEXT("Игрок %s хочет с вами торговать. Согласиться?"),
		pcls->world_chars.chars_array[wotNode.getArrayIdx()]->charName );

	// create MessageBoxTimeout dialog (15 seconds)
	MessageBoxTimeout *mb = new MessageBoxTimeout( pcls->hWnd, WMMY_UI_MESSAGEBOXTIMEOUTREPLY,
		MessageBoxTimeout::TYPE_TRADEREQUEST, szQuestion, 15 );
	mb->run( pcls->usr.charName );
}
