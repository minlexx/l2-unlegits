#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

// 0 - Finders Keepers
// 1 - Random
// 2 - Random including spoil
// 3 - by turn
// 4 - by turn including spoil
void L2Client::handle_AskJoinParty( const wchar_t *requester, unsigned int itemDistribution )
{
	// OnAskJoinParty must look into config to possible auto-accept pary invites from some players
	const char *party_auto_accept_names = botcfg.getValStr( "party_auto_accept_names" );
	if( party_auto_accept_names && (this->bot_enabled > 0) )
	{
		char arequester[256] = {0};
		WideCharToMultiByte( CP_ACP, 0, requester, -1, arequester, 255, NULL, NULL );
		if( BotCfg_IsInList( arequester, party_auto_accept_names ) )
		{
			send_RequestAnswerJoinParty( 1 );
			return;
		}
	}
	// not enabled auto accept or wrong char.
	// Display UI request
	TCHAR question[512] = {0};
	TCHAR lootType[64] = {0};
	switch( itemDistribution )
	{
	case 0: lstrcpy( lootType, TEXT("Поднявшему") ); break;
	case 1: lstrcpy( lootType, TEXT("Случайно") ); break;
	case 2: lstrcpy( lootType, TEXT("Случайно, включая спойл") ); break;
	case 3: lstrcpy( lootType, TEXT("По очереди") ); break;
	case 4: lstrcpy( lootType, TEXT("По очереди, включая спойл") ); break;
	}
	wsprintf( question, TEXT("%s приглашает вас в партию.\nРаспределение дропа: %s"),
		requester, lootType );
	MessageBoxTimeout *mb = new MessageBoxTimeout( hWnd, WMMY_UI_MESSAGEBOXTIMEOUTREPLY,
		MessageBoxTimeout::TYPE_PARTY, question, 15 );
	mb->run( this->usr.charName );
}

// packet senders

void L2Client::send_RequestAnswerJoinParty( unsigned int accepted )
{
	if( this->state != STATE_IN_GAME ) return; // can answer only in game
	// answer AskJoinParty request
	L2GamePacket *pack = new L2GamePacket();
	pack->writeReset();
	pack->setPacketType( 0x43 ); // RequestAnswerJoinParty
	pack->writeUInt( accepted );
	this->sendPacket( pack, true );
	delete pack; pack = NULL;
}

void L2Client::send_RequestJoinParty( const wchar_t *target, unsigned int itemDistribution )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	// send RequestJoinParty
	L2Game_RequestJoinParty *pack = new L2Game_RequestJoinParty();
	pack->create( target, itemDistribution );
	sendPacket( pack, true );
	delete pack; pack = NULL;
}

void L2Client::send_RequestWithdrawalParty()
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket *pack = new L2GamePacket();
	pack->writeReset();
	pack->setPacketType( 0x44 ); // RequestWithDrawalParty
	sendPacket( pack, true );
	delete pack; pack = NULL;
}

void L2Client::send_RequestOustPartyMember( const wchar_t *playerName )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	if( !playerName ) return;
	L2GamePacket *pack = new L2GamePacket();
	pack->writeReset();
	pack->setPacketType( 0x45 ); // RequestOustPartyMember
	pack->writeUnicodeString( playerName );
	sendPacket( pack, true );
	delete pack; pack = NULL;
}

void L2Client::send_RequestChangePartyLeader( const wchar_t *playerName )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	if( !playerName ) return;
	L2GamePacket *pack = new L2GamePacket();
	pack->writeReset();
	pack->setPacketType( 0xD0 ); // D0:000C RequestChangePartyLeader
	pack->writeUShort( 0x0000C );
	pack->writeUnicodeString( playerName );
	sendPacket( pack, true );
	delete pack; pack = NULL;
}

