#include "stdafx.h"
#include "ClanWarList.h"
#include "Logger.h"

ClanWarList::ClanWarList()
{
	int i;
	numDeclared = numUnderAttack = 0;
	for( i=0; i<MAX_CLANWARS; i++ )
	{
		cnDeclared[i] = NULL;
		cnUnderAttack[i] = NULL;
	}
}

ClanWarList::~ClanWarList()
{
	clear();
}

void ClanWarList::clear()
{
	int i;
	numDeclared = numUnderAttack = 0;
	for( i=0; i<MAX_CLANWARS; i++ )
	{
		if( cnDeclared[i] ) free( cnDeclared[i] );
		if( cnUnderAttack[i] ) free( cnUnderAttack[i] );
		cnDeclared[i] = NULL;
		cnUnderAttack[i] = NULL;
	}
}

void ClanWarList::clearDeclared()
{
	int i;
	numDeclared = 0;
	for( i=0; i<MAX_CLANWARS; i++ )
	{
		if( cnDeclared[i] ) free( cnDeclared[i] );
		cnDeclared[i] = NULL;
	}
}

void ClanWarList::clearUnderAttack()
{
	int i;
	numUnderAttack = 0;
	for( i=0; i<MAX_CLANWARS; i++ )
	{
		if( cnUnderAttack[i] ) free( cnUnderAttack[i] );
		cnUnderAttack[i] = NULL;
	}
}

/*void ClanWarList::parse_PledgeReceiveWarList( const unsigned char *bytes, unsigned int length )
{
	if( !bytes || (length<1) ) return;
	L2GamePacket *p = new L2GamePacket( bytes, length );*/
void ClanWarList::parse_PledgeReceiveWarList( void *l2_game_packet )
{
	if( !l2_game_packet ) return;
	L2GamePacket *p = (L2GamePacket *)l2_game_packet;
	//
	p->getPacketType(); // 0xFE
	p->readUShort();    // 0x003F
	//
	int i;
	wchar_t *strTemp;
	int tab = p->readInt();
	// clear data
	switch( tab )
	{
	case 0: clearDeclared(); break;
	case 1: clearUnderAttack(); break;
	}
	//
	p->readInt(); // page, always 0x00.. on L2J
	int warsCount = p->readInt();
	for( i=0; i<warsCount; i++ )
	{
		strTemp = p->readUnicodeString(); // clan name
		p->readInt(); // ??
		p->readInt(); // ??
		// add
		switch( tab )
		{
		case 0: { cnDeclared[i]    = strTemp; numDeclared++;    } break;
		case 1: { cnUnderAttack[i] = strTemp; numUnderAttack++; } break;
		}
	}
	//delete p; // packet deleted by calling function
#ifdef _DEBUG
	log_error( LOG_USERAI, "PledgeReceiveWarList results: %d declared, %d under attack\n", numDeclared, numUnderAttack );
	//log_error( LOG_USERAI, "Declared: " );
	//for( i=0; i<numDeclared; i++ ) log_error_np( LOG_OK, "[%S] ", cnDeclared[i] );
	//log_error_np( LOG_USERAI, "\n" );
	//log_error( LOG_USERAI, "Under attack: " );
	//for( i=0; i<numUnderAttack; i++ ) log_error_np( LOG_OK, "[%S] ", cnUnderAttack[i] );
	//log_error_np( LOG_USERAI, "\n" );
#endif
}

/*
protected void writeImpl()
{
	writeC(0xfe); // FE:3F PledgeReceiveWarList
	writeH(0x3f);

	writeD(_tab); // type : 0 = Declared, 1 = Under Attack
	writeD(0x00); // page
	writeD(_tab == 0 ? _clan.getWarList().size() : _clan.getAttackerList().size()); // warsCount
	for(Integer i : _tab == 0 ? _clan.getWarList() : _clan.getAttackerList())
	{
		L2Clan clan = ClanTable.getInstance().getClan(i);
		if (clan == null) continue;

		writeS(clan.getName());
		writeD(_tab); //??
		writeD(_tab); //??
	}
}
*/

CLANWARSTATE ClanWarList::getWarForClan( const wchar_t *wszClanName )
{
	if( !wszClanName ) return CLANWAR_NOWAR;
	if( (numDeclared == 0) && (numUnderAttack == 0) ) return CLANWAR_NOWAR;
	CLANWARSTATE ret;;
	int declared, underattack;
	int i;
	// declared?
	declared = 0;
	if( numDeclared > 0 )
	{
		for( i=0; i<numDeclared; i++ )
		{
			if( wcscmp( wszClanName, cnDeclared[i] ) == 0 )
			{
				declared = 1;
				break;
			}
		}
	}
	// underattack?
	underattack = 0;
	if( numUnderAttack > 0 )
	{
		for( i=0; i<numUnderAttack; i++ )
		{
			if( wcscmp( wszClanName, cnUnderAttack[i] ) == 0 )
			{
				underattack = 1;
				break;
			}
		}
	}
	ret = CLANWAR_NOWAR;
	if( declared && !underattack ) ret = CLANWAR_DECLARED;
	if( !declared && underattack ) ret = CLANWAR_UNDERATTACK;
	if( declared && underattack ) ret = CLANWAR_BOTH;
	return ret;
}
