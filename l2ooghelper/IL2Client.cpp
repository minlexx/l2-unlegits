#include "stdafx.h"
#include "logger.h"
#include "IL2Client.h"
#include "L2Client.h"

IL2Client::IL2Client( void *pvL2Client )
{
	m_pcl = pvL2Client;
	m_lastNPCHTML = NULL;
	m_lastNPCHTMLoid = 0;
}

IL2Client::~IL2Client()
{
	m_pcl = NULL;
	if( m_lastNPCHTML )
	{
		free( m_lastNPCHTML );
		m_lastNPCHTML = NULL;
	}
	m_lastNPCHTMLoid = 0;
}

BotConfig *IL2Client::get_BotConfig()
{
	L2Client *cl = (L2Client *)m_pcl;
	return &(cl->botcfg);
}

bool IL2Client::isBotCombatEnabled() const
{
	L2Client *cl = (L2Client *)m_pcl;
	if( cl->bot_combat_enabled ) return true;
	return false;
}

void IL2Client::botCombatEnable( bool bEnable )
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->bottingCombatEnable( bEnable );
}

int IL2Client::is_IN_GAME()
{
	L2Client *cl = (L2Client *)m_pcl;
	return (cl->getState() == L2Client::STATE_IN_GAME ? 1 : 0);
}

// user info
UserInfo *IL2Client::get_UserInfo()
{
	L2Client *cl = (L2Client *)m_pcl;
	return &(cl->usr);
}

UserStorageMaxCount *IL2Client::get_StorageMaxCount()
{
	L2Client *cl = (L2Client *)m_pcl;
	return &(cl->storageMaxCount);
}

UserInventory *IL2Client::get_Inventory()
{
	L2Client *cl = (L2Client *)m_pcl;
	return &(cl->inv);
}

UserSkills *IL2Client::get_Skills()
{
	L2Client *cl = (L2Client *)m_pcl;
	return &(cl->skills);
}

UserEtcStatus *IL2Client::get_EtcStatus()
{
	L2Client *cl = (L2Client *)m_pcl;
	return &(cl->etcStatus);
}

UserBuffs *IL2Client::get_Buffs()
{
	L2Client *cl = (L2Client *)m_pcl;
	return &(cl->buffs);
}

UserParty *IL2Client::get_Party()
{
	L2Client *cl = (L2Client *)m_pcl;
	return &(cl->party);
}

// world state
WorldObjectTree *IL2Client::get_WorldObjectTree()
{
	L2Client *cl = (L2Client *)m_pcl;
	return &(cl->world_tree);
}

CharArray *IL2Client::get_WorldChars()
{
	L2Client *cl = (L2Client *)m_pcl;
	return &(cl->world_chars);
}

NpcArray *IL2Client::get_WorldNpcs()
{
	L2Client *cl = (L2Client *)m_pcl;
	return &(cl->world_npcs);
}

GIArray *IL2Client::get_WorldItems()
{
	L2Client *cl = (L2Client *)m_pcl;
	return &(cl->world_ground_items);
}

ClanList *IL2Client::get_WorldClans()
{
	L2Client *cl = (L2Client *)m_pcl;
	return &(cl->world_clans);
}


void IL2Client::game_Party_Accept( bool bAccept )
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->send_RequestAnswerJoinParty( bAccept ? 1 : 0 );
}

void IL2Client::game_Party_InvitePlayer( const wchar_t *playerName, unsigned int partyLoot )
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->send_RequestJoinParty( playerName, partyLoot );
}

void IL2Client::game_Party_Leave()
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->send_RequestWithdrawalParty();
}

void IL2Client::game_Party_KickPlayer( const wchar_t *playerName )
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->send_RequestOustPartyMember( playerName );
}

void IL2Client::game_Party_ChangeLeader( const wchar_t *playerName )
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->send_RequestChangePartyLeader( playerName );
}

// chat
void IL2Client::game_Say( unsigned int channel, const wchar_t *text )
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->send_Say2( channel, text, NULL );
}

void IL2Client::game_SayPm( const wchar_t *text, const wchar_t *to )
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->send_Say2( L2_CHAT_MESSAGE::TELL, text, to );
}

void IL2Client::game_SendBypass( const wchar_t *bypass )
{
	if( !bypass ) return;
	L2Client *cl = (L2Client *)m_pcl;
	if( wcsstr( bypass, L"bypass -h " ) )
	{
		cl->send_RequestBypassToServer( bypass + 10 /*wcslen( L"bypass -h " )*/ );
	}
	else if( wcsstr( bypass, L"link " ) )
	{
		cl->send_RequestLinkHtml( bypass + 5 /*wcslen( L"link " )*/  );
	}
	else
	{
		log_error( LOG_WARNING, "IL2Client::game_SendBypass(): string must start with \"bypass -h \"!\n" );
	}
}

// move
void IL2Client::game_MoveBackwardToLocation( int xDst, int yDst, int zDst )
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->send_MoveBackwardToLocation( xDst, yDst, zDst );
}

void IL2Client::game_MoveToDelta( int dx, int dy, int dz )
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->game_MoveToDelta( dx, dy, dz );
}
void IL2Client::game_SitStand()
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->send_RequestActionUse( 0 ); // 0 - sit/stand
}

void IL2Client::game_RunWalk()
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->send_RequestActionUse( 1 ); // 1 - run/walk
}


// use skill, item
void IL2Client::game_SkillUse( unsigned int skillID, unsigned int ctrlPressed, unsigned char shiftPressed )
{
	L2Client *cl = (L2Client *)m_pcl;
	UserSkill skill;
	cl->skills.getSkillInfoBySkillId( skillID, &skill );
	if( skill.level > 0 ) cl->send_RequestMagicSkillUse( skillID, ctrlPressed, shiftPressed );
}

int  IL2Client::game_getSkillLevel( unsigned int skillID ) // 0 if no such skill
{
	L2Client *cl = (L2Client *)m_pcl;
	UserSkill skill;
	cl->skills.getSkillInfoBySkillId( skillID, &skill );
	return skill.level;
}

void IL2Client::game_UseItemByItemId( unsigned int itemID )
{
	L2Client *cl = (L2Client *)m_pcl;
	UserInventoryItem it;
	cl->inv.getItemInfoByItemId( itemID, &it );
	if( it.objectID > 0  &&  it.count > 0 )
		cl->send_UseItem( it.objectID );
}

void IL2Client::game_UseItemByObjectId( unsigned int objectID )
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->send_UseItem( objectID );
}

unsigned long long int IL2Client::game_getItemCount( unsigned int itemID ) // 0 if no such item in inv
{
	L2Client *cl = (L2Client *)m_pcl;
	UserInventoryItem it;
	cl->inv.getItemInfoByItemId( itemID, &it );
	return it.count;
}

void IL2Client::game_DestroyItem( unsigned int objectID, long long int count )
{
	L2Client *cl = (L2Client *)m_pcl;
	UserInventoryItem it;
	cl->inv.getItemInfoByObjectId( objectID, &it );
	if( it.count >= (unsigned int)count ) cl->send_RequestDestroyItem( objectID, count );
}

void IL2Client::game_DropItem( unsigned int objectID, long long int count, int x, int y, int z )
{
	L2Client *cl = (L2Client *)m_pcl;
	UserInventoryItem it;
	cl->inv.getItemInfoByObjectId( objectID, &it );
	if( it.count >= (unsigned int)count ) cl->send_RequestDropItem( objectID, count, x, y, z );
}

void IL2Client::game_CrystallizeItem( unsigned int objectID, long long int count )
{
	L2Client *cl = (L2Client *)m_pcl;
	UserInventoryItem it;
	cl->inv.getItemInfoByObjectId( objectID, &it );
	if( it.count >= (unsigned int)count ) cl->send_RequestCrystallizeItem( objectID, count );
}

void IL2Client::game_GiveItemToPet( unsigned int objectID, long long int count )
{
	L2Client *cl = (L2Client *)m_pcl;
	UserInventoryItem it;
	cl->inv.getItemInfoByObjectId( objectID, &it );
	if( it.count >= (unsigned int)count ) cl->send_RequestGiveItemToPet( objectID, count );
}


// attack, target
void IL2Client::game_Action( unsigned int objectID )
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->send_Action( objectID );
}

void IL2Client::game_TargetByName( const wchar_t *name )
{
	//L2Client *cl = (L2Client *)m_pcl;
	UserInfo *usr = get_UserInfo();
	CharArray *ca = get_WorldChars();
	NpcArray *na = get_WorldNpcs();
	unsigned int nPassed = 0, i = 0, count = ca->GetCount();
	unsigned int objectID = 0;
	if( _wcsicmp( name, usr->charName ) == 0 )
	{
		objectID = usr->objectID;
	}
	if( objectID == 0 )
	{
		// try chars
		count = ca->GetCount();
		nPassed = 0;
		if( count > 0 )
		{
			ca->Lock();
			for( i=0; i<CharArray::CHARARRAY_MAX_CHARS; i++ )
			{
				if( ca->chars_array[i]->isUnused() ) continue;
				nPassed++;
				if( _wcsicmp( name, ca->chars_array[i]->charName ) == 0 )
				{
					objectID = ca->chars_array[i]->objectID;
					break;
				}
				if( nPassed >= count ) break;
			}
			ca->Unlock();
		}
	}
	if( objectID == 0 )
	{
		// try NPCs
		count = na->getCount();
		nPassed = 0;
		if( count > 0 )
		{
			na->Lock();
			for( i=0; i<NpcArray::NPCA_MAX_NPCS; i++ )
			{
				if( na->npcs_array[i]->isUnused() ) continue;
				nPassed++;
				if( _wcsicmp( name, na->npcs_array[i]->charName ) == 0 )
				{
					objectID = na->npcs_array[i]->objectID;
					break;
				}
				if( nPassed >= count ) break;
			}
			na->Unlock();
		}
	}
	if( objectID == 0 ) return;
	game_Action( objectID );
}

void IL2Client::game_TargetCancel()
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->send_RequestTargetCanceld();
}

void IL2Client::game_Attack( unsigned int objectID )
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->send_AttackRequest( objectID );
}

void IL2Client::game_autoSoulshot( unsigned int itemID, bool bEnable )
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->send_RequestAutoSoulshot( itemID, bEnable );
}

void IL2Client::game_buffCancel( unsigned int skillID )
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->send_RequestDispel( skillID );
}

void IL2Client::game_sendPacketHex( const char *szHexStr )
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->send_hackPacketHex( szHexStr );
}



int IL2Client::get_MapScale()
{
	L2Client *cl = (L2Client *)m_pcl;
	return cl->getScaleSliderPos();
}

void IL2Client::getMapShowParams( st_MapShowParams *st )
{
	L2Client *cl = (L2Client *)m_pcl;
	cl->getMapShowParams( st );
}

void IL2Client::set_last_NPC_HTML( const wchar_t *html, unsigned int objectID )
{
	if( !html ) return;
	if( html[0] == 0 ) return;
	size_t memsize = wcslen( html ) * 2 + 2;
	if( this->m_lastNPCHTML ) free( this->m_lastNPCHTML );
	this->m_lastNPCHTML = NULL;
	this->m_lastNPCHTML = (wchar_t *)malloc( memsize );
	if( !this->m_lastNPCHTML ) return;
	memset( this->m_lastNPCHTML, 0, memsize );
	wcscpy( this->m_lastNPCHTML, html );
	this->m_lastNPCHTMLoid = objectID;
	if( objectID == 0 )
	{
		log_error( LOG_WARNING, "SE: set_last_NPC_HTML(): objectID is 0!\n" );
		return;
	}
}

wchar_t *IL2Client::get_last_NPC_HTML( unsigned int *pObjectID )
{
	if( pObjectID ) (*pObjectID) = this->m_lastNPCHTMLoid;
	return this->m_lastNPCHTML;
}

void IL2Client::clear_last_NPC_HTML()
{
	m_lastNPCHTMLoid = 0;
	if( m_lastNPCHTML )
	{
		free( m_lastNPCHTML );
		m_lastNPCHTML = NULL;
	}
}
