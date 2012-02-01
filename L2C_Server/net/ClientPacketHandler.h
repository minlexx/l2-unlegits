#pragma once

class GameClient;

class ClientPacketHandler
{
public:
	ClientPacketHandler( GameClient *cl );
	~ClientPacketHandler();

public:
	void handlePacket( L2GamePacket *pack );

protected:
	L2GamePacket *ProtocolVersion( L2GamePacket *pack );
	L2GamePacket *AuthLogin( L2GamePacket *pack );
	L2GamePacket *NewCharacter( L2GamePacket *pack );
	L2GamePacket *CharacterCreate( L2GamePacket *pack );
	L2GamePacket *RequestGotoLobby( L2GamePacket *pack );
	L2GamePacket *Logout( L2GamePacket *pack );
	L2GamePacket *CharacterSelect( L2GamePacket *pack );

protected:
	void invalidPacket( L2GamePacket *p, unsigned int opcode, unsigned int opcode2 );

protected:
	GameClient *m_cl;
};
