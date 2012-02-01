#include "stdafx.h"
#include "Logger.h"
#include "UserAI.h"
#include "RadarDllWnd.h"

void UserAI::UAI_Parse_MoveToLocation( class UserAI *cls, void *l2_game_packet )
{
	if( !l2_game_packet ) return;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	// first try pass it to user
	if( cls->usr.parse_MoveToLocation( p ) ) return; // this was user movement, return
	//
	p->getPacketType();
	unsigned int objectID = p->readUInt();
	L2OBJECT_TYPE objType = L2OT_NONE;
	int idx = -1;
	if( WorldObjectTree_GetInfoByObjectID( objectID, &objType, &idx ) )
	{
		// is it other char move?
		if( (objType == L2OT_PC) && (idx>=0) )
		{
			if( !chars_array[idx]->isUnused() )
			{
				chars_array[idx]->parse_MoveToLocation( p );
			}
		}
		else if( (objType == L2OT_NPC) && (idx>=0) )
		{
			if( !npc_array[idx]->isUnused() )
			{
				npc_array[idx]->parse_MoveToLocation( p );
			}
		}
	}
	// something is moving, but not found in chars array? invis GM!
	else
	{
		p->getPacketType();
		objectID = p->readUInt();
		int dx = p->readD();
		int dy = p->readD();
		int dz = p->readD();
		int x = p->readD();
		int y = p->readD();
		int z = p->readD();
		cls->invisGM.handlePossibleInvisGM( objectID, cls->invisGM.ACT_MOVE,
			x, y, z, dx, dy, dz, 0, 0 );
		// sound alert
		cls->playSoundAlert( cls->soundAlert_alertOnInvisGM );
	}
}




void UserAI::UAI_Parse_StopMove( class UserAI *cls, void *l2_game_packet )
{
	if( !l2_game_packet ) return;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	// parse
	p->getPacketType();
	unsigned int objectID = p->readUInt();
	int x = p->readD();
	int y = p->readD();
	int z = p->readD();
	unsigned int heading = p->readD(); // heading
	//
	if( cls->usr.objectID == objectID )
	{
		// stop user move
		cls->usr.xDst = cls->usr.yDst = cls->usr.zDst = 0x7fffffff;
		cls->usr.heading = heading;
		return;
	}
	int idx = -1;
	// update also party
	if( cls->party.isInParty( objectID, &idx ) )
	{
		L2Player *pl = cls->party.getPartyPlayer( idx );
		if( pl )
		{
			pl->x = x;
			pl->y = y;
			pl->z = z;
			pl->heading = heading;
			pl->stopMove();
		}
	}
	// not user move, try to stop move world object
	L2OBJECT_TYPE objType = L2OT_NONE;
	if( WorldObjectTree_GetInfoByObjectID( objectID, &objType, &idx ) )
	{
		// is it other char move?
		if( (objType == L2OT_PC) && (idx>=0) )
		{
			if( !chars_array[idx]->isUnused() )
			{
				chars_array[idx]->x = x;
				chars_array[idx]->y = y;
				chars_array[idx]->z = z;
				chars_array[idx]->heading = heading;
				chars_array[idx]->stopMove();
			}
		}
		else if( (objType == L2OT_NPC) && (idx>=0) )
		{
			if( !npc_array[idx]->isUnused() )
			{
				npc_array[idx]->x = x;
				npc_array[idx]->y = y;
				npc_array[idx]->z = z;
				npc_array[idx]->heading = heading;
				npc_array[idx]->stopMove();
			}
		}
	}
	else // something stopped move, but not found?
	{
		cls->invisGM.handlePossibleInvisGM( objectID, cls->invisGM.ACT_STOP_MOVE, x, y, z, 0,0,0, 0,0 );
		// sound alert
		cls->playSoundAlert( cls->soundAlert_alertOnInvisGM );
	}
}

// TODO: может где-то тут вычислять новые dZ в оокне радара?

void UserAI::UAI_process_ChangeMoveType( unsigned char *bytes, unsigned int len )
{
	L2GamePacket p( bytes, len );
	p.getPacketType();
	unsigned int oid = p.readUInt();
	int isRunning = p.readD();
	int idx = 0;
	L2OBJECT_TYPE objType = L2OT_NONE;
	if( oid == usr.objectID )
	{
		usr.isRunning = isRunning;
	}
	else if( party.isInParty( oid, &idx ) )
	{
		party.getPartyPlayer( idx )->isRunning = isRunning;
	}
	else if( WorldObjectTree_GetInfoByObjectID( oid, &objType, &idx ) )
	{
		if( objType == L2OT_PC )
			chars_array[idx]->isRunning = isRunning;
	}
	else log_error( LOG_ERROR, "UAI_process_ChangeMoveType(): cannot find oid [%u]\n", oid );
}

void UserAI::UAI_process_ChangeWaitType( unsigned char *bytes, unsigned int len )
{
	L2GamePacket p( bytes, len );
	p.getPacketType();
	unsigned int oid = p.readUInt();
	int waitType = p.readD();
	const int SITTING = 0;
	//const int STANDING = 1;
	//const int START_FAKEDEATH = 2;
	//const int STOP_FAKEDEATH = 3;
	int idx = 0;
	L2OBJECT_TYPE objType = L2OT_NONE;
	if( oid == usr.objectID )
	{
		usr.isSitting = (waitType == SITTING) ? 1 : 0;
	}
	else if( party.isInParty( oid, &idx ) )
	{
		party.getPartyPlayer( idx )->isSitting = (waitType == SITTING) ? 1 : 0;
	}
	else if( WorldObjectTree_GetInfoByObjectID( oid, &objType, &idx ) )
	{
		if( objType == L2OT_PC )
			chars_array[idx]->isSitting = (waitType == SITTING) ? 1 : 0;
	}
	else log_error( LOG_ERROR, "UAI_process_ChangeWaitType(): cannot find oid [%u]\n", oid );
}

/**
writeC(0x22);
writeD(_targetObjId);
writeD(_x);
writeD(_y);
writeD(_z);
writeD(0x00); // in >= G.Final */
void UserAI::UAI_Parse_TeleportToLocation( class UserAI *cls, void *l2_game_packet )
{
	if( !l2_game_packet ) return;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	// parse
	p->getPacketType();
	unsigned int objectID = p->readUInt();
	int x = p->readD();
	int y = p->readD();
	int z = p->readD();
	unsigned int heading = p->readD(); // heading
	// we are teleporting?
	if( cls->usr.objectID == objectID )
	{
		// update user coords
		cls->usr.xDst = cls->usr.yDst = cls->usr.zDst = 0x7fffffff;
		cls->usr.x = x;  cls->usr.y = y;  cls->usr.z = z;
		cls->usr.lastTick = GetTickCount();
		cls->usr.heading = heading;
		// clear all world objects info except party
		log_error( LOG_USERAI, "You teleported to (%d,%d,%d); clearing all world objects info\n", x, y, z );
		CharArray_DeleteAll();
		NpcArray_DeleteAll();
		GIArray::getInstance()->DeleteAll();
		// clear chars & npc from radar window
		RadarWnd_ForceUpdateCharsList();
		RadarWnd_ForceUpdateNpcsList();
		return;
	}
	int idx = -1;
	L2OBJECT_TYPE objType = L2OT_NONE;
	if( cls->party.isInParty( objectID, &idx ) )
	{
		L2Player *pl = cls->party.getPartyPlayer( idx );
		if( pl )
		{
			pl->x = x;  pl->y = y;  pl->z = z;  pl->heading = heading;
		}
	}
	else if( WorldObjectTree_GetInfoByObjectID( objectID, &objType, &idx ) )
	{
		if( objType == L2OT_PC )
		{
			chars_array[idx]->x = x;  chars_array[idx]->y = y;  chars_array[idx]->z = z;
			chars_array[idx]->heading = heading;
		}
	}
	else log_error( LOG_ERROR, "UAI_Parse_TeleportToLocation(): cannot find objectID [%u]\n", objectID );
}
