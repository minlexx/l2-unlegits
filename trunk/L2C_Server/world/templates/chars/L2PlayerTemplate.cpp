#include "pch.h"
#include "L2PlayerTemplate.h"

L2PlayerTemplate::L2PlayerTemplate( StatsSet& set ): L2CharTemplate( set )
{
	set.getInt( "classId", &iClassId );
	classId = ClassIdTree::getInstance()->getClassId( iClassId );
	set.getInt( "raceId", (int *)&race );
	
	set.getInt( "spawnX", &spawnX );
	set.getInt( "spawnY", &spawnY );
	set.getInt( "spawnZ", &spawnZ );
	
	set.getInt( "classBaseLevel", &classBaseLevel );

	set.getDouble( "lvlHpAdd", &lvlHpAdd );
	set.getDouble( "lvlHpMod", &lvlHpMod );
	set.getDouble( "lvlCpAdd", &lvlCpAdd );
	set.getDouble( "lvlCpMod", &lvlCpMod );
	set.getDouble( "lvlMpAdd", &lvlMpAdd );
	set.getDouble( "lvlMpMod", &lvlMpMod );

	// missed loaded vars
	set.getInt( "baseLoad", &baseLoad, 62500 );
	set.getBool( "canCraft", &canCraft, false );
}

void L2PlayerTemplate::addItem( int itemId, int amount, bool equipped )
{
	PcTemplateItem it( itemId, amount, equipped );
	m_items.push_back( it );
}

const std::list<L2PlayerTemplate::PcTemplateItem>& L2PlayerTemplate::getItems() const
{
	return m_items;
}




L2PlayerTemplate::PcTemplateItem::PcTemplateItem()
{
	m_itemId = 0;
	m_amount = 0;
	m_equipped = false;
}

L2PlayerTemplate::PcTemplateItem::PcTemplateItem( int itemId, int amount, bool equipped )
{
	m_itemId = itemId;
	m_amount = amount;
	m_equipped = equipped;
}

L2PlayerTemplate::PcTemplateItem::PcTemplateItem( const L2PlayerTemplate::PcTemplateItem& other )
{
	this->operator=( other );
}

const L2PlayerTemplate::PcTemplateItem& L2PlayerTemplate::PcTemplateItem::operator=( const L2PlayerTemplate::PcTemplateItem& other )
{
	if( this == &other ) return (*this);
	m_itemId = other.m_itemId;
	m_amount = other.m_amount;
	m_equipped = other.m_equipped;
	return (*this);
}

int L2PlayerTemplate::PcTemplateItem::getItemId() const
{
	return m_itemId;
}

int L2PlayerTemplate::PcTemplateItem::getAmount() const
{
	return m_amount;
}

bool L2PlayerTemplate::PcTemplateItem::isEquipped() const
{
	return m_equipped;
}
