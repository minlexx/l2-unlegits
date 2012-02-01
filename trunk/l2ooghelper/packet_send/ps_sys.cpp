#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::send_AuthLogin()
{
	// create AuthLogin
	L2Game_AuthLogin p_game_al;
	p_game_al.create( account.login, login_sessionKey1, login_sessionKey2 );
	// send AuthLogin
	sendPacket( &p_game_al, true ); // true - obfuscate and XOR encode
}

void L2Client::send_Logout()
{
	L2GamePacket *pack = new L2GamePacket();
	pack->writeChar( 0x00 ); // Logout
	sendPacket( pack, true );
	delete pack; pack = NULL;
}

void L2Client::send_RequestRestart()
{
	L2GamePacket *pack = new L2GamePacket();
	pack->writeChar( 0x57 ); // RequestRestart
	sendPacket( pack, true );
	delete pack; pack = NULL;
}

void L2Client::send_CharacterSelect( int iCharSlot )
{
	// create CharSelect
	L2Game_CharacterSelect *p_game_charselect = new L2Game_CharacterSelect();
	p_game_charselect->create( (unsigned int)iCharSlot );
	// send
	sendPacket( p_game_charselect, true );
	delete p_game_charselect; p_game_charselect = NULL;
}

void L2Client::send_RequestGotoLobby()
{
	L2Game_RequestGotoLobby p;
	p.create( account.getL2Version() );
	sendPacket( &p, true );
}

void L2Client::send_NewCharacter()
{
	L2Game_NewCharacter p;
	p.create( account.getL2Version() );
	sendPacket( &p, true );
}

void L2Client::send_CharacterDelete( int iCharSlot )
{
	L2Game_CharacterDelete p;
	p.p_charSlot = iCharSlot;
	p.create( account.getL2Version() );
	sendPacket( &p, true );
}

void L2Client::send_CharacterRestore( int iCharSlot )
{
	L2Game_CharacterRestore p;
	p.p_charSlot = iCharSlot;
	p.create( account.getL2Version() );
	sendPacket( &p, true );
}

void L2Client::send_CharacterCreate( const wchar_t *name, const L2Game_NewCharacterTemplate *tmpl,
		int hairStyle, int hairColor, int face, int is_female )
{
	L2Game_CharacterCreate p;
	p.create( tmpl, name, hairStyle, hairColor, face, is_female );
	sendPacket( &p, true );
}

void L2Client::send_GameGuardReply( unsigned int r1, unsigned int r2, unsigned int r3, unsigned int r4 )
{
	L2GamePacket p;
	p.setPacketType( 0xCB ); // GameGuardReply
	p.writeUInt( r1 );
	p.writeUInt( r2 );
	p.writeUInt( r3 );
	p.writeUInt( r4 );
	sendPacket( &p, true );
}
