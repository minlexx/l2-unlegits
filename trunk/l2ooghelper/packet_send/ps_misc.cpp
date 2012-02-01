#include "stdafx.h"
#include "Logger.h"
#include "L2Client.h"

void L2Client::handle_ConfirmDlg( wchar_t *question, unsigned int sm_ID,
								 unsigned int requestId, unsigned int timeLimit, const wchar_t *text )
{
	const char *party_auto_accept_names = botcfg.getValStr( "party_auto_accept_names" );
	if( party_auto_accept_names && (this->bot_enabled > 0) && (text != NULL) )
	{
		char arequester[256] = {0};
		WideCharToMultiByte( CP_ACP, 0, text, -1, arequester, 255, NULL, NULL );
		if( BotCfg_IsInList( arequester, party_auto_accept_names ) )
		{
			send_DlgAnswer( sm_ID, 0x00000001, requestId );
			return;
		}
	}
	MessageBoxTimeout *mb = new MessageBoxTimeout( this->hWnd, 0,
		MessageBoxTimeout::TYPE_CONFIRMDLG, question, (int)timeLimit );
	mb->runConfirmDlg( this->usr.charName, requestId, sm_ID, (void *)this );
	if( question ) free( question );
}

// packet senders

void L2Client::send_DlgAnswer( unsigned int sm_ID, int answer, unsigned int requestId )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket p;
	p.setPacketType( 0xC6 ); // DlgAnswer
	p.writeUInt( sm_ID ); // systemMessageID
	p.writeInt( answer ); // 0x00000001 = accepted, 0x00000000 - no =[
	p.writeUInt( requestId );
	sendPacket( &p, true );
}


/** * RequestGMList Client: Len    3 | 03 00 / 8B / */
void L2Client::send_RequestGMList()
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket p;
	p.setPacketType( 0x8B );
	sendPacket( &p, true );
}

void L2Client::send_RequestActionUse( unsigned int actionID, unsigned int ctrlPressed, unsigned char shiftPressed )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2GamePacket *gp = new L2GamePacket();
	gp->setPacketType( 0x56 );     // RequestActionUse opcode :)
	gp->writeUInt( actionID );      // actionID; 0 - sitstand; 1 - runwalk
	gp->writeUInt( ctrlPressed );    // ctrlPressed
	gp->writeUChar( shiftPressed );   // shiftPressed
	sendPacket( gp, true );
	delete gp; gp = NULL;
}

void L2Client::send_RequestUserCommand( unsigned int commandID )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	L2Game_RequestUserCommand *pack = new L2Game_RequestUserCommand();
	pack->create( commandID );
	sendPacket( pack, true );
	delete pack; pack = NULL;
}

// Client: Len 43 [SendBypassBuildCmd] | 2B 00 / 74 / 63 00 72 00  .... 30 00 00 00
// _command = readS();
void L2Client::send_SendBypassBuildCmd( const wchar_t *cmd )
{
	if( !cmd ) return;
	if( this->state != STATE_IN_GAME ) return; // only in game
	log_error( LOG_DEBUG, "use admin command: [%S]\n", cmd );
	L2GamePacket p;
	p.setPacketType( 0x74 );
	p.writeUnicodeString( cmd + 2 ); // string starts with "//", pass 1st 2 chars
	sendPacket( &p, true );
}

// pcode 0xC2; read: _targetId = readD();
void L2Client::send_RequestEvaluate()
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	if( usr.targetObjectID == 0 ) return;
	L2GamePacket p;
	p.setPacketType( 0xC2 ); // RequestEvaluate
	p.writeUInt( usr.targetObjectID );
	sendPacket( &p, true );
}

/**   Client: Len   13 [RequestDispel]
0D 00
D0 4E 00        // D0:004E RequestDispel      D0:004B RequestExDspel (G.Final)
4B 05 00 00     // skillID    1355 (PoWater?)
01 00 00 00     // skillLvl    lvl 1   **/
void L2Client::send_RequestDispel( unsigned int skillID )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	// only in Gracia T2
	if( this->account.getL2Version() < L2_VERSION_T2 ) return;
	UserBuff buf;
	buffs.getBuffnfoBySkillId( skillID, &buf );
	if( (buf.skillID == 0) || (buf.skillLvl == 0) ) return; // error, no such buff!
	// create packet
	L2GamePacket *p = new L2GamePacket();
	p->setPacketType( 0xD0 );
	// opcode is different in Gracia Final
	if( this->account.serverVersion <= (int)L2_VERSION_T22 )
		p->writeUShort( 0x004E ); // D0:004E RequestDispel L2 <= T22
	else
		p->writeUShort( 0x004B ); // D0:004B RequestDispel L2 >= T23 Gracia Final
	p->writeUInt( skillID );
	p->writeUInt( buf.skillLvl );
	sendPacket( p, true );
	delete p; p = NULL;
}

void L2Client::send_RequestBypassToServer( const wchar_t *bypassStr )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	if( !bypassStr ) return;
	L2GamePacket p;
	p.setPacketType( 0x23 ); // RequestBypassToServer
	p.writeUnicodeString( bypassStr );
	sendPacket( &p, true );
}

void L2Client::send_RequestLinkHtml( const wchar_t *link )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	if( !link ) return;
	L2GamePacket p;
	p.setPacketType( 0x22 ); // RequestLinkHtml
	p.writeUnicodeString( link );
	sendPacket( &p, true );
}

/***   Client: Len    7 [RequestRestartPoint]
07 00
7D
00 00 00 00    // pointType

case 1: // to clanhall
case 2: // to castle
case 3: // to fortress
case 4: // to siege HQ
case 5: // Fixed or Player is a festival participant

after successful restart point server sends TeleportToLocation
then client send Appearing
then server sends Revive         ***/
void L2Client::send_RequestRestartPoint( unsigned int pointType/* = 0*/ )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	// only if we are dead!
	if( !usr.isDead )
	{
		MessageBox( hWnd, TEXT("Вы не мертвы!"), TEXT("Ашыпка"), MB_ICONWARNING );
		return;
	}
	if( (pointType == 0) && (usr.canResurrectToVillage == 0) )
	{
		MessageBox( hWnd, TEXT("Вы не можете встать в деревню!"), TEXT("Ашыпка"), MB_ICONWARNING );
		return;
	}
	if( (pointType == 1) && (usr.canResurrectToCH == 0) )
	{
		MessageBox( hWnd, TEXT("Вы не можете встать в КланХолл!"), TEXT("Ашыпка"), MB_ICONWARNING );
		return;
	}
	if( (pointType == 2) && (usr.canResurrectToCastle == 0) )
	{
		MessageBox( hWnd, TEXT("Вы не можете встать в замок!"), TEXT("Ашыпка"), MB_ICONWARNING );
		return;
	}
	if( (pointType == 3) && (usr.canResurrectToFortress == 0) )
	{
		MessageBox( hWnd, TEXT("Вы не можете встать в форт!"), TEXT("Ашыпка"), MB_ICONWARNING );
		return;
	}
	if( (pointType == 4) && (usr.canResurrectToSiegeHQ == 0) )
	{
		MessageBox( hWnd, TEXT("Вы не можете встать к флагу!"), TEXT("Ашыпка"), MB_ICONWARNING );
		return;
	}
	if( (pointType == 5) && (usr.canResurrectFixed == 0) )
	{
		MessageBox( hWnd, TEXT("Вы не можете встать на месте!"), TEXT("Ашыпка"), MB_ICONWARNING );
		return;
	}
	L2GamePacket p;
	p.setPacketType( 0x7D ); // RequestRestartPoint
	p.writeUInt( pointType );
	sendPacket( &p, true );
}

void L2Client::send_Say2( unsigned int chat_type, const wchar_t *text, const wchar_t *to )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
	// send Say2
	L2GamePacket *pack = new L2GamePacket();
	pack->writeReset();
	pack->setPacketType( 0x49 ); // Say2
	pack->writeUnicodeString( text );
	pack->writeUInt( chat_type );
	if( chat_type == L2_CHAT_MESSAGE::TELL && to ) pack->writeUnicodeString( to );
	this->sendPacket( pack, true );
	delete pack; pack = NULL;
}


// hack function, do not use!
// input: string consisting of sequence of hex chars, representing packet, starting from packet type
// packet length will be prefixed automatically!
// example: 1F39A400109ACB00003D2BFFFFA9F3FFFF00 - 1F Action packet, without packet len
void L2Client::send_hackPacketHex( const char *szHexStr )
{
	if( this->state != STATE_IN_GAME ) return; // only in game
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
	sendPacket( pack, true );
	delete pack;
	pack = NULL;
}

