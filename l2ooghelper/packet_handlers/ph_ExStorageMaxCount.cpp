#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_ExStorageMaxCount( class L2Client *pcls, L2GamePacket *p )
{
	pcls->storageMaxCount.parse_ExStorageMaxCount( p );
	pcls->inv.invSize = pcls->storageMaxCount.inventory;
	pcls->postUpdateUI( UPDATE_INV );
}
