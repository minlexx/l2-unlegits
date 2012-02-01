#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::ph_RecipeShopMsg( class L2Client *pcls, L2GamePacket *p )
{
	p->getPacketType(); // 0xE1
	unsigned int oid = p->readUInt();
	const wchar_t *msg = p->readUnicodeStringPtr();
	// is it user?
	if( oid == pcls->usr.objectID )
	{
		memset( pcls->usr.privateStoreMsgRecipe, 0, 128 );
		wcsncpy( pcls->usr.privateStoreMsgRecipe, msg, 63 );
		pcls->usr.privateStoreMsgRecipe[63] = 0;
		return;
	}
	// try to find oid in world tree, must be player
	WorldObjectTreeNode wotNode;
	if( pcls->world_tree.GetInfoByObjectID( oid, &wotNode ) )
	{
		int idx = wotNode.getArrayIdx();
		if( wotNode.getObjectType() == L2OT_PC )
		{
			memset( pcls->world_chars.chars_array[idx]->privateStoreMsgRecipe, 0, 128 );
			wcsncpy( pcls->world_chars.chars_array[idx]->privateStoreMsgRecipe, msg, 63 );
			pcls->world_chars.chars_array[idx]->privateStoreMsgRecipe[63] = 0;
		}
		else log_error( LOG_ERROR, "RecipeShopMsg: oid is not player!\n" );
	}
	else log_error( LOG_ERROR, "RecipeShopMsg: cannot find objectID!\n" );
}
