#include "stdafx.h"
#include "GameClientCounters.h"

GameClientCounters::GameClientCounters()
{
	resetCounters();
}

GameClientCounters::GameClientCounters( const GameClientCounters& other )
{
	this->operator=( other );
}

GameClientCounters& GameClientCounters::operator=( const GameClientCounters& other )
{
	this->ullBytesRcvd   = other.ullBytesRcvd;
	this->ullBytesSent   = other.ullBytesSent;
	this->ullPacketsRcvd = other.ullPacketsRcvd;
	this->ullPacketsSent = other.ullPacketsSent;
	this->startTime      = other.startTime;
	this->ullStartingExp = other.ullStartingExp;
	return (*this);
}

void GameClientCounters::resetCounters()
{
	this->ullBytesRcvd = this->ullBytesSent = 
		this->ullPacketsRcvd = this->ullPacketsSent = 0;
	this->startTime = 0;
	this->ullStartingExp = 0;
}

void GameClientCounters::startTimer()
{
	this->startTime = (unsigned)time( NULL );
}

void GameClientCounters::addSentPacket( unsigned long long int packetLen )
{
	this->ullPacketsSent++;
	this->ullBytesSent += packetLen;
}

void GameClientCounters::addRcvdPacket( unsigned long long int packetLen )
{
	this->ullPacketsRcvd++;
	this->ullBytesRcvd += packetLen;
}

void GameClientCounters::calcNowStats( GameClient_NetStats *s )
{
	unsigned int timeNow = (unsigned)time(NULL);
	s->timePassedSecs = ((double)(timeNow - startTime));
	s->avgSendSpeed = ((double)ullBytesSent) / (s->timePassedSecs);
	s->avgRecvSpeed = ((double)ullBytesRcvd) / (s->timePassedSecs);
	s->avgSendPacketsPerSec = ((double)ullPacketsSent) / (s->timePassedSecs);
	s->avgRecvPacketsPerSec = ((double)ullPacketsRcvd) / (s->timePassedSecs);
	s->avgRecvPacketSize = ((double)ullBytesRcvd) / ((double)ullPacketsRcvd);
	s->avgSendPacketSize = ((double)ullBytesSent) / ((double)ullPacketsSent);
}
