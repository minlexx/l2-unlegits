#include "stdafx.h"
#include "ClanTreeNode.h"

ClanTreeNode::ClanTreeNode(): BinTreeNode()
{
	m_clanID = 0;
	m_clanName[0] = m_allyName[0] = 0;
	//
	//wcscpy( m_str_null, L"NULL" );
}

ClanTreeNode::ClanTreeNode( unsigned int clanID,
		const wchar_t *clanName,
		const wchar_t *allyName ): BinTreeNode()
{
	key = clanID;
	m_clanID = clanID;
	setClanName( clanName );
	setAllyName( allyName );
	//
	//wcscpy( m_str_null, L"NULL" );
}

ClanTreeNode::~ClanTreeNode()
{
	m_clanID = 0;
	m_clanName[0] = m_allyName[0] = 0;
}

void ClanTreeNode::setClanName( const wchar_t *clanName )
{
	m_clanName[0] = 0;
	if( clanName ) wcsncpy( m_clanName, clanName, 127 );
	m_clanName[127] = 0;
}

void ClanTreeNode::setAllyName( const wchar_t *allyName )
{
	m_allyName[0] = 0;
	if( allyName ) wcsncpy( m_allyName, allyName, 127 );
	m_allyName[127] = 0;
}

const wchar_t *ClanTreeNode::clanName() const
{
	((class ClanTreeNode *)this)->m_clanName[127] = 0; // for safe
	return m_clanName;
}

const wchar_t *ClanTreeNode::allyName() const
{
	((class ClanTreeNode *)this)->m_allyName[127] = 0; // for safe
	return m_allyName;
}
