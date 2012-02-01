#ifndef H_PGEN
#define H_PGEN

// to server
void PGen_Action( unsigned int oid, int x, int y, int z, unsigned char useShift );
void PGen_AttackRequest( unsigned int oid, int x, int y, int z, unsigned char useShift );
void PGen_GameGuardReply_L2J();
void PGen_MoveBackwardToLocation( int dst_x, int dst_y, int dst_z, int src_x, int src_y, int src_z );
void PGen_RequestActionUse( unsigned int actionId, bool ctrlPressed, bool shiftPressed );
void PGen_RequestAutoSoulshot( unsigned int itemID, bool enable );
void PGen_RequestBypassToserver( const wchar_t *bypassStr );
void PGen_RequestChangePartyLeader( const wchar_t *playerName );
void PGen_RequestDispel( unsigned int skillID );
void PGen_RequestJoinParty( const wchar_t *invitePlayer, unsigned int lootRules );
void PGen_RequestLinkHtml( const wchar_t *link );
void PGen_RequestMagicSkillUse( unsigned int skillID, unsigned int ctrlPressed, unsigned char shiftPressed );
void PGen_RequestOustPartyMember( const wchar_t *dismissPlayer );
void PGen_RequestTargetCanceld( bool abortCast );
void PGen_RequestWithdrawalParty();
void PGen_Say2C( unsigned int channelID, const wchar_t *text, const wchar_t *player_to = NULL );
void PGen_UseItem( unsigned int oid );

// to client
void PGenC_CreatureSay( unsigned int objectID, unsigned int channelID, const wchar_t *senderName, const wchar_t *text );
void PGenC_DeleteObject( unsigned int objectID );
void PGenC_MoveToLocation( unsigned int objectID, int x, int y, int z, int dx, int dy, int dz );
void PGenC_NpcHtmlMessage( unsigned int npcObjectId, const wchar_t *text, unsigned int itemId = 0 );
void PGenC_StatusUpdate( L2Player *pl );
void PGenC_StopMove( unsigned int objectID, int x, int y, int z, unsigned int heading );
void PGenC_ShowMiniMap( unsigned int mapID = 1665 );

// hack!..
void PGen_send_hackPacketHex_toServer( const char *szHexStr );

#endif
