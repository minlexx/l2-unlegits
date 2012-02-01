#pragma once

class GameClientNetStats
{
public:
	GameClientNetStats();
	void clear();
	void addSentPacket( unsigned long long len );
	void addRcvdPacket( unsigned long long len );
public:
	unsigned long long ullPacketsSent;
	unsigned long long ullBytesSent;
	unsigned long long ullPacketsRcvd;
	unsigned long long ullBytesRcvd;
};
