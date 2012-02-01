#ifndef H_GAME_CLIENT_COUNTERS
#define H_GAME_CLIENT_COUNTERS

class GameClient_NetStats
{
public:
	double timePassedSecs;
	double avgSendSpeed;
	double avgRecvSpeed;
	double avgSendPacketsPerSec;
	double avgRecvPacketsPerSec;
	double avgSendPacketSize;
	double avgRecvPacketSize;
};

class GameClientCounters
{
public:
	GameClientCounters();
	GameClientCounters( const GameClientCounters& other );
	GameClientCounters& operator=( const GameClientCounters& other );
public:
	void resetCounters();
	void startTimer();
public:
	void addSentPacket( unsigned long long int packetLen );
	void addRcvdPacket( unsigned long long int packetLen );
public:
	void calcNowStats( GameClient_NetStats *s );
public:
	unsigned int startTime;
	unsigned long long int ullBytesSent;
	unsigned long long int ullBytesRcvd;
	unsigned long long int ullPacketsSent;
	unsigned long long int ullPacketsRcvd;
	unsigned long long int ullStartingExp;
};

#endif
