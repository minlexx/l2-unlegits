#include "stdafx.h"
#include "UserAI.h"
#include "Logger.h"
#include "ConfigIni.h"
#include "RadarDllWnd.h"
#include "PacketInjector.h"

extern class CConfig g_cfg;

void UserAI::UAI_Parse_PartySmallWindowAdd( class UserAI *cls, void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	L2Player pl;
	p->getPacketType();
	//
	p->readUInt(); // user objectID (why?...)
	p->readUInt(); // 0x00
	pl.objectID = p->readUInt(); // party member objectID
	pl.setName( p->readUnicodeStringPtr() );
	pl.curCp = (double)p->readInt();
	pl.maxCp = (double)p->readInt();
	pl.curHp = (double)p->readInt();
	pl.maxHp = (double)p->readInt();
	pl.curMp = (double)p->readInt();
	pl.maxMp = (double)p->readInt();
	pl.level = p->readInt();
	pl.classID = p->readUInt();
	// p->readInt(); // 0x01
	// p->readInt(); // 0x00 можно не дочитывать конец пакета, нельзя только читать того чего нет
	//
	cls->party.addPlayer( &pl );
	RadarWnd_ForceUpdateCharsList();
	// inject StatusUpdate to client with info on new party member?
	if( g_cfg.L2Walker_InjectStatusUpdate ) PGenC_StatusUpdate( &pl );

}

void UserAI::UAI_Parse_PartySmallWindowAll( class UserAI *cls, void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	unsigned int leaderOid = 0;
	unsigned int lootType = 0;
	unsigned int nMembers = 0;
	unsigned int i = 0;
	unsigned int petObjectID = 0;
	L2Player pl;
	//
	cls->party.clear();
	//
	p->getPacketType();
	leaderOid = p->readUInt();
	lootType = p->readUInt();
	nMembers = p->readUInt();
	//
	cls->party.setLeaderObjectId( leaderOid );
	cls->party.setItemDistribution( lootType );
	//
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
		if( g_cfg.L2_version <= (int)L2_VERSION_T22 )
		{
			petObjectID = p->readUInt();
			if( petObjectID != 0 )
			{
				p->readD(); // writeD(member.getPet().getNpcId()+1000000);
				p->readUnicodeStringPtr(); // writeS(member.getPet().getName());
				p->readD(); // writeD((int)member.getPet().getCurrentHp());
				p->readD(); // writeD(member.getPet().getMaxHp());
				p->readD(); // writeD((int)member.getPet().getCurrentMp());
				p->readD(); // writeD(member.getPet().getMaxMp());
				p->readD(); // writeD(member.getPet().getLevel());
			}
		}
		else if( g_cfg.L2_version >= (int)L2_VERSION_T23 )
		{
			p->readInt(); // 0x00
			p->readInt(); // 0x00
			petObjectID = p->readUInt();
			if( petObjectID != 0 )
			{
				p->readD(); // writeD(member.getPet().getNpcId()+1000000);
				p->readUnicodeStringPtr(); // writeS(member.getPet().getName());
				p->readD(); // writeD((int)member.getPet().getCurrentHp());
				p->readD(); // writeD(member.getPet().getMaxHp());
				p->readD(); // writeD((int)member.getPet().getCurrentMp());
				p->readD(); // writeD(member.getPet().getMaxMp());
				p->readD(); // writeD(member.getPet().getLevel());
			}
		}
		//
		cls->party.addPlayer( &pl );
		// inject StatusUpdate to client with info on new party member?
		if( g_cfg.L2Walker_InjectStatusUpdate ) PGenC_StatusUpdate( &pl );
	}
	RadarWnd_ForceUpdateCharsList();
}

void UserAI::UAI_Parse_PartySmallWindowDelete( class UserAI *cls, void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	p->getPacketType();
	unsigned int oid = p->readUInt();
	//const wchar_t *playerName = p->readUnicodeStringPtr();
	cls->party.delPlayer( oid );
	RadarWnd_ForceUpdateCharsList();
}

void UserAI::UAI_Parse_PartySmallWindowDeleteAll( class UserAI *cls, void *l2_game_packet )
{
	UNREFERENCED_PARAMETER(l2_game_packet);
	// only trigger, no parse :)
	cls->party.deleteAll();
	cls->party.clear();
	RadarWnd_ForceUpdateCharsList();
}

void UserAI::UAI_Parse_PartySmallWindowUpdate( class UserAI *cls, void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	L2Player pl;
	p->getPacketType();
	//
	pl.objectID = p->readUInt();
	pl.setName( p->readUnicodeStringPtr() );
	pl.curCp = (double)p->readInt();
	pl.maxCp = (double)p->readInt();
	pl.curHp = (double)p->readInt();
	pl.maxHp = (double)p->readInt();
	pl.curMp = (double)p->readInt();
	pl.maxMp = (double)p->readInt();
	pl.level = p->readInt();
	pl.classID = p->readUInt();
	//
	cls->party.updatePlayer( &pl );
	// inject StatusUpdate to client with info on new party member?
	if( g_cfg.L2Walker_InjectStatusUpdate ) PGenC_StatusUpdate( &pl );
}

void UserAI::UAI_Parse_PartySpelled( class UserAI *cls, void *l2_game_packet )
{
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	cls->party.parse_PartySpelled( p );
}
