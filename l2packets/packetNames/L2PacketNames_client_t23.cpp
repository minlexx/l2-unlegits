#include "stdafx.h"
#include "../L2_versions.h"
#include "L2PacketNames.h"

char l2_packet_names_client_t23[256][128];
char l2_packet_names_client2_t23[256][128];
char l2_packet_names_client3_t23[256][128];
char l2_packet_names_client_supercommands_t23[256][128];

void L2PacketNames_InitClient_t23()
{
	memset( l2_packet_names_client_t23, 0, sizeof(l2_packet_names_client_t23) );
	memset( l2_packet_names_client2_t23, 0, sizeof(l2_packet_names_client2_t23) );
	memset( l2_packet_names_client3_t23, 0, sizeof(l2_packet_names_client3_t23) );
	memset( l2_packet_names_client_supercommands_t23, 0, sizeof(l2_packet_names_client_supercommands_t23) );

	// for state IN_GAME, single-opcode packet names
	strcpy( l2_packet_names_client_t23[0], "Logout" );
	strcpy( l2_packet_names_client_t23[1], "AttackRequest" );
	strcpy( l2_packet_names_client_t23[2], "..." ); // unused
	strcpy( l2_packet_names_client_t23[3], "RequestStartPledgeWar" );
	strcpy( l2_packet_names_client_t23[4], "RequestReplyStartPledgeWar" );
	strcpy( l2_packet_names_client_t23[5], "RequestStopPledgeWar" );
	strcpy( l2_packet_names_client_t23[6], "RequestReplyStopPledgeWar" );
	strcpy( l2_packet_names_client_t23[7], "RequestSurrenderPledgeWar" );
	strcpy( l2_packet_names_client_t23[8], "RequestReplySurrenderPledgeWar" );
	strcpy( l2_packet_names_client_t23[9], "RequestSetPledgeCrest" );
	strcpy( l2_packet_names_client_t23[10], "..." ); // unused
	strcpy( l2_packet_names_client_t23[11], "RequestGiveNickName" );
	strcpy( l2_packet_names_client_t23[12], "..." ); // unused
	strcpy( l2_packet_names_client_t23[13], "..." ); // unused
	strcpy( l2_packet_names_client_t23[14], "ProtocolVersion" );
	strcpy( l2_packet_names_client_t23[15], "MoveBackwardToLocation" );
	strcpy( l2_packet_names_client_t23[16], "Say (unused)" );
	strcpy( l2_packet_names_client_t23[17], "EnterWorld" );
	strcpy( l2_packet_names_client_t23[18], "CharacterSelect" );
	strcpy( l2_packet_names_client_t23[19], "..." ); // unused
	strcpy( l2_packet_names_client_t23[20], "RequestItemList" );
	strcpy( l2_packet_names_client_t23[21], "RequestEquipItem (unused)" );
	strcpy( l2_packet_names_client_t23[22], "RequestUnEquipItem" );
	strcpy( l2_packet_names_client_t23[23], "RequestDropItem" );
	strcpy( l2_packet_names_client_t23[24], "..." ); // unused
	strcpy( l2_packet_names_client_t23[25], "UseItem" );
	strcpy( l2_packet_names_client_t23[26], "TradeRequest" );
	strcpy( l2_packet_names_client_t23[27], "AddTradeItem" );
	strcpy( l2_packet_names_client_t23[28], "TradeDone" );
	strcpy( l2_packet_names_client_t23[29], "..." ); // unused
	strcpy( l2_packet_names_client_t23[30], "..." ); // unused
	strcpy( l2_packet_names_client_t23[31], "Action" );
	strcpy( l2_packet_names_client_t23[32], "..." ); // unused
	strcpy( l2_packet_names_client_t23[33], "..." ); // unused
	strcpy( l2_packet_names_client_t23[34], "RequestLinkHtml" );
	strcpy( l2_packet_names_client_t23[35], "RequestBypassToServer" );
	strcpy( l2_packet_names_client_t23[36], "RequestBBSwrite" );
	strcpy( l2_packet_names_client_t23[37], "RequestCreatePledge (unused)" );
	strcpy( l2_packet_names_client_t23[38], "RequestJoinPledge" );
	strcpy( l2_packet_names_client_t23[39], "RequestAnswerJoinPledge" );
	strcpy( l2_packet_names_client_t23[40], "RequestWithdrawalPledge" );
	strcpy( l2_packet_names_client_t23[41], "RequestOustPledgeMember" );
	strcpy( l2_packet_names_client_t23[42], "..." ); // unused
	strcpy( l2_packet_names_client_t23[43], "AuthLogin" );
	strcpy( l2_packet_names_client_t23[44], "RequestGetItemFromPet" );
	strcpy( l2_packet_names_client_t23[45], "..." ); // unused
	strcpy( l2_packet_names_client_t23[46], "RequestAllyInfo" );
	strcpy( l2_packet_names_client_t23[47], "RequestCrystallizeItem" );
	strcpy( l2_packet_names_client_t23[48], "RequestPrivateStoreManageSell" );
	strcpy( l2_packet_names_client_t23[49], "SetPrivateStoreListSell" );
	strcpy( l2_packet_names_client_t23[50], "AttackRequest" );
	strcpy( l2_packet_names_client_t23[51], "RequestTeleportPacket (unused)" );
	strcpy( l2_packet_names_client_t23[52], "RequestSocialAction" );
	strcpy( l2_packet_names_client_t23[53], "ChangeMoveType2" );
	strcpy( l2_packet_names_client_t23[54], "ChangeWaitType2" );
	strcpy( l2_packet_names_client_t23[55], "RequestSellItem" );
	strcpy( l2_packet_names_client_t23[56], "RequestMagicSkillList (unused)" );
	strcpy( l2_packet_names_client_t23[57], "RequestMagicSkillUse" );
	strcpy( l2_packet_names_client_t23[58], "Appearing" );
	strcpy( l2_packet_names_client_t23[59], "SendWareHouseDepositList" );
	strcpy( l2_packet_names_client_t23[60], "SendWareHouseWithDrawList" );
	strcpy( l2_packet_names_client_t23[61], "RequestShortCutReg" );
	strcpy( l2_packet_names_client_t23[62], "..." ); // unused
	strcpy( l2_packet_names_client_t23[63], "RequestShortCutDel" );
	strcpy( l2_packet_names_client_t23[64], "RequestBuyItem" );
	strcpy( l2_packet_names_client_t23[65], "RequestDismissPledge (unused)" );
	strcpy( l2_packet_names_client_t23[66], "RequestJoinParty" );
	strcpy( l2_packet_names_client_t23[67], "RequestAnswerJoinParty" );
	strcpy( l2_packet_names_client_t23[68], "RequestWithDrawalParty" );
	strcpy( l2_packet_names_client_t23[69], "RequestOustPartyMember" );
	strcpy( l2_packet_names_client_t23[70], "RequestDismissParty (unused)" );
	strcpy( l2_packet_names_client_t23[71], "CannotMoveAnymore" );
	strcpy( l2_packet_names_client_t23[72], "RequestTargetCanceld" );
	strcpy( l2_packet_names_client_t23[73], "Say2" );
	strcpy( l2_packet_names_client_t23[74], "..." ); // unused
	strcpy( l2_packet_names_client_t23[75], "..." ); // unused
	strcpy( l2_packet_names_client_t23[76], "..." ); // unused
	strcpy( l2_packet_names_client_t23[77], "RequestPledgeMemberList" );
	strcpy( l2_packet_names_client_t23[78], "..." ); // unused
	strcpy( l2_packet_names_client_t23[79], "RequestMagicList (unused)" );
	strcpy( l2_packet_names_client_t23[80], "RequestSkillList" );
	strcpy( l2_packet_names_client_t23[81], "..." ); // unused
	strcpy( l2_packet_names_client_t23[82], "MoveWithDelta" );
	strcpy( l2_packet_names_client_t23[83], "RequestGetOnVehicle" );
	strcpy( l2_packet_names_client_t23[84], "RequestGetOffVehicle" );
	strcpy( l2_packet_names_client_t23[85], "AnswerTradeRequest" );
	strcpy( l2_packet_names_client_t23[86], "RequestActionUse" );
	strcpy( l2_packet_names_client_t23[87], "RequestRestart" );
	strcpy( l2_packet_names_client_t23[88], "RequestSiegeInfo" );
	strcpy( l2_packet_names_client_t23[89], "ValidatePosition" );
	strcpy( l2_packet_names_client_t23[90], "RequestSEKCustom (unused)" );
	strcpy( l2_packet_names_client_t23[91], "StartRotating (unused)" );
	strcpy( l2_packet_names_client_t23[92], "FinishRotating (unused)" );
	strcpy( l2_packet_names_client_t23[93], "..." ); // unused
	strcpy( l2_packet_names_client_t23[94], "RequestShowBoard" );
	strcpy( l2_packet_names_client_t23[95], "RequestEnchantItem" );
	strcpy( l2_packet_names_client_t23[96], "RequestDestroyItem" );
	strcpy( l2_packet_names_client_t23[97], "..." ); // unused
	strcpy( l2_packet_names_client_t23[98], "RequestQuestList" );
	strcpy( l2_packet_names_client_t23[99], "RequestQuestAbort" );
	strcpy( l2_packet_names_client_t23[100], "..." ); // unused
	strcpy( l2_packet_names_client_t23[101], "RequestPledgeInfo" );
	strcpy( l2_packet_names_client_t23[102], "RequestPledgeExtendedInfo" );
	strcpy( l2_packet_names_client_t23[103], "RequestPledgeCrest" );
	strcpy( l2_packet_names_client_t23[104], "..." ); // unused
	strcpy( l2_packet_names_client_t23[105], "..." ); // unused
	strcpy( l2_packet_names_client_t23[106], "..." ); // unused
	strcpy( l2_packet_names_client_t23[107], "RequestSendFriendMsg" );
	strcpy( l2_packet_names_client_t23[108], "RequestShowMiniMap" );
	strcpy( l2_packet_names_client_t23[109], "RequestSendMsnChatLog (unused)" );
	strcpy( l2_packet_names_client_t23[110], "RequestRecordInfo" );
	strcpy( l2_packet_names_client_t23[111], "RequestHennaEquip" );
	strcpy( l2_packet_names_client_t23[112], "RequestHennaUnequipList (unused)" );
	strcpy( l2_packet_names_client_t23[113], "RequestHennaUnequipInfo (unused)" );
	strcpy( l2_packet_names_client_t23[114], "RequestHennaUnequip (unused)" );
	strcpy( l2_packet_names_client_t23[115], "RequestAquireSkillInfo" );
	strcpy( l2_packet_names_client_t23[116], "SendBypassBuildCmd" );
	strcpy( l2_packet_names_client_t23[117], "RequestMoveToLocationInVehicle" );
	strcpy( l2_packet_names_client_t23[118], "CannotMoveAnymoreInVehicle" );
	strcpy( l2_packet_names_client_t23[119], "RequestFriendInvite" );
	strcpy( l2_packet_names_client_t23[120], "RequestAnswerFriendInvite" );
	strcpy( l2_packet_names_client_t23[121], "RequestFriendList" );
	strcpy( l2_packet_names_client_t23[122], "RequestFriendDel" );
	strcpy( l2_packet_names_client_t23[123], "..." ); // unused
	strcpy( l2_packet_names_client_t23[124], "RequestAquireSkill" );
	strcpy( l2_packet_names_client_t23[125], "RequestRestartPoint" );
	strcpy( l2_packet_names_client_t23[126], "RequestGMCommand" );
	strcpy( l2_packet_names_client_t23[127], "RequestPartyMatchConfig" );
	strcpy( l2_packet_names_client_t23[128], "RequestPartyMatchList" );
	strcpy( l2_packet_names_client_t23[129], "RequestPartyMatchDetail" );
	strcpy( l2_packet_names_client_t23[130], "..." ); // unused
	strcpy( l2_packet_names_client_t23[131], "RequestPrivateStoreBuy" );
	strcpy( l2_packet_names_client_t23[132], "..." ); // unused
	strcpy( l2_packet_names_client_t23[133], "RequestTutorialLinkHtml" );
	strcpy( l2_packet_names_client_t23[134], "RequestTutorialPassCmdToServer" );
	strcpy( l2_packet_names_client_t23[135], "RequestTutorialQuestionMark" );
	strcpy( l2_packet_names_client_t23[136], "RequestTutorialClientEvent" );
	strcpy( l2_packet_names_client_t23[137], "RequestPetition" );
	strcpy( l2_packet_names_client_t23[138], "RequestPetitionCancel" );
	strcpy( l2_packet_names_client_t23[139], "RequestGmList" );
	strcpy( l2_packet_names_client_t23[140], "RequestJoinAlly" );
	strcpy( l2_packet_names_client_t23[141], "RequestAnswerJoinAlly" );
	strcpy( l2_packet_names_client_t23[142], "AllyLeave" );
	strcpy( l2_packet_names_client_t23[143], "AllyDismiss" );
	strcpy( l2_packet_names_client_t23[144], "RequestDismissAlly" );
	strcpy( l2_packet_names_client_t23[145], "RequestSetAllyCrest" );
	strcpy( l2_packet_names_client_t23[146], "RequestAllyCrest" );
	strcpy( l2_packet_names_client_t23[147], "RequestChangePetName" );
	strcpy( l2_packet_names_client_t23[148], "RequestPetUseItem" );
	strcpy( l2_packet_names_client_t23[149], "RequestGiveItemToPet" );
	strcpy( l2_packet_names_client_t23[150], "RequestPrivateStoreQuitSell" );
	strcpy( l2_packet_names_client_t23[151], "SetPrivateStoreMsgSell" );
	strcpy( l2_packet_names_client_t23[152], "RequestPetGetItem" );
	strcpy( l2_packet_names_client_t23[153], "RequestPrivateStoreManageBuy" );
	strcpy( l2_packet_names_client_t23[154], "SetPrivateStoreList" );
	strcpy( l2_packet_names_client_t23[155], "..." ); // unused
	strcpy( l2_packet_names_client_t23[156], "RequestPrivateStoreQuitBuy" );
	strcpy( l2_packet_names_client_t23[157], "SetPrivateStoreMsgBuy" );
	strcpy( l2_packet_names_client_t23[158], "..." ); // unused
	strcpy( l2_packet_names_client_t23[159], "RequestPrivateStoreSell" );
	strcpy( l2_packet_names_client_t23[160], "SendTimeCheckPacket (unused)" );
	strcpy( l2_packet_names_client_t23[161], "..." ); // unused
	strcpy( l2_packet_names_client_t23[162], "..." ); // unused
	strcpy( l2_packet_names_client_t23[163], "..." ); // unused
	strcpy( l2_packet_names_client_t23[164], "..." ); // unused
	strcpy( l2_packet_names_client_t23[165], "..." ); // unused
	strcpy( l2_packet_names_client_t23[166], "RequestSkillCoolTime (unused)" );
	strcpy( l2_packet_names_client_t23[167], "RequestPackageSendableItemList" );
	strcpy( l2_packet_names_client_t23[168], "RequestPackageSend" );
	strcpy( l2_packet_names_client_t23[169], "RequestBlock" );
	strcpy( l2_packet_names_client_t23[170], "RequestSiegeInfo" );
	strcpy( l2_packet_names_client_t23[171], "RequestSiegeAttackerList" );
	strcpy( l2_packet_names_client_t23[172], "RequestSiegeDefenderList" );
	strcpy( l2_packet_names_client_t23[173], "RequestJoinSiege" );
	strcpy( l2_packet_names_client_t23[174], "RequestConfirmSiegeWaitingList" );
	strcpy( l2_packet_names_client_t23[175], "RequestSetCastleSiegeTime (unused)" );
	strcpy( l2_packet_names_client_t23[176], "MultiSellChoose" );
	strcpy( l2_packet_names_client_t23[177], "NetPing" );
	strcpy( l2_packet_names_client_t23[178], "RequestRemainTime (unused)" );
	strcpy( l2_packet_names_client_t23[179], "BypassUserCmd" );
	strcpy( l2_packet_names_client_t23[180], "SnoopQuit" );
	strcpy( l2_packet_names_client_t23[181], "RequestRecipeBookOpen" );
	strcpy( l2_packet_names_client_t23[182], "RequestRecipeBookDestroy" );
	strcpy( l2_packet_names_client_t23[183], "RequestRecipeItemMakeInfo" );
	strcpy( l2_packet_names_client_t23[184], "RequestRecipeItemMakeSelf" );
	strcpy( l2_packet_names_client_t23[185], "RequestRecipeShopManageList (unused)" );
	strcpy( l2_packet_names_client_t23[186], "RequestRecipeShopMessageSet" );
	strcpy( l2_packet_names_client_t23[187], "RequestRecipeShopListSet" );
	strcpy( l2_packet_names_client_t23[188], "RequestRecipeShopManageQuit" );
	strcpy( l2_packet_names_client_t23[189], "RequestRecipeShopManageCancel (unused)" );
	strcpy( l2_packet_names_client_t23[190], "RequestRecipeShopMakeInfo" );
	strcpy( l2_packet_names_client_t23[191], "RequestRecipeShopMakeItem" );
	strcpy( l2_packet_names_client_t23[192], "RequestRecipeShopManagePrev" );
	strcpy( l2_packet_names_client_t23[193], "ObserverReturn" );
	strcpy( l2_packet_names_client_t23[194], "RequestEvaluate" );
	strcpy( l2_packet_names_client_t23[195], "RequestHennaList" );
	strcpy( l2_packet_names_client_t23[196], "RequestHennaItemInfo" );
	strcpy( l2_packet_names_client_t23[197], "RequestBuySeed" );
	strcpy( l2_packet_names_client_t23[198], "DlgAnswer" );
	strcpy( l2_packet_names_client_t23[199], "RequestWearItem" );
	strcpy( l2_packet_names_client_t23[200], "RequestSSQStatus" );
	strcpy( l2_packet_names_client_t23[201], "PetitionVote (unused)" );
	strcpy( l2_packet_names_client_t23[202], "..." ); // unused
	strcpy( l2_packet_names_client_t23[203], "GameGuardReply" );
	strcpy( l2_packet_names_client_t23[204], "RequestPledgePower" );
	strcpy( l2_packet_names_client_t23[205], "RequestMakeMacro" );
	strcpy( l2_packet_names_client_t23[206], "RequestDeleteMacro" );
	strcpy( l2_packet_names_client_t23[207], "RequestBuyProcure" );

	// Double opcode cpackets (D0:xx)
	strcpy( l2_packet_names_client2_t23[1], "RequestManorList" );
	strcpy( l2_packet_names_client2_t23[2], "RequestProcureCropList" );
	strcpy( l2_packet_names_client2_t23[3], "RequestSetSeed" );
	strcpy( l2_packet_names_client2_t23[4], "RequestSetCrop" );
	strcpy( l2_packet_names_client2_t23[5], "RequestWriteHeroWords" );
	strcpy( l2_packet_names_client2_t23[6], "RequestExAskJoinMPCC" );
	strcpy( l2_packet_names_client2_t23[7], "RequestExAcceptJoinMPCC" );
	strcpy( l2_packet_names_client2_t23[8], "RequestExOustFromMPCC" );
	strcpy( l2_packet_names_client2_t23[9], "RequestOustFromPartyRoom" );
	strcpy( l2_packet_names_client2_t23[10], "RequestDismissPartyRoom" );
	strcpy( l2_packet_names_client2_t23[11], "RequestWithdrawPartyRoom" );
	strcpy( l2_packet_names_client2_t23[12], "RequestChangePartyLeader" );
	strcpy( l2_packet_names_client2_t23[13], "RequestAutoSoulShot" );
	strcpy( l2_packet_names_client2_t23[14], "RequestExEnchantSkillInfo" );
	strcpy( l2_packet_names_client2_t23[15], "RequestExEnchantSkill" );
	strcpy( l2_packet_names_client2_t23[16], "RequestExPledgeCrestLarge" );
	strcpy( l2_packet_names_client2_t23[17], "RequestExSetPledgeCrestLarge" );
	strcpy( l2_packet_names_client2_t23[18], "RequestPledgeSetAcademyMaster" );
	strcpy( l2_packet_names_client2_t23[19], "RequestPledgePowerGradeList" );
	strcpy( l2_packet_names_client2_t23[20], "RequestPledgeMemberPowerInfo" );
	strcpy( l2_packet_names_client2_t23[21], "RequestPledgeSetMemberPowerGrade" );
	strcpy( l2_packet_names_client2_t23[22], "RequestPledgeMemberInfo" );
	strcpy( l2_packet_names_client2_t23[23], "RequestPledgeWarList" );
	strcpy( l2_packet_names_client2_t23[24], "RequestExFishRanking" );
	strcpy( l2_packet_names_client2_t23[25], "RequestPCCafeCouponUse" );
	strcpy( l2_packet_names_client2_t23[27], "RequestDuelStart" );
	strcpy( l2_packet_names_client2_t23[28], "RequestDuelAnswerStart" );
	strcpy( l2_packet_names_client2_t23[29], "RequestExSetTutorial (unused)" );
	strcpy( l2_packet_names_client2_t23[30], "RequestExRqItemLink" );
	strcpy( l2_packet_names_client2_t23[31], "CanNotMoveAnymoreAirShip (unused)" );
	strcpy( l2_packet_names_client2_t23[32], "MoveToLocationInAirShip" );
	strcpy( l2_packet_names_client2_t23[33], "RequestKeyMapping" );
	strcpy( l2_packet_names_client2_t23[34], "RequestSaveKeyMapping (unused)" );
	strcpy( l2_packet_names_client2_t23[35], "RequestExRemoveItemAttribute" );
	strcpy( l2_packet_names_client2_t23[36], "RequestSaveInventoryOrder" );
	strcpy( l2_packet_names_client2_t23[37], "RequestExitPartyMatchingWaitingRoom" );
	strcpy( l2_packet_names_client2_t23[38], "RequestConfirmTargetItem" );
	strcpy( l2_packet_names_client2_t23[39], "RequestConfirmRefinerItem" );
	strcpy( l2_packet_names_client2_t23[40], "RequestConfirmGemStone" );
	strcpy( l2_packet_names_client2_t23[41], "RequestOlympiadObserverEnd" );
	strcpy( l2_packet_names_client2_t23[42], "RequestCursedWeaponList" );
	strcpy( l2_packet_names_client2_t23[43], "RequestCursedWeaponLocation" );
	strcpy( l2_packet_names_client2_t23[44], "RequestPledgeReorganizeMember" );
	strcpy( l2_packet_names_client2_t23[45], "RequestExMPCCShowPartyMembersInfo" );
	strcpy( l2_packet_names_client2_t23[46], "RequestOlympiadMatchList" );
	strcpy( l2_packet_names_client2_t23[47], "RequestAskJoinPartyRoom" );
	strcpy( l2_packet_names_client2_t23[48], "AnswerJoinPartyRoom" );
	strcpy( l2_packet_names_client2_t23[49], "RequestListPartyMatchingWaitingRoom" );
	strcpy( l2_packet_names_client2_t23[50], "RequestExEnchantSkillSafe" );
	strcpy( l2_packet_names_client2_t23[51], "RequestExEnchantSkillUntrain" );
	strcpy( l2_packet_names_client2_t23[52], "RequestExEnchantSkillRouteChange" );
	strcpy( l2_packet_names_client2_t23[53], "RequestExEnchantItemAttribute" );
	strcpy( l2_packet_names_client2_t23[54], "ExGetOnAirShip" );
	strcpy( l2_packet_names_client2_t23[56], "MoveToLocationAirShip (unused)" );
	strcpy( l2_packet_names_client2_t23[57], "RequestBidItemAuction (unused)" );
	strcpy( l2_packet_names_client2_t23[58], "RequestInfoItemAuction (unused)" );
	strcpy( l2_packet_names_client2_t23[59], "RequestExChangeName (unused)" );
	strcpy( l2_packet_names_client2_t23[60], "RequestAllCastleInfo" );
	strcpy( l2_packet_names_client2_t23[61], "RequestAllFortressInfo" );
	strcpy( l2_packet_names_client2_t23[62], "RequestAllAgitInfo" );
	strcpy( l2_packet_names_client2_t23[63], "RequestFortressSiegeInfo" );
	strcpy( l2_packet_names_client2_t23[64], "RequestGetBossRecord" );
	strcpy( l2_packet_names_client2_t23[65], "RequestRefine" );
	strcpy( l2_packet_names_client2_t23[66], "RequestConfirmCancelItem" );
	strcpy( l2_packet_names_client2_t23[67], "RequestRefineCancel" );
	strcpy( l2_packet_names_client2_t23[68], "RequestExMagicSkillUseGround" );
	strcpy( l2_packet_names_client2_t23[69], "RequestDuelSurrender" );
	strcpy( l2_packet_names_client2_t23[70], "RequestExEnchantSkillInfoDetail" );
	strcpy( l2_packet_names_client2_t23[72], "RequestFortressMapInfo" );
	strcpy( l2_packet_names_client2_t23[73], "RequestPVPMatchRecord (unused)" );
	strcpy( l2_packet_names_client2_t23[74], "SetPrivateStoreWholeMsg" );
	strcpy( l2_packet_names_client2_t23[75], "RequestDispel" );
	strcpy( l2_packet_names_client2_t23[76], "RequestExTryToPutEnchantTargetItem" );
	strcpy( l2_packet_names_client2_t23[77], "RequestExTryToPutEnchantSupportItem" );
	strcpy( l2_packet_names_client2_t23[78], "RequestExCancelEnchantItem" );
	strcpy( l2_packet_names_client2_t23[79], "RequestChangeNicknameColor" );
	strcpy( l2_packet_names_client2_t23[80], "RequestResetNickname" );
	strcpy( l2_packet_names_client2_t23[117], "RequestRefundItem" );
	strcpy( l2_packet_names_client2_t23[118], "RequestBuySellUIClose" );

	// Triple opcode packets (D0:51:xx)
	strcpy( l2_packet_names_client3_t23[0], "RequestBookMarkSlotInfo" );
	strcpy( l2_packet_names_client3_t23[1], "RequestSaveBookMarkSlot" );
	strcpy( l2_packet_names_client3_t23[2], "RequestModifyBookMarkSlot" );
	strcpy( l2_packet_names_client3_t23[3], "RequestDeleteBookMarkSlot" );
	strcpy( l2_packet_names_client3_t23[4], "RequestTeleportBookMark" );
	strcpy( l2_packet_names_client3_t23[5], "RequestChangeBookMarkSlot (unused)" );
	strcpy( l2_packet_names_client3_t23[82], "RequestWithDrawPremiumItem (unused)" );
	strcpy( l2_packet_names_client3_t23[83], "RequestJump (unused)" );
	strcpy( l2_packet_names_client3_t23[84], "RequestStartShowCrataeCubeRank (unused)" );
	strcpy( l2_packet_names_client3_t23[85], "RequestStopShowCrataeCubeRank (unused)" );
	strcpy( l2_packet_names_client3_t23[86], "NotifyStartMiniGame (unused)" );
	strcpy( l2_packet_names_client3_t23[87], "RequestJoinDominionWar (unused)" );
	strcpy( l2_packet_names_client3_t23[88], "RequestDominionInfo (unused)" );
	strcpy( l2_packet_names_client3_t23[89], "RequestExCleftEnter (unused)" );
	strcpy( l2_packet_names_client3_t23[90], "RequestExBlockGameEnter (unused)" );
	strcpy( l2_packet_names_client3_t23[91], "EndScenePlayer (unused)" );
	strcpy( l2_packet_names_client3_t23[92], "RequestExBlockGameVote (unused)" );
	strcpy( l2_packet_names_client3_t23[99], "RequestSeedPhase (unused)" );
	//strcpy( l2_packet_names_client3_t23[238], "RequestChangePartyLeader(da" ); // also unused

	// Super commands (4A:xx)
	strcpy( l2_packet_names_client_supercommands_t23[0], "SuperCmdCharacterInfo (unused)" );
	strcpy( l2_packet_names_client_supercommands_t23[1], "SuperCmdSummonCmd (unused)" );
	strcpy( l2_packet_names_client_supercommands_t23[2], "SuperCmdServerStatus (unused)" );
	strcpy( l2_packet_names_client_supercommands_t23[3], "SendL2ParamSetting (unused)" );
}

void L2Packets_GetL2PacketName_fromClient_t23(
	unsigned char   opcode1,
	unsigned short  opcode2,
	unsigned short  opcode3,
	char           *outPacketName,
	unsigned int cchMax )
{
	if( opcode1 == 0xD0 ) // double-byte or triple-byte packet
	{
		if( opcode2 == 0x51 ) // triple-opcode packet
			strncpy( outPacketName, l2_packet_names_client3_t23[opcode3], cchMax );
		else // double-byte packet
			strncpy( outPacketName, l2_packet_names_client2_t23[opcode2], cchMax );
	}
	else if( opcode1 == 0x4A ) // SuperCommand? O_o
	{
		strncpy( outPacketName, l2_packet_names_client_supercommands_t23[opcode2], cchMax );
	}
	else // usual single-opcode packet
	{
		strncpy( outPacketName, l2_packet_names_client_t23[opcode1], cchMax );
	}
}


