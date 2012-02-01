#include "stdafx.h"
#include "../L2_versions.h"
#include "L2PacketNames.h"

// prototypes for internal functions
void L2PacketNames_InitServer();
void L2PacketNames_InitClient();

void L2Packets_GetL2PacketName_fromServer(
	unsigned char   opcode1,
	unsigned short  opcode2,
	char           *outPacketName,
	unsigned int    cchMax
);

void L2Packets_GetL2PacketName_fromClient(
	unsigned char   opcode1,
	unsigned short  opcode2,
	char           *outPacketName,
	unsigned int    cchMax
);

// Gracia Final
void L2PacketNames_InitClient_t23();
void L2PacketNames_InitServer_t23();

void L2Packets_GetL2PacketName_fromClient_t23(
	unsigned char   opcode1,
	unsigned short  opcode2,
	unsigned short  opcode3,
	char           *outPacketName,
	unsigned int cchMax );

void L2Packets_GetL2PacketName_fromServer_t23(
	unsigned char opcode1,
	unsigned short opcode2,
	char *outPacketName,
	unsigned int cchMax );

void L2PacketNames_Init()
{
	L2PacketNames_InitServer();
	L2PacketNames_InitClient();
	L2PacketNames_InitClient_t23();
	L2PacketNames_InitServer_t23();
}

void L2Packets_GetL2PacketName(
	L2_VERSION       l2_version,
	bool             fromServer,
	unsigned char    opcode1,
	unsigned short   opcode2,
	unsigned short   opcode3,
	char            *outPacketName,
	unsigned int     cchMax
)
{
	switch( l2_version )
	{
	case L2_VERSION_T1:
	case L2_VERSION_T15:
	case L2_VERSION_T2:
	case L2_VERSION_T22:
		{
			// Kamael -  Gracia Part 2 have same packets (only adds, no changes/deletes)
			if( fromServer )
				L2Packets_GetL2PacketName_fromServer( opcode1, opcode2, outPacketName, cchMax );
			else
				L2Packets_GetL2PacketName_fromClient( opcode1, opcode2, outPacketName, cchMax );
		} break;
	// Gracia Final has too many differences...
	case L2_VERSION_T23:
	case L2_VERSION_T24: // for a while think that Epilogue has same opcodes as Gracia Final
		{
			if( fromServer )
				L2Packets_GetL2PacketName_fromServer_t23( opcode1, opcode2, outPacketName, cchMax );
			else
				L2Packets_GetL2PacketName_fromClient_t23( opcode1, opcode2, opcode3, outPacketName, cchMax );
		} break;
	}

	outPacketName[cchMax] = 0;
}

