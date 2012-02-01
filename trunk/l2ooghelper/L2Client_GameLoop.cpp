#include "stdafx.h"
#include "Resource.h"
#include "Logger.h"
#include "MainWindow.h"
#include "L2Client.h"
#include "ChooseCharDlg.h"

extern HWND g_hWnd;

bool L2Client::L2ClientThread_Game( class L2Client *pcls )
{
	// vars
	bool                           was_error = false;
	int                            rdyRead = 0;
	int                            rdyWrite = 0;
	unsigned char                 *packbuffer = NULL;
	int                            r = -1;
	long                           timerResolutionMsec = 250;
	DWORD                          lastWorldTickTime = GetTickCount();
	unsigned int                   plen = 0;
	// packet objects
	L2GamePacket                  *pack = NULL;
	L2Game_ProtocolVersion        *p_game_pv = NULL;
	char                           szPacketName[256];
	// memsets
	memset( pcls->game_key_send, 0, sizeof(pcls->game_key_send) );
	memset( pcls->game_key_recv, 0, sizeof(pcls->game_key_recv) );
	memset( szPacketName,        0, sizeof(szPacketName) );
	pcls->game_opcodeObfuscatorSeed = 0;
	pcls->game_pCodeObfuscator      = NULL;
	pcls->game_logoutRequestSent    = false;

	// allocate recv packet buffer
	packbuffer = (unsigned char *)malloc( 10240 ); // 10 kb;
	if( !packbuffer )
	{
		log_error( LOG_ERROR, "Recv buffer malloc() failed!\n" );
		pcls->threadNetCleanup( pcls );
		pcls->addChatToTab( CHAT_SYS, L"Recv buffer malloc() failed!" );
		return false;
	}

	// create socket
	pcls->sock = L2PNet_TCPsocket_create( true );
	if( pcls->sock == INVALID_SOCKET )
	{
		log_error( LOG_ERROR, "Game: sock crt fail!\n" );
		pcls->threadNetCleanup( pcls );
		pcls->addChatToTab( CHAT_SYS, L"Socket create failed!" );
		return false;
	}

	// update net STATE, UI
	pcls->setState( L2Client::STATE_CONNECTING_GAME );
	log_error( LOG_DEBUG, "Game: connecting to %s:%d...\n",
		pcls->login_selectedGsIP, pcls->login_selectedGsPort );
	pcls->addChatToTabFormat( CHAT_SYS, L"Connecting to GS %S:%d...",
		pcls->login_selectedGsIP, pcls->login_selectedGsPort );

	// conenct
	L2PNet_connect( pcls->sock, pcls->login_selectedGsIP, (unsigned short)pcls->login_selectedGsPort );
	L2PNet_select( pcls->sock, L2PNET_SELECT_WRITE, 10000, &rdyRead, &rdyWrite );
	if( !rdyWrite )
	{
		log_error( LOG_ERROR, "Game: connect failed!\n" );
		pcls->threadNetCleanup( pcls );
		pcls->addChatToTab( CHAT_SYS, L"Connect to GS failed!" );
		return false;
	}

	// update net state, UI
	pcls->botipc_index = -1; // no BotIPC
	pcls->setState( L2Client::STATE_CONNECTED_GAME );
	log_error( LOG_DEBUG, "Game: connected to GS %s:%d.\n",
		pcls->login_selectedGsIP, pcls->login_selectedGsPort );
	pcls->addChatToTabFormat( CHAT_SYS, L"Connected to game server %S:%d.",
		pcls->login_selectedGsIP, pcls->login_selectedGsPort );

	// create ProtocolVersion packet
	p_game_pv = new L2Game_ProtocolVersion();
	if( pcls->account.useDefaultProtocolVersion )
	{
		log_error( LOG_PACKETNAME, "Using default protocol version from config "
			"for L2_VERSION %d\n", pcls->account.serverVersion );
		switch( pcls->account.getL2Version() )
		{
		case L2_VERSION_T1:  p_game_pv->createDefaultKamael(); break;
		case L2_VERSION_T15: p_game_pv->createDefaultHellbound(); break;
		case L2_VERSION_T2:  p_game_pv->createDefaultGracia1(); break;
		case L2_VERSION_T22: p_game_pv->createDefaultGracia2(); break;
		case L2_VERSION_T23: p_game_pv->createDefaultGracia3(); break;
		case L2_VERSION_T24: p_game_pv->createDefaultGracia4(); break;
		default:
			{
				log_error( LOG_ERROR, "wrong serverVersion setting in config! "
					"creating default Gracia Part 1!\n" );
				p_game_pv->createDefaultGracia1();
				pcls->addChatToTab( CHAT_SYS, L"Wrong serverVersion setting in config!" );
				pcls->addChatToTab( CHAT_SYS, L"...Creating default protocolVersion Gracia Part 1!" );
			} break;
		}
	}
	else
	{
		log_error( LOG_PACKETNAME, "Creating manual protocolVersion %u\n",
			pcls->account.protocolVersion );
		p_game_pv->createDefaultGracia3( pcls->account.protocolVersion );
		pcls->addChatToTabFormat( CHAT_SYS, L"Sending manual ProtocolVersion: %u",
			pcls->account.protocolVersion );
	}

	// send ProtocolVersion
	r = pcls->sendPacket( p_game_pv, false ); // false - do not obfuscate and XOR encode
	// free packet mem
	delete p_game_pv;
	p_game_pv = NULL;
	if( r <= 0 ) goto netErrorSend;

netRestartOK:
	pcls->game_logoutRequestSent = false;

	try
	{

#pragma warning( push, 3 )
	while( true )
#pragma warning( pop )
	{
		if( pcls->threadSignal != 0 )
		{
			if( pcls->threadSignal == THREAD_SIGNAL_FORCE_DISCONNECT )
			{
				pcls->send_Logout(); // send Logout, not needed, but...
				break;
			}
			if( pcls->threadSignal == THREAD_SIGNAL_DISCONNECT )
			{
				if( pcls->game_logoutRequestSent == false )
				{
					pcls->send_Logout();
					pcls->game_logoutRequestSent = true;
				}
			}
			if( pcls->threadSignal == THREAD_SIGNAL_LOGOUT )
			{
				if( pcls->game_logoutRequestSent == false )
				{
					pcls->send_RequestRestart();
					pcls->game_logoutRequestSent = true;
				}
			}
		}
		// process BotIPC events
		if( pcls->botipc_index >= 0 ) L2ClientThread_Game_BotIPC( pcls );
		// process AI tasks
		if( pcls->m_ai ) pcls->m_ai->think();
		// next deal with network packets

		// check if we have to receive some packet
		r = L2PNet_select( pcls->sock, L2PNET_SELECT_READ, timerResolutionMsec, &rdyRead, &rdyWrite );
		DWORD curTick = GetTickCount();
		if( curTick - lastWorldTickTime >= (unsigned)timerResolutionMsec )
		{
			lastWorldTickTime = curTick;
			pcls->worldTick();
		}
		// select result
		if( r == -1 ) break; // error?
		if( r == 0 ) continue; // timeout
		// receive packet!
		r = L2PacketReceive_buffer( pcls->sock, pcls->game_recvTimeoutMsec, &plen, packbuffer );
		if( r <= 0 ) goto netErrorRecv;

		// decode XOR
		if( pcls->game_XOR_enabled )
			L2GamePacket::decodeXOR_buffer( packbuffer, plen, pcls->game_key_recv );

		// deobfuscate opcode if enabled after XOR enc
		//pcls->OpcodeDeObfuscate( packbuffer, plen );
		// FIXED: de-obfuscation is not used for S->C packets!

		unsigned char opcode1 = packbuffer[2];
		unsigned short int opcode2 = ( ((unsigned short int)packbuffer[3]) |
			(((unsigned short int)packbuffer[4]) << 8) );
		unsigned short int opcode3 = ( ((unsigned short int)packbuffer[5]) |
			(((unsigned short int)packbuffer[6]) << 8) );

		L2Packets_GetL2PacketName( pcls->account.getL2Version(), true,
			opcode1, opcode2, opcode3, szPacketName, 255 );
		if( opcode1 != 0xFE ) log_error( LOG_PACKETNAME, "Server: %02X %s\n", opcode1, szPacketName );
		else log_error( LOG_PACKETNAME, "Server: FE:%02X %s\n", opcode2, szPacketName );

		// create packet object
		pack = new L2GamePacket();
		pack->setBytes( packbuffer, plen );

		// dispatch packet to handlers
		// packet parsing is based on opcodes, which may be the same in
		// different connection states, so parsing is based on connection state
		// same opcodes may be parsed differently in different states
		switch( pcls->getState() )
		{
			// check for bad states
			case L2Client::STATE_OFFLINE:
			case L2Client::STATE_CONNECTING_LOGIN:
			case L2Client::STATE_CONNECTED_LOGIN:
			case L2Client::STATE_AUTHED_LOGIN:
			case L2Client::STATE_CONNECTING_GAME:
				break; // not used here

			case L2Client::STATE_CONNECTED_GAME:
			{
				switch( opcode1 )
				{
				case 0x2E: pcls->ph_KeyPacket( pcls, pack ); break;
				case 0x09: pcls->ph_CharSelectionInfo( pcls, pack ); break; // changes state to AUTHED_GAME
				case 0x0A: pcls->ph_AuthLoginFail( pcls, pack ); break;
				}
			} break; /* STATE_CONNECTED_GAME */

			case L2Client::STATE_AUTHED_GAME:
			{
				switch( opcode1 )
				{
				case 0x09: pcls->ph_CharSelectionInfo( pcls, pack ); break;
				case 0x0B: pcls->ph_CharSelected( pcls, pack ); break; // changes state to IN_GAME
				case 0x0D: pcls->ph_NewCharacterSuccess( pcls, pack ); break;
				case 0x0F: pcls->ph_CharCreateSuccess( pcls, pack ); break;
				case 0x10: pcls->ph_CharCreateFail( pcls, pack ); break;
				case 0x1D: pcls->ph_CharDeleteSuccess( pcls, pack ); break;
				case 0x1E: pcls->ph_CharDeleteFail( pcls, pack ); break;
				case 0x73: pcls->ph_SSQInfo( pcls, pack ); break;
				}
			} break;
			/* STATE_AUTHED_GAME */

			case L2Client::STATE_IN_GAME:
			{
				switch( opcode1 )
				{
				case 0x00: pcls->ph_Die( pcls, pack ); break;
				case 0x01: pcls->ph_Revive( pcls, pack ); break;
				case 0x05: pcls->ph_SpawnItem( pcls, pack ); break;
				case 0x06: pcls->ph_SellList( pcls, pack ); break;
				case 0x07: pcls->ph_BuyList( pcls, pack ); break;
				case 0x08: pcls->ph_DeleteObject( pcls, pack ); break;
				case 0x0c: pcls->ph_NpcInfo( pcls, pack ); break;
				case 0x11: pcls->ph_ItemList( pcls, pack ); break;
				case 0x14: pcls->ph_TradeStart( pcls, pack ); break;
				case 0x16: pcls->ph_DropItem( pcls, pack ); break;
				case 0x17: pcls->ph_GetItem( pcls, pack ); break;
				case 0x18: pcls->ph_StatusUpdate( pcls, pack ); break;
				case 0x19: pcls->ph_NpcHtmlMessage( pcls, pack ); break;
				case 0x1A: pcls->ph_TradeOwnAdd( pcls, pack ); break;
				case 0x1B: pcls->ph_TradeOtherAdd( pcls, pack ); break;
				case 0x1C: pcls->ph_TradeDone( pcls, pack ); break;
				case 0x21: pcls->ph_InventoryUpdate( pcls, pack ); break;
				case 0x22: pcls->ph_TeleportToLocation( pcls, pack ); break;
				case 0x23: pcls->ph_TargetSelected( pcls, pack ); break;
				case 0x24: pcls->ph_TargetUnselected( pcls, pack ); break;
				case 0x28: pcls->ph_ChangeMoveType( pcls, pack ); break;
				case 0x29: pcls->ph_ChangeWaitType( pcls, pack ); break;
				case 0x2F: pcls->ph_MoveToLocation( pcls, pack ); break;
				case 0x31: pcls->ph_CharInfo( pcls, pack ); break;
				case 0x32: pcls->ph_UserInfo( pcls, pack ); break;
				case 0x39: pcls->ph_AskJoinParty( pcls, pack ); break;
				case 0x3a: pcls->ph_JoinParty( pcls, pack ); break;
				case 0x47: pcls->ph_StopMove( pcls, pack ); break;
				case 0x48: pcls->ph_MagicSkillUse( pcls, pack ); break;
				case 0x49: pcls->ph_MagicSkillCanceld( pcls, pack ); break;
				case 0x4a: pcls->ph_CreatureSay( pcls, pack ); break;
				case 0x4E: pcls->ph_PartySmallWindowAll( pcls, pack ); break;
				case 0x4F: pcls->ph_PartySmallWindowAdd( pcls, pack ); break;
				case 0x50: pcls->ph_PartySmallWindowDeleteAll( pcls, pack ); break;
				case 0x51: pcls->ph_PartySmallWindowDelete( pcls, pack ); break;
				case 0x52: pcls->ph_PartySmallWindowUpdate( pcls, pack ); break;
				case 0x5f: pcls->ph_SkillList( pcls, pack ); break;
				case 0x62: pcls->ph_SystemMessage( pcls, pack ); break;
				case 0x6B: pcls->ph_SetupGauge( pcls, pack ); break;
				case 0x70: pcls->ph_SendTradeRequest( pcls, pack ); break;
				case 0x71: // RestartResponse
					{
						pack->getPacketType(); // 0x71
						int restartOK = pack->readD();
						if( (restartOK == 0x01) && (pcls->threadSignal == THREAD_SIGNAL_LOGOUT) )
						{
							pcls->threadSignal = 0;
							pcls->addChatToTab( CHAT_SYS, L"Restart OK" );
							// remove bot from bot IPC
							pcls->ensureRemoveSelfFromBotIPC();
							goto netRestartOK;
						}
						if( restartOK == 0x00 )
						{
							log_error( LOG_USERAI, "RestartResponse: logout failed!\n" );
							pcls->addChatToTab( CHAT_SYS, L"Logout failed!" );
						}
					} break;
				case 0x72: pcls->ph_MoveToPawn( pcls, pack ); break;
					// SSQInfo may be sent also when IN_GAME...
				case 0x73: pcls->ph_SSQInfo( pcls, pack ); break;
				case 0x74: pcls->ph_GameGuardQuery( pcls, pack ); break;
				case 0x79: pcls->ph_ValidateLocation( pcls, pack ); break;
				case 0x84: // LeaveWorld
					{
						if( pcls->threadSignal == THREAD_SIGNAL_DISCONNECT )
						{
							pcls->threadSignal = 0;
							pcls->addChatToTab( CHAT_SYS, L"Disconnect OK" );
							// remove bot from bot IPC
							pcls->ensureRemoveSelfFromBotIPC();
							goto netLeaveWorld;
						}
					} break;
				case 0x85: pcls->ph_AbnormalStatusUpdate( pcls, pack ); break;
				case 0x89: pcls->ph_PledgeInfo( pcls, pack ); break;
				case 0xA0: pcls->ph_PrivateStoreManageListSell( pcls, pack ); break;
				case 0xA1: pcls->ph_PrivateStoreListSell( pcls, pack ); break;
				case 0xA2: pcls->ph_PrivateStoreMsgSell( pcls, pack ); break;
				case 0xBA: pcls->ph_PartyMemberPosition( pcls, pack ); break;
				case 0xBD: pcls->ph_PrivateStoreManageListBuy( pcls, pack ); break;
				case 0xBE: pcls->ph_PrivateStoreListBuy( pcls, pack ); break;
				case 0xBF: pcls->ph_PrivateStoreMsgBuy( pcls, pack ); break;
				case 0xC7: pcls->ph_SkillCoolTime( pcls, pack ); break;
				case 0xDE: pcls->ph_RecipeShopManageList( pcls, pack ); break;
				case 0xE1: pcls->ph_RecipeShopMsg( pcls, pack ); break;
				case 0xF3: pcls->ph_ConfirmDlg( pcls, pack ); break;
				case 0xF4: pcls->ph_PartySpelled( pcls, pack ); break;
				case 0xF9: pcls->ph_EtcStatusUpdate( pcls, pack ); break;
				case 0xFA: pcls->ph_ShortBuffStatusUpdate( pcls, pack ); break;
				case 0xFE:
					{
						switch( opcode2 )
						{
						case 0x002f: pcls->ph_ExStorageMaxCount( pcls, pack ); break;
						case 0x0080: pcls->ph_ExSetPrivateStoreWholeMsg( pcls, pack ); break;
						case 0x008D: pcls->ph_ExNpcQuestHtmlMessage( pcls, pack ); break;
						}
					} break;
				}
			} break;
			/* STATE_IN_GAME */

		} // switch state

		// delete packet object
		delete pack;
		pack = NULL;

	} // while( 1 )

	} // try
	catch( L2P_Exception& e )
	{
		log_error( LOG_ERROR, "L2ClientThread_Game: L2P_Exception: %s\n", e.what() );
	}
	catch( std::exception& e )
	{
		log_error( LOG_ERROR, "L2ClientThread_Game: std::exception: %s\n", e.what() );
	}

netLeaveWorld:

	//
	was_error = false;
	goto normalEnd;
	// error handlers
netErrorRecv:
	was_error = true;
	log_error( LOG_ERROR, "L2ClientThread: recv failed; some network error?\n" );
	pcls->addChatToTab( CHAT_SYS, L"Network error! (recv)" );
	goto normalEnd;
netErrorSend:
	was_error = true;
	log_error( LOG_ERROR, "L2ClientThread: send failed; some network error?\n" );
	pcls->addChatToTab( CHAT_SYS, L"Network error! (send)" );
	goto normalEnd;
normalEnd:
	if( pcls->game_pCodeObfuscator ) delete pcls->game_pCodeObfuscator;
	pcls->game_pCodeObfuscator = NULL;
	pcls->addChatToTab( CHAT_SYS, L"Disconnected." );
	free( packbuffer );
	packbuffer = NULL;
	pcls->threadNetCleanup( pcls );
	if( was_error ) return false;
	return true;
}


