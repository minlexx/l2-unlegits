#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_ItemList( class L2Client *pcls, L2GamePacket *p )
{
	pcls->inv.parse_ItemList( p, pcls->account.getL2Version() );
	pcls->postUpdateUI( UPDATE_INV );
}
