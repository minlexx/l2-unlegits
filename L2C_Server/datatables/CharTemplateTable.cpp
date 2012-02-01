#include "pch.h"
#include "Log.h"
#include "CharTemplateTable.h"
#include "world/model/base/ClassIdTree.h"
#include "ItemTable.h"
#include "GS.h"

CharTemplateTable *CharTemplateTable::s_instance = NULL;
int CharTemplateTable::s_refCount = 0;

CharTemplateTable *CharTemplateTable::getInstance()
{
	if( s_instance == NULL )
	{
		s_instance = new CharTemplateTable();
		s_refCount++;
	}
	return s_instance;
}

void CharTemplateTable::freeInstance()
{
	if( s_instance )
	{
		s_refCount--;
		if( s_refCount == 0 )
		{
			delete s_instance;
			s_instance = NULL;
		}
	}
}

CharTemplateTable::CharTemplateTable()
{
	m_templates.clear();
}

CharTemplateTable::~CharTemplateTable()
{
	std::map<int, L2PlayerTemplate *>::const_iterator iter = m_templates.begin();
	while( iter != m_templates.end() )
	{
		L2PlayerTemplate *tmpl = iter->second;
		if( tmpl ) delete tmpl;
		iter++;
	}
	m_templates.clear();
}

void CharTemplateTable::load()
{
	GameServer *gs = GameServer::getInstance();
	MysqlConnection *con = gs->getDBConnection();
	//
	MysqlQuery q;
	q.create( L"SELECT * FROM class_list, char_templates, lvlupgain "
		L"WHERE class_list.id = char_templates.classId AND class_list.id = lvlupgain.classId "
		L"ORDER BY class_list.id" );
	if( con->executeQuery( q ) )
	{
		while( q.getNextRow() )
		{
			StatsSet set;
			int classId;
			// l2 player template vars
			classId = q.getFieldInt( "id" );
			set.setInt( "classId", classId );
			set.setInt( "raceId", q.getFieldInt( "raceId" ) );
			set.setInt( "spawnX", q.getFieldInt( "x" ) );
			set.setInt( "spawnY", q.getFieldInt( "y" ) );
			set.setInt( "spawnZ", q.getFieldInt( "z" ) );
			set.setInt( "classBaseLevel", q.getFieldInt( "class_lvl" ) );
			set.setDouble( "lvlHpAdd", q.getFieldDouble( "defaultHpAdd" ) );
			set.setDouble( "lvlHpMod", q.getFieldDouble( "defaultHpMod" ) );
			set.setDouble( "lvlCpAdd", q.getFieldDouble( "defaultCpAdd" ) );
			set.setDouble( "lvlCpMod", q.getFieldDouble( "defaultCpMod" ) );
			set.setDouble( "lvlMpAdd", q.getFieldDouble( "defaultMpAdd" ) );
			set.setDouble( "lvlMpMod", q.getFieldDouble( "defaultMpMod" ) );
			set.setDouble( "baseHpMax", q.getFieldDouble( "defaultHpBase" ) );
			set.setDouble( "baseMpMax", q.getFieldDouble( "defaultMpBase" ) );
			set.setDouble( "baseCpMax", q.getFieldDouble( "defaultCpBase" ) );
			set.setDouble( "baseHpReg", 1.5 ); // O_o constants...?
			set.setDouble( "baseMpReg", 0.9 );
			// l2 char template vars
			set.setInt( "baseSTR", q.getFieldInt( "STR" ) );
			set.setInt( "baseCON", q.getFieldInt( "CON" ) );
			set.setInt( "baseDEX", q.getFieldInt( "DEX" ) );
			set.setInt( "baseINT", q.getFieldInt( "_INT" ) );
			set.setInt( "baseWIT", q.getFieldInt( "WIT" ) );
			set.setInt( "baseMEN", q.getFieldInt( "MEN" ) );
			//
			set.setInt( "basePAtk", q.getFieldInt( "P_ATK" ) );
			set.setInt( "basePDef", q.getFieldInt( "P_DEF" ) );
			set.setInt( "baseMAtk", q.getFieldInt( "M_ATK" ) );
			set.setInt( "baseMDef", q.getFieldInt( "M_DEF" ) );
			set.setInt( "basePAtkSpd", q.getFieldInt( "P_SPD" ) );
			set.setInt( "baseMAtkSpd", q.getFieldInt( "M_SPD" ) );
			set.setInt( "baseCritRate", q.getFieldInt( "CRITICAL" ) );
			set.setInt( "baseRunSpd", q.getFieldInt( "MOVE_SPD" ) );
			set.setInt( "baseWalkSpd", 0 );
			set.setInt( "baseShldDef", 0 );
			set.setInt( "baseShldRate", 0 );
			set.setInt( "baseAtkRange", 40 );
			set.setInt( "collision_radius", (int)q.getFieldDouble( "M_COL_R" ) );
			set.setInt( "collision_height", (int)q.getFieldDouble( "M_COL_H" ) );
			// TODO: MISSED char_templates vars
			set.setInt( "baseAccuracy", q.getFieldInt( "ACC" ) );
			set.setInt( "baseEvasion", q.getFieldInt( "EVASION" ) );
			set.setInt( "baseLoad", q.getFieldInt( "_LOAD" ) );
			set.setBool( "canCraft", q.getFieldBool( "canCraft" ) );
			//
			L2PlayerTemplate *tmpl = new L2PlayerTemplate( set );
			m_templates[classId] = tmpl;
		}
		Log( L"CharTemplateTable: loaded %u character templates.", m_templates.size() );
	}
	else
	{
		LogError( L"CharTemplateTable::load(): Mysql error: %s", con->getErrorStr() );
	}

	// load starting items
	ItemTable *itemTable = ItemTable::getInstance();
	q.create( L"SELECT classId, itemId, amount, equipped FROM char_creation_items" );
	if( con->executeQuery( q ) )
	{
		int classId, itemId, amount;
		bool equipped;
		while( q.getNextRow() )
		{
			classId = q.getFieldInt( "classId" );
			itemId = q.getFieldInt( "itemId" );
			amount = q.getFieldInt( "amount" );
			equipped = false;
			if( strcmp( q.getFieldStr( "equipped" ), "true" ) == 0 ) equipped = true;
			// check for item existance
			if( itemTable->getTemplate( itemId ) != NULL )
			{
				if( classId == -1 )
				{
					this->addItemToAllTemplates( itemId, amount, equipped );
				}
				else
				{
					L2PlayerTemplate *pct = const_cast<L2PlayerTemplate *>(CharTemplateTable::getTemplate( classId ));
					if( pct != NULL )
						pct->addItem( itemId, amount, equipped );
					else
						LogWarning( L"CharTemplateTable: load: Entry for undefined class, classId: %d", classId );
				}
			}
			else
			{
				const wchar_t * className = L"(all)";
				if( classId >= 0 ) className = ClassIdTree::getInstance()->getClassId( classId )->getName();
				LogWarning( L"CharTemplateTable: load: No data for itemId: %d defined for classId %d (%s)",
					itemId, classId, className );
			}
		}
	}
	else
	{
		LogError( L"CharTemplateTable::load(): Mysql error: %s", con->getErrorStr() );
	}

	//
	gs->releaseDBConnection( con );
}

const wchar_t *CharTemplateTable::getClassNameById( int classId )
{
	ClassIdTree *cidtree = ClassIdTree::getInstance();
	const ClassId *cls = cidtree->getClassId( classId );
	if( !cls ) return NULL;
	return cls->getName();
}

const L2PlayerTemplate *CharTemplateTable::getTemplate( int classId )
{
	CharTemplateTable *ctt = CharTemplateTable::getInstance();
	std::map<int, L2PlayerTemplate *>::const_iterator iter;
	iter = ctt->m_templates.find( classId );
	if( iter == ctt->m_templates.end() ) return NULL;
	return iter->second;
}

void CharTemplateTable::addItemToAllTemplates( int itemId, int amount, bool equipped )
{
	std::map<int, L2PlayerTemplate *>::const_iterator iter;
	iter = m_templates.begin();
	while( iter != m_templates.end() )
	{
		L2PlayerTemplate *tmpl = iter->second;
		if( tmpl ) tmpl->addItem( itemId, amount, equipped );
		iter++;
	}
}
