#pragma once
#include "../GameObject.h"

class GameCharacter: public GameObject
{
public:
	GameCharacter( unsigned int objectId );
	virtual ~GameCharacter();

public:
	void setName( const wchar_t *name );
	void setLevel( int level );

public:
	const wchar_t *getName() const;
	int            getLevel() const { return m_level; }

public:
	virtual wchar_t *toString() const;

protected:
	wchar_t       *m_name;
	int            m_level;
};
