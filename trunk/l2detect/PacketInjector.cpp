#include "stdafx.h"
#include "GameClient.h"
#include "PacketInjector.h"
#include "Logger.h"
#include "ConfigIni.h"

extern class GameClient *g_game_client; // in main.cpp
extern class CConfig     g_cfg; // in main.cpp

void PGen_Action( unsigned int oid, int x, int y, int z, unsigned char useShift )
{
	if( !g_game_client ) return;
	L2Game_Action p;
	p.create( oid, x, y, z, useShift );
	unsigned char bytes[32];
	memcpy( bytes, p.getBytesPtr(), p.getPacketSize() );
	g_game_client->InjectPacketToServer( bytes, p.getPacketSize(), true );
}

void PGen_AttackRequest( unsigned int oid, int x, int y, int z, unsigned char useShift )
{
	L2GamePacket p;
	p.setPacketType( 0x01 ); // AttackRequest
	p.writeUInt( oid );
	p.writeInt( x );
	p.writeInt( y );
	p.writeInt( z );
	p.writeUChar( useShift );
	g_game_client->InjectPacketToServer( (unsigned char *)p.getBytesPtr(), p.getPacketSize(), true );
}

void PGen_UseItem( unsigned int oid )
{
	if( !g_game_client ) return;
	L2Game_UseItem p;
	p.create( oid );
	unsigned char bytes[32];
	memcpy( bytes, p.getBytesPtr(), p.getPacketSize() );
	g_game_client->InjectPacketToServer( bytes, p.getPacketSize(), true );
}

void PGen_RequestJoinParty( const wchar_t *invitePlayer, unsigned int lootRules )
{
	if( !g_game_client ) return;
	L2Game_RequestJoinParty p;
	p.create( invitePlayer, lootRules );
	g_game_client->InjectPacketToServer( const_cast<unsigned char *>(p.getBytesPtr()), p.getPacketSize(), true );
}

void PGen_RequestOustPartyMember( const wchar_t *dismissPlayer )
{
	if( !dismissPlayer ) return;
	L2GamePacket p;
	p.writeReset();
	p.writeUChar( 0x45 );
	p.writeUnicodeString( dismissPlayer );
	g_game_client->InjectPacketToServer( (unsigned char *)p.getBytesPtr(), p.getPacketSize(), true );
}

void PGen_GameGuardReply_L2J()
{
	L2GamePacket pack;
	pack.setPacketType( 0xCB ); // GameGuardReply
	pack.writeUInt( 0x78F0977F );
	pack.writeUInt( 0xD6E63C04 );
	pack.writeUInt( 0x89F60C71 );
	pack.writeUInt( 0x70069EDD );
	g_game_client->InjectPacketToServer( (unsigned char *)pack.getBytesPtr(), pack.getPacketSize(), true );
}

void PGen_RequestTargetCanceld( bool abortCast )
{
	L2GamePacket pack;
	pack.setPacketType( 0x48 ); // RequestTargetCanceld
	pack.writeH( abortCast ? 0x00 : 0x01 ); // abort cast or cancel target
	g_game_client->InjectPacketToServer( (unsigned char *)pack.getBytesPtr(), pack.getPacketSize(), true );
}

/*	public final static int ALL = 0;
	public final static int SHOUT = 1; //!
	public final static int TELL = 2;
	public final static int PARTY = 3; //#
	public final static int CLAN = 4;  //@
	public final static int GM = 5;
	public final static int PETITION_PLAYER = 6; // used for petition
	public final static int PETITION_GM = 7; //* used for petition
	public final static int TRADE = 8; //+
	public final static int ALLIANCE = 9; //$
	public final static int ANNOUNCEMENT = 10;
	public final static int PARTYROOM_ALL = 16; //(Red)
	public final static int PARTYROOM_COMMANDER = 15; //(Yellow)
	public final static int HERO_VOICE = 17;
	public final static int BATTLEFIELD = 20; */
void PGen_Say2C( unsigned int channelID, const wchar_t *text, const wchar_t *player_to /*= NULL*/ )
{
	L2GamePacket pack;
	pack.setPacketType( 0x49 ); // Say2
	pack.writeS( text );       // текст
	pack.writeD( channelID );  // тип чата
	// если тип чата = ПМ, и указано имя цели, писать и его в пакет
	if( (channelID == 2 /*TELL*/) && player_to ) pack.writeS( player_to );
	g_game_client->InjectPacketToServer( (unsigned char *)pack.getBytesPtr(), pack.getPacketSize(), true );
}

void PGen_MoveBackwardToLocation( int dst_x, int dst_y, int dst_z, int src_x, int src_y, int src_z )
{
	L2GamePacket p;
	p.setPacketType( 0x0F ); // MoveBackwardToLocation
	p.writeD( dst_x );
	p.writeD( dst_y );
	p.writeD( dst_z );
	p.writeD( src_x );
	p.writeD( src_y );
	p.writeD( src_z );
	p.writeD( 0x01 ); // 0x01 = for movement was used mouse
	g_game_client->InjectPacketToServer( (unsigned char *)p.getBytesPtr(), p.getPacketSize(), true );
}

void PGen_RequestBypassToserver( const wchar_t *bypassStr )
{
	L2GamePacket p;
	p.setPacketType( 0x23 ); // RequestBypassToServer
	p.writeS( bypassStr );
	g_game_client->InjectPacketToServer( (unsigned char *)p.getBytesPtr(), p.getPacketSize(), true );
}

void PGen_RequestLinkHtml( const wchar_t *link )
{
	L2GamePacket p;
	p.setPacketType( 0x22 ); // RequestLinkHtml
	p.writeS( link );
	g_game_client->InjectPacketToServer( (unsigned char *)p.getBytesPtr(), p.getPacketSize(), true );
}

void PGen_RequestActionUse( unsigned int actionId, bool ctrlPressed, bool shiftPressed )
{
	L2GamePacket p;
	p.setPacketType( 0x56 ); // RequestActionUse
	p.writeUInt( actionId );
	p.writeUInt( ctrlPressed ? 0x01 : 0x00 );
	p.writeC( shiftPressed ? 0x01 : 0x00 );
	g_game_client->InjectPacketToServer( (unsigned char *)p.getBytesPtr(), p.getPacketSize(), true );
}

void PGen_RequestAutoSoulshot( unsigned int itemID, bool enable )
{
	L2GamePacket p;
	p.setPacketType( 0xD0 );
	p.writeUShort( 0x000D ); // D0:000D RequestAutoSoulshot
	p.writeUInt( itemID );
	enable ? p.writeUInt( 1 ) : p.writeUInt( 0 );
	g_game_client->InjectPacketToServer( (unsigned char *)p.getBytesPtr(), p.getPacketSize(), true );
}

/**   Client: Len   13 [RequestDispel]
0D 00
D0 4E 00        // D0:004E RequestDispel      D0:004B RequestExDspel (G.Final)
4B 05 00 00     // skillID    1355 (PoWater?)
01 00 00 00     // skillLvl    lvl 1   **/
void PGen_RequestDispel( unsigned int skillID )
{
	// only since Gracia T2
	if( g_cfg.L2_version < (int)L2_VERSION_T2 ) return;
	UserBuff buf;
	g_game_client->ai.buffs.getBuffnfoBySkillId( skillID, &buf );
	if( (buf.skillID == 0) || (buf.skillLvl == 0) ) return; // error, no such buff!
	// create packet
	L2GamePacket p;
	p.setPacketType( 0xD0 );
	// opcode is different in Gracia Final
	if( g_cfg.L2_version <= (int)L2_VERSION_T22 )
		p.writeUShort( 0x004E ); // D0:004E RequestDispel L2 <= T22
	else
		p.writeUShort( 0x004B ); // D0:004B RequestDispel L2 >= T23 Gracia Final
	p.writeUInt( skillID );
	p.writeUInt( buf.skillLvl );
	g_game_client->InjectPacketToServer( (unsigned char *)p.getBytesPtr(), p.getPacketSize(), true );
}

void PGen_RequestMagicSkillUse( unsigned int skillID, unsigned int ctrlPressed, unsigned char shiftPressed )
{
	L2GamePacket p;
	p.setPacketType( 0x39 ); // RequestMagicSkillUse
	p.writeUInt( skillID );
	p.writeUInt( ctrlPressed );
	p.writeUChar( shiftPressed );
	g_game_client->InjectPacketToServer( (unsigned char *)p.getBytesPtr(), p.getPacketSize(), true );
}

void PGen_RequestWithdrawalParty()
{
	L2GamePacket p;
	p.setPacketType( 0x44 ); // RequestWithDrawalParty
	g_game_client->InjectPacketToServer( (unsigned char *)p.getBytesPtr(), p.getPacketSize(), true );
}

void PGen_RequestChangePartyLeader( const wchar_t *playerName )
{
	if( !playerName ) return;
	L2GamePacket p;
	p.setPacketType( 0xD0 ); // D0:000C RequestChangePartyLeader
	p.writeUShort( 0x0000C );
	p.writeUnicodeString( playerName );
	g_game_client->InjectPacketToServer( (unsigned char *)p.getBytesPtr(), p.getPacketSize(), true );
}

// hack function, do not use!
// input: string consisting of sequence of hex chars, representing packet, starting from packet type
// packet length will be prefixed automatically!
// example: 1F39A400109ACB00003D2BFFFFA9F3FFFF00 - 1F Action packet, without packet len
void PGen_send_hackPacketHex_toServer( const char *szHexStr )
{
	if( !szHexStr ) return;
	int ll = (int)strlen( szHexStr );
	if( ll % 2 ) return;
	int nBytes = ll / 2;
	int i;
	unsigned char *bytebuffer = (unsigned char *)malloc( nBytes );
	if( !bytebuffer ) return;
	memset( bytebuffer, 0, nBytes );
	//log_error_np( LOG_OK, "\ngame_sendPacketHex(): %d bytes\n", nBytes );
	for( i=0; i<ll; i+=2 )
	{
		char c1 = szHexStr[i];
		char c2 = szHexStr[i+1];
		//log_error_np( LOG_OK, "Parsing byte [%d] %c%c... ", i/2, c1, c2 );
		int i1 = 0;
		int i2 = 0;
		if( (c1>='0') && (c1<='9') ) i1 =  0 + c1 - '0';
		if( (c1>='a') && (c1<='f') ) i1 = 10 + c1 - 'a';
		if( (c1>='A') && (c1<='F') ) i1 = 10 + c1 - 'A';
		if( (c2>='0') && (c2<='9') ) i2 =  0 + c2 - '0';
		if( (c2>='a') && (c2<='f') ) i2 = 10 + c2 - 'a';
		if( (c2>='A') && (c2<='F') ) i2 = 10 + c2 - 'A';
		//log_error_np( LOG_OK, "%d * 16 + %d... ", i1, i2 );
		unsigned char b = (unsigned char)( ((i1 << 4) | i2) & 0xFF );
		//log_error_np( LOG_OK, "%d (%02X)\n", (int)b, b );
		bytebuffer[i/2] = b;
	}
	L2GamePacket *pack = new L2GamePacket();
	pack->writeReset();
	pack->writeBytes( bytebuffer, nBytes );
	g_game_client->InjectPacketToServer( (unsigned char *)pack->getBytesPtr(), pack->getPacketSize(), true );
	delete pack;
	pack = NULL;
}










void PGenC_NpcHtmlMessage( unsigned int npcObjectId, const wchar_t *text, unsigned int itemId )
{
	L2GamePacket p;
	p.setPacketType( 0x19 ); // NpcHtmlMessage
	p.writeD( npcObjectId );
	p.writeS( text );
	p.writeD( itemId );
	g_game_client->InjectPacketToClient( (unsigned char *)p.getBytesPtr(), p.getPacketSize() );
}

void PGenC_DeleteObject( unsigned int objectID )
{
	L2GamePacket p;
	p.setPacketType( 0x08 ); // DeleteObject
	p.writeUInt( objectID );
	g_game_client->InjectPacketToClient( (unsigned char *)p.getBytesPtr(), p.getPacketSize() );
}

void PGenC_MoveToLocation( unsigned int objectID, int x, int y, int z, int dx, int dy, int dz )
{
	L2GamePacket p;
	p.setPacketType( 0x2f ); // MoveToLocation
	p.writeUInt( objectID );
	p.writeD( dx );
	p.writeD( dy );
	p.writeD( dz );
	p.writeD( x );
	p.writeD( y );
	p.writeD( z );
	g_game_client->InjectPacketToClient( (unsigned char *)p.getBytesPtr(), p.getPacketSize() );
}

void PGenC_StopMove( unsigned int objectID, int x, int y, int z, unsigned int heading )
{
	L2GamePacket p;
	p.setPacketType( 0x47 ); // StopMove
	p.writeUInt( objectID );
	p.writeD( x );
	p.writeD( y );
	p.writeD( z );
	p.writeUInt( heading );
	g_game_client->InjectPacketToClient( (unsigned char *)p.getBytesPtr(), p.getPacketSize() );
}

void PGenC_CreatureSay( unsigned int objectID, unsigned int channelID, const wchar_t *senderName, const wchar_t *text )
{
	L2GamePacket p;
	p.setPacketType( 0x4A ); // CreatureSay
	p.writeD( objectID );    // objectID
	p.writeD( channelID );   // channelID (15 - Commander, 17 - Hero)
	p.writeS( senderName );  // sender name
	p.writeS( text );        // text
	g_game_client->InjectPacketToClient( (unsigned char *)p.getBytesPtr(), p.getPacketSize() );
}

void PGenC_StatusUpdate( L2Player *pl )
{
	L2GamePacket p;
	// first send info about curHp/maxHp
	p.setPacketType( 0x18 ); // StatusUpdate
	p.writeUInt( pl->objectID );
	p.writeUInt( 2 ); // number of attributes
	// cur HP
	p.writeUInt( 0x09 );
	p.writeUInt( (unsigned int)pl->curHp );
	// max HP
	p.writeUInt( 0x0A );
	p.writeUInt( (unsigned int)pl->maxHp );
	g_game_client->InjectPacketToClient( (unsigned char *)p.getBytesPtr(), p.getPacketSize() );

	// then send info about curMp/maxMp
	p.setPacketType( 0x18 ); // StatusUpdate
	p.writeUInt( pl->objectID );
	p.writeUInt( 2 ); // number of attributes
	// cur MP
	p.writeUInt( 0x0B );
	p.writeUInt( (unsigned int)pl->curHp );
	// max MP
	p.writeUInt( 0x0C );
	p.writeUInt( (unsigned int)pl->maxHp );
	g_game_client->InjectPacketToClient( (unsigned char *)p.getBytesPtr(), p.getPacketSize() );
	//log_error( LOG_USERAI, "Injected StatusUpdate about [%S]\n", pl->charName );
}

void PGenC_ShowMiniMap( unsigned int mapID /*= 1665*/ )
{
	L2GamePacket p;
	p.setPacketType( 0xA3 ); // ShowMiniMap
	p.writeUInt( mapID );
	g_game_client->InjectPacketToClient( (unsigned char *)p.getBytesPtr(), p.getPacketSize() );
}
