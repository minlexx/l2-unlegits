#include "pch.h"
#include "Log.h"
#include "l2c_utils.h"
#include "net/GameClient/GameClient.h"
#include "../ClientPacketHandler.h"
#include "../ServerPackets.h"
#include "GS.h"
#include "utils/Utils.h"
#include "datatables/CharNameTable.h"
#include "datatables/CharTemplateTable.h"
#include "datatables/ItemTable.h"

L2GamePacket *ClientPacketHandler::CharacterCreate( L2GamePacket *pack )
{
	pack->getPacketType();
	const wchar_t *name = pack->readUnicodeStringPtr(); // _name = readS();
	int race = pack->readD();                           // _race = readD();
	int sex = pack->readD();                            // _sex = readD();
	int classId = pack->readD();                        // _classId = readD();
	pack->readD(); //_int = readD();
	pack->readD(); //_str = readD();
	pack->readD(); //_con = readD();
	pack->readD(); //_men = readD();
	pack->readD(); //_dex = readD();
	pack->readD(); //_wit = readD();
	int hairStyle = pack->readD(); //_hairStyle = readD();
	int hairColor = pack->readD(); //_hairColor = readD();
	int face      = pack->readD(); //_face = readD();

	// CharacterCreate
	GameServer *gs = GameServer::getInstance();

	int name_len = wcslen( name );
	if( name_len < 1  || name_len > 16 )
	{
		if( gs->getConfig()->Debug )
			LogWarning( L"%s: Character Creation Failure: Character name [%s] is invalid. "
				L"Message generated: Your title cannot exceed 16 characters in length. Please try again.",
				m_cl->toString(), name );
		return ServerPackets::CharCreateFail( m_cl, L2Game_CharCreateFail::REASON_16_ENG_CHARS );
	}

	// alphanumeric name
	if( !Utils_isValidCharName( name ) )
	{
		if( gs->getConfig()->Debug )
			LogWarning( L"%s: Character Creation Failure: Character name [%s] is invalid. "
				L"Message generated: Incorrect name. Please try again.", m_cl->toString(), name );
		return ServerPackets::CharCreateFail( m_cl, L2Game_CharCreateFail::REASON_INCORRECT_NAME );
	}

	bool create_ok = false;

	CharNameTable::getInstance()->LockDB();
	if( !CharNameTable::getInstance()->doesCharNameExist( name ) )
	{
		// check maximum char count
		int current_char_count = CharNameTable::getInstance()->getAccountCharCount( m_cl->getAccount() );
		// TODO: CharacterCreate maximum char count may be in GS config?
		if( current_char_count >= 7 )
		{
			if( gs->getConfig()->Debug ) LogWarning( L"%s: Max number of characters reached. Creation failed.", m_cl->toString() );
			CharNameTable::getInstance()->UnlockDB();
			return ServerPackets::CharCreateFail( m_cl, L2Game_CharCreateFail::REASON_TOO_MANY_CHARACTERS );
		}

		// get class template
		const L2PlayerTemplate *tmpl = CharTemplateTable::getTemplate( classId );
		if( !tmpl )
		{
			LogError( L"%s: CharacterCreate: template is NULL!", m_cl->toString() );
			CharNameTable::getInstance()->UnlockDB();
			return ServerPackets::CharCreateFail( m_cl, L2Game_CharCreateFail::REASON_CREATION_FAILED );
		}
		// base class must be 1 lvl
		if( tmpl->classBaseLevel > 1 )
		{
			LogError( L"%s: CharacterCreate: template base level must be 1, but: %d!",
				m_cl->toString(), tmpl->classBaseLevel );
			CharNameTable::getInstance()->UnlockDB();
			return ServerPackets::CharCreateFail( m_cl, L2Game_CharCreateFail::REASON_CREATION_FAILED );
		}

		// generate new objectId for char
		unsigned int oid = gs->getIdFactory()->getNextObjectId();

		// get race by class id
		const ClassId *class_id = ClassIdTree::getInstance()->getClassId( classId );
		assert( class_id != NULL );
		int race_real = (int)class_id->getRace();
		if( race_real != race )
			LogWarning( L"%s: possible hacker: invalid race in CharacterCreate (%d)!=(%d)",
				m_cl->toString(), race, race_real );

		// insert new character into DB
		MysqlQuery q;
		q.create(
			L"INSERT INTO characters ("
			L"account_name,charId,char_name,level,maxHp,curHp,maxCp,curCp,maxMp,curMp,"
			L"x,y,z,heading,"
			L"face,hairStyle,hairColor,sex,exp,sp,karma,fame,pvpkills,pkkills,clanid,race,classid,"
			L"deletetime,cancraft,title,accesslevel,online,isin7sdungeon,clan_privs,wantspeace,base_class,"
			L"newbie,nobless,power_grade,last_recom_date) values ("
			L"'%s',%u,'%s',1, %d,%d,%d,%d,%d,%d, " // acc,oid,charName, ...
			L"%d,%d,%d,0, " // x,y,z,heading
			L"%d,%d,%d, %d,0,0,0,0,0,0,0,%d,%d, " // face,hs,hc, ..
			L"0,0,'',0,0,0,0,0,%d, " // deltime, ...
			L"1,0,0,%u)", /////////////////////////////////////////////////////////////////////////
			m_cl->getAccount(), oid, name, (int)(tmpl->baseHpMax), (int)(tmpl->baseHpMax),
				(int)(tmpl->baseCpMax), (int)(tmpl->baseCpMax), (int)(tmpl->baseMpMax), (int)(tmpl->baseMpMax),
			tmpl->spawnX, tmpl->spawnY, tmpl->spawnZ,
			face, hairStyle, hairColor, sex, race_real, classId, classId, (unsigned int)time( NULL )
		);
		MysqlConnection *con = gs->getDBConnection();
		if( con->executeQuery( q ) )
		{
			create_ok = true;
		}
		else
		{
			LogError( L"%s: CharacterCreate: cannot create: MySQL error: %s\n", m_cl->toString(), con->getErrorStr() );
		}
		q.clear();

		// add shortcuts
		//(int slotId, int pageId, int shortcutType, int shortcutId, int shortcutLevel, int characterType)
		// add attack shortcut  L2ShortCut(0, 0, 3, 2, 0, 1);
		q.create( L"INSERT INTO character_shortcuts (charId,slot,page,type,shortcut_id,level,class_index) VALUES ('%u',0,0,3,2,0,1)", oid );
		if( !con->executeQuery( q ) ) LogError( L"CharacterCreate: shortcuts: MySQL: %s", con->getErrorStr() );
		// add take shortcut L2ShortCut(3, 0, 3, 5, 0, 1);
		q.create( L"INSERT INTO character_shortcuts (charId,slot,page,type,shortcut_id,level,class_index) VALUES ('%u',3,0,3,5,0,1)", oid );
		if( !con->executeQuery( q ) ) LogError( L"CharacterCreate: shortcuts: MySQL: %s", con->getErrorStr() );
		// add sit shortcut L2ShortCut(10, 0, 3, 0, 0, 1);
		q.create( L"INSERT INTO character_shortcuts (charId,slot,page,type,shortcut_id,level,class_index) VALUES ('%u',10,0,3,0,0,1)", oid );
		if( !con->executeQuery( q ) ) LogError( L"CharacterCreate: shortcuts: MySQL: %s", con->getErrorStr() );

		// add start items
		const std::list<L2PlayerTemplate::PcTemplateItem> itemsList = tmpl->getItems();
		std::list<L2PlayerTemplate::PcTemplateItem>::const_iterator itemsListIter = itemsList.begin();
		while( itemsListIter != itemsList.end() )
		{
			L2PlayerTemplate::PcTemplateItem item = (*itemsListIter);
			const L2ItemTemplate *item_tmpl = ItemTable::getInstance()->getTemplate( item.getItemId() );
			if( !item_tmpl )
			{
				LogError( L"CharacterCreate: cannot add starter item: template NULL (%d)", item.getItemId() );
				continue;
			}
			//
			q.clear();
			unsigned int item_oid = gs->getIdFactory()->getNextObjectId();
			if( item.isEquipped() )
			{
				// location set to paperdoll
				q.create( L"INSERT INTO items (owner_id,item_id,count,loc,loc_data,enchant_level,"
					L"object_id,custom_type1,custom_type2,mana_left,time) VALUES "
					L"('%u','%d','%d','PAPERDOLL','%d',0,'%u','%d','%d','%d','%d')",
					oid, item.getItemId(), item.getAmount(), (int)item_tmpl->getBodyPart(),
					item_oid, item_tmpl->getType1(), item_tmpl->getType2(),
					item_tmpl->getDuration(), item_tmpl->getTime() );
			}
			else
			{
				// location set to inventory
				q.create( L"INSERT INTO items (owner_id,item_id,count,loc,loc_data,enchant_level,"
					L"object_id,custom_type1,custom_type2,mana_left,time) VALUES "
					L"('%u','%d','%d','INVENTORY','%d',0,'%u','%d','%d','%d','%d')",
					oid, item.getItemId(), item.getAmount(), (int)item_tmpl->getBodyPart(),
					item_oid, item_tmpl->getType1(), item_tmpl->getType2(),
					item_tmpl->getDuration(), item_tmpl->getTime() );
			}
			if( !con->executeQuery( q ) )
				LogError( L"CharacterCreate: cannot add starter item: MySQL error: %s", con->getErrorStr() );
			// add tutbook shortcut
			if( item.getItemId() == 5588 ) // CharacterCreate add Tutorial Guide...
			{
				// L2ShortCut(11, 0, 1, item.getObjectId(), 0, 1);
				q.create( L"INSERT INTO character_shortcuts (charId,slot,page,type,shortcut_id,level,class_index) "
					L"VALUES ('%u',11,0,1,'%u',0,1)", oid, item_oid );
				if( !con->executeQuery( q ) )
					LogError( L"CharacterCreate add Tutorial Guide: MySQL: %s", con->getErrorStr() );
			}
			//
			itemsListIter++;
		}
		q.clear();

		// TODO: CharacterCreate add starting skills (need SkillTable & SkillLearn)

		gs->releaseDBConnection( con );
	}
	else
	{
		if( gs->getConfig()->Debug )
			LogWarning( L"%s: Character Creation Failure: REASON_NAME_ALREADY_EXISTS.", m_cl->toString() );
		CharNameTable::getInstance()->UnlockDB();
		return ServerPackets::CharCreateFail( m_cl, L2Game_CharCreateFail::REASON_NAME_ALREADY_EXISTS );
	}
	CharNameTable::getInstance()->UnlockDB();

	if( !create_ok )
	{
		return NULL;
	}

	return ServerPackets::CharCreateOK( m_cl );
}

/*
private static final String INSERT_CHARACTER = "INSERT INTO characters (account_name,charId,char_name,level,maxHp,curHp,maxCp,curCp,maxMp,curMp,face,hairStyle,hairColor,sex,exp,sp,karma,fame,pvpkills,pkkills,clanid,race,classid,deletetime,cancraft,title,accesslevel,online,isin7sdungeon,clan_privs,wantspeace,base_class,newbie,nobless,power_grade,last_recom_date) values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
private static final String UPDATE_CHARACTER = "UPDATE characters SET level=?,maxHp=?,curHp=?,maxCp=?,curCp=?,maxMp=?,curMp=?,face=?,hairStyle=?,hairColor=?,sex=?,heading=?,x=?,y=?,z=?,exp=?,expBeforeDeath=?,sp=?,karma=?,fame=?,pvpkills=?,pkkills=?,rec_have=?,rec_left=?,clanid=?,race=?,classid=?,deletetime=?,title=?,accesslevel=?,online=?,isin7sdungeon=?,clan_privs=?,wantspeace=?,base_class=?,onlinetime=?,punish_level=?,punish_timer=?,newbie=?,nobless=?,power_grade=?,subpledge=?,last_recom_date=?,lvl_joined_academy=?,apprentice=?,sponsor=?,varka_ketra_ally=?,clan_join_expiry_time=?,clan_create_expiry_time=?,char_name=?,death_penalty_level=?,bookmarkslot=?,vitality_points=? WHERE charId=?";
private static final String RESTORE_CHARACTER = "SELECT account_name, charId, char_name, level, maxHp, curHp, maxCp, curCp, maxMp, curMp, face, hairStyle, hairColor, sex, heading, x, y, z, exp, expBeforeDeath, sp, karma, fame, pvpkills, pkkills, clanid, race, classid, deletetime, cancraft, title, rec_have, rec_left, accesslevel, online, char_slot, lastAccess, clan_privs, wantspeace, base_class, onlinetime, isin7sdungeon, punish_level, punish_timer, newbie, nobless, power_grade, subpledge, last_recom_date, lvl_joined_academy, apprentice, sponsor, varka_ketra_ally,clan_join_expiry_time,clan_create_expiry_time,death_penalty_level,bookmarkslot,vitality_points FROM characters WHERE charId=?";
*/
