#include "server.h"
#include "exception.h"
#include "log.h"

#include <string>
#include <WinSock2.h>

namespace server
{
// ---------------------------------------------

int Server::servers = 0;

// ---------------------------------------------

Server::Server(void)
{
	if ( servers < 0 )
		throw Exception( std::string( "Negative number of working servers." ), 
		                 std::string( "server.cpp : Server::Server(void)" ), 
						 Exception::ExcData( &servers, sizeof( servers ) ) );

	if ( servers < MAX_SERVERS )
	{
		const WORD requestedVersion = MAKEWORD( 2, 2 );
        WSADATA wsaData;
		const int errorCode = WSAStartup( requestedVersion, &wsaData );

		if ( errorCode )
		{
			throw Exception( std::string( "Error while init sockets." ), 
		                     std::string( "server.cpp : Server::Server(void)" ), 
							 Exception::ExcData( &errorCode, sizeof( errorCode ) ) );
		}
		else
		{
			++servers;
			Log::getLog() << std::string( "started new server #" ).append( std::to_string( servers ) );
		}
	}
	else
		throw Exception( std::string( "Maximum num of servers already runned." ), 
		                 std::string( "server.cpp : Server::Server(void)" ), 
						 Exception::ExcData( &servers, sizeof( servers ) ) );
}

Server::~Server(void)
{

}

}
