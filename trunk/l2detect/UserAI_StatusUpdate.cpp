#include "stdafx.h"
#include "Logger.h"
#include "UserAI.h"

void UserAI::UAI_Parse_StatusUpdate( class UserAI *cls, void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	p->getPacketType();
	unsigned int objectID = p->readUInt();
	// check if it is user's StatusUpdate or its target StatusUpdate
	if( (objectID == cls->usr.objectID) || (objectID == cls->usr.targetObjectID) )
	{
		cls->usr.parse_StatusUpdate( l2_game_packet );
		return;
	}
	// this is some other's StatusUpdate
	// find this object
	L2Character *chr = NULL;
	int idx = -1;
	L2OBJECT_TYPE objType = L2OT_NONE;
	if( !WorldObjectTree_GetInfoByObjectID( objectID, &objType, &idx ) )
	{
		log_error( LOG_WARNING, "UserAI::UAI_Parse_StatusUpdate(): cannot find objectID [%u]\n", objectID );
		return;
	}
	switch( objType )
	{
	case L2OT_NONE:
	case L2OT_ITEM:
		log_error( LOG_WARNING, "UserAI::UAI_Parse_StatusUpdate(): invalid object type (%d) for objectID [%u] (item or none)\n",
			(int)objType, objectID );
		return;
		break;
	case L2OT_PC:
		if( idx >= CHARARRAY_MAX_CHARS )
		{
			log_error( LOG_ERROR, "UserAI::UAI_Parse_StatusUpdate(): idx[%d] is too big for chars_array! [oid %u]\n",
				idx, objectID );
			return;
		}
		chr = (L2Character *)chars_array[idx];
		if( chr == NULL )
			log_error( LOG_ERROR, "UserAI::UAI_Parse_StatusUpdate(): chars_array[%d] is NULL! [oid %u]\n",
				idx, objectID );
		break;
	case L2OT_NPC:
		if( idx >= NPCA_MAX_NPCS )
		{
			log_error( LOG_ERROR, "UserAI::UAI_Parse_StatusUpdate(): idx[%d] is too big for npc_array! [oid %u]\n",
				idx, objectID );
			return;
		}
		chr = (L2Character *)npc_array[idx];
		if( chr == NULL )
			log_error( LOG_ERROR, "UserAI::UAI_Parse_StatusUpdate(): npc_array[%d] is NULL! [oid %u]\n",
				idx, objectID );
		break;
	}
	//
	const int CUR_HP = 0x09;
	const int MAX_HP = 0x0a;
	const int CUR_MP = 0x0b;
	const int MAX_MP = 0x0c;
	// parse next
	int i = 0;
	int numberOfAttributes = p->readInt();
	int attr_id, attr_val;
	for( i=0; i<numberOfAttributes; i++ )
	{
		attr_id  = p->readUInt();
		attr_val = p->readUInt();
		switch( attr_id )
		{
		case CUR_HP: chr->curHp = attr_val; break;
		case MAX_HP: chr->maxHp = attr_val; break;
		case CUR_MP: chr->curMp = attr_val; break;
		case MAX_MP: chr->maxMp = attr_val; break;
		}
	}
}
