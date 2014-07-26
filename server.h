#pragma once

#include <WinSock2.h>
#include <mutex>
#include <thread>

#define MUTEXED_ASSIGN(mutex, a, b) { mutex.lock(); a = b; mutex.unlock(); }

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

	std::thread* listenGo( int queue_size = QUEUE_SIZE );
	void listenStop( void );
private:
	void listenState( void );
private:
	static int servers;

	SOCKET listenSock;

	bool isListen;
	std::mutex isListenMutex;

	std::thread* listenThread;
};

}
