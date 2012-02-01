#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_PrivateStoreMsgBuy( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType(); // 0xBF
	unsigned int oid = p->readUInt();
	const wchar_t *msg = p->readUnicodeStringPtr();
	// is it user?
	if( oid == pcls->usr.objectID )
	{
		memset( pcls->usr.privateStoreMsgBuy, 0, 128 );
		wcsncpy( pcls->usr.privateStoreMsgBuy, msg, 63 );
		pcls->usr.privateStoreMsgBuy[63] = 0;
		return;
	}
	// try to find oid in world tree, must be player
	WorldObjectTreeNode wotNode;
	if( pcls->world_tree.GetInfoByObjectID( oid, &wotNode ) )
	{
		int idx = wotNode.getArrayIdx();
		if( wotNode.getObjectType() == L2OT_PC  &&  idx > 0 )
		{
			memset( pcls->world_chars.chars_array[idx]->privateStoreMsgBuy, 0, 128 );
			wcsncpy( pcls->world_chars.chars_array[idx]->privateStoreMsgBuy, msg, 63 );
			pcls->world_chars.chars_array[idx]->privateStoreMsgBuy[63] = 0;
		}
		else log_error( LOG_ERROR, "PrivateStoreMsgBuy: oid is not player!\n" );
	}
	else log_error( LOG_ERROR, "PrivateStoreMsgBuy: cannot find objectID!\n" );
}
