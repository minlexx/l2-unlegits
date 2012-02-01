#include "pch.h"
#include "GameNpc.h"

GameNpc::GameNpc( unsigned int objectId, int npcTemplateId ): GameCharacter( objectId )
{
	m_npcTemplateId = npcTemplateId;
}

GameNpc::~GameNpc()
{
}
