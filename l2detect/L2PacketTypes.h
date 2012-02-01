#ifndef H_L2_PACKET_TYPES
#define H_L2_PACKET_TYPES


void L2PacketTypes_LogClient
(
	L2_VERSION      l2_version,
	int             state,
	unsigned char   ptype,
	unsigned short  ptype2,
	unsigned short  ptype3
);

void L2PacketTypes_LogServer
(
	L2_VERSION      l2_version,
	int             state,
	unsigned char   ptype,
	unsigned short  ptype2,
	unsigned short  ptype3
);

#endif
