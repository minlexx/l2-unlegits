#ifndef L2PACKETS_H_
#define L2PACKETS_H_

/** \file L2Packets.h
* Main include file to include all other library files related to Lineage II.
*/

// detect OS
#if defined(_WIN32) || defined(WIN32) || defined(WINVER) || defined(_WIN32_WINNT)
#undef L2PACKETS_WINDOWS
#define L2PACKETS_WINDOWS
#else
#undef L2PACKETS_LINUX
#define L2PACKETS_LINUX
#endif

/* some #defines come here
 * they determine some behaviour of L2Packets library
 *
 * L2LOGINP_DEBUGOUT_BLOWFISH - if defined, library reports to stdout errors/successes
 *    about Blowfish encoding/decoding process of login server packets
 * L2LOGINP_DEBUGOUT_XOR - if defined, library reports to stdout errors/successes about
 *    Init packet XOR decoding process
 * L2LOGINP_DEBUGOUT_CHKSUM - if defined, library reports to stdout about process of
 *    appending checksum to login server packets
 * L2LOGINP_DEBUGOUT_PADDING - if defined, library reports to stdout about process of
 *    padding login server packets to 8-byte boundary
 *
 * L2P_NETWORK_DEBUGOUT - debug messages about sending/receiving packets process
*/

// those should be defined in login/L2LoginPacket.h:
//#define L2LOGINP_DEBUGOUT_BLOWFISH
//#define L2LOGINP_DEBUGOUT_XOR
//#define L2LOGINP_DEBUGOUT_CHKSUM
//#define L2LOGINP_DEBUGOUT_PADDING

// these should be defined in net_io headers (in both)
//#define L2P_NETWORK_DEBUGOUT

#include "L2_versions.h"

#include "base/ByteArray.h"
#include "base/L2BasePacket.h"

#include "net_io/L2Packet_NetLayer.h"
#include "net_io/L2PacketReceiver.h"
#include "net_io/L2PacketSender.h"

#include "login/L2LoginPacket.h"
#include "login/server/L2Login_Init.h"
#include "login/client/L2Login_RequestGGAuth.h"
#include "login/server/L2Login_GGAuthResponse.h"
#include "login/client/L2Login_RequestAuthLogin.h"
#include "login/server/L2Login_LoginOK.h"
#include "login/server/L2Login_LoginFail.h"
#include "login/server/L2Login_AccountKicked.h"
#include "login/client/L2Login_RequestServerList.h"
#include "login/server/L2Login_ServerList.h"
#include "login/client/L2Login_RequestServerLogin.h"
#include "login/server/L2Login_PlayOK.h"
#include "login/server/L2Login_PlayFail.h"

#include "game/L2GamePacket.h"
// game: from server
#include "game/server/L2Game_AuthLoginFail.h"
#include "game/server/L2Game_CharCreateFail.h"
#include "game/server/L2Game_CharCreateSuccess.h"
#include "game/server/L2Game_CharDeleteFail.h"
#include "game/server/L2Game_CharDeleteSuccess.h"
#include "game/server/L2Game_CharSelected.h"
#include "game/server/L2Game_CharSelectionInfo.h"
#include "game/server/L2Game_KeyPacket.h"
#include "game/server/L2Game_NewCharacterSuccess.h"
#include "game/server/L2Game_SSQInfo.h"
// game: from client
#include "game/client/L2Game_Action.h"
#include "game/client/L2Game_AuthLogin.h"
#include "game/client/L2Game_CharacterCreate.h"
#include "game/client/L2Game_CharacterDelete.h"
#include "game/client/L2Game_CharacterRestore.h"
#include "game/client/L2Game_CharacterSelect.h"
#include "game/client/L2Game_EnterWorld.h"
#include "game/client/L2Game_NewCharacter.h"
#include "game/client/L2Game_ProtocolVersion.h"
#include "game/client/L2Game_RequestJoinParty.h"
#include "game/client/L2Game_RequestGotoLobby.h"
#include "game/client/L2Game_RequestUserCommand.h"
#include "game/client/L2Game_UseItem.h"

#include "pcode_obfuscator/L2PCodeObfuscator.h"
#include "l2data/L2Data.h"
#include "l2world/L2World.h"
#include "packetNames/L2PacketNames.h"
#include "xcpt/L2Packets_xcpt.h"

#endif /*L2PACKETS_H_*/
