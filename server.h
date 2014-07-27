#pragma once

#include "threadholder.h"

#include <WinSock2.h>

namespace server
{

class Server
{
public:
	Server( u_short port );
	~Server(void);

	void listenGo( void );
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

	ThreadHolder listenThread;
	ThreadHolder clientThreads;
};

}
