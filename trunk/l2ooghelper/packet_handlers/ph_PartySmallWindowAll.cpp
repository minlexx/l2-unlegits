#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

/*
6C 08 00 10          // party leader object id
00 00 00 00          // party loot type
01 00 00 00          // number of members (except you)
// for every member
6C 08 00 10     // member oobject id
41 00 65 00  72 00 69 00  6E 00 00 00    // member name
CD 06 00 00     // current cp
CD 06 00 00     // max cp
61 0D 00 00     // current hp
61 0D 00 00     // max hp
7B 0A 00 00     // current mp
7B 0A 00 00     // max mp
50 00 00 00     // level
1E 00 00 00     // class id
00 00 00 00     // constant 00 00 00 00
01 00 00 00     // race
00 00 00 00     // constant 00 00 00 00 */
void L2Client::ph_PartySmallWindowAll( class L2Client *pcls, L2GamePacket *p )
{
	unsigned int leaderOid = 0;
	unsigned int lootType = 0;
	unsigned int nMembers = 0;
	unsigned int i = 0;
	L2Player pl;
	//
	pcls->party.clear();
	//
	p->getPacketType();
	leaderOid = p->readUInt();
	lootType = p->readUInt();
	//
	pcls->party.setLeaderObjectId( leaderOid );
	pcls->party.setItemDistribution( lootType );
	//
	nMembers = p->readUInt();
	for( i=0; i<nMembers; i++ )
	{
		pl.setUnused();
		pl.objectID = p->readUInt();
		pl.setName( p->readUnicodeStringPtr() );
		pl.curCp = (double)p->readInt();
		pl.maxCp = (double)p->readInt();
		pl.curHp = (double)p->readInt();
		pl.maxHp = (double)p->readInt();
		pl.curMp = (double)p->readInt();
		pl.maxMp = (double)p->readInt();
		pl.level = p->readInt();
		pl.classID = p->readInt();
		p->readInt();
		pl.race = p->readInt();
		p->readInt(); // 0x00
		// Gracia Final?
		if( pcls->account.serverVersion == (int)L2_VERSION_T23 )
		{
			p->readInt(); // T2.3 0x00
			// pet info
			unsigned int petObjectID = p->readUInt(); // or 0x00, if no pet
			if( petObjectID != 0 ) // must read pet info
			{
				p->readInt(); // pet npcId + 1000000
				p->readUnicodeStringPtr(); // pet name
				p->readInt(); // pet curHp
				p->readInt(); // pet maxHp
				p->readInt(); // pet curMp
				p->readInt(); // pet maxMp
				p->readInt(); // pet level
			}
		}
		//
		pcls->party.addPlayer( &pl );
	}
	pcls->postUpdateUI( UPDATE_PARTY );
}