#include "pch.h"
#include "GameClientNetStats.h"

GameClientNetStats::GameClientNetStats()
{
	clear();
}

void GameClientNetStats::clear()
{
	ullPacketsSent = ullBytesSent = ullPacketsRcvd = ullBytesRcvd = 0;
}

void GameClientNetStats::addSentPacket( unsigned long long len )
{
	ullPacketsSent++;
	ullBytesSent += len;
}

void GameClientNetStats::addRcvdPacket( unsigned long long len )
{
	ullPacketsRcvd++;
	ullBytesRcvd += len;
}

