#include "pch.h"
#include "GameCharacter.h"

GameCharacter::GameCharacter( unsigned int objectId ): GameObject( objectId )
{
	m_name = NULL;
}

GameCharacter::~GameCharacter()
{
	if( m_name )
	{
		free( m_name );
		m_name = NULL;
	}
	GameObject::~GameObject();
}

void GameCharacter::setName( const wchar_t *name )
{
	if( m_name ) free( m_name );
	m_name = NULL;
	if( name ) m_name = _wcsdup( name );
}

void GameCharacter::setLevel( int level )
{
	m_level = level;
	if( m_level < 0 ) m_level = 0;
}

const wchar_t *GameCharacter::getName() const
{
	return (const wchar_t *)m_name;
}

wchar_t *GameCharacter::toString() const
{
	GameCharacter *nc_this = const_cast<GameCharacter *>(this);
	if( !nc_this->m_toString_buffer )
		nc_this->m_toString_buffer = (wchar_t *)malloc( 512 );
	if( nc_this->m_toString_buffer )
		swprintf( nc_this->m_toString_buffer, 255, L"%s lvl %d [oid %u]", m_name, m_level, m_objectId );
	return m_toString_buffer;
}
