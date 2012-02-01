#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_InventoryUpdate( class L2Client *pcls, L2GamePacket *p )
{
	pcls->inv.parse_InventoryUpdate( p, pcls->account.getL2Version() );
	pcls->postUpdateUI( UPDATE_INV );
}
