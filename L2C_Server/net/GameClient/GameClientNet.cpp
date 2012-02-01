#include "pch.h"
#include "Log.h"
#include "l2c_utils.h"
#include "GameClient.h"
#include "GS.h"
#include "../ServerPackets.h"
#include "utils/Exception.h"

bool GameClient::sendPacket( L2GamePacket *pack, bool deleteAfterSend /*= false*/ )
{
	if( !pack ) return false;
	if( m_sock == INVALID_SOCKET ) return false;
	//
	int r = 0;
	unsigned int sentLen = 0;
	unsigned int pack_size = pack->getPacketSize();
	bool ret = true;
	// encode xor if xor enabled
	if( m_xor_enabled )
	{
		// KeyPacket is first packet sent to client by server.
		// it is never encrypted
		if( m_netStats.ullPacketsSent > 0 )
			pack->encodeXOR( m_xor_key_send );
	}
	// send packet (lock before send, release lock after send)
	m_cs_send.Lock();
	r = L2PacketSend( m_sock, pack, 2000, &sentLen );
	m_cs_send.Unlock();
	// delete pack, if set to do so
	if( deleteAfterSend ) delete pack;
	// validate send OK
	if( r<=0 || (sentLen != pack_size) ) // some error
	{
		ret = false;
		throw Exception( "%S: send error: sent %u, retval %d", toString(), sentLen, r );
	}
	// count stats
	m_netStats.addSentPacket( sentLen );
	//
	return ret;
}

void GameClient::enable_XOR_crypt( bool bEnable, unsigned char *initial_key )
{
	m_xor_enabled = bEnable;
	if( bEnable )
	{
		memcpy( m_xor_key_recv, initial_key, sizeof(m_xor_key_recv) );
		memcpy( m_xor_key_send, initial_key, sizeof(m_xor_key_send) );
	}
	else
	{
		memset( m_xor_key_recv, 0, sizeof(m_xor_key_recv) );
		memset( m_xor_key_send, 0, sizeof(m_xor_key_send) );
	}
}

void GameClient::notifySessionKeysOK()
{
#ifdef _DEBUG
	LogDebug( L"GameClient::notifySessionKeysOK()" );
#endif
	m_state = CLIENT_STATE_AUTHED;
	swprintf( m_tostring, 128, L"Client Acc: %s [IP %s:%d]", m_account, m_wip, m_port );
	sendPacket( ServerPackets::CharacterSelectionInfo( this ), true );
}
