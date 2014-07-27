#include "server.h"
#include "exception.h"
#include "log.h"
#include "servertypes.h"

#include <string>

namespace server
{
// ---------------------------------------------

int Server::servers = 0;

// ---------------------------------------------

Server::Server( u_short port )
	:listenSock(0)
	,isListen(false)
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

			if ( bind( listenSock, ( sockaddr* )&local_addr, sizeof( local_addr ) ) )
			{
				errorCode = WSAGetLastError();
				throw Exception( std::string( "Error while binding socket." ), 
								 std::string( "server.cpp : Server::Server(void)" ), 
								 Exception::ExcData( &errorCode, sizeof( errorCode ) ) );
			}
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
	if ( isListen )
	{
		isListen = false;
		listenThread.waitAll();
		listenThread.clean();
	}

	if ( listenSock )
		closesocket( listenSock );

	if ( clientThreads.getThreadsNum() )
	{
		clientThreads.waitAll();
		clientThreads.clean();
	}

	--servers;

	Log::getLog() << std::string( "server shutdown. Still working: " ).append( std::to_string( servers ) );

	if ( servers == 0 )
		WSACleanup();
}

// ---------------------------------------------

void Server::listenGo( int queueSize )
{
	if ( isListen )
		return;

	isListen = true;
	Server* ref = this;
	listenThread.addThread( std::async( std::launch::async, 
		                                [ queueSize, ref ] {  listenState( queueSize, ref ); 
	                                    return true; } ) );
}

void Server::listenStop( void )
{
	if ( !isListen )
		return;

	isListen = false;
	listenThread.waitAll();
	listenThread.clean();
}

void listenState( int queueSize, Server* obj )
{
	int errorCode;

	if ( listen( obj->listenSock, queueSize ) )
	{
		errorCode = WSAGetLastError();
		obj->isListen = false;
		throw Exception( std::string( "Error while trying listen socket." ), 
			             std::string( "server.cpp : void Server::listenGo( void )" ), 
						 Exception::ExcData( &errorCode, sizeof( errorCode ) ) );
	}

	SOCKET clientSocket;   
    sockaddr_in clientAddr;    
    int clientAddrSize = sizeof( clientAddr );
	
	fd_set readSet;
    timeval timeout;
    timeout.tv_sec = 0; 
    timeout.tv_usec = 0;

	while ( obj->isListen )
	{
		FD_ZERO( &readSet );
	    FD_SET( obj->listenSock, &readSet );

		int temp = select( obj->listenSock, &readSet, NULL, NULL, &timeout );
		if( temp == 1 )
		{
			clientSocket = accept( obj->listenSock, ( sockaddr* )&clientAddr, &clientAddrSize );
			Log::getLog() << "looks like somebody connected";

			obj->clientThreads.addThread( std::async( std::launch::async, 
		                                  [ clientSocket, obj ] {  clientRoutine( clientSocket, obj ); 
	                                      return true; } ) );
		}
	}

	//std::thread clientThread( &Server::clientRoutine, obj, clientSocket );
}

// ---------------------------------------------

size_t Server::recvData( SOCKET sock, void* buf, size_t bufLen )
{
	if ( !buf || !bufLen )
		return 0;

	char* charBuf = ( char* )buf;
	size_t bytesRecved = 0;
	size_t totalRecved = 0;
	while ( bufLen && ( bytesRecved = recv( sock, charBuf, bufLen, 0 ) ) )
	{
		bufLen      -= bytesRecved;
		totalRecved += bytesRecved;
		charBuf     += bytesRecved;
	}	

	return totalRecved;
}

size_t Server::sendData( SOCKET sock, const void* data, size_t dataLen )
{
	if ( !data || !dataLen )
		return 0;

	char* charData = ( char* )data;
	size_t bytesSended = 0;
	size_t totalSended = 0;
	while ( dataLen && ( bytesSended = send( sock, charData, dataLen, 0 ) ) )
	{
		dataLen     -= bytesSended;
		totalSended += bytesSended;
		charData    += bytesSended;
	}

	return totalSended;
}

// ---------------------------------------------

void clientRoutine( SOCKET sock, Server* obj )
{
	Greeting helloStruct;
    const size_t helloSize = obj->recvData( sock, &helloStruct, sizeof( helloStruct ) );

	if ( helloSize == sizeof( Greeting ) && CHECK_MAGIC( helloStruct ) )
	{
		Log::getLog() << std::string( "ok, user ").append( helloStruct.id ) \
		                                      .append( " says: " ) \
											  .append( std::to_string( helloStruct.cmd ) );
	}
	else
	{
		Log::getLog() << "client refused";
	}	
}

}
