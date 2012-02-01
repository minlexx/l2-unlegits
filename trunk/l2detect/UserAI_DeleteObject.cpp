#include "stdafx.h"
#include "UserAI.h"
#include "Logger.h"
#include "RadarDllWnd.h"

void UserAI::UAI_Parse_DeleteObject( class UserAI *cls, void *l2_game_packet )
{
	if( !l2_game_packet ) return;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	//
	p->getPacketType();
	unsigned int objectID = p->readUInt();
	//
	L2OBJECT_TYPE L2ObjectType = L2OT_NONE;
	int indexInArray = 0;
	if( WorldObjectTree_GetInfoByObjectID( objectID, &L2ObjectType, &indexInArray ) )
	{
		if( L2ObjectType == L2OT_NPC )
		{
			//log_error( LOG_USERAI, "UserAI: DeleteObject: oid %u is NPC\n", objectID );
			NpcArray_DelNPCByIdx( indexInArray );
		}
		else if( L2ObjectType == L2OT_PC )
		{
			//log_error( LOG_USERAI, "UserAI: DeleteObject: oid %u is Player at index %d\n", objectID, indexInArray );
			// remove from radar window
			RadarWnd_NotifyTarget( objectID, 0, false );
			// was it our offparty BD/SWS?
			if( cls->off_BD_SWS.inv_bd_enable )
			{
				if( cls->offBD_inrange == true )
				{
					if( wcscmp( chars_array[indexInArray]->charName, cls->off_BD_SWS.nameBD ) == 0 )
					{
						cls->offBD_inrange = false;
						log_error( LOG_USERAI, "BD [%S] out of range\n", cls->off_BD_SWS.nameBD );
					}
				}
			}
			if( cls->off_BD_SWS.inv_sws_enable )
			{
				if( cls->offSWS_inrange == true )
				{
					if( wcscmp( chars_array[indexInArray]->charName, cls->off_BD_SWS.nameSWS ) == 0 )
					{
						cls->offSWS_inrange = false;
						log_error( LOG_USERAI, "SWS [%S] out of range\n", cls->off_BD_SWS.nameBD );
					}
				}
			}
			CharArray_DeleteCharByIdx( indexInArray );
		}
		else if( L2ObjectType == L2OT_ITEM )
		{
			//log_error( LOG_USERAI, "DeleteObject: deleting item %u...\n", objectID );
			GIArray *gia = GIArray::getInstance();
			gia->DelGIByArrayIdx( indexInArray );
			WorldObjectTree_DelObject( objectID );
		}
	}
}
