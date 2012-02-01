#include "pch.h"
#include "Log.h"
#include "../../../net/GameClient/GameClient.h"
#include "GamePlayer.h"

GamePlayer::GamePlayer( GameClient *clnt, unsigned int objectId ): GameCharacter( objectId )
{
	m_gameClient = clnt;
}

GamePlayer::~GamePlayer()
{
	m_gameClient = NULL;
}

GameClient *GamePlayer::getGameClient()
{
	return m_gameClient;
}
