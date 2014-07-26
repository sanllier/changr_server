#include "server.h"
#include "exception.h"
#include "log.h"

#include <string>

namespace server
{
// ---------------------------------------------

int Server::servers = 0;

// ---------------------------------------------

Server::Server( u_short port )
	:listenSock(0)
	,isListen(false)
	,listenThread(0)
{
	if ( servers < 0 )
		throw Exception( std::string( "Negative number of working servers." ), 
		                 std::string( "server.cpp : Server::Server(void)" ), 
						 Exception::ExcData( &servers, sizeof( servers ) ) );

	if ( servers < MAX_SERVERS )
	{
		const WORD requestedVersion = MAKEWORD( LO_VER, HI_VER );
        WSADATA wsaData;
		int errorCode = WSAStartup( requestedVersion, &wsaData );

		if ( errorCode )
		{
			throw Exception( std::string( "Error while init sockets." ), 
		                     std::string( "server.cpp : Server::Server(void)" ), 
							 Exception::ExcData( &errorCode, sizeof( errorCode ) ) );
		}
		else
		{
			if ( LOBYTE( wsaData.wVersion ) != LO_VER || HIBYTE( wsaData.wVersion ) != HI_VER )
				throw Exception( std::string( "WinSoc DLL is not support requested version." ), 
								 std::string( "server.cpp : Server::Server(void)" ), 
								 Exception::ExcData( &requestedVersion, sizeof( requestedVersion ) ) );

			++servers;
			Log::getLog() << std::string( "winsock was initialized" );

			if ( ( listenSock = socket( AF_INET, SOCK_STREAM, 0) ) < 0 )
				throw Exception( std::string( "Error while creating socket." ), 
								 std::string( "server.cpp : Server::Server(void)" ), 
								 Exception::ExcData( &listenSock, sizeof( listenSock ) ) );
			Log::getLog() << std::string( "listen socket was initialized" );

			sockaddr_in local_addr;
            local_addr.sin_family = AF_INET;
            local_addr.sin_port = htons( port );
            local_addr.sin_addr.s_addr = 0;

			if ( ( errorCode = bind( listenSock, ( sockaddr* )&local_addr, sizeof( local_addr ) ) ) )
				throw Exception( std::string( "Error while binding socket." ), 
								 std::string( "server.cpp : Server::Server(void)" ), 
								 Exception::ExcData( &errorCode, sizeof( errorCode ) ) );
			Log::getLog() << std::string( "listen socket was bound" );
			Log::getLog() << std::string( "server was initialized" );
		}
	}
	else
		throw Exception( std::string( "Maximum num of servers already runned." ), 
		                 std::string( "server.cpp : Server::Server(void)" ), 
						 Exception::ExcData( &servers, sizeof( servers ) ) );
}

Server::~Server(void)
{
	if ( listenSock )
		 closesocket( listenSock );

	--servers;
	Log::getLog() << std::string( "server shutdown. Still working: " ).append( std::to_string( servers ) );

	if ( servers == 0 )
		WSACleanup();
}

// ---------------------------------------------

std::thread* Server::listenGo( int queue_size )
{
	if ( isListen )
		return listenThread;

	MUTEXED_ASSIGN( isListenMutex, isListen, true );
	listenThread = new std::thread( &Server::listenState, this );

	return listenThread;
}

void Server::listenStop( void )
{
	if ( !isListen )
		return;

    MUTEXED_ASSIGN( isListenMutex, isListen, false );
	listenThread = 0;
}

void Server::listenState( void )
{
	int errorCode;

	while ( isListen )
	{
		if ( errorCode = listen( listenSock, QUEUE_SIZE ) )
		{
			MUTEXED_ASSIGN( isListenMutex, isListen, false );
			throw Exception( std::string( "Error while trying listen socket." ), 
				             std::string( "server.cpp : void Server::listenGo( void )" ), 
							 Exception::ExcData( &errorCode, sizeof( errorCode ) ) );
		}

		SOCKET client_socket;   
        sockaddr_in client_addr;    
        int client_addr_size = sizeof( client_addr );

		while( isListen && ( client_socket = accept(listenSock, ( sockaddr* )&client_addr, &client_addr_size ) ) )
        {
			Log::getLog() << "looks like somebody connected";

            HOSTENT* host;
            host = gethostbyaddr( ( char* )&client_addr.sin_addr.s_addr, 4, AF_INET );
			Log::getLog() << std::string( "connected: " ).append( host->h_name ) \
				                                         .append( " " )          \
														 .append( inet_ntoa( client_addr.sin_addr ) );
		}
	}
}

}
