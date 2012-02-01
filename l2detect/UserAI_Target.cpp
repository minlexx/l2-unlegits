#include "stdafx.h"
#include "UserAI.h"
#include "Logger.h"
#include "RadarDllWnd.h"
#include "PacketInjector.h"
#include "GameClient.h"
extern class GameClient *g_game_client;

void UserAI::UAI_Parse_TargetSelected( class UserAI *cls, void *l2_game_packet )
{
	if( !l2_game_packet ) return;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	p->getPacketType();
	// objectID targets targetObjectID :)
	// x,y,z are coords of objectID (on L2J)
	unsigned int objectID       = p->readUInt();
	unsigned int targetObjectID = p->readUInt();
	int x                       = p->readInt();
	int y                       = p->readInt();
	int z                       = p->readInt();
	x; y; z; // fix warning C4189: y: локальная переменная инициализирована, но не использована
	if( objectID == cls->usr.objectID )
	{
		//log_error( LOG_USERAI, "TargetSelected (User targets %u at [%d,%d,%d])\n", targetObjectID, x,y,z );
		cls->usr.targetObjectID = targetObjectID;
		cls->usr.targetCurHP = cls->usr.targetMaxHP = 0;
		cls->lockTargetMgr.onUserTargetSelected( targetObjectID );
	}
	else
	{
		//log_error( LOG_USERAI, "TargetSelected (oid %u targets %u at [%d,%d,%d])\n", objectID, targetObjectID, x,y,z );
		int partyIdx = 0;
		if( cls->party.isInParty( objectID, &partyIdx ) )
		{
			log_error( LOG_USERAI, "TargetSelected (Party member %S selected %u)\n",
				cls->party.getPartyPlayer( partyIdx )->charName, targetObjectID );
			cls->party.getPartyPlayer( partyIdx )->targetObjectID = targetObjectID;
		}
		//
		int idx = -1;
		L2OBJECT_TYPE objType = L2OT_NONE;
		if( WorldObjectTree_GetInfoByObjectID( objectID, &objType, &idx ) )
		{
			if( objType == L2OT_PC )
			{
				CharArray_Lock();
				chars_array[idx]->targetObjectID = targetObjectID;
				// are YOU on target?
				if( targetObjectID == cls->usr.objectID )
				{
					// you are targeted by other player
					log_error( LOG_USERAI, "You are on target of [%S]\n", chars_array[idx]->charName );
					// alert using sound
					bool shouldAlert = true;
					// do not alert for party members, if set
					if( (cls->soundAlert_noPartyTargetAlert) && (cls->party.getCount() > 0) )
						if( cls->party.isInParty( objectID, NULL ) ) shouldAlert = false;
					// do not alert for clan members, if set
					if( (cls->soundAlert_noClanTargetAlert) && (cls->usr.clanID == chars_array[idx]->clanID) )
						shouldAlert = false;
					if( shouldAlert ) cls->playSoundAlert( cls->soundAlert_alertOnTargeted );
					// send string message, if set
					if( shouldAlert && (cls->soundAlert_screenMessage) )
					{
						wchar_t screenMsg[64];
						// channelID (15 - Commander, 17 - Hero)
						swprintf( screenMsg, 63, L"%s targets you", chars_array[idx]->charName );
						PGenC_CreatureSay( 0, 15, L"L2Detect", screenMsg );
					}
				}
				CharArray_Unlock();
			}
		}
	}
	// add info to radar window
	RadarWnd_NotifyTarget( objectID, targetObjectID, true );
}

void UserAI::UAI_Parse_TargetUnselected( class UserAI *cls, void *l2_game_packet )
{
	if( !l2_game_packet ) return;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	p->getPacketType();
	// objectID is ID ob object whos target is cancelled
	// x,y,z are coords of objectID (on L2J)
	unsigned int objectID       = p->readUInt();
	int x                       = p->readInt();
	int y                       = p->readInt();
	int z                       = p->readInt();
	x; y; z; // fix warning C4189: y: локальная переменная инициализирована, но не использована
	if( objectID == cls->usr.objectID )
	{
		//log_error( LOG_USERAI, "TargetUnselected (User at [%d,%d,%d])\n", x,y,z );
		cls->usr.targetObjectID = 0;
		cls->usr.targetCurHP = cls->usr.targetMaxHP = 0;
		cls->lockTargetMgr.onUserTargetLost();
	}
	else
	{
		//log_error( LOG_USERAI, "TargetUnselected (objectID %u at [%d,%d,%d])\n", objectID, x,y,z );
		int partyIdx = 0;
		if( cls->party.isInParty( objectID, &partyIdx ) )
		{
			log_error( LOG_USERAI, "TargetUnselected (Party member %S)\n",
				cls->party.getPartyPlayer( partyIdx )->charName );
			cls->party.getPartyPlayer( partyIdx )->targetObjectID = 0;
		}
		int idx = -1;
		L2OBJECT_TYPE objType = L2OT_NONE;
		if( WorldObjectTree_GetInfoByObjectID( objectID, &objType, &idx ) )
		{
			if( objType == L2OT_PC )
			{
				chars_array[idx]->targetObjectID = 0;
			}
		}
	}
	// remove from radar window
	RadarWnd_NotifyTarget( objectID, 0, false );
}

/** TargetSelected
protected final void writeImpl()
	{
		writeC(0x23);
		writeD(_objectId);
		writeD(_targetObjId);
		writeD(_x);
		writeD(_y);
		writeD(_z);
		writeD(0x00);
	} */

/** TargetUnselected
protected final void writeImpl()
	{
		writeC(0x24);
		writeD(_targetObjId);
		writeD(_x);
		writeD(_y);
		writeD(_z);
		writeD(0x00); //??
	} */


void UserAI::UAI_Parse_MyTargetSelected( class UserAI *cls, void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	// parse
	unsigned char ptype = p->getPacketType();
	// p->readH(); // color code, depends on level difference
	// p->readD(); // 0x00 always
	if( ptype != 0xB9 )
	{
		log_error( LOG_WARNING, "UserAI::UAI_Parse_MyTargetSelected: ptype != 0xB9!\n" );
		return;
	}
	unsigned int targetObjectID = p->readUInt();
	// end parsing
	cls->usr.targetObjectID = targetObjectID;
	cls->usr.targetCurHP = cls->usr.targetMaxHP = 0;
	// notify radar window
	RadarWnd_NotifyTarget( cls->usr.objectID, targetObjectID, true );
}

/* writeC(0xb9);
   writeD(_objectId);
   writeH(_color);
   writeD(0x00); */
//unsigned int objectID = bytes[3] | (bytes[4] << 8) | (bytes[5] << 16) | (bytes[6] <<24);