#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

// FE:0080: d(oid) S(msg)
void L2Client::ph_ExSetPrivateStoreWholeMsg( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType(); // 0xFE
	p->readUShort(); // 0x0080
	unsigned int oid = p->readUInt();
	const wchar_t *msg = p->readUnicodeStringPtr();
	// is it user?
	if( oid == pcls->usr.objectID )
	{
		memset( pcls->usr.privateStoreMsgSell, 0, 128 );
		wcsncpy( pcls->usr.privateStoreMsgSell, msg, 63 );
		pcls->usr.privateStoreMsgSell[63] = 0;
		return;
	}
	// try to find oid in world tree, must be player
	WorldObjectTreeNode wotNode;
	if( pcls->world_tree.GetInfoByObjectID( oid, &wotNode ) )
	{
		int idx = wotNode.getArrayIdx();
		if( wotNode.getObjectType() == L2OT_PC )
		{
			memset( pcls->world_chars.chars_array[idx]->privateStoreMsgSell, 0, 128 );
			wcsncpy( pcls->world_chars.chars_array[idx]->privateStoreMsgSell, msg, 63 );
			pcls->world_chars.chars_array[idx]->privateStoreMsgSell[63] = 0;
		}
		else log_error( LOG_ERROR, "ExSetPrivateStoreWholeMsg: oid is not player!\n" );
	}
	else log_error( LOG_ERROR, "ExSetPrivateStoreWholeMsg: cannot find objectID!\n" );
}
