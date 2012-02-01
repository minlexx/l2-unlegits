#pragma once
#include "GameCharacter.h"

class GameNpc: public GameCharacter
{
public:
	GameNpc( unsigned int objectId, int npcTemplateId );
	virtual ~GameNpc();

public:
	int getNpcTemplateId() const { return m_npcTemplateId; }

public:
	void setNpcTemplateId( int tmplId ) { m_npcTemplateId = tmplId; }

protected:
	int m_npcTemplateId;
};
