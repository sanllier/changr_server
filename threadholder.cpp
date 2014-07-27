#include "threadholder.h"

#include <chrono>

namespace server
{

ThreadHolder::ThreadHolder(void)
{

}

ThreadHolder::~ThreadHolder(void)
{
	waitAll();
	clean();
}

// ---------------------------------------------

void ThreadHolder::addThread( std::future< bool >& fut )
{
	std::future< bool >* tempFut = new std::future< bool >( std::move( fut ) );
	workingThreads.push_back( tempFut );

	if ( workingThreads.size() > THREADS_THRESHOLD )
		clean();
}

void ThreadHolder::clean( void )
{
	std::list< std::future< bool >* > temp( workingThreads );
	workingThreads.clear();

	for ( auto fut : temp )
	{
		auto status = fut->wait_for( std::chrono::milliseconds(0) );
		if ( status != std::future_status::ready )
			workingThreads.push_back( fut );
		else
		{
			delete fut;
			fut = 0;
		}
	}
}

void ThreadHolder::waitAll( void )
{
	const std::chrono::milliseconds span(100);
	for ( auto fut : workingThreads )
		while ( fut->wait_for( span ) == std::future_status::timeout ) {}
}

int ThreadHolder::getThreadsNum( void )
{
	return workingThreads.size();
}

}
