#pragma once

#include <fstream>
#include <string>
#include <mutex>

class Log
{
private:
	Log( const std::string&  fileName );
	~Log( void );
public:
	Log& operator<<( const std::string& logData );

    static Log& getLog( void )
	{
		static Log log( logFile );
		return log;
	}
	static void setLogFileName( std::string& name )
	{
		memcpy( logFile, name.c_str(), name.length() + 1 );
	}
private:
	static char logFile[ 255 ];

	std::ofstream logStream;
	std::mutex outMutex;
};
