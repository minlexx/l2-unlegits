#pragma once
#include "GameCharacter.h"
class GameClient; // forward decl

class GamePlayer: public GameCharacter
{
public:
	GamePlayer( GameClient *clnt, unsigned int objectId );
	virtual ~GamePlayer();

public:
	GameClient *getGameClient();

protected:
	GameClient *m_gameClient;
};
