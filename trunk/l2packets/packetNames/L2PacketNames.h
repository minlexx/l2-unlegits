#ifndef H_L2_PACKET_NAMES
#define H_L2_PACKET_NAMES

/** \file L2PacketNames.h
* Header to define functions to work with packet opcodes/packet names.
*/

/** Initializes strings */
void L2PacketNames_Init();

/** Get packet name (string) by packet opcode
* \param l2_version Lineage game version to get packet name for
* \param fromServer - must be true for Server->Client packet
* \param opcode1 - main packet opcode
* \param opcode2 - extended packet opcode. Used only for packets S->C FE and C->S D0
* \param opcode3 - extended packet opcode. Used only in Gracia Final for packets S->C FE:?? and C->S D0:51
* \param outPacketName - pointer to buffer to receive packet name
* \param cchMax - buffer size
* \return resulting string in outPacketName
*/
void L2Packets_GetL2PacketName(
	L2_VERSION       l2_version,
	bool             fromServer,
	unsigned char    opcode1,
	unsigned short   opcode2,
	unsigned short   opcode3,
	char            *outPacketName,
	unsigned int     cchMax
);

#endif
