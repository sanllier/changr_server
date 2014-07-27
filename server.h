#pragma once

#include "threadholder.h"

#include <WinSock2.h>

// #define MUTEXED_ASSIGN(mutex, a, b) { mutex.lock(); a = b; mutex.unlock(); }

namespace server
{

static const int MAX_SERVERS = 32;
static const int HI_VER = 2;
static const int LO_VER = 2;
static const int QUEUE_SIZE = 100;

class Server
{
public:
	Server( u_short port );
	~Server(void);

	void listenGo( int queueSize = QUEUE_SIZE );
	void listenStop( void );
private:
	size_t recvData( SOCKET sock, void* buf, size_t bufLen );
	size_t sendData( SOCKET sock, const void* data, size_t dataLen );

	friend void listenState( int queueSize, Server* obj );
	friend void clientRoutine( SOCKET soc, Server* obj );
private:
	static int servers;

	SOCKET listenSock;
	bool isListen;
	// std::mutex isListenMutex;

	ThreadHolder listenThread;
	ThreadHolder clientThreads;
};

}
