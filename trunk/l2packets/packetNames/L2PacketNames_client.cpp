#include "stdafx.h"
#include "../L2_versions.h"
#include "L2PacketNames.h"

char l2_packet_names_client[256][128];
char l2_packet_names_client2[256][128];

// prototype
void L2Packets_GetL2PacketName_fromClient_t23(
	unsigned char opcode1, unsigned short opcode2, unsigned short opcode3,
	char *outPacketName, unsigned int cchMax );

void L2PacketNames_InitClient()
{
	memset( l2_packet_names_client, 0, sizeof(l2_packet_names_client) );
	memset( l2_packet_names_client2, 0, sizeof(l2_packet_names_client2) );

	strcpy( l2_packet_names_client[ 0x00 ], "Logout" );
	strcpy( l2_packet_names_client[ 0x01 ], "AttackRequest" );
	strcpy( l2_packet_names_client[ 0x03 ], "RequestStartPledgeWar" );
	strcpy( l2_packet_names_client[ 0x04 ], "RequestReplyStartPledgeWar" );
	strcpy( l2_packet_names_client[ 0x05 ], "RequestStopPledgeWar" );
	strcpy( l2_packet_names_client[ 0x06 ], "RequestReplyStopPledgeWar" );
	strcpy( l2_packet_names_client[ 0x07 ], "RequestSurrenderPledgeWar" );
	strcpy( l2_packet_names_client[ 0x08 ], "RequestReplySurrenderPledgeWar" );
	strcpy( l2_packet_names_client[ 0x09 ], "RequestSetPledgeCrest" );
	strcpy( l2_packet_names_client[ 0x0b ], "RequestGiveNickName" );
	strcpy( l2_packet_names_client[ 0x0e ], "ProtocolVersion" );
	strcpy( l2_packet_names_client[ 0x0f ], "MoveBackwardToLocation" );
	strcpy( l2_packet_names_client[ 0x11 ], "EnterWorld" );
	strcpy( l2_packet_names_client[ 0x14 ], "RequestItemList" );
	strcpy( l2_packet_names_client[ 0x16 ], "RequestUnEquipItem" );
	strcpy( l2_packet_names_client[ 0x17 ], "RequestDropItem" );
	strcpy( l2_packet_names_client[ 0x19 ], "UseItem" );
	strcpy( l2_packet_names_client[ 0x1a ], "TradeRequest" );
	strcpy( l2_packet_names_client[ 0x1b ], "AddTradeItem" );
	strcpy( l2_packet_names_client[ 0x1c ], "TradeDone" );
	strcpy( l2_packet_names_client[ 0x1f ], "Action" );
	strcpy( l2_packet_names_client[ 0x22 ], "RequestLinkHtml" );
	strcpy( l2_packet_names_client[ 0x23 ], "RequestBypassToServer" );
	strcpy( l2_packet_names_client[ 0x24 ], "RequestBBSwrite" );
	strcpy( l2_packet_names_client[ 0x26 ], "RequestJoinPledge" );
	strcpy( l2_packet_names_client[ 0x27 ], "RequestAnswerJoinPledge" );
	strcpy( l2_packet_names_client[ 0x28 ], "RequestWithdrawalPledge" );
	strcpy( l2_packet_names_client[ 0x29 ], "RequestOustPledgeMember" );
	strcpy( l2_packet_names_client[ 0x2b ], "AuthLogin" );
	strcpy( l2_packet_names_client[ 0x2c ], "RequestGetItemFromPet" );
	strcpy( l2_packet_names_client[ 0x2e ], "RequestAllyInfo" );
	strcpy( l2_packet_names_client[ 0x2f ], "RequestCrystallizeItem" );
	strcpy( l2_packet_names_client[ 0x30 ], "RequestPrivateStoreManageSell" );
	strcpy( l2_packet_names_client[ 0x31 ], "SetPrivateStoreListSell" );
	strcpy( l2_packet_names_client[ 0x34 ], "RequestSocialAction" );
	strcpy( l2_packet_names_client[ 0x35 ], "ChangeMoveType2" );
	strcpy( l2_packet_names_client[ 0x36 ], "ChangeWaitType2" );
	strcpy( l2_packet_names_client[ 0x37 ], "RequestSellItem" );
	strcpy( l2_packet_names_client[ 0x39 ], "RequestMagicSkillUse" );
	strcpy( l2_packet_names_client[ 0x3a ], "Appearing" );
	strcpy( l2_packet_names_client[ 0x3b ], "SendWareHouseDepositList" );
	strcpy( l2_packet_names_client[ 0x3c ], "SendWareHouseWithDrawList" );
	strcpy( l2_packet_names_client[ 0x3d ], "RequestShortCutReg" );
	strcpy( l2_packet_names_client[ 0x3f ], "RequestShortCutDel" );
	strcpy( l2_packet_names_client[ 0x40 ], "RequestBuyItem" );
	strcpy( l2_packet_names_client[ 0x42 ], "RequestJoinParty" );
	strcpy( l2_packet_names_client[ 0x43 ], "RequestAnswerJoinParty" );
	strcpy( l2_packet_names_client[ 0x44 ], "RequestWithDrawalParty" );
	strcpy( l2_packet_names_client[ 0x45 ], "RequestOustPartyMember" );
	strcpy( l2_packet_names_client[ 0x46 ], "RequestDismissParty" );
	strcpy( l2_packet_names_client[ 0x47 ], "CannotMoveAnymore" );
	strcpy( l2_packet_names_client[ 0x48 ], "RequestTargetCanceld" );
	strcpy( l2_packet_names_client[ 0x49 ], "Say2" );
	strcpy( l2_packet_names_client[ 0x4d ], "RequestPledgeMemberList" );
	strcpy( l2_packet_names_client[ 0x4f ], "DummyPacket" );
	strcpy( l2_packet_names_client[ 0x50 ], "RequestSkillList" );
	strcpy( l2_packet_names_client[ 0x52 ], "MoveWithDelta" );
	strcpy( l2_packet_names_client[ 0x53 ], "RequestGetOnVehicle" );
	strcpy( l2_packet_names_client[ 0x54 ], "RequestGetOffVehicle" );
	strcpy( l2_packet_names_client[ 0x55 ], "AnswerTradeRequest" );
	strcpy( l2_packet_names_client[ 0x56 ], "RequestActionUse" );
	strcpy( l2_packet_names_client[ 0x57 ], "RequestRestart" );
	strcpy( l2_packet_names_client[ 0x58 ], "RequestSiegeInfo" );
	strcpy( l2_packet_names_client[ 0x59 ], "ValidatePosition" );
	strcpy( l2_packet_names_client[ 0x5b ], "StartRotating" );
	strcpy( l2_packet_names_client[ 0x5c ], "FinishRotating" );
	strcpy( l2_packet_names_client[ 0x5e ], "RequestShowBoard" );
	strcpy( l2_packet_names_client[ 0x5f ], "RequestEnchantItem" );
	strcpy( l2_packet_names_client[ 0x60 ], "RequestDestroyItem" );
	strcpy( l2_packet_names_client[ 0x62 ], "RequestQuestList" );
	strcpy( l2_packet_names_client[ 0x63 ], "RequestQuestAbort" );
	strcpy( l2_packet_names_client[ 0x65 ], "RequestPledgeInfo" );
	strcpy( l2_packet_names_client[ 0x66 ], "RequestPledgeExtendedInfo" );
	strcpy( l2_packet_names_client[ 0x67 ], "RequestPledgeCrest" );
	strcpy( l2_packet_names_client[ 0x6f ], "RequestHennaEquip" );
	strcpy( l2_packet_names_client[ 0x73 ], "RequestAquireSkillInfo" );
	strcpy( l2_packet_names_client[ 0x74 ], "SendBypassBuildCmd" );
	strcpy( l2_packet_names_client[ 0x75 ], "RequestMoveToLocationInVehicle" );
	strcpy( l2_packet_names_client[ 0x76 ], "CannotMoveAnymoreInVehicle" );
	strcpy( l2_packet_names_client[ 0x77 ], "RequestFriendInvite" );
	strcpy( l2_packet_names_client[ 0x78 ], "RequestAnswerFriendInvite" );
	strcpy( l2_packet_names_client[ 0x79 ], "RequestFriendList" );
	strcpy( l2_packet_names_client[ 0x7a ], "RequestFriendDel" );
	strcpy( l2_packet_names_client[ 0x7c ], "RequestAquireSkill" );
	strcpy( l2_packet_names_client[ 0x7d ], "RequestRestartPoint" );
	strcpy( l2_packet_names_client[ 0x7e ], "RequestGMCommand" );
	strcpy( l2_packet_names_client[ 0x7f ], "RequestPartyMatchConfig" );
	strcpy( l2_packet_names_client[ 0x80 ], "RequestPartyMatchList" );
	strcpy( l2_packet_names_client[ 0x81 ], "RequestPartyMatchDetail" );
	strcpy( l2_packet_names_client[ 0x83 ], "RequestPrivateStoreBuy" );
	strcpy( l2_packet_names_client[ 0x85 ], "RequestTutorialLinkHtml" );
	strcpy( l2_packet_names_client[ 0x86 ], "RequestTutorialPassCmdToServer" );
	strcpy( l2_packet_names_client[ 0x87 ], "RequestTutorialQuestionMark" );
	strcpy( l2_packet_names_client[ 0x88 ], "RequestTutorialClientEvent" );
	strcpy( l2_packet_names_client[ 0x89 ], "RequestPetition" );
	strcpy( l2_packet_names_client[ 0x8a ], "RequestPetitionCancel" );
	strcpy( l2_packet_names_client[ 0x8b ], "RequestGmList" );
	strcpy( l2_packet_names_client[ 0x8c ], "RequestJoinAlly" );
	strcpy( l2_packet_names_client[ 0x8d ], "RequestAnswerJoinAlly" );
	strcpy( l2_packet_names_client[ 0x8e ], "AllyLeave" );
	strcpy( l2_packet_names_client[ 0x8f ], "AllyDismiss" );
	strcpy( l2_packet_names_client[ 0x90 ], "RequestDismissAlly" );
	strcpy( l2_packet_names_client[ 0x91 ], "RequestSetAllyCrest" );
	strcpy( l2_packet_names_client[ 0x92 ], "RequestAllyCrest" );
	strcpy( l2_packet_names_client[ 0x93 ], "RequestChangePetName" );
	strcpy( l2_packet_names_client[ 0x94 ], "RequestPetUseItem" );
	strcpy( l2_packet_names_client[ 0x95 ], "RequestGiveItemToPet" );
	strcpy( l2_packet_names_client[ 0x96 ], "RequestPrivateStoreQuitSell" );
	strcpy( l2_packet_names_client[ 0x97 ], "SetPrivateStoreMsgSell" );
	strcpy( l2_packet_names_client[ 0x98 ], "RequestPetGetItem" );
	strcpy( l2_packet_names_client[ 0x99 ], "RequestPrivateStoreManageBuy" );
	strcpy( l2_packet_names_client[ 0x9a ], "SetPrivateStoreListBuy " );
	strcpy( l2_packet_names_client[ 0x9c ], "RequestPrivateStoreQuitBuy" );
	strcpy( l2_packet_names_client[ 0x9d ], "SetPrivateStoreMsgBuy" );
	strcpy( l2_packet_names_client[ 0x9f ], "RequestPrivateStoreSell" );
	strcpy( l2_packet_names_client[ 0xa6 ], "RequestSkillCoolTime" );
	strcpy( l2_packet_names_client[ 0xa7 ], "RequestPackageSendableItemList" );
	strcpy( l2_packet_names_client[ 0xa8 ], "RequestPackageSend" );
	strcpy( l2_packet_names_client[ 0xa9 ], "RequestBlock" );
	strcpy( l2_packet_names_client[ 0xaa ], "RequestCastleSiegeInfo" );
	strcpy( l2_packet_names_client[ 0xab ], "RequestSiegeAttackerList" );
	strcpy( l2_packet_names_client[ 0xac ], "RequestSiegeDefenderList" );
	strcpy( l2_packet_names_client[ 0xad ], "RequestJoinSiege" );
	strcpy( l2_packet_names_client[ 0xae ], "RequestConfirmSiegeWaitingList" );
	strcpy( l2_packet_names_client[ 0xb0 ], "MultiSellChoose" );
	strcpy( l2_packet_names_client[ 0xb1 ], "NetPing" );
	strcpy( l2_packet_names_client[ 0xb3 ], "RequestUserCommand" );
	strcpy( l2_packet_names_client[ 0xb4 ], "SnoopQuit" );
	strcpy( l2_packet_names_client[ 0xb5 ], "RequestRecipeBookOpen" );
	strcpy( l2_packet_names_client[ 0xb6 ], "RequestRecipeBookDestroy" );
	strcpy( l2_packet_names_client[ 0xb7 ], "RequestRecipeItemMakeInfo" );
	strcpy( l2_packet_names_client[ 0xb8 ], "RequestRecipeItemMakeSelf" );
	strcpy( l2_packet_names_client[ 0xba ], "RequestRecipeShopMessageSet" );
	strcpy( l2_packet_names_client[ 0xbb ], "RequestRecipeShopListSet" );
	strcpy( l2_packet_names_client[ 0xbc ], "RequestRecipeShopManageQuit" );
	strcpy( l2_packet_names_client[ 0xbe ], "RequestRecipeShopMakeInfo" );
	strcpy( l2_packet_names_client[ 0xbf ], "RequestRecipeShopMakeItem" );
	strcpy( l2_packet_names_client[ 0xc0 ], "RequestRecipeShopManagePrev" );
	strcpy( l2_packet_names_client[ 0xc1 ], "ObserverReturn" );
	strcpy( l2_packet_names_client[ 0xc2 ], "RequestEvaluate" );
	strcpy( l2_packet_names_client[ 0xc3 ], "RequestHennaList" );
	strcpy( l2_packet_names_client[ 0xc4 ], "RequestHennaItemInfo" );
	strcpy( l2_packet_names_client[ 0xcc ], "RequestPledgePower" );
	strcpy( l2_packet_names_client[ 0xcd ], "RequestMakeMacro" );
	strcpy( l2_packet_names_client[ 0xce ], "RequestDeleteMacro" );
	strcpy( l2_packet_names_client[ 0xcf ], "RequestBuyProcure" );
	strcpy( l2_packet_names_client[ 0xc5 ], "RequestBuySeed" );
	strcpy( l2_packet_names_client[ 0xc6 ], "DlgAnswer" );
	strcpy( l2_packet_names_client[ 0xc7 ], "RequestWearItem" );
	strcpy( l2_packet_names_client[ 0xc8 ], "RequestSSQStatus" );
	strcpy( l2_packet_names_client[ 0xcb ], "GameGuardReply" );
	strcpy( l2_packet_names_client[ 0x6b ], "RequestSendFriendMsg" );
	strcpy( l2_packet_names_client[ 0x6c ], "RequestShowMiniMap" );
	strcpy( l2_packet_names_client[ 0x6e ], "RequestRecordInfo" );
	// double-opcode
	strcpy( l2_packet_names_client2[ 0x01 ], "RequestManorList" );
	strcpy( l2_packet_names_client2[ 0x02 ], "RequestProcureCropList" );
	strcpy( l2_packet_names_client2[ 0x03 ], "RequestSetSeed" );
	strcpy( l2_packet_names_client2[ 0x04 ], "RequestSetCrop" );
	strcpy( l2_packet_names_client2[ 0x05 ], "RequestWriteHeroWords" );
	strcpy( l2_packet_names_client2[ 0x06 ], "RequestExAskJoinMPCC" );
	strcpy( l2_packet_names_client2[ 0x07 ], "RequestExAcceptJoinMPCC" );
	strcpy( l2_packet_names_client2[ 0x08 ], "RequestExOustFromMPCC" );
	strcpy( l2_packet_names_client2[ 0x09 ], "RequestOustFromPartyRoom" );
	strcpy( l2_packet_names_client2[ 0x0a ], "RequestDismissPartyRoom" );
	strcpy( l2_packet_names_client2[ 0x0b ], "RequestWithdrawPartyRoom" );
	strcpy( l2_packet_names_client2[ 0x0c ], "RequestChangePartyLeader" );
	strcpy( l2_packet_names_client2[ 0x0d ], "RequestAutoSoulShot" );
	strcpy( l2_packet_names_client2[ 0x0e ], "RequestExEnchantSkillInfo" );
	strcpy( l2_packet_names_client2[ 0x0f ], "RequestExEnchantSkill" );
	strcpy( l2_packet_names_client2[ 0x10 ], "RequestExPledgeCrestLarge" );
	strcpy( l2_packet_names_client2[ 0x11 ], "RequestExSetPledgeCrestLarge" );
	strcpy( l2_packet_names_client2[ 0x12 ], "RequestPledgeSetAcademyMaster" );
	strcpy( l2_packet_names_client2[ 0x13 ], "RequestPledgePowerGradeList" );
	strcpy( l2_packet_names_client2[ 0x14 ], "RequestPledgeMemberPowerInfo" );
	strcpy( l2_packet_names_client2[ 0x15 ], "RequestPledgeSetMemberPowerGrade" );
	strcpy( l2_packet_names_client2[ 0x16 ], "RequestPledgeMemberInfo" );
	strcpy( l2_packet_names_client2[ 0x17 ], "RequestPledgeWarList" );
	strcpy( l2_packet_names_client2[ 0x18 ], "RequestExFishRanking" );
	strcpy( l2_packet_names_client2[ 0x19 ], "RequestPCCafeCouponUse" );
	strcpy( l2_packet_names_client2[ 0x1b ], "RequestDuelStart" );
	strcpy( l2_packet_names_client2[ 0x1c ], "RequestDuelAnswerStart" );
	strcpy( l2_packet_names_client2[ 0x1e ], "RequestExRqItemLink" );
	strcpy( l2_packet_names_client2[ 0x21 ], "RequestKeyMapping" );
	strcpy( l2_packet_names_client2[ 0x24 ], "RequestSaveInventoryOrder" );
	strcpy( l2_packet_names_client2[ 0x25 ], "RequestExitPartyMatchingWaitingRoom" );
	strcpy( l2_packet_names_client2[ 0x26 ], "RequestConfirmTargetItem" );
	strcpy( l2_packet_names_client2[ 0x27 ], "RequestConfirmRefinerItem" );
	strcpy( l2_packet_names_client2[ 0x28 ], "RequestConfirmGemStone" );
	strcpy( l2_packet_names_client2[ 0x29 ], "RequestOlympiadObserverEnd" );
	strcpy( l2_packet_names_client2[ 0x2a ], "RequestCursedWeaponList" );
	strcpy( l2_packet_names_client2[ 0x2b ], "RequestCursedWeaponLocation" );
	strcpy( l2_packet_names_client2[ 0x2c ], "RequestPledgeReorganizeMember" );
	strcpy( l2_packet_names_client2[ 0x2e ], "RequestExMPCCShowPartyMembersInfo" );
	strcpy( l2_packet_names_client2[ 0x2f ], "RequestOlympiadMatchList" );
	strcpy( l2_packet_names_client2[ 0x30 ], "RequestAskJoinPartyRoom" );
	strcpy( l2_packet_names_client2[ 0x31 ], "AnswerJoinPartyRoom" );
	strcpy( l2_packet_names_client2[ 0x32 ], "RequestListPartyMatchingWaitingRoom" );
	strcpy( l2_packet_names_client2[ 0x33 ], "RequestExEnchantSkillSafe" );
	strcpy( l2_packet_names_client2[ 0x34 ], "RequestExEnchantSkillUntrain" );
	strcpy( l2_packet_names_client2[ 0x35 ], "RequestExEnchantSkillRouteChange" );
	strcpy( l2_packet_names_client2[ 0x36 ], "ExGetOnAirShip" );
	strcpy( l2_packet_names_client2[ 0x38 ], "RequestExEnchantItemAttribute" );
	strcpy( l2_packet_names_client2[ 0x3f ], "RequestAllCastleInfo" );
	strcpy( l2_packet_names_client2[ 0x40 ], "RequestAllFortressInfo" );
	strcpy( l2_packet_names_client2[ 0x41 ], "RequestAllAgitInfo" );
	strcpy( l2_packet_names_client2[ 0x42 ], "RequestFortressSiegeInfo" );
	strcpy( l2_packet_names_client2[ 0x43 ], "RequestGetBossRecord" );
	strcpy( l2_packet_names_client2[ 0x44 ], "RequestRefine" );
	strcpy( l2_packet_names_client2[ 0x45 ], "RequestConfirmCancelItem" );
	strcpy( l2_packet_names_client2[ 0x46 ], "RequestRefineCancel" );
	strcpy( l2_packet_names_client2[ 0x47 ], "RequestExMagicSkillUseGround" );
	strcpy( l2_packet_names_client2[ 0x48 ], "RequestDuelSurrender" );
	strcpy( l2_packet_names_client2[ 0x49 ], "RequestExEnchantSkillInfoDetail" );
	strcpy( l2_packet_names_client2[ 0x4b ], "RequestFortressMapInfo" );
	strcpy( l2_packet_names_client2[ 0x4d ], "SetPrivateStoreWholeMsg" );
	strcpy( l2_packet_names_client2[ 0x4e ], "RequestDispel" );
	strcpy( l2_packet_names_client2[ 0x4f ], "RequestExTryToPutEnchantTargetItem" );
	strcpy( l2_packet_names_client2[ 0x50 ], "RequestExTryToPutEnchantSupportItem" );
	strcpy( l2_packet_names_client2[ 0x51 ], "RequestExCancelEnchantItem" );
	strcpy( l2_packet_names_client2[ 0x52 ], "RequestChangeNicknameColor" );
	strcpy( l2_packet_names_client2[ 0x53 ], "RequestResetNickname" );
}

void L2Packets_GetL2PacketName_fromClient(
	unsigned char   opcode1,
	unsigned short  opcode2,
	char           *outPacketName,
	unsigned int    cchMax
)
{
	if( opcode1 != 0xD0 )
	{
		strncpy( outPacketName, l2_packet_names_client[opcode1], cchMax );
	}
	else
	{
		strncpy( outPacketName, l2_packet_names_client2[opcode2], cchMax );
	}
}
