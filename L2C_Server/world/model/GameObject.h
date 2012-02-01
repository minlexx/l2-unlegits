#pragma once
#include "l2c_utils.h"

class GameObject
{
public:
	GameObject( unsigned int objectId );
	GameObject( int x, int y, int z, int reflection, unsigned int objectId );
	virtual ~GameObject();

public:
	unsigned int getObjectId() const { return m_objectId; }
	int getX() const { return m_x; }
	int getY() const { return m_y; }
	int getZ() const { return m_z; }
	int getReflection() const { return m_reflection; }

public:
	void setXYZ( int x, int y, int z );
	void setReflection( int rId );

public:
	virtual void LockObject();
	virtual void LockRelease();
	virtual wchar_t *toString() const;

protected:
	unsigned int          m_objectId;
	int                   m_x;
	int                   m_y;
	int                   m_z;
	int                   m_reflection;
	wchar_t              *m_toString_buffer;
	CriticalSection       m_lock;
};
