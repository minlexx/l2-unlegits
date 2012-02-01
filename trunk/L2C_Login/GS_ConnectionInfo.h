#pragma once

struct GS_ConnectionInfo
{
	bool             flagBreak;
	char             ip[32];
	unsigned int     sock;
	GameServerTable *gst;
	RSA             *rsaKey;
	unsigned char    bfKey[64];
	unsigned int     bfKeyLen;
	GameServerInfo  *gsi;
};
