#pragma once

#include <list>
#include <future>

namespace server
{

static const int THREADS_THRESHOLD = 100;

class ThreadHolder
{
public:
	ThreadHolder( void );
	~ThreadHolder( void );

	void addThread( std::future< bool >& fut );
	void clean( void );
	void waitAll( void );
	int getThreadsNum( void );
private:
	std::list< std::future< bool >* > workingThreads;
};

}
