#include "pch.h"
#include "ClassId.h"
#include "ClassIdTree.h"

ClassId::ClassId( int id, const wchar_t *name, bool isMage, bool isSummoner, Race race, int parentId )
{
	m_id = id; m_isMage = isMage; m_isSummoner = isSummoner;
	m_race = race; m_parentId = parentId;
	m_name[0] = 0;
	if( name )
	{
		wcsncpy( m_name, name, sizeof(m_name)/sizeof(m_name[0]) );
		m_name[sizeof(m_name)/sizeof(m_name[0])-1] = 0;
	}
}

int  ClassId::getId() const       { return m_id; }
const wchar_t *ClassId::getName() const { return m_name; }
bool ClassId::isMage() const      { return m_isMage; }
bool ClassId::isSummoner() const  { return m_isSummoner; }
Race ClassId::getRace() const     { return m_race; }
int  ClassId::getParentId() const { return m_parentId; }

int ClassId::level() const
{
	if( m_id != 0x87 && m_parentId == -1 ) return 0; // every non-Inspector without a parent is 0th prof
	if( m_id == 0x87 ) return 2; // Inspector, has no parent, but is 2nd class
	if( m_id == 0x88 ) return 3; // Judicator
	return 1 + ClassIdTree::getInstance()->getParentClassId( m_id )->level();
}

bool ClassId::isChildOf( int classId ) const
{
	if( m_parentId == -1 ) return false; // no parents...
	if( m_parentId == classId ) return true; // direct child of
	// maybe is parent's child?
	return ClassIdTree::getInstance()->getClassId( m_parentId )->isChildOf( classId );
}

bool ClassId::isEqualOrChildOf( int classId ) const
{
	return (m_id == classId) || isChildOf( classId );
}
