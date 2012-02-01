#include "pch.h"
#include "Log.h"
#include "GameObject.h"

GameObject::GameObject( unsigned int objectId )
{
	m_objectId = objectId;
	m_x = m_y = m_z = m_reflection = 0;
	m_toString_buffer = NULL;
}

GameObject::GameObject( int x, int y, int z, int reflection, unsigned int objectId )
{
	m_x = x; m_y = y; m_z = z;
	m_reflection = reflection;
	m_objectId = objectId;
	m_toString_buffer = NULL;
}

GameObject::~GameObject()
{
	if( m_toString_buffer )
	{
		free( m_toString_buffer );
		m_toString_buffer = NULL;
	}
}

void GameObject::LockObject()
{
	m_lock.Lock();
}

void GameObject::LockRelease()
{
	m_lock.Unlock();
}

void GameObject::setXYZ( int x, int y, int z )
{
	m_x = x;
	m_y = y;
	m_z = z;
}

void GameObject::setReflection( int rId )
{
	m_reflection = rId;
}

wchar_t *GameObject::toString() const
{
	GameObject *non_const_this = const_cast<GameObject *>(this);
	if( non_const_this->m_toString_buffer == NULL )
		non_const_this->m_toString_buffer = (wchar_t *)malloc( 512 );
	if( non_const_this->m_toString_buffer )
		swprintf( non_const_this->m_toString_buffer, 255, L"ObjectID[%u] (%d,%d,%d,%d)", m_objectId, m_x, m_y, m_z, m_reflection );
	return non_const_this->m_toString_buffer;
}
