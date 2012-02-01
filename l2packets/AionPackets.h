#ifndef AIONPACKETS_H_
#define AIONPACKETS_H_

/** \file AionPackets.h
* Main include file to include all library files related to Aion.
*/

// detect OS
#if defined(_WIN32) || defined(WIN32) || defined(WINVER) || defined(_WIN32_WINNT)
#undef L2PACKETS_WINDOWS
#define L2PACKETS_WINDOWS
#else
#undef L2PACKETS_LINUX
#define L2PACKETS_LINUX
#endif

// base of library
#include "L2_versions.h"
#include "xcpt/L2Packets_xcpt.h"
#include "base/ByteArray.h"
#include "base/L2BasePacket.h"

// network
#include "net_io/L2Packet_NetLayer.h"
#include "net_io/L2PacketReceiver.h"
#include "net_io/L2PacketSender.h"

//#include "login/AionLoginPacket.h"
#include "game/AionGamePacket.h"

#endif /* AIONPACKETS_H_ */
