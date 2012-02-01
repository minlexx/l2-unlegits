#pragma once

class ServerPackets
{
public:
	static L2GamePacket *KeyPacket( GameClient *pl );
	static L2GamePacket *CharacterSelectionInfo( GameClient *pl );
	static L2GamePacket *NewCharacterSuccess( GameClient *pl );
	static L2GamePacket *CharCreateFail( GameClient *pl, int reason );
	static L2GamePacket *CharCreateOK( GameClient *pl );
};
